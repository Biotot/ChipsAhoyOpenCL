#include "OpenCLLoad.h"


using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;

OpenCLLoad::~OpenCLLoad() 
{
	m_Queue.finish();
}
OpenCLLoad::OpenCLLoad()
{
}

void OpenCLLoad::Setup(int tPlatform, int tDevice) 
{
	std::vector<cl::Platform> aPlatforms;
	cl::Platform::get(&aPlatforms);
	std::vector<cl::Device> aDevices;
	cl::Device aDevice;

	bool aPreselected = true;
	if (tPlatform == -1 && tDevice == -1)
	{
		aPreselected = false;
	}

	if (!aPreselected)
	{
		for (int x = 0; x < aPlatforms.size(); x++)
		{
			std::cout << x << " : For Platform:" << aPlatforms[x].getInfo<CL_PLATFORM_NAME>() << "\n";
			aPlatforms[x].getDevices(CL_DEVICE_TYPE_ALL, &aDevices);
			for (int y = 0; y < aDevices.size(); y++)
			{
				std::cout << y << " : For Device:" << aDevices[y].getInfo<CL_DEVICE_NAME>() << "\n";
				//m_Device = aDevices[y];
			}
		}

		bool aPlatformSelected = false;
		string aPlatformString;
		while (!aPlatformSelected)
		{
			cout << "Please enter a Platform ID: ";
			cin >> aPlatformString;
			tPlatform = atoi(aPlatformString.c_str());
			aPlatformSelected = true;
		}

		bool aDeviceSelected = false;
		string aDeviceString;
		while (!aDeviceSelected)
		{
			cout << "Please enter a Device ID: ";
			cin >> aDeviceString;
			tDevice = atoi(aDeviceString.c_str());
			aDeviceSelected = true;
		}

	}

	string aFileName = "CLKernal.cl";
	std::ifstream in;
	in.open(aFileName.c_str());
	m_KernalCode = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	in.close();

	aPlatforms[tPlatform].getDevices(CL_DEVICE_TYPE_ALL, &aDevices);
	m_Device = aDevices[tDevice];


	std::cout << "Selected Device: " << m_Device.getInfo<CL_DEVICE_NAME>() << "\n";
	m_Context = cl::Context({ m_Device });
	cl::Program::Sources aSource;

	aSource.push_back({ m_KernalCode.c_str(),m_KernalCode.length() });

	m_Program = cl::Program(m_Context, aSource);
	m_Queue = cl::CommandQueue(m_Context, m_Device);
	
	if (m_Program.build({ m_Device }) != CL_SUCCESS) {
		std::cout << " Error building: " << m_Program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_Device) << "\n";
		system("PAUSE");
		exit(1);
	}
} 


void OpenCLLoad::LoadMarkets(Market* tMarketList, int tMarketCount, int tBrokerCount)
{
	m_MarketBufferList.clear();
	m_CountBufferList.clear();
	//clReleaseMemObject();
	
	m_Queue.enqueueReleaseGLObjects();
	m_BrokerBuffer = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(Broker)*tBrokerCount);
	m_HoldDaysBuffer = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(int));

	int aHoldDays = 20;
	m_Queue.enqueueWriteBuffer(m_HoldDaysBuffer, CL_TRUE, 0, sizeof(int), &aHoldDays);
	cl_int aResult;
	for (int x = 0; x < tMarketCount; x++)
	{
		Market aTempMarket = tMarketList[x];
		cl::Buffer aMarketBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(MarketPrice)*tMarketList[x].m_MarketPriceCount);
		cl::Buffer aPriceCountBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(int));
		aResult = m_Queue.enqueueWriteBuffer(aMarketBuffer, CL_TRUE, 0, sizeof(MarketPrice)*tMarketList[x].m_MarketPriceCount, tMarketList[x].PriceList);
		if (aResult != CL_SUCCESS)
		{
			cout << "FAILED TO ALLOCATE MARKETPRICE BUFFER:  " + aResult << endl;
		}

		aResult = m_Queue.enqueueWriteBuffer(aPriceCountBuffer, CL_TRUE, 0, sizeof(int), &(tMarketList[x].m_MarketPriceCount));
		if (aResult != CL_SUCCESS)
		{
			cout << "FAILED TO ALLOCATE MARKETPRICE COUNT BUFFER:  " + aResult << endl;
		}


		//Stupid things here. m_MarketDifferenceBufferList
		
		MarketPrice* aDifferenceList = new MarketPrice[tMarketList[x].m_MarketPriceCount];
		CalcMarketDifferences(tMarketList[x].PriceList, aDifferenceList, &tMarketList[x].m_MarketPriceCount);


		cl::Buffer aDifferenceBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(MarketPrice)*tMarketList[x].m_MarketPriceCount);
		aResult = m_Queue.enqueueWriteBuffer(aDifferenceBuffer, CL_TRUE, 0, sizeof(MarketPrice)*tMarketList[x].m_MarketPriceCount, aDifferenceList);
		if (aResult != CL_SUCCESS)
		{
			cout << "FAILED TO ALLOCATE PRICE DIFFERENCE BUFFER:  " + aResult << endl;
		}

		m_MarketBufferList.push_back(aMarketBuffer);
		m_CountBufferList.push_back(aPriceCountBuffer);
		m_MarketDifferenceBufferList.push_back(aDifferenceBuffer);
		delete aDifferenceList;
	}

}

