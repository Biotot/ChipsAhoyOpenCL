#include "SimControl.h"
using std::vector;
using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;

void UpdateBrokers()
{
	BrokerLoader aLoader; 

	std::vector<Broker> aSavedBrokerList = aLoader.LoadSavedBrokers("Broker\\Saved\\StoredBrokers.xml");

	SimControl aHost;
	for (int x = 0; x < aSavedBrokerList.size(); x++)
	{
		int aSimDepth = 1;
		aHost.m_BestBroker = aSavedBrokerList[x];
		aHost.Run(aSimDepth, 1);
	}

}

void BruteBrokers()
{
	ChipsDB aDB;
	BrokerLoader aLoader;

	SimControl aHost;
	int aBrokerCount[] = { 50000,25000,10000,5000,1000,500 };
	int aBrokerSelection = 0;
	cout << "Please enter a desired BrokerCount: " << endl;
	cout << "0 : 50000" << endl << "1 : 25000" << endl << "2 : 10000" << endl;
	cout << "3 : 5000" << endl << "4 : 1000" << endl << "5 : 500" << endl;
	cin >> aBrokerSelection;

	


	srand(time(NULL));
	while (true)
	{
		//for (int x = 5; x >1; x--)
		for (int x = 2; x <5; x++)
		{
			if (aBrokerSelection > 5)
			{
				aHost.Run(x, aBrokerSelection);
			}
			else
			{
				aHost.Run(x, aBrokerCount[aBrokerSelection]);
			}
		}
	}
}

int main()
{

	UpdateBrokers();
	//BruteBrokers();


	
	return 0;
}

SimControl::SimControl()
{
	double aBestList[] = { -22.688600,	-74.854800,	-90.234800,	336.930200,	-439.737200,	-499.700000,	145.229600,	-490.503200,	54.278400,	351.530600,	-253.556400,	316.203200,	100.638400,	267.888800,	-271.065000,	11.467800,	-0.816200,	448.896400,	-208.513800,	-19.728600,	-251.636200,	-117.589000,	-190.517600,	120.604200,	-174.972400,	385.215600,	-375.954400,	168.776400,	-495.727200,	494.867200,	-210.970200,	-218.082000,	475.991200,	-198.840600,	-457.002000,	-473.665600,	-498.118400,	234.666600,	51.428400,	-320.715000,	182.062600,	65.921400,	-499.764400,	-500.000000,	-303.226800,	-2.121800,	-478.662400,	-355.311200,	-494.456800,	218.082000 };
	bool aSeededSetting = false;
	
	m_BestBroker.m_Budget = 0;
	m_BestBroker.m_BudgetPerMarket = 10000;
	m_BestBroker.m_ProfitPerShare = 0;
	m_BestBroker.m_NetWorth = 0;
	m_BestBroker.m_BrokerScore = 0;
	m_BestBroker.m_SettingsCount = 50;
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
	//OpenCLLoad m_Loader;
}


SimControl::~SimControl()
{
}

