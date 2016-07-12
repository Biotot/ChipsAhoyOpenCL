#include "SimControl.h"
using std::vector;
int main()
{
	ChipsDB aDB;

	SimControl aHost;
	
	int aBrokerCount[] = {100000,50000,25000,10000,5000,1000};
	int aBrokerSelection = 0;
	cout << "Please enter a desired BrokerCount: "<<endl;
	cout << "0 : 100000" << endl << "1 : 50000" << endl << "2 : 25000" << endl;
	cout << "3 : 10000" << endl << "4 : 5000" << endl << "5 : 1000" << endl;
	cin >> aBrokerSelection;

	srand(time(NULL));
	while (true)
	{
		//for (int x = 5; x >0; x--)
		for (int x = 1; x <5; x ++)
		{
			aHost.Run(x, aBrokerCount[aBrokerSelection]);
		}
	}
	
	return 0;
}

SimControl::SimControl()
{
	m_BestBroker.m_Budget = 0;
	m_BestBroker.m_BudgetPerMarket = 10000;
	m_BestBroker.m_ProfitPerShare = 0;
	m_BestBroker.m_NetWorth = 0;
	m_BestBroker.m_BrokerScore = 0;
	m_BestBroker.m_SettingsCount = 50;
	m_BestBroker.m_TotalShareCount = 0;
	m_BestBroker.m_BrokerGuid = rand();
	for (int y = 0; y < m_BestBroker.m_SettingsCount; y++)
	{
		m_BestBroker.m_Settings[y] = 0;
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
	//vector<string> aMarketStringList = { "GE" }; 

	int aMarketCount = aMarketStringList.size();
	Market* aMarketList = new Market[aMarketCount];
	for (int x = 0; x < aMarketCount; x++)
	{
		MarketLoader aLoader(aMarketStringList[x], 1980, (tSimDepth*1000)+ONEYEAR);
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
#pragma region MAINLOOP
	Log("~~~~~~~~~STARTING NEW RUN~~~~~~~~~~~~~~~" + to_string(tSimDepth), true, "Log");
	int aIdleLoopCount = 0;
	int aTotalSimCount = 0;
	for (int y = 0; (y < 20) && (aIdleLoopCount < 3); y++)
	{
		printf("\nRunning with %i Brokers\n", aBrokerCount);
		aIdleLoopCount++;
		int aRange = 200;
		double aMillion = 1000000;
		for (int x = 0; (x < aMillion); x+=aBrokerCount)
		{
			aRange = floor(200 * ((aMillion - x)/aMillion));
			clock_t aStartLoop = clock();
			Broker* aBrokerList = new Broker[aBrokerCount];
			ReworkBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, aMarketCount, aRange);

			m_Loader.TestRun(aBrokerList, aBrokerCount, aMarketList, aMarketCount);
			aTotalSimCount += aBrokerCount;
			Broker aTempBroker = m_BestBroker;

			m_BestBroker = CalcDeviations(aBrokerList, aBrokerCount);


			if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
			{
				LogBroker(m_BestBroker, " ", true);
				aIdleLoopCount = 0;
				x = 0;
			}
			else
			{
				if ((x % 50000) == 0)
				{
					printf("%iK", x/1000);
				}
				else
				{
					printf(".");
				}
			}
			clock_t aEndLoop = clock() - aStartLoop;
			delete aBrokerList;
		}

		aRange = 300;
		bool aRefining = true;
		for (int aLoop = 0; aRefining && (aLoop < 5); aLoop++)
		{
			printf("\nRefining Results");
			aRefining = false;
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
					aIdleLoopCount = 0;
					x = 0;
					aRefining = true;
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
	for (int x=0; x<aMarketCount; x++)
	{
		m_Loader.DisplayBestBroker(m_BestBroker, aMarketList[x]);
	}

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
			tBrokerList[x].m_BrokerGuid = rand();
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
			tBrokerList[x].m_BrokerGuid = rand();
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