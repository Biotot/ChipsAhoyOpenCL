#include "SimControl.h"
#include "dirent.h"
//#include <vld.h> 

using std::vector;
using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;


void UpdateBrokers(int tPlatform, int tDevice)
{
	BrokerLoader aLoader;
	SimControl aHost;
	int aSimDepth = 1;
	aHost.SetMarketList(aSimDepth, tPlatform, tDevice, 1);

	DIR *dir;
	struct dirent *ent;

	int aFileCount = 0;

	if ((dir = opendir("Broker")) != NULL) {
		/* print all the files and directories within directory */
		while (((ent = readdir(dir)) != NULL)){//&&aFileCount<1) {
			printf("LOADING BROKER: %s\n", ent->d_name);

			string aFileName = ent->d_name;
			int aIndex = aFileName.find(".xml");
			if (aIndex > 0)
			{
				aHost.m_BestBroker = aLoader.ParseXML(aFileName);
				aFileCount++;
				//cout << aHost.m_BestBroker.m_BrokerGuid + " RUNNING " << endl;
				aHost.Run(aSimDepth, 1);
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return;
	}
}

void BruteBrokers(int tPlatform, int tDevice, int tBrokerCount)
{
	ChipsDB aDB;
	SimControl aHost;
	BrokerLoader aLoader;

	int aBrokerCount[] = { 50000,25000,10000,5000,1000,500 };

	if (tBrokerCount == -1)
	{
		cout << "Please enter a desired BrokerCount: " << endl;
		cout << "0 : 50000" << endl << "1 : 25000" << endl << "2 : 10000" << endl;
		cout << "3 : 5000" << endl << "4 : 1000" << endl << "5 : 500" << endl;
		cin >> tBrokerCount;
	}

	


	srand(time(NULL));
	while (true)
	{
		//for (int x = 5; x >1; x--)
		for (int x = 2; x <5; x++)
		{
			if (tBrokerCount > 5)
			{
				aHost.SetDefaultBroker();
				aHost.SetMarketList(x, tPlatform, tDevice, tBrokerCount);
				aHost.Run(x, tBrokerCount);
			}
			else
			{
				aHost.SetDefaultBroker();
				aHost.SetMarketList(x, tPlatform, tDevice, aBrokerCount[tBrokerCount]);
				aHost.Run(x, aBrokerCount[tBrokerCount]);
			}
		}
	}
}

int main(int argc, char* argv[])
{

	bool aDefaultBrute = true;
	int aDefaultDevice = -1;
	int aDefaultPlatform = -1;
	int aDefaultBrokerCount = -1;

	for (int x = 0; x < argc; x++)
	{
		string aArg = string(argv[x]);
		if (aArg=="BRUTE")
		{
			aDefaultBrute = true;
		}
		if (aArg == "UPDATE")
		{
			aDefaultBrute = false;
		}
		else if (std::strcmp(argv[x], "-d") >= 0)
		{
			int aCount = atoi(argv[x]);
			if (aCount > 100)
			{
				aDefaultBrokerCount = aCount;
			}
			else
			{
				aDefaultDevice = aCount % 10;
				aDefaultPlatform = aCount / 10;
			}
		}

	}


	if (aDefaultBrute)
	{
		BruteBrokers(aDefaultPlatform, aDefaultDevice, aDefaultBrokerCount);
	}
	else
	{
		UpdateBrokers(aDefaultPlatform, aDefaultDevice);
	}
	

	//UpdateBrokers();
	//BruteBrokers();


	
	return 0;
}

SimControl::SimControl()
{
	SetDefaultBroker();
	//OpenCLLoad m_Loader;
}


SimControl::~SimControl()
{
	if (m_MarketList != NULL)
	{
		for (int x = 0; x < m_MarketCount; x++)
		{
			delete m_MarketList[x].PriceList;
		}
		delete m_MarketList;
	}
}

void SimControl::SetDefaultBroker()
{
	double aBestList[] = { -22.688600,	-74.854800,	-90.234800,	336.930200,	-439.737200,	-499.700000,	145.229600,	-490.503200,	54.278400,	351.530600,	-253.556400,	316.203200,	100.638400,	267.888800,	-271.065000,	11.467800,	-0.816200,	448.896400,	-208.513800,	-19.728600,	-251.636200,	-117.589000,	-190.517600,	120.604200,	-174.972400,	385.215600,	-375.954400,	168.776400,	-495.727200,	494.867200,	-210.970200,	-218.082000,	475.991200,	-198.840600,	-457.002000,	-473.665600,	-498.118400,	234.666600,	51.428400,	-320.715000,	182.062600,	65.921400,	-499.764400,	-500.000000,	-303.226800,	-2.121800,	-478.662400,	-355.311200,	-494.456800,	218.082000 };
	bool aSeededSetting = false;

	m_BestBroker.m_Budget = 0;
	m_BestBroker.m_BudgetPerMarket = 1000000;
	m_BestBroker.m_ProfitPerShare = 0;
	m_BestBroker.m_NetWorth = 0;
	m_BestBroker.m_BrokerScore = 0;
	m_BestBroker.m_SettingsCount = 51;
	m_BestBroker.m_TotalShareCount = 0;
	m_BestBroker.m_BrokerGuid = rand()*rand();
	for (int y = 0; y < m_BestBroker.m_SettingsCount; y++)
	{
		if (aSeededSetting)
		{
			m_BestBroker.m_Settings[y] = aBestList[y];
		}
		else
		{
			m_BestBroker.m_Settings[y] = 0;
		}
	}
}

void SimControl::SetMarketList(int tSimDepth, int tPlatform, int tDevice, int tBrokerCount)
{
	vector<string> aMarketStringList = { "GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "WFC", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA", "NFLX", "TWTR", "MSFT", "GPRO", "BABA", "SIRI", "CHK", "CSCO", "C", "ZNGA", "VZ", "MYL", "CXW" , "^IXIC", "^GSPC", "^DJI"};

	//vector<string> aMarketStringList = { "GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "WFC", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA"};
	//vector<string> aMarketStringList = { "GE" }; 

	
	m_MarketCount = aMarketStringList.size();
	m_MarketList = new Market[m_MarketCount];
	m_TotalPriceCount = 0;
	for (int x = 0; x < m_MarketCount; x++)
	{
		MarketLoader aLoader(aMarketStringList[x], 1980, (tSimDepth * 3 * ONEYEAR) + ONEYEAR);
		m_MarketList[x] = aLoader.m_Market;
		m_TotalPriceCount += aLoader.m_Market.m_MarketPriceCount;
	}

	m_Loader.Setup(tPlatform, tDevice);
	m_Loader.LoadMarkets(m_MarketList, m_MarketCount, tBrokerCount);
}

void SimControl::Run(int tSimDepth, int tBrokerCount)
{

	int aBrokerCount = tBrokerCount;

	cout << "Running with: " << m_TotalPriceCount << " price points" << endl;

	m_BestBroker.m_MarketCount = m_MarketCount;
	//aOpenCLCaller.DisplayBestBroker(m_BestBroker, aMarketList[0]);
	vector<Broker> aBestofTheBest;
	aBestofTheBest.insert(aBestofTheBest.begin(), m_BestBroker);
	std::cout << fixed;

	//int aMilliseconds = time(NULL);
	int aIdleLoopCount = 0;
	int aTotalSimCount = 0;
	double aMaxTarget = 1000000;
	if (tBrokerCount == 1)
	{
		aMaxTarget = 1;
	}

	/*for (int x = 0; x < m_MarketList[0].m_MarketPriceCount; x++)
	{
		std::cout << m_MarketList[0].PriceList[x].m_Close.m_Price<<" : " << string(m_MarketList[0].PriceList[x].m_Timestamp) << endl;
	}*/

#pragma region MAINLOOP
	Log("~~~~~~~~~STARTING NEW RUN~~~~~~~~~~~~~~~" + to_string(tSimDepth), true, "Log");
	for (int y = 0; (y < 20) && (aIdleLoopCount < 1); y++)
	{
		printf("\nRunning with %i Brokers\n", aBrokerCount);
		aIdleLoopCount++;
		int aRange = 200;
		for (int x = 0; (x < aMaxTarget); x += aBrokerCount)
		{
			aRange = floor(200 * ((aMaxTarget - x) / aMaxTarget));
			clock_t aStartLoop = clock();
			Broker* aBrokerList = new Broker[aBrokerCount];
			ReworkBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, m_MarketCount, aRange);
			m_Loader.RunBrokers(aBrokerList, aBrokerCount);
			aTotalSimCount += aBrokerCount;
			Broker aTempBroker;// = m_BestBroker;

			aTempBroker = CalcDeviations(aBrokerList, aBrokerCount);

			//x = (x / 2);
			//x = x-(x % 50000);
			if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
			{
				aBestofTheBest.insert(aBestofTheBest.begin(), aTempBroker);
				aTempBroker = CalcDeviations(aBestofTheBest.data(), aBestofTheBest.size());

				if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
				{
					m_BestBroker = aTempBroker;
					LogBroker(m_BestBroker, " ", true);
					aIdleLoopCount = 0;

					x = (x * 0.5);
					x = x - (x % 50000);
				}
				else
				{
					aBestofTheBest.erase(aBestofTheBest.begin());
				}

			}
			else if (aBrokerCount == 1)
			{
				m_BestBroker = aTempBroker;
			}

			if ((x % 50000) == 0)
			{
				printf("%iK", x / 1000);
			}
			else
			{
				printf(".");
			}
			
			clock_t aEndLoop = clock() - aStartLoop;
			delete aBrokerList;
		}


		if (tBrokerCount != 1)
		{
			aRange = 500;
			printf("\nRefining Results");
			int aSettingNum = 0;
			bool aLooping = true;
			while (aLooping)
			{
				aLooping = false;
				for (int x = 0; (x < m_BestBroker.m_SettingsCount); x++)
				{
					clock_t aStartLoop = clock();
					Broker* aBrokerList = new Broker[aBrokerCount];
					Broker aBestofTheBestofTheBest = m_BestBroker;
					RefineBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, m_MarketCount, aRange, x);
					m_Loader.RunBrokers(aBrokerList, aBrokerCount);
					aTotalSimCount += aBrokerCount;

					bool aNewBest = false;
					Broker aTempBroker = CalcDeviations(aBrokerList, aBrokerCount);
					if (aTempBroker.m_BrokerGuid != m_BestBroker.m_BrokerGuid)// aBestofTheBestofTheBest.m_BrokerNum)
					{
						aBestofTheBest.insert(aBestofTheBest.begin(), aTempBroker);
						Broker aTempBroker = CalcDeviations(aBestofTheBest.data(), aBestofTheBest.size());

						if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
						{
							m_BestBroker = aTempBroker;
							aNewBest = true;
							LogBroker(m_BestBroker, "", true);
							aIdleLoopCount = 0;
							aLooping = true;
						}
					}

					if (!aNewBest)
					{
						if ((x % 10) == 0)
						{
							printf("%i", x);
						}
						else
						{
							printf(".");
						}
					}
					clock_t aEndLoop = clock() - aStartLoop;
					delete aBrokerList;
					aSettingNum++;
				}
			}
		}

		LogBroker(m_BestBroker, "BEST OF RUN:" + to_string(y), false);
	}
	 
	printf("\nCOMPLETED AFTER %i SIMULATIONS", aTotalSimCount);
	string aOutput = LogBroker(m_BestBroker, " BEST OF THE BEST", true);
	Log(aOutput, true, "Best");

	ptree aBrokerXML;

	ptree aSettingsNode;
	aSettingsNode = BrokerSettingsToNode(m_BestBroker);
	//aSimBroker.m_NetWorth = 0;
	Broker aTempBroker;// = m_BestBroker;
	//THIS NETWORTH IS SAVED FROM THE SIM
	double aProfit = m_BestBroker.m_NetWorth - (m_BestBroker.m_BudgetPerMarket*m_MarketCount);
	ptree aBrokerDetails;
	aBrokerDetails.add("AlgorithmID", m_BestBroker.m_AlgorithmID);
	aBrokerDetails.add("BrokerGUID", m_BestBroker.m_BrokerGuid);
	aBrokerDetails.add("TotalProfit", aProfit);
	aBrokerDetails.add("ShareCount", m_BestBroker.m_TotalShareCount);
	aBrokerDetails.add("ProfitPerShare", aProfit / m_BestBroker.m_TotalShareCount);

	aBrokerDetails.add_child("Settings", aSettingsNode);
	aBrokerXML.add_child("Broker.Details", aBrokerDetails);

	for (int x=0; x<m_MarketCount; x++)
	{
		aTempBroker = m_BestBroker;
		//aTempBroker.m_TotalShareCount = 0;
		MarketPrice* aMarketPriceDifference = new MarketPrice[m_MarketList[x].m_MarketPriceCount];
		m_Loader.CalcMarketDifferences(m_MarketList[x].PriceList, aMarketPriceDifference, &m_MarketList[x].m_MarketPriceCount);
		ptree aMarketDisplay = m_Loader.DisplayBestBroker(&aTempBroker, m_MarketList[x], aMarketPriceDifference);
		aBrokerXML.add_child("Broker.MarketResult", aMarketDisplay);/*
		aProfit += (aTempBroker.m_NetWorth - (aTempBroker.m_BudgetPerMarket));
		aTotalShareCount += aTempBroker.m_TotalShareCount;*/
		delete aMarketPriceDifference;
	}
	
	string aSaveFile = to_string(m_BestBroker.m_BrokerGuid) + "__" + to_string(tSimDepth) + ".xml";
	if (tBrokerCount == 1)
	{
		write_xml("Broker\\Saved\\" + aSaveFile, aBrokerXML);
	}
	else
	{
		write_xml("Broker\\" + aSaveFile, aBrokerXML);
	}
	//delete aMarketList;
	//system("PAUSE");

	aBestofTheBest.clear();

#pragma endregion
	 
}