void SimControl::Run(int tSimDepth, int tBrokerCount)
{

	int aBrokerCount = tBrokerCount;
	m_TotalPriceCount = 0;
	vector<string> aMarketStringList = {"GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "WFC", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA", "NFLX", "TWTR", "MSFT", "GPRO", "BABA", "FIT"};

	//vector<string> aMarketStringList = { "GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "WFC", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA"};
	//vector<string> aMarketStringList = { "GE" }; 

	int aMarketCount = aMarketStringList.size();
	Market* aMarketList = new Market[aMarketCount];
	for (int x = 0; x < aMarketCount; x++)
	{
		MarketLoader aLoader(aMarketStringList[x], 1980, (tSimDepth*3*ONEYEAR)+ONEYEAR);
		aMarketList[x] = aLoader.m_Market;
		m_TotalPriceCount += aLoader.m_Market.m_MarketPriceCount;
	}
	cout << "Running with: " << m_TotalPriceCount << " price points" << endl;

	m_BestBroker.m_MarketCount = aMarketCount;

	//aOpenCLCaller.DisplayBestBroker(m_BestBroker, aMarketList[0]);
	//vector<Broker> aBestofTheBest;
	//aBestofTheBest.push_back(m_BestBroker);
	std::cout << fixed;

	//int aMilliseconds = time(NULL);
	int aIdleLoopCount = 0;
	int aTotalSimCount = 0;
	double aMillion = 1000000;
	if (tBrokerCount == 1)
	{
		aMillion = 1;
	}
#pragma region MAINLOOP
		Log("~~~~~~~~~STARTING NEW RUN~~~~~~~~~~~~~~~" + to_string(tSimDepth), true, "Log");
		for (int y = 0; (y < 20) && (aIdleLoopCount < 1); y++)
		{
			printf("\nRunning with %i Brokers\n", aBrokerCount);
			aIdleLoopCount++;
			int aRange = 200;
			for (int x = 0; (x < aMillion); x += aBrokerCount)
			{
				aRange = floor(200 * ((aMillion - x) / aMillion));
				clock_t aStartLoop = clock();
				Broker* aBrokerList = new Broker[aBrokerCount];
				ReworkBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, aMarketCount, aRange);

				m_Loader.TestRun(aBrokerList, aBrokerCount, aMarketList, aMarketCount);
				aTotalSimCount += aBrokerCount;
				Broker aTempBroker = m_BestBroker;

				m_BestBroker = CalcDeviations(aBrokerList, aBrokerCount);

				//x = (x / 2);
				//x = x-(x % 50000);
				if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
				{
					LogBroker(m_BestBroker, " ", true);
					aIdleLoopCount = 0;

					x = (x / 2);
					x = x - (x % 50000);
				}
				else
				{
					if ((x % 50000) == 0)
					{
						printf("%iK", x / 1000);
					}
					else
					{
						printf(".");
					}
				}
				clock_t aEndLoop = clock() - aStartLoop;
				delete aBrokerList;
			}


			if (tBrokerCount != 1)
			{
				aRange = 500;
				printf("\nRefining Results");
				for (int x = 0; (x < m_BestBroker.m_SettingsCount); x++)
				{
					clock_t aStartLoop = clock();
					Broker* aBrokerList = new Broker[aBrokerCount];
					Broker aBestofTheBestofTheBest = m_BestBroker;
					RefineBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, aMarketCount, aRange, x);
					m_Loader.TestRun(aBrokerList, aBrokerCount, aMarketList, aMarketCount);
					aTotalSimCount += aBrokerCount;

					Broker aTempBroker = m_BestBroker;
					m_BestBroker = CalcDeviations(aBrokerList, aBrokerCount);
					if (aTempBroker.m_BrokerGuid != m_BestBroker.m_BrokerGuid)// aBestofTheBestofTheBest.m_BrokerNum)
					{
						LogBroker(m_BestBroker, "", true);
						//aIdleLoopCount = 0;
						x = 0;
					}
					else
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
	Broker aTempBroker = m_BestBroker;
	double aProfit = m_BestBroker.m_NetWorth - (m_BestBroker.m_BudgetPerMarket*aMarketCount);
	ptree aBrokerDetails;
	aBrokerDetails.add("AlgorithmID", m_BestBroker.m_AlgorithmID);
	aBrokerDetails.add("BrokerGUID", m_BestBroker.m_BrokerGuid);
	aBrokerDetails.add("TotalProfit", aProfit);
	aBrokerDetails.add("ShareCount", m_BestBroker.m_TotalShareCount);
	aBrokerDetails.add("ProfitPerShare", aProfit / m_BestBroker.m_TotalShareCount);

	aBrokerDetails.add_child("Settings", aSettingsNode);
	aBrokerXML.add_child("Broker.Details", aBrokerDetails);

	for (int x=0; x<aMarketCount; x++)
	{
		aTempBroker = m_BestBroker;
		ptree aMarketDisplay = m_Loader.DisplayBestBroker(&aTempBroker, aMarketList[x]);
		aBrokerXML.add_child("Broker.MarketResult", aMarketDisplay);/*
		aProfit += (aTempBroker.m_NetWorth - (aTempBroker.m_BudgetPerMarket));
		aTotalShareCount += aTempBroker.m_TotalShareCount;*/
	}
	
	string aSaveFile = to_string(m_BestBroker.m_BrokerGuid) + "__" + to_string(tSimDepth) + ".xml";
	write_xml("Broker\\" + aSaveFile, aBrokerXML);
	delete aMarketList;
	//system("PAUSE");

#pragma endregion

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
		tBrokerList[x].m_BudgetPerMarket = 10000;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_NetWorth = 0;
		tBrokerList[x].m_BrokerScore = 0;
		tBrokerList[x].m_SettingsCount = tBrokerList[0].m_SettingsCount;
		tBrokerList[x].m_TotalShareCount = 0;
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
		tBrokerList[x].m_BudgetPerMarket = 10000;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_NetWorth = 0;
		tBrokerList[x].m_BrokerScore = 0;
		tBrokerList[x].m_SettingsCount = tBrokerList[0].m_SettingsCount;
		tBrokerList[x].m_TotalShareCount = 0;
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
	return CalcDeviations(aBrokerList, tBrokerCount);
}

Broker SimControl::CalcDeviations(Broker* tBrokerList, int tBrokerCount)
{
	double aProfitPerShareMean = 0;
	double aTotalProfithMean = 0;
	for (int x = 0; x < tBrokerCount; x++)
	{
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
			int aUsableMarketCount = tBrokerList[x].m_TotalShareCount - ONEYEAR;
			if ((aUsableMarketCount > m_TotalPriceCount / 30) && (aUsableMarketCount < m_TotalPriceCount / 7))
			{
				tBrokerList[x].m_BrokerScore = aScore;
			}
			else
			{
				//tBrokerList[x].m_BrokerScore = aScore;
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
		aRet.add("Setting" + to_string(x), to_string(tBroker.m_Settings[x]));
	}
	return aRet;
}