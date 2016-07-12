#include "OpenCLLoad.h"

OpenCLLoad::~OpenCLLoad() 
{
}

OpenCLLoad::OpenCLLoad() 
{
	std::vector<cl::Platform> aPlatforms;
	cl::Platform::get(&aPlatforms);
	std::vector<cl::Device> aDevices;
	cl::Device aDevice;
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
	int aPlatformInt;
	while (!aPlatformSelected)
	{
		cout << "Please enter a Platform ID: ";
		cin >> aPlatformString;
		aPlatformInt = atoi(aPlatformString.c_str());
		aPlatformSelected = true;
	}

	bool aDeviceSelected = false;
	string aDeviceString;
	int aDeviceInt;
	while (!aDeviceSelected)
	{
		cout << "Please enter a Device ID: ";
		cin >> aDeviceString;
		aDeviceInt = atoi(aDeviceString.c_str());
		aDeviceSelected = true;
	}

	aPlatforms[aPlatformInt].getDevices(CL_DEVICE_TYPE_ALL, &aDevices);
	m_Device = aDevices[aDeviceInt];

	std::cout << "Selected Device: " << m_Device.getInfo<CL_DEVICE_NAME>() << "\n";
	string aFileName = "CLKernal.cl";
	std::ifstream in;
	in.open(aFileName.c_str());
	m_KernalCode = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	in.close();
} 