void UpdateBrokers(vector<std::string> tBrokerFileList, int tConcurentBrokers)
{

}



void SimControl::ReworkBrokerList(Broker* tBrokerList, Broker *tBestBroker, int tBrokerCount, int tMarketCount, double tRange)
{
	tBrokerList[0] = *tBestBroker;
	int aModRange = 10000;
	double aMaxRange = 500;
	for (int x = 0; x < tBrokerCount; x++)
	{
		tBrokerList[x].m_MarketCount = tMarketCount;
		tBrokerList[x].m_Budget = 0;
		tBrokerList[x].m_BudgetPerMarket = 1000000;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_NetWorth = 0;
		tBrokerList[x].m_BrokerScore = 0;
		tBrokerList[x].m_SettingsCount = tBrokerList[0].m_SettingsCount;
		tBrokerList[x].m_TotalShareCount = 0;
		tBrokerList[x].m_TotalProfit = 0;
		tBrokerList[x].m_ShareCount = 0;
		if (x > 0)
		{
			tBrokerList[x].m_BrokerGuid = rand()*rand();
			for (int y = 0; y < tBrokerList[x].m_SettingsCount; y++)
			{
				double aModSized = (double)((rand() % aModRange) / (double)aModRange);
				double aDSetting = ((aModSized - 0.5)* (tRange * 2));
				double aSetting = tBestBroker->m_Settings[y] + aDSetting;
				aSetting = (aSetting > aMaxRange) ? aMaxRange : aSetting;
				aSetting = (aSetting < -aMaxRange) ? -aMaxRange : aSetting;

				tBrokerList[x].m_Settings[y] = aSetting;
			}
		}
	}
}