void OpenCLLoad::RunBrokers(Broker* tBrokerList, int tBrokerCount)
{
	//clReleaseMemObject((cl_mem) m_BrokerBuffer);
	//m_Queue.enqueueFillBuffer(m_BrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);
	//m_Queue.
	cl_int aResult = m_Queue.enqueueWriteBuffer(m_BrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);
	if (aResult != CL_SUCCESS)
	{
		cout << "FAILED TO ALLOCATE BROKER BUFFER:  " + aResult << endl;
	}

	for (int x = 0; x < m_MarketBufferList.size(); x++)
	{
		cl::Kernel kernel_add = cl::Kernel(m_Program, "ShortTerm");
		kernel_add.setArg(0, m_BrokerBuffer);
		kernel_add.setArg(1, m_MarketBufferList[x]);
		kernel_add.setArg(2, m_CountBufferList[x]);
		kernel_add.setArg(3, m_MarketDifferenceBufferList[x]);
		kernel_add.setArg(4, m_HoldDaysBuffer);
		aResult = m_Queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(tBrokerCount), cl::NullRange);


		if (aResult != CL_SUCCESS)
		{
			cout << "FAILED TO ENQUEUE NEW TASK:  " + aResult << endl;
		}
		//aResult = m_Queue.finish();
		if (aResult != CL_SUCCESS)
		{
			cout << "FAILED TO COMPLETE NEW TASK:  " + aResult << endl;
		}
	}
	aResult = m_Queue.finish();
	m_Queue.enqueueReadBuffer(m_BrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);

	//
	/*for (int x = 0; x < tBrokerCount; x++)
	{
		Broker aTemp = tBrokerList[x];
		int a=0;
		a++;
	}*/

}

ConstQueue OpenCLLoad::CreateQueue()
{
	ConstQueue aQueue;
	aQueue.m_Front = 0;
	aQueue.m_Back = 0;
	for (int x = 0; x<100; x++)
	{
		aQueue.m_Queue[x] = 0;
	}
	return aQueue;
};

bool OpenCLLoad::AddRear(ConstQueue *tConstQueue, int tTarget)
{
	int aNewIndex = tConstQueue->m_Back+1;
	if (aNewIndex > 99)
	{
		aNewIndex = 0;
	}
	if (aNewIndex != tConstQueue->m_Front)
	{

		tConstQueue->m_Queue[tConstQueue->m_Back] = tTarget;
		tConstQueue->m_Back=aNewIndex;
		return true;
	}
	else
	{
		return false;
	}
}
bool OpenCLLoad::RemoveFront(ConstQueue *tConstQueue)
{
	if (tConstQueue->m_Front != tConstQueue->m_Back)
	{
		tConstQueue->m_Queue[tConstQueue->m_Front] = 0;
		tConstQueue->m_Front++;
		if (tConstQueue->m_Front > 99)
		{
			tConstQueue->m_Front = 0;
		}
		return true;
	}
	else
	{
		return false;
	}
}


PriceAverage OpenCLLoad::CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay)
{
	PriceAverage aRet;
	aRet.m_Price = tCurrentDay.m_Price - tPrevDay.m_Price;
	aRet.m_Avg5Day = tCurrentDay.m_Avg5Day - tPrevDay.m_Avg5Day;
	aRet.m_Avg15Day = tCurrentDay.m_Avg15Day - tPrevDay.m_Avg15Day;
	aRet.m_Avg30Day = tCurrentDay.m_Avg30Day - tPrevDay.m_Avg30Day;
	aRet.m_Avg3Month = tCurrentDay.m_Avg3Month - tPrevDay.m_Avg3Month;
	aRet.m_Avg6Month = tCurrentDay.m_Avg6Month - tPrevDay.m_Avg6Month;
	aRet.m_Avg1Year = tCurrentDay.m_Avg1Year - tPrevDay.m_Avg1Year;

	aRet.m_Price = (aRet.m_Price / tPrevDay.m_Price) * 100;
	aRet.m_Avg5Day = (aRet.m_Avg5Day / tPrevDay.m_Avg5Day) * 100;
	aRet.m_Avg15Day = (aRet.m_Avg15Day / tPrevDay.m_Avg15Day) * 100;
	aRet.m_Avg30Day = (aRet.m_Avg30Day / tPrevDay.m_Avg30Day) * 100;
	aRet.m_Avg3Month = (aRet.m_Avg3Month / tPrevDay.m_Avg3Month) * 100;
	aRet.m_Avg6Month = (aRet.m_Avg6Month / tPrevDay.m_Avg6Month) * 100;
	aRet.m_Avg1Year = (aRet.m_Avg1Year / tPrevDay.m_Avg1Year) * 100;

	return aRet;
}