void OpenCLLoad::TestRun(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount)
{
	cl::Context aContext({ m_Device });
	cl::Program::Sources aSource;

	aSource.push_back({ m_KernalCode.c_str(),m_KernalCode.length() });

	cl::Program program(aContext, aSource);
	if (program.build({ m_Device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_Device) << "\n";
		system("PAUSE");
		exit(1);
	}

	int aTotalPriceCount = tMarketCount;
	for (int x = 0; x < tMarketCount; x++)
	{
		aTotalPriceCount += tMarketList[x].m_MarketPriceCount;
	}
	MarketPrice *aPriceList = new MarketPrice[aTotalPriceCount];
	MarketPrice aDefaultPrice;
	aDefaultPrice.m_Open.m_Price = 0;

	int aCurrentIndex = 0;
	for (int x = 0; x < tMarketCount; x++)
	{
		for (int y = 0; y < tMarketList[x].m_MarketPriceCount; y++)
		{
			aPriceList[aCurrentIndex] = tMarketList[x].PriceList[y];
			aCurrentIndex++;
		}
		aPriceList[aCurrentIndex] = aDefaultPrice;
		aCurrentIndex++;
	}

	// create buffers on the device
	cl::Buffer aBrokerBuffer(aContext, CL_MEM_READ_WRITE, sizeof(Broker)*tBrokerCount);
	cl::Buffer aPriceBuffer(aContext, CL_MEM_READ_WRITE, sizeof(MarketPrice)*aTotalPriceCount);
	cl::Buffer aPriceCountBuffer(aContext, CL_MEM_READ_WRITE, sizeof(int));

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(aContext, m_Device);

	//write arrays A and B to the device
	queue.enqueueWriteBuffer(aBrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);
	queue.enqueueWriteBuffer(aPriceBuffer, CL_TRUE, 0, sizeof(MarketPrice)*aTotalPriceCount, aPriceList);
	queue.enqueueWriteBuffer(aPriceCountBuffer, CL_TRUE, 0, sizeof(int), &aTotalPriceCount);

	//alternative way to run the kernel
	cl::Kernel kernel_add = cl::Kernel(program, "FullRun");
	kernel_add.setArg(0, aBrokerBuffer);
	kernel_add.setArg(1, aPriceBuffer);
	kernel_add.setArg(2, aPriceCountBuffer);
	//kernel_add.setArg(2,buffer_C);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(tBrokerCount), cl::NullRange);
	queue.finish();

	//read result C from the device to array C
	queue.enqueueReadBuffer(aBrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);

	int aCurrentMarketIndex = 0;
	aCurrentIndex = 0;
	for (int x = 0; x < aTotalPriceCount; x++)
	{
		if (aPriceList[x].m_Open.m_Price==0)
		{
			aCurrentMarketIndex++;
			aCurrentIndex = 0;
		}
		else
		{
			tMarketList[aCurrentMarketIndex].PriceList[aCurrentIndex] = aPriceList[x];
			aCurrentIndex++;
		}
	}

	delete aPriceList;
}
void OpenCLLoad::RunBrokers(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount)
{
	cl::Context context({ m_Device });
	cl::Program::Sources sources;

	sources.push_back({ m_KernalCode.c_str(),m_KernalCode.length() });

	cl::Program program(context, sources);
	if (program.build({ m_Device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_Device) << "\n";
		system("PAUSE");
		exit(1);
	}

	// create buffers on the device
	cl::Buffer aBrokerBuffer(context, CL_MEM_READ_WRITE, sizeof(Broker)*tBrokerCount);
	cl::Buffer aMarketBuffer(context, CL_MEM_READ_WRITE, sizeof(Market)*tMarketCount);
	cl::Buffer aPriceBuffer(context, CL_MEM_READ_WRITE, sizeof(MarketPrice)*(tMarketList[0].m_MarketPriceCount));
	//cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(C));

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, m_Device);

	//write arrays A and B to the device
	queue.enqueueWriteBuffer(aBrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);
	queue.enqueueWriteBuffer(aMarketBuffer, CL_TRUE, 0, sizeof(Market)*tMarketCount, tMarketList);
	queue.enqueueWriteBuffer(aPriceBuffer, CL_TRUE, 0, sizeof(MarketPrice)*(tMarketList[0].m_MarketPriceCount), tMarketList[0].PriceList);

	//alternative way to run the kernel
	cl::Kernel kernel_add = cl::Kernel(program, "FullRun");
	kernel_add.setArg(0, aBrokerBuffer);
	kernel_add.setArg(1, aMarketBuffer);
	kernel_add.setArg(2, aPriceBuffer);
	//kernel_add.setArg(2,buffer_C);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(tBrokerCount), cl::NullRange);
	queue.finish();

	//read result C from the device to array C
	queue.enqueueReadBuffer(aBrokerBuffer, CL_TRUE, 0, sizeof(Broker)*tBrokerCount, tBrokerList);
}


void OpenCLLoad::DisplayBestBroker(Broker tBroker, Market tMarket)
{
	Broker aSimBroker = tBroker;
	string aMarketName(tMarket.m_MarketName);
	Log("RUNNING " + aMarketName, tMarket.m_MarketName, false);

	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	MarketPrice *tMarketPriceList;
	tMarketPriceList = tMarket.PriceList;
	double aInvestment = 0;
	int aOldestStock = 0;
	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	aSimBroker.m_NetWorth = 0;
	int aMarketStockCount = 0;

	for (int y = 0; y<tMarket.m_MarketPriceCount; y++)
	{
		bool aValid = true;
		if (tMarketPriceList[y].m_Open.m_Price == 0.0)
		{
			//Resetting the market
			aSimBroker.m_NetWorth += aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
			aSimBroker.m_ShareCount = 0;
			aValid = false;
			aOldestStock = 0;
		}

		aMarketStockCount++;
		if (aMarketStockCount < 365)
		{
			aValid = false;
		}

		//Check if the stock split
		if ((aSimBroker.m_ShareCount > 0))
		{
			//Just assume a stock would never lose half it's value over night
			if (aValid && ((tMarketPriceList[y - 1].m_Close.m_Price / tMarketPriceList[y].m_Open.m_Price)>1.9))
			{
				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				Log("Sell SPLIT: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
				aInvestment = 0;

				aSimBroker.m_ShareCount += aSimBroker.m_ShareCount;
				aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
				aSimBroker.m_ShareCount = 0;
				aOldestStock = y;
				aValid = false;
			}

			if (aOldestStock + 30 < y)
			{
				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				Log("Sell ALL: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
				aInvestment = 0;

				aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount;
				aSimBroker.m_ShareCount = 0;
				aOldestStock = y;
				aValid = false;
			}
		}

		if (aValid)
		{
			double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];

			double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

			double aDecisionPoint = 0.0;
			PriceAverage aDOpen;
			aDOpen.m_Price = tMarketPriceList[y].m_Open.m_Price - tMarketPriceList[y - 1].m_Open.m_Price;
			aDOpen.m_Avg5Day = tMarketPriceList[y].m_Open.m_Avg5Day - tMarketPriceList[y - 1].m_Open.m_Avg5Day;
			aDOpen.m_Avg15Day = tMarketPriceList[y].m_Open.m_Avg15Day - tMarketPriceList[y - 1].m_Open.m_Avg15Day;
			aDOpen.m_Avg30Day = tMarketPriceList[y].m_Open.m_Avg30Day - tMarketPriceList[y - 1].m_Open.m_Avg30Day;
			aDOpen.m_Avg3Month = tMarketPriceList[y].m_Open.m_Avg3Month - tMarketPriceList[y - 1].m_Open.m_Avg3Month;
			aDOpen.m_Avg6Month = tMarketPriceList[y].m_Open.m_Avg6Month - tMarketPriceList[y - 1].m_Open.m_Avg6Month;
			aDOpen.m_Avg1Year = tMarketPriceList[y].m_Open.m_Avg1Year - tMarketPriceList[y - 1].m_Open.m_Avg1Year;
			aDecisionPoint += aDOpen.m_Price*aSimBroker.m_Settings[4] / tMarketPriceList[y].m_Open.m_Price;
			aDecisionPoint += aDOpen.m_Avg5Day*aSimBroker.m_Settings[5] / tMarketPriceList[y].m_Open.m_Avg5Day;
			aDecisionPoint += aDOpen.m_Avg15Day*aSimBroker.m_Settings[6] / tMarketPriceList[y].m_Open.m_Avg15Day;
			aDecisionPoint += aDOpen.m_Avg30Day*aSimBroker.m_Settings[7] / tMarketPriceList[y].m_Open.m_Avg30Day;
			aDecisionPoint += aDOpen.m_Avg3Month*aSimBroker.m_Settings[8] / tMarketPriceList[y].m_Open.m_Avg3Month;
			aDecisionPoint += aDOpen.m_Avg6Month*aSimBroker.m_Settings[9] / tMarketPriceList[y].m_Open.m_Avg6Month;
			aDecisionPoint += aDOpen.m_Avg1Year*aSimBroker.m_Settings[10] / tMarketPriceList[y].m_Open.m_Avg1Year;
			PriceAverage aDHigh;
			aDHigh.m_Price = tMarketPriceList[y].m_High.m_Price - tMarketPriceList[y - 1].m_High.m_Price;
			aDHigh.m_Avg5Day = tMarketPriceList[y].m_High.m_Avg5Day - tMarketPriceList[y - 1].m_High.m_Avg5Day;
			aDHigh.m_Avg15Day = tMarketPriceList[y].m_High.m_Avg15Day - tMarketPriceList[y - 1].m_High.m_Avg15Day;
			aDHigh.m_Avg30Day = tMarketPriceList[y].m_High.m_Avg30Day - tMarketPriceList[y - 1].m_High.m_Avg30Day;
			aDHigh.m_Avg3Month = tMarketPriceList[y].m_High.m_Avg3Month - tMarketPriceList[y - 1].m_High.m_Avg3Month;
			aDHigh.m_Avg6Month = tMarketPriceList[y].m_High.m_Avg6Month - tMarketPriceList[y - 1].m_High.m_Avg6Month;
			aDHigh.m_Avg1Year = tMarketPriceList[y].m_High.m_Avg1Year - tMarketPriceList[y - 1].m_High.m_Avg1Year;
			aDecisionPoint += aDHigh.m_Price*aSimBroker.m_Settings[11];
			aDecisionPoint += aDHigh.m_Avg5Day*aSimBroker.m_Settings[12];
			aDecisionPoint += aDHigh.m_Avg15Day*aSimBroker.m_Settings[13];
			aDecisionPoint += aDHigh.m_Avg30Day*aSimBroker.m_Settings[14];
			aDecisionPoint += aDHigh.m_Avg3Month*aSimBroker.m_Settings[15];
			aDecisionPoint += aDHigh.m_Avg6Month*aSimBroker.m_Settings[16];
			aDecisionPoint += aDHigh.m_Avg1Year*aSimBroker.m_Settings[17];
			PriceAverage aDLow;
			aDLow.m_Price = tMarketPriceList[y].m_Low.m_Price - tMarketPriceList[y - 1].m_Low.m_Price;
			aDLow.m_Avg5Day = tMarketPriceList[y].m_Low.m_Avg5Day - tMarketPriceList[y - 1].m_Low.m_Avg5Day;
			aDLow.m_Avg15Day = tMarketPriceList[y].m_Low.m_Avg15Day - tMarketPriceList[y - 1].m_Low.m_Avg15Day;
			aDLow.m_Avg30Day = tMarketPriceList[y].m_Low.m_Avg30Day - tMarketPriceList[y - 1].m_Low.m_Avg30Day;
			aDLow.m_Avg3Month = tMarketPriceList[y].m_Low.m_Avg3Month - tMarketPriceList[y - 1].m_Low.m_Avg3Month;
			aDLow.m_Avg6Month = tMarketPriceList[y].m_Low.m_Avg6Month - tMarketPriceList[y - 1].m_Low.m_Avg6Month;
			aDLow.m_Avg1Year = tMarketPriceList[y].m_Low.m_Avg1Year - tMarketPriceList[y - 1].m_Low.m_Avg1Year;
			aDecisionPoint += aDLow.m_Price*aSimBroker.m_Settings[18];
			aDecisionPoint += aDLow.m_Avg5Day*aSimBroker.m_Settings[19];
			aDecisionPoint += aDLow.m_Avg15Day*aSimBroker.m_Settings[20];
			aDecisionPoint += aDLow.m_Avg30Day*aSimBroker.m_Settings[21];
			aDecisionPoint += aDLow.m_Avg3Month*aSimBroker.m_Settings[22];
			aDecisionPoint += aDLow.m_Avg6Month*aSimBroker.m_Settings[23];
			aDecisionPoint += aDLow.m_Avg1Year*aSimBroker.m_Settings[24];

			PriceAverage aDClose;
			aDClose.m_Price = tMarketPriceList[y].m_Close.m_Price - tMarketPriceList[y - 1].m_Close.m_Price;
			aDClose.m_Avg5Day = tMarketPriceList[y].m_Close.m_Avg5Day - tMarketPriceList[y - 1].m_Close.m_Avg5Day;
			aDClose.m_Avg15Day = tMarketPriceList[y].m_Close.m_Avg15Day - tMarketPriceList[y - 1].m_Close.m_Avg15Day;
			aDClose.m_Avg30Day = tMarketPriceList[y].m_Close.m_Avg30Day - tMarketPriceList[y - 1].m_Close.m_Avg30Day;
			aDClose.m_Avg3Month = tMarketPriceList[y].m_Close.m_Avg3Month - tMarketPriceList[y - 1].m_Close.m_Avg3Month;
			aDClose.m_Avg6Month = tMarketPriceList[y].m_Close.m_Avg6Month - tMarketPriceList[y - 1].m_Close.m_Avg6Month;
			aDClose.m_Avg1Year = tMarketPriceList[y].m_Close.m_Avg1Year - tMarketPriceList[y - 1].m_Close.m_Avg1Year;
			aDecisionPoint += aDClose.m_Price*aSimBroker.m_Settings[25];
			aDecisionPoint += aDClose.m_Avg5Day*aSimBroker.m_Settings[26];
			aDecisionPoint += aDClose.m_Avg15Day*aSimBroker.m_Settings[27];
			aDecisionPoint += aDClose.m_Avg30Day*aSimBroker.m_Settings[28];
			aDecisionPoint += aDClose.m_Avg3Month*aSimBroker.m_Settings[29];
			aDecisionPoint += aDClose.m_Avg6Month*aSimBroker.m_Settings[30];
			aDecisionPoint += aDClose.m_Avg1Year*aSimBroker.m_Settings[31];

			aDecisionPoint += aDOpen.m_Avg5Day - aDOpen.m_Avg15Day*aSimBroker.m_Settings[32];
			aDecisionPoint += aDOpen.m_Avg15Day - aDOpen.m_Avg30Day*aSimBroker.m_Settings[33];
			aDecisionPoint += aDHigh.m_Avg5Day - aDHigh.m_Avg15Day*aSimBroker.m_Settings[34];
			aDecisionPoint += aDHigh.m_Avg15Day - aDHigh.m_Avg30Day*aSimBroker.m_Settings[35];
			aDecisionPoint += aDLow.m_Avg5Day - aDLow.m_Avg15Day*aSimBroker.m_Settings[36];
			aDecisionPoint += aDLow.m_Avg15Day - aDLow.m_Avg30Day*aSimBroker.m_Settings[37];
			aDecisionPoint += aDClose.m_Avg5Day - aDClose.m_Avg15Day*aSimBroker.m_Settings[38];
			aDecisionPoint += aDClose.m_Avg15Day - aDClose.m_Avg30Day*aSimBroker.m_Settings[39];

			aDecisionPoint += aDOpen.m_Avg5Day - aDClose.m_Avg5Day*aSimBroker.m_Settings[40];
			aDecisionPoint += aDOpen.m_Avg15Day - aDClose.m_Avg15Day*aSimBroker.m_Settings[41];

			aDecisionPoint += aDHigh.m_Avg5Day - aDLow.m_Avg5Day*aSimBroker.m_Settings[42];
			aDecisionPoint += aDHigh.m_Avg15Day - aDLow.m_Avg15Day*aSimBroker.m_Settings[43];

			if (aDecisionPoint > aBuyPoint)
			{
				if (aSimBroker.m_Budget > tMarketPriceList[y].m_Open.m_Price)
				{
					aInvestment += tMarketPriceList[y].m_Open.m_Price;
					string aTimestamp(tMarketPriceList[y].m_Timestamp);
					Log("Buy: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Budget), aMarketName);

					aSimBroker.m_ShareCount++;
					aSimBroker.m_TotalShareCount++;
					aSimBroker.m_Budget -= tMarketPriceList[y].m_Open.m_Price;
					aOldestStock = y;
				}
			}

			if (aSimBroker.m_ShareCount>0)
			{
				if ((aDecisionPoint < aSellPoint))
				{
					string aTimestamp(tMarketPriceList[y].m_Timestamp);
					Log("Sell: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount), aMarketName);
					aInvestment -= tMarketPriceList[y].m_Open.m_Price;

					aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price;
					aSimBroker.m_ShareCount--;
					aOldestStock = y;
				}
				if (y == tMarket.m_MarketPriceCount - 1)
				{
					string aTimestamp(tMarketPriceList[y].m_Timestamp);
					Log("Sell ALL: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
					aInvestment = 0;

					aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount;
					aSimBroker.m_ShareCount = 0;
					aOldestStock = y;
					aValid = false;
				}
			}
		}

	}
	Log("Final: " + to_string(aSimBroker.m_Budget - aSimBroker.m_BudgetPerMarket), aMarketName);
}

/*
string aTimestamp(tMarketPriceList[y].m_Timestamp);
Log("Sell SPLIT: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y-1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y-1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;


string aTimestamp(tMarketPriceList[y].m_Timestamp);
Log("Sell ALL: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;


aInvestment += tMarketPriceList[y].m_Open.m_Price;
string aTimestamp(tMarketPriceList[y].m_Timestamp);
Log("Buy: " + aTimestamp.substr(0,15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Budget), aMarketName);

string aTimestamp(tMarketPriceList[y].m_Timestamp);
Log("Sell: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount), aMarketName);
aInvestment -= tMarketPriceList[y].m_Open.m_Price;


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
		myfile.open("Markets\\" + aMarketName + "MarketResult.txt", std::fstream::app);
	}
	else
	{
		myfile.open("Markets\\" + aMarketName + "MarketResult.txt");
	}
	myfile << tMessage << endl;
	myfile.close();
}