void SimControl::RefineBrokerList(Broker* tBrokerList, Broker *tBestBroker, int tBrokerCount, int tMarketCount, double tRange, int tSettingNum)
{
	tBrokerList[0] = *tBestBroker;
	double aMaxRange = 500;
	for (int x = 0; x < tBrokerCount; x++)
	{
		tBrokerList[x].m_MarketCount = tMarketCount;
		tBrokerList[x].m_Budget = 0;
		tBrokerList[x].m_BudgetPerMarket = 1000000;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_NetWorth = 0;
		tBrokerList[x].m_BrokerScore = 0;
		tBrokerList[x].m_SettingsCount = tBrokerList[0].m_SettingsCount;
		tBrokerList[x].m_TotalShareCount = 0;
		tBrokerList[x].m_TotalProfit = 0;
		tBrokerList[x].m_ShareCount = 0;
		if (x > 0)
		{
			tBrokerList[x].m_BrokerGuid = rand()*rand();
			for (int y = 0; y < tBrokerList[x].m_SettingsCount; y++)
			{
				if (y == tSettingNum)
				{
					double aDSetting = (((x / (double)tBrokerCount) - 0.5)* (tRange * 2));
					double aSetting = tBestBroker->m_Settings[y] + aDSetting;
					aSetting = (aSetting > aMaxRange) ? aMaxRange : aSetting;
					aSetting = (aSetting < -aMaxRange) ? -aMaxRange : aSetting;

					tBrokerList[x].m_Settings[tSettingNum] = aSetting;
				}
				else
				{
					tBrokerList[x].m_Settings[y] = tBestBroker->m_Settings[y];
				}
			}
		}
	}
}