void OpenCLLoad::CalcMarketDifferences(const MarketPrice* tMarketPriceList, MarketPrice* tMarketChanges, const int *tMarketPriceCount)
{
	for (int x = 1; x < tMarketPriceCount[0]; x++)
	{
		MarketPrice MarketPrice;
		MarketPrice.m_Open = CalcDPrice(tMarketPriceList[x].m_Open, tMarketPriceList[x-1].m_Open);
		MarketPrice.m_Close = CalcDPrice(tMarketPriceList[x].m_Close, tMarketPriceList[x-1].m_Close);
		MarketPrice.m_High = CalcDPrice(tMarketPriceList[x].m_High, tMarketPriceList[x-1].m_High);
		MarketPrice.m_Low = CalcDPrice(tMarketPriceList[x].m_Low, tMarketPriceList[x-1].m_Low);
		MarketPrice.m_Volume = CalcDPrice(tMarketPriceList[x].m_Volume, tMarketPriceList[x-1].m_Volume);

		tMarketChanges[x] = MarketPrice;
	}
}

ptree OpenCLLoad::LogLongTermBroker(Broker *tBroker, Market tMarket, MarketPrice* tMarketDifferenceList, bool tDisplayActions)
{
	Broker aSimBroker = *tBroker;
	string aMarketName(tMarket.m_MarketName);
	Log("RUNNING " + aMarketName, tMarket.m_MarketName, false);

	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	aSimBroker.m_TotalShareCount = 0;
	MarketPrice *tMarketPriceList;
	tMarketPriceList = tMarket.PriceList;
	double aInvestment = 0;
	int aOldestStock = 0;
	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_NetWorth = 0;
	aSimBroker.m_Investment = 0;
	int aMarketStockCount = 0;

	//ALTER SO THAT IT INCRIMENTS m_TotalInvestment daily. 
	//
	ptree aMarketXML;
	aMarketXML.add("MarketActions.<xmlattr>.Name", string(tMarket.m_MarketName).substr(0, 15));
	aMarketXML.add("MarketActions.<xmlattr>.Depth", tMarket.m_MarketPriceCount);


	/*for (int x = 0; x < tMarket.m_MarketPriceCount; x++)
	{
		std::cout << tMarketPriceList[x].m_Close.m_Price << " : " << string(tMarketPriceList[x].m_Timestamp, 15) << endl;
	}*/

	double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];

	double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

	if (aBuyPoint < aSellPoint)
	{
		aSellPoint = aBuyPoint;
	}
	for (int y = 1; y < tMarket.m_MarketPriceCount; y++)
	{
		bool aValid = true;
		bool aBuy = false;
		bool aSell = false;
		bool aSellOld = false;
		bool aSellSplit = false;
		bool aSellEnd = false;

		aMarketStockCount++;
		if (aMarketStockCount < 250)
		{
			aValid = false;
		}

		//Check if the stock split
		if ((aSimBroker.m_ShareCount > 0))
		{
			//Just assume a stock would never lose half it's value over night
			if (aValid && ((tMarketPriceList[y - 1].m_Close.m_Price / tMarketPriceList[y].m_Open.m_Price) > 1.9))
			{
				aSellSplit = true;//Sell Split
				aValid = false;
			}

			if (aOldestStock + 30 < y)
			{
				aSellOld = true;//Sell Old
				aValid = false;
			}
		}

		if (aValid)
		{

			double aDecisionPoint = 0.0;
			PriceAverage aDOpen;
			aDOpen = tMarketDifferenceList[y].m_Open;
			//CalcDPrice(tMarketPriceList[y].m_Open, tMarketPriceList[y - 1].m_Open);
			aDecisionPoint += aDOpen.m_Price*aSimBroker.m_Settings[4];
			aDecisionPoint += aDOpen.m_Avg5Day*aSimBroker.m_Settings[5];
			aDecisionPoint += aDOpen.m_Avg15Day*aSimBroker.m_Settings[6];
			aDecisionPoint += aDOpen.m_Avg30Day*aSimBroker.m_Settings[7];
			aDecisionPoint += aDOpen.m_Avg3Month*aSimBroker.m_Settings[8];
			aDecisionPoint += aDOpen.m_Avg6Month*aSimBroker.m_Settings[9];
			aDecisionPoint += aDOpen.m_Avg1Year*aSimBroker.m_Settings[10];

			PriceAverage aDHigh;
			aDHigh = tMarketDifferenceList[y].m_High;
			//aDHigh = CalcDPrice(tMarketPriceList[y].m_High, tMarketPriceList[y - 1].m_High);
			aDecisionPoint += aDHigh.m_Price*aSimBroker.m_Settings[11];
			aDecisionPoint += aDHigh.m_Avg5Day*aSimBroker.m_Settings[12];
			aDecisionPoint += aDHigh.m_Avg15Day*aSimBroker.m_Settings[13];
			aDecisionPoint += aDHigh.m_Avg30Day*aSimBroker.m_Settings[14];
			aDecisionPoint += aDHigh.m_Avg3Month*aSimBroker.m_Settings[15];
			aDecisionPoint += aDHigh.m_Avg6Month*aSimBroker.m_Settings[16];
			aDecisionPoint += aDHigh.m_Avg1Year*aSimBroker.m_Settings[17];

			PriceAverage aDLow;
			aDLow = tMarketDifferenceList[y].m_Low;
			//aDLow = CalcDPrice(tMarketPriceList[y].m_Low, tMarketPriceList[y - 1].m_Low);
			aDecisionPoint += aDLow.m_Price*aSimBroker.m_Settings[18];
			aDecisionPoint += aDLow.m_Avg5Day*aSimBroker.m_Settings[19];
			aDecisionPoint += aDLow.m_Avg15Day*aSimBroker.m_Settings[20];
			aDecisionPoint += aDLow.m_Avg30Day*aSimBroker.m_Settings[21];
			aDecisionPoint += aDLow.m_Avg3Month*aSimBroker.m_Settings[22];
			aDecisionPoint += aDLow.m_Avg6Month*aSimBroker.m_Settings[23];
			aDecisionPoint += aDLow.m_Avg1Year*aSimBroker.m_Settings[24];

			PriceAverage aDClose;
			aDClose = tMarketDifferenceList[y].m_Close;
			//aDClose = CalcDPrice(tMarketPriceList[y].m_Close, tMarketPriceList[y - 1].m_Close);
			aDecisionPoint += aDClose.m_Price*aSimBroker.m_Settings[25];
			aDecisionPoint += aDClose.m_Avg5Day*aSimBroker.m_Settings[26];
			aDecisionPoint += aDClose.m_Avg15Day*aSimBroker.m_Settings[27];
			aDecisionPoint += aDClose.m_Avg30Day*aSimBroker.m_Settings[28];
			aDecisionPoint += aDClose.m_Avg3Month*aSimBroker.m_Settings[29];
			aDecisionPoint += aDClose.m_Avg6Month*aSimBroker.m_Settings[30];
			aDecisionPoint += aDClose.m_Avg1Year*aSimBroker.m_Settings[31];


			PriceAverage aDVolume;
			aDVolume = tMarketDifferenceList[y].m_Close;
			//aDVolume = CalcDPrice(tMarketPriceList[y].m_Volume, tMarketPriceList[y - 1].m_Volume);
			aDecisionPoint += aDVolume.m_Price*aSimBroker.m_Settings[25];
			aDecisionPoint += aDVolume.m_Avg5Day*aSimBroker.m_Settings[26];
			aDecisionPoint += aDVolume.m_Avg15Day*aSimBroker.m_Settings[27];
			aDecisionPoint += aDVolume.m_Avg30Day*aSimBroker.m_Settings[28];
			aDecisionPoint += aDVolume.m_Avg3Month*aSimBroker.m_Settings[29];
			aDecisionPoint += aDVolume.m_Avg6Month*aSimBroker.m_Settings[30];
			aDecisionPoint += aDVolume.m_Avg1Year*aSimBroker.m_Settings[31];

			aDecisionPoint += (aDOpen.m_Price - aDOpen.m_Avg5Day)*aSimBroker.m_Settings[32];
			aDecisionPoint += (aDOpen.m_Avg5Day - aDOpen.m_Avg15Day)*aSimBroker.m_Settings[33];
			aDecisionPoint += (aDOpen.m_Avg15Day - aDOpen.m_Avg30Day)*aSimBroker.m_Settings[34];
			aDecisionPoint += (aDOpen.m_Avg30Day - aDOpen.m_Avg3Month)*aSimBroker.m_Settings[35];
			aDecisionPoint += (aDOpen.m_Avg3Month - aDOpen.m_Avg6Month)*aSimBroker.m_Settings[36];
			aDecisionPoint += (aDOpen.m_Avg6Month - aDOpen.m_Avg1Year)*aSimBroker.m_Settings[37];

			aDecisionPoint += (aDClose.m_Price - aDClose.m_Avg5Day)*aSimBroker.m_Settings[38];
			aDecisionPoint += (aDClose.m_Avg5Day - aDClose.m_Avg15Day)*aSimBroker.m_Settings[39];
			aDecisionPoint += (aDClose.m_Avg15Day - aDClose.m_Avg30Day)*aSimBroker.m_Settings[40];
			aDecisionPoint += (aDClose.m_Avg30Day - aDClose.m_Avg3Month)*aSimBroker.m_Settings[41];
			aDecisionPoint += (aDClose.m_Avg3Month - aDClose.m_Avg6Month)*aSimBroker.m_Settings[42];
			aDecisionPoint += (aDClose.m_Avg6Month - aDClose.m_Avg1Year)*aSimBroker.m_Settings[43];

			aDecisionPoint += (aDHigh.m_Price - aDHigh.m_Avg5Day)*aSimBroker.m_Settings[44];
			aDecisionPoint += (aDHigh.m_Avg5Day - aDHigh.m_Avg15Day)*aSimBroker.m_Settings[45];
			aDecisionPoint += (aDHigh.m_Avg15Day - aDHigh.m_Avg30Day)*aSimBroker.m_Settings[46];
			aDecisionPoint += (aDHigh.m_Avg30Day - aDHigh.m_Avg3Month)*aSimBroker.m_Settings[47];
			aDecisionPoint += (aDHigh.m_Avg3Month - aDHigh.m_Avg6Month)*aSimBroker.m_Settings[48];
			aDecisionPoint += (aDHigh.m_Avg6Month - aDHigh.m_Avg1Year)*aSimBroker.m_Settings[49];

			aDecisionPoint += (aDClose.m_Price - aDClose.m_Avg5Day)*aSimBroker.m_Settings[50];
			aDecisionPoint += (aDClose.m_Avg5Day - aDClose.m_Avg15Day)*aSimBroker.m_Settings[51];
			aDecisionPoint += (aDClose.m_Avg15Day - aDClose.m_Avg30Day)*aSimBroker.m_Settings[52];
			aDecisionPoint += (aDClose.m_Avg30Day - aDClose.m_Avg3Month)*aSimBroker.m_Settings[53];
			aDecisionPoint += (aDClose.m_Avg3Month - aDClose.m_Avg6Month)*aSimBroker.m_Settings[54];
			aDecisionPoint += (aDClose.m_Avg6Month - aDClose.m_Avg1Year)*aSimBroker.m_Settings[55];

			if (aDecisionPoint > aBuyPoint)
			{
				aBuy = true;//Buy
			}
			if ((aDecisionPoint < aSellPoint))
			{
				aSell = true;//Sell
			}
			if (y == tMarket.m_MarketPriceCount - 1)
			{
				aSellEnd = true;//Sell End
			}

			if (aBuy&&aSell)
			{
				int aTest = 0;
			}
			else if (aBuy && !aSell)
			{
				int aTest = 0;
			}
		}


		if (aBuy)
		{
			if (aSimBroker.m_Budget > tMarketPriceList[y].m_Close.m_Price)
			{
				aSimBroker.m_Investment += tMarketPriceList[y].m_Close.m_Price;
				//aSimBroker.m_TotalInvestment += tMarketPriceList[y].m_Close.m_Price;

				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				string aAction = "Buy";
				ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
				aMarketXML.add_child("MarketActions.Action", aNode);


				aSimBroker.m_ShareCount++;
				aSimBroker.m_TotalShareCount++;
				aSimBroker.m_Budget -= tMarketPriceList[y].m_Close.m_Price;
				aOldestStock = y;
			}
		}
		if (aSell)
		{

			if (aSimBroker.m_ShareCount > 0)
			{
				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				string aAction = "Sell";
				ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
				aMarketXML.add_child("MarketActions.Action", aNode);
				Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
				aSimBroker.m_Investment -= tMarketPriceList[y].m_Close.m_Price;
				//aSimBroker.m_TotalInvestment -= tMarketPriceList[y].m_Close.m_Price;

				aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price;
				aSimBroker.m_ShareCount--;
				aOldestStock = y;
			}
		}
		if (aSellOld)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell OLD";
			ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);

			//aSimBroker.m_TotalInvestment -= tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount; 
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		if (aSellSplit)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell SPLIT";
			ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);

			//aSimBroker.m_TotalInvestment -= tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount; 
			aSimBroker.m_Investment = 0;

			//aSimBroker.m_ShareCount += aSimBroker.m_ShareCount;
			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		if (aSellEnd)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell END";
			ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
			//aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_NetWorth = aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
	}
	Log("Final: " + to_string(aSimBroker.m_Budget - aSimBroker.m_BudgetPerMarket), aMarketName);

	double aProfit = aSimBroker.m_NetWorth -aSimBroker.m_BudgetPerMarket;
	ptree aMarketDetails;
	aMarketDetails.add("Profit", aProfit);
	aMarketDetails.add("ShareCount", aSimBroker.m_TotalShareCount);
	if (aSimBroker.m_TotalShareCount > 0)
	{
		aMarketDetails.add("ProfitPerShare", aProfit / aSimBroker.m_TotalShareCount);
	}
	else
	{
		aMarketDetails.add("ProfitPerShare", 0);
	}
	double aAverageInvestment = aSimBroker.m_TotalInvestment / tMarket.m_MarketPriceCount;
	double aPercentReturn = aProfit / (aAverageInvestment*(tMarket.m_MarketPriceCount/ONEYEAR));

	aMarketDetails.add("AverageInvestment", aAverageInvestment);
	aMarketDetails.add("PercentReturn", aPercentReturn);
	aMarketXML.add_child("MarketActions.Details", aMarketDetails);
	*tBroker = aSimBroker;
	return aMarketXML;

	//string aSaveFile = string(tMarket.m_MarketName).substr(0, 15) + to_string(aSimBroker.m_BrokerGuid) +  + ".xml";
	//write_xml("Broker\\"+aSaveFile, aMarketXML);
}