Broker SimControl::CalcDeviations(vector<Broker> tBrokerList, int tBrokerCount)
{
	Broker* aBrokerList = new Broker[tBrokerCount];
	for (int x = 0; x < tBrokerCount; x++)
	{
		aBrokerList[x] = tBrokerList[x];
	}
	Broker aBest = CalcDeviations(aBrokerList, tBrokerCount);
	delete aBrokerList;
	return aBest;
}

Broker SimControl::CalcDeviations(Broker* tBrokerList, int tBrokerCount)
{
	double aProfitPerShareMean = 0;
	double aTotalProfithMean = 0;
	for (int x = 0; x < tBrokerCount; x++)
	{
		Broker aTestBroker = tBrokerList[x];
		tBrokerList[x].m_BrokerScore = -50;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_TotalProfit = 0;
		if (tBrokerList[x].m_TotalShareCount > 0)
		{
			tBrokerList[x].m_TotalProfit = tBrokerList[x].m_NetWorth - (tBrokerList[x].m_BudgetPerMarket * tBrokerList[x].m_MarketCount);
			tBrokerList[x].m_ProfitPerShare = tBrokerList[x].m_TotalProfit / tBrokerList[x].m_TotalShareCount;

			aTotalProfithMean += tBrokerList[x].m_TotalProfit;
			aProfitPerShareMean += tBrokerList[x].m_ProfitPerShare;
		}
	}
	aTotalProfithMean = aTotalProfithMean / tBrokerCount;
	aProfitPerShareMean = aProfitPerShareMean / tBrokerCount;

	double aProfitPerShareDev = 0;
	double atotalProfitDev = 0;

	for (int x = 0; x < tBrokerCount; x++)
	{
		aProfitPerShareDev += abs(aProfitPerShareMean - tBrokerList[x].m_ProfitPerShare);
		atotalProfitDev += abs(aTotalProfithMean - tBrokerList[x].m_TotalProfit);
	}
	aProfitPerShareDev = aProfitPerShareDev / tBrokerCount;
	atotalProfitDev = atotalProfitDev / tBrokerCount;

	Broker aBestBroker = tBrokerList[0];
	for (int x = 0; x < tBrokerCount; x++)
	{
		if (tBrokerList[x].m_TotalProfit != 0)
		{
			double aScore = ((tBrokerList[x].m_TotalProfit - aTotalProfithMean) / atotalProfitDev);
			aScore += 3*((tBrokerList[x].m_ProfitPerShare - aProfitPerShareMean) / aProfitPerShareDev);
			int aUsableMarketCount = m_TotalPriceCount - (ONEYEAR*m_MarketCount);
			if ((tBrokerList[x].m_TotalShareCount > aUsableMarketCount / 20) && (tBrokerList[x].m_TotalShareCount < aUsableMarketCount / 2))
			{
				tBrokerList[x].m_BrokerScore = aScore;
			}
			else
			{
				tBrokerList[x].m_BrokerScore = aScore;
			}
		}
		

		if (aBestBroker.m_BrokerScore < tBrokerList[x].m_BrokerScore)
		{
			aBestBroker = tBrokerList[x];
		}
	}
	return aBestBroker;
}