ptree OpenCLLoad::LogShortTermBroker(Broker *tBroker, Market tMarket, MarketPrice* tMarketDifferenceList, bool tDisplayActions, int tHoldDays)
{
	Broker aSimBroker = *tBroker;
	string aMarketName(tMarket.m_MarketName);
	Log("RUNNING " + aMarketName, tMarket.m_MarketName, false);

	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	aSimBroker.m_TotalShareCount = 0;
	MarketPrice *tMarketPriceList;
	tMarketPriceList = tMarket.PriceList;
	double aInvestment = 0;
	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_NetWorth = 0;
	aSimBroker.m_Investment = 0;
	int aMarketStockCount = 0;


	//aSellDate.assign()
	ptree aMarketXML;
	aMarketXML.add("MarketActions.<xmlattr>.Name", string(tMarket.m_MarketName).substr(0, 15));
	aMarketXML.add("MarketActions.<xmlattr>.Depth", tMarket.m_MarketPriceCount);

	ConstQueue aPurchaseQueue = CreateQueue();
	double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];

	double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

	if (aBuyPoint < aSellPoint)
	{
		aSellPoint = aBuyPoint;
	}
	//No sell point;

	for (int y = 1; y < tMarket.m_MarketPriceCount; y++)
	{
		bool aValid = true;
		bool aBuy = false;
		bool aSell = false;
		bool aSellSplit = false;
		bool aSellEnd = false;

		aMarketStockCount++;
		if (aMarketStockCount < 250)
		{
			aValid = false;
		}

		//Check if the stock split
		if ((aSimBroker.m_ShareCount > 0))
		{
			//Just assume a stock would never lose half it's value over night
			if (aValid && (((tMarketPriceList[y - 1].m_Close.m_Price / tMarketPriceList[y].m_Open.m_Price) > 1.9) || (tMarketPriceList[y - 1].m_Close.m_Price / tMarketPriceList[y].m_Open.m_Price) < 0.52))
			{
				aSellSplit = true;//Sell Split
				aValid = false;
			}

		}

		if (aValid)
		{

			double aDecisionPoint = 0.0;
			PriceAverage aDOpen;
			aDOpen = tMarketDifferenceList[y].m_Open;
			//CalcDPrice(tMarketPriceList[y].m_Open, tMarketPriceList[y - 1].m_Open);
			aDecisionPoint += aDOpen.m_Price*aSimBroker.m_Settings[4];
			aDecisionPoint += aDOpen.m_Avg5Day*aSimBroker.m_Settings[5];
			aDecisionPoint += aDOpen.m_Avg15Day*aSimBroker.m_Settings[6];
			aDecisionPoint += aDOpen.m_Avg30Day*aSimBroker.m_Settings[7];
			aDecisionPoint += aDOpen.m_Avg3Month*aSimBroker.m_Settings[8];
			aDecisionPoint += aDOpen.m_Avg6Month*aSimBroker.m_Settings[9];
			aDecisionPoint += aDOpen.m_Avg1Year*aSimBroker.m_Settings[10];

			PriceAverage aDHigh;
			aDHigh = tMarketDifferenceList[y].m_High;
			//aDHigh = CalcDPrice(tMarketPriceList[y].m_High, tMarketPriceList[y - 1].m_High);
			aDecisionPoint += aDHigh.m_Price*aSimBroker.m_Settings[11];
			aDecisionPoint += aDHigh.m_Avg5Day*aSimBroker.m_Settings[12];
			aDecisionPoint += aDHigh.m_Avg15Day*aSimBroker.m_Settings[13];
			aDecisionPoint += aDHigh.m_Avg30Day*aSimBroker.m_Settings[14];
			aDecisionPoint += aDHigh.m_Avg3Month*aSimBroker.m_Settings[15];
			aDecisionPoint += aDHigh.m_Avg6Month*aSimBroker.m_Settings[16];
			aDecisionPoint += aDHigh.m_Avg1Year*aSimBroker.m_Settings[17];

			PriceAverage aDLow;
			aDLow = tMarketDifferenceList[y].m_Low;
			//aDLow = CalcDPrice(tMarketPriceList[y].m_Low, tMarketPriceList[y - 1].m_Low);
			aDecisionPoint += aDLow.m_Price*aSimBroker.m_Settings[18];
			aDecisionPoint += aDLow.m_Avg5Day*aSimBroker.m_Settings[19];
			aDecisionPoint += aDLow.m_Avg15Day*aSimBroker.m_Settings[20];
			aDecisionPoint += aDLow.m_Avg30Day*aSimBroker.m_Settings[21];
			aDecisionPoint += aDLow.m_Avg3Month*aSimBroker.m_Settings[22];
			aDecisionPoint += aDLow.m_Avg6Month*aSimBroker.m_Settings[23];
			aDecisionPoint += aDLow.m_Avg1Year*aSimBroker.m_Settings[24];

			PriceAverage aDClose;
			aDClose = tMarketDifferenceList[y].m_Close;
			//aDClose = CalcDPrice(tMarketPriceList[y].m_Close, tMarketPriceList[y - 1].m_Close);
			aDecisionPoint += aDClose.m_Price*aSimBroker.m_Settings[25];
			aDecisionPoint += aDClose.m_Avg5Day*aSimBroker.m_Settings[26];
			aDecisionPoint += aDClose.m_Avg15Day*aSimBroker.m_Settings[27];
			aDecisionPoint += aDClose.m_Avg30Day*aSimBroker.m_Settings[28];
			aDecisionPoint += aDClose.m_Avg3Month*aSimBroker.m_Settings[29];
			aDecisionPoint += aDClose.m_Avg6Month*aSimBroker.m_Settings[30];
			aDecisionPoint += aDClose.m_Avg1Year*aSimBroker.m_Settings[31];


			PriceAverage aDVolume;
			aDVolume = tMarketDifferenceList[y].m_Close;
			//aDVolume = CalcDPrice(tMarketPriceList[y].m_Volume, tMarketPriceList[y - 1].m_Volume);
			aDecisionPoint += aDVolume.m_Price*aSimBroker.m_Settings[25];
			aDecisionPoint += aDVolume.m_Avg5Day*aSimBroker.m_Settings[26];
			aDecisionPoint += aDVolume.m_Avg15Day*aSimBroker.m_Settings[27];
			aDecisionPoint += aDVolume.m_Avg30Day*aSimBroker.m_Settings[28];
			aDecisionPoint += aDVolume.m_Avg3Month*aSimBroker.m_Settings[29];
			aDecisionPoint += aDVolume.m_Avg6Month*aSimBroker.m_Settings[30];
			aDecisionPoint += aDVolume.m_Avg1Year*aSimBroker.m_Settings[31];

			aDecisionPoint += (aDOpen.m_Price - aDOpen.m_Avg5Day)*aSimBroker.m_Settings[32];
			aDecisionPoint += (aDOpen.m_Avg5Day - aDOpen.m_Avg15Day)*aSimBroker.m_Settings[33];
			aDecisionPoint += (aDOpen.m_Avg15Day - aDOpen.m_Avg30Day)*aSimBroker.m_Settings[34];
			aDecisionPoint += (aDOpen.m_Avg30Day - aDOpen.m_Avg3Month)*aSimBroker.m_Settings[35];
			aDecisionPoint += (aDOpen.m_Avg3Month - aDOpen.m_Avg6Month)*aSimBroker.m_Settings[36];
			aDecisionPoint += (aDOpen.m_Avg6Month - aDOpen.m_Avg1Year)*aSimBroker.m_Settings[37];

			aDecisionPoint += (aDClose.m_Price - aDClose.m_Avg5Day)*aSimBroker.m_Settings[38];
			aDecisionPoint += (aDClose.m_Avg5Day - aDClose.m_Avg15Day)*aSimBroker.m_Settings[39];
			aDecisionPoint += (aDClose.m_Avg15Day - aDClose.m_Avg30Day)*aSimBroker.m_Settings[40];
			aDecisionPoint += (aDClose.m_Avg30Day - aDClose.m_Avg3Month)*aSimBroker.m_Settings[41];
			aDecisionPoint += (aDClose.m_Avg3Month - aDClose.m_Avg6Month)*aSimBroker.m_Settings[42];
			aDecisionPoint += (aDClose.m_Avg6Month - aDClose.m_Avg1Year)*aSimBroker.m_Settings[43];

			aDecisionPoint += (aDHigh.m_Price - aDHigh.m_Avg5Day)*aSimBroker.m_Settings[44];
			aDecisionPoint += (aDHigh.m_Avg5Day - aDHigh.m_Avg15Day)*aSimBroker.m_Settings[45];
			aDecisionPoint += (aDHigh.m_Avg15Day - aDHigh.m_Avg30Day)*aSimBroker.m_Settings[46];
			aDecisionPoint += (aDHigh.m_Avg30Day - aDHigh.m_Avg3Month)*aSimBroker.m_Settings[47];
			aDecisionPoint += (aDHigh.m_Avg3Month - aDHigh.m_Avg6Month)*aSimBroker.m_Settings[48];
			aDecisionPoint += (aDHigh.m_Avg6Month - aDHigh.m_Avg1Year)*aSimBroker.m_Settings[49];

			aDecisionPoint += (aDClose.m_Price - aDClose.m_Avg5Day)*aSimBroker.m_Settings[50];
			aDecisionPoint += (aDClose.m_Avg5Day - aDClose.m_Avg15Day)*aSimBroker.m_Settings[51];
			aDecisionPoint += (aDClose.m_Avg15Day - aDClose.m_Avg30Day)*aSimBroker.m_Settings[52];
			aDecisionPoint += (aDClose.m_Avg30Day - aDClose.m_Avg3Month)*aSimBroker.m_Settings[53];
			aDecisionPoint += (aDClose.m_Avg3Month - aDClose.m_Avg6Month)*aSimBroker.m_Settings[54];
			aDecisionPoint += (aDClose.m_Avg6Month - aDClose.m_Avg1Year)*aSimBroker.m_Settings[55];

			if (aDecisionPoint > aBuyPoint)
			{
				aBuy = true;//Buy
			}
			if ((aDecisionPoint < aSellPoint)||(aPurchaseQueue.m_Queue[aPurchaseQueue.m_Front] == y))
			{
				aSell = true;
			}
			if (y == tMarket.m_MarketPriceCount - 1)
			{
				aSellEnd = true;//Sell End
			}

		}


		if (aBuy)
		{
			if (aSimBroker.m_Budget > tMarketPriceList[y].m_Close.m_Price)
			{
				if (AddRear(&aPurchaseQueue, y + tHoldDays))
				{
					if (!aSell)
					{
						aSimBroker.m_Investment += tMarketPriceList[y].m_Close.m_Price;

						string aTimestamp(tMarketPriceList[y].m_Timestamp);
						string aAction = "Buy";
						ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
						aMarketXML.add_child("MarketActions.Action", aNode);

						aSimBroker.m_ShareCount++;
						aSimBroker.m_TotalShareCount++;
						aSimBroker.m_Budget -= tMarketPriceList[y].m_Close.m_Price;
					}

				}
			}
		}

		//Check if the front of the purchase queue is set to sell today
		if (aSell)
		{

			if (aSimBroker.m_ShareCount > 0)
			{
				
				if (RemoveFront(&aPurchaseQueue))
				{
					if (!aBuy)
					{
						string aTimestamp(tMarketPriceList[y].m_Timestamp);
						string aAction = "Sell";
						ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
						aMarketXML.add_child("MarketActions.Action", aNode);
						Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
						
						aSimBroker.m_Investment -= tMarketPriceList[y].m_Close.m_Price;
						aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price;
						aSimBroker.m_ShareCount--;
					}
					else
					{
						string aTimestamp(tMarketPriceList[y].m_Timestamp);
						string aAction = "Hold";
						ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
						aMarketXML.add_child("MarketActions.Action", aNode);
						Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);

					}
				}
				
			}
		}
		if (aSellSplit)
		{

			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell SPLIT";
			ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);

			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			//Resetting the queue
			aPurchaseQueue = CreateQueue();
		}
		if (aSellEnd)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell END";
			ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aSimBroker.m_Investment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Investment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
			//aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_NetWorth = aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
		}
		aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
	}
	Log("Final: " + to_string(aSimBroker.m_Budget - aSimBroker.m_BudgetPerMarket), aMarketName);

	double aProfit = aSimBroker.m_NetWorth - aSimBroker.m_BudgetPerMarket;
	ptree aMarketDetails;
	aMarketDetails.add("Profit", aProfit);
	aMarketDetails.add("ShareCount", aSimBroker.m_TotalShareCount);
	aMarketDetails.add("EndShareCount", aSimBroker.m_ShareCount);
	if (aSimBroker.m_TotalShareCount > 0)
	{
		aMarketDetails.add("ProfitPerShare", aProfit / aSimBroker.m_TotalShareCount);
	}
	else
	{
		aMarketDetails.add("ProfitPerShare", 0);
	}
	double aAverageInvestment = aSimBroker.m_TotalInvestment / tMarket.m_MarketPriceCount;
	double aPercentReturn = aProfit / (aAverageInvestment*(tMarket.m_MarketPriceCount / ONEYEAR));

	aMarketDetails.add("AverageInvestment", aAverageInvestment);
	aMarketDetails.add("PercentReturn", aPercentReturn);
	aMarketXML.add_child("MarketActions.Details", aMarketDetails);
	*tBroker = aSimBroker;
	return aMarketXML;

	//string aSaveFile = string(tMarket.m_MarketName).substr(0, 15) + to_string(aSimBroker.m_BrokerGuid) +  + ".xml";
	//write_xml("Broker\\"+aSaveFile, aMarketXML);
}
/*
aInvestment += tMarketPriceList[y].m_Close.m_Price;
string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Buy";
ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);


string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell OLD";
ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;

string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell";
ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment -= tMarketPriceList[y].m_Close.m_Price;

string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell SPLIT";
ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;

string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell END";
ptree aNode = CreateNode(aAction, aTimestamp, aSimBroker.m_Budget, tMarketPriceList[y].m_Close.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;


*/