double SimControl::GetTime()
{
	double sysTime = time(0);
	return sysTime * 1000;
}

std::string SimControl::LogBroker(Broker tBroker, std::string tMessage, bool tWrite)
{
	string aBrokerLog;
	aBrokerLog += "$pS" + to_string(tBroker.m_ProfitPerShare) + ",\t$" + to_string(tBroker.m_TotalProfit) + ",\tB" + to_string(tBroker.m_Settings[0]* tBroker.m_Settings[1])
		+ ",\tS" + to_string(tBroker.m_Settings[2]* tBroker.m_Settings[3]) + ",\tSC" + to_string(tBroker.m_TotalShareCount)+ ",\tNum" + to_string(tBroker.m_BrokerNum)
		+",\tGUID" + to_string(tBroker.m_BrokerGuid) + "\t\t";
	
	printf(("\n"+aBrokerLog + tMessage + "\n").c_str());
	if (tWrite)
	{
		for (int x = 0; x < tBroker.m_SettingsCount; x++)
		{
			aBrokerLog += ",\t" + to_string(tBroker.m_Settings[x]);
		}
		Log(aBrokerLog, false, "Log");
	}
	return aBrokerLog;
}
void SimControl::Log(string tLog, bool tConsole, string tFileName)
{
	if (tConsole)
	{
		printf((tLog + "\n").c_str());
	}
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	string aDate = to_string(now->tm_year + 1900) + "/" + to_string(now->tm_mon + 1) + '/' + to_string(now->tm_mday) + '_' + to_string(now->tm_hour) + ':' + to_string(now->tm_min) + ':' + to_string(now->tm_sec);

	ofstream myfile;
	myfile.open("Markets\\"+tFileName+".txt", std::fstream::app);
	myfile << aDate + "||\t" + tLog << endl;
	myfile.close();
}

ptree SimControl::BrokerSettingsToNode(Broker tBroker)
{
	ptree aRet;
	for (int x = 0; x < tBroker.m_SettingsCount; x++)
	{
		if (tBroker.m_Settings[x] == NULL)
		{
			aRet.add("Setting" + to_string(x),"0");
		}
		else
		{
			aRet.add("Setting" + to_string(x), to_string(tBroker.m_Settings[x]));
		}
	}
	return aRet;
}