void OpenCLLoad::Log(string tMessage, string tMarketName, bool tAppend)
{
	string aMarketName;
	for (int x = 0; x < 15; x++)
	{
		aMarketName += tMarketName[x];
	}
	ofstream myfile;
	if (tAppend)
	{
		//myfile.open("Markets\\" + aMarketName + "MarketResult.txt", std::fstream::app);
	}
	else
	{
		//myfile.open("Markets\\" + aMarketName + "MarketResult.txt");
	}
	//myfile << tMessage << endl;
	//myfile.close();
}

ptree OpenCLLoad::CreateNode(string tBrokerAction, string tTimeStamp, double tBudget, double aPrice, int tShareCount, double tInvestment, double tValue)
{
	ptree aActionTree;
	aActionTree.add("<xmlattr>.Action", tBrokerAction);
	aActionTree.add("<xmlattr>.TimeStamp", tTimeStamp.substr(0,10));
	aActionTree.add("<xmlattr>.Price", aPrice);
	aActionTree.add("<xmlattr>.ShareCount", tShareCount);
	aActionTree.add("<xmlattr>.Investment", tInvestment);
	aActionTree.add("<xmlattr>.Value", tValue);
	aActionTree.add("<xmlattr>.Budget", tBudget);
	return aActionTree;
}