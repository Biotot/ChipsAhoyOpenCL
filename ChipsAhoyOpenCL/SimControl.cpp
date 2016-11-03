#include "SimControl.h"
#include "dirent.h"
//#include <vld.h> 

using std::vector;
using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;


void UpdateBrokers(int tPlatform, int tDevice)
{
	SimControl aHost;
	int aSimDepth = 1;
	aHost.SetMarketList(aSimDepth, tPlatform, tDevice, 1);


	vector<std::string> aBrokerFileList;
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
				aBrokerFileList.push_back(aFileName);
				aFileCount++;
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
	}


	aHost.UpdateBrokers(aBrokerFileList, 10);
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
		//for (int x = 10; x>2; x--)
		for (int x = 3; x <10; x++)
		{
			if (tBrokerCount > 5)
			{
				aHost.SetDefaultBroker();
				aHost.SetMarketList(x, tPlatform, tDevice, tBrokerCount);
				aHost.RunBrute(x, tBrokerCount);
			}
			else
			{
				aHost.SetDefaultBroker();
				aHost.SetMarketList(x, tPlatform, tDevice, aBrokerCount[tBrokerCount]);
				aHost.RunBrute(x, aBrokerCount[tBrokerCount]);
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
	//double aBestList[] = { -22.688600,	-74.854800,	-90.234800,	336.930200,	-439.737200,	-499.700000,	145.229600,	-490.503200,	54.278400,	351.530600,	-253.556400,	316.203200,	100.638400,	267.888800,	-271.065000,	11.467800,	-0.816200,	448.896400,	-208.513800,	-19.728600,	-251.636200,	-117.589000,	-190.517600,	120.604200,	-174.972400,	385.215600,	-375.954400,	168.776400,	-495.727200,	494.867200,	-210.970200,	-218.082000,	475.991200,	-198.840600,	-457.002000,	-473.665600,	-498.118400,	234.666600,	51.428400,	-320.715000,	182.062600,	65.921400,	-499.764400,	-500.000000,	-303.226800,	-2.121800,	-478.662400,	-355.311200,	-494.456800,	218.082000 };
	bool aSeededSetting = false;

	m_BestBroker = BrokerLoader::CreateDefaultBroker();
	m_BestBroker.m_BrokerGuid = rand()*rand();
	for (int y = 0; y < m_BestBroker.m_SettingsCount; y++)
	{
		if (aSeededSetting)
		{
			//m_BestBroker.m_Settings[y] = aBestList[y];
		}
		else
		{
			m_BestBroker.m_Settings[y] = 0;
		}
	}
}

void SimControl::SetMarketList(int tSimDepth, int tPlatform, int tDevice, int tBrokerCount)
{
	vector<string> aMarketStringList;

	aMarketStringList = { "GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA", "NFLX", "TWTR", "MSFT", "GPRO", "BABA", "ZNGA", "VZ", "MYL", "CXW" , "^IXIC", "^GSPC", "^DJI",
	"PG", "CHK", "DB" , "COTY" , "WFC" , "CTSH" , "SIRI" , "QCOM" , "WLL" , "CSCO" , "FCX" , "KR" , "DO" , "C" , "QQQ" , "NXPI" , "ECA" , "WFT" , "AA" , "NVFY" , "XNY" , "CNET" , "PYDS" , "PTCT" , "ITUS" , "MXC" ,
		"LEJU" , "JTPY" , "TWER" , "AUPH" , "LXK" , "EBS", "DRNA", "WAC", "GBT", "EVI", "COOL", "BVX", "COLL", "IDRA", "PGNX", "SGM", "SPY", "EEM", "EWJ", "XLF", "GDX", "UVXY", "USO", "VXX", "IWM", "EFA",  "NUGT",
	"UWTI", "EWZ", "XIV", "XOP", "XLP", "DGAZ", "JNUG", "GDXJ", "XLU", "XLE", "HYG", "TLT", "SLV", "XLI", "JNK", "TVIX", "IYR", "RSX", "XLK"};

	//vector<string> aMarketStringList = { "GE", "F", "AMD", "INTC", "BAC","AMZN","GOOGL", "WFC", "AAPL", "YHOO", "CMCSA", "TWX", "TSLA"};
	//aMarketStringList = { "GE" }; 
	for (int x = 0; x < aMarketStringList.size(); x++)
	{
		for (int y = 0; y < x; y++)
		{
			if (aMarketStringList[x] == aMarketStringList[y])
			{
				cout << aMarketStringList[x] << " DUPLICATED" << endl;
				/*auto it = std::find(aMarketStringList.begin(), aMarketStringList.end(), y);
				if (it != aMarketStringList.end())
				{
					aMarketStringList.erase(it);
					x--;
				}*/
			}

		}
	}


	
	m_MarketCount = aMarketStringList.size();
	m_MarketList = new Market[m_MarketCount];
	m_TotalPriceCount = 0;
	for (int x = 0; x < m_MarketCount; x++)
	{
		MarketLoader aLoader(aMarketStringList[x], 1980, (tSimDepth * ONEYEAR) + ONEYEAR);
		m_MarketList[x] = aLoader.m_Market;
		m_TotalPriceCount += aLoader.m_Market.m_MarketPriceCount;
	}

	m_Loader.Setup(tPlatform, tDevice);
	m_Loader.LoadMarkets(m_MarketList, m_MarketCount, tBrokerCount);
}

void SimControl::RunBrute(int tSimDepth, int tBrokerCount)
{

	int aBrokerCount = tBrokerCount;

	cout << "Running with: " << m_TotalPriceCount << " price points" << endl;

	m_BestBroker.m_MarketCount = m_MarketCount;
	std::cout << fixed;


	int aAlphaCount = 100;
	Broker* aSeedHerd = new Broker[aAlphaCount];
	ReworkBrokerList(aSeedHerd, &m_BestBroker, aAlphaCount, m_MarketCount, 500);

	vector<Broker> aBrokerHerd;
	aBrokerHerd.insert(aBrokerHerd.begin(), aSeedHerd, aSeedHerd + aAlphaCount);
	double aMaxTarget = 10000000;

	int aRange = 500;
	int aInnnerSimCount = 55;
	for (int x = 0; (x < aMaxTarget);)
	{
		aRange = floor(500 * ((aMaxTarget - x) / aMaxTarget));
		Broker aTempBroker = m_BestBroker;
		aBrokerHerd = NaturalSelection(aBrokerHerd, tBrokerCount, aRange, "RAND", aInnnerSimCount);
		m_BestBroker = aBrokerHerd[0];
		x += aBrokerCount*aInnnerSimCount;
		if ((m_BestBroker.m_TotalProfit != aTempBroker.m_TotalProfit) || (m_BestBroker.m_ProfitPerShare != aTempBroker.m_ProfitPerShare) || (m_BestBroker.m_PercentReturn != aTempBroker.m_PercentReturn))
		{
			aMaxTarget += aBrokerCount*aInnnerSimCount;//Skip a sim chunk
		}

		LogBroker(m_BestBroker, " ", true);
		aBrokerHerd = NaturalSelection(aBrokerHerd, tBrokerCount, aRange, "TARG", aInnnerSimCount);
		m_BestBroker = aBrokerHerd[0];
		x += aBrokerCount*aInnnerSimCount;

		if ((m_BestBroker.m_TotalProfit != aTempBroker.m_TotalProfit) || (m_BestBroker.m_ProfitPerShare != aTempBroker.m_ProfitPerShare) || (m_BestBroker.m_PercentReturn != aTempBroker.m_PercentReturn))
		{
			aMaxTarget += aBrokerCount*aInnnerSimCount;//Skip a sim chunk
		}
		LogBroker(m_BestBroker, " [" + to_string(x) + " |/| " + to_string(aMaxTarget) + "]", true);
	}
	

	delete aSeedHerd;
	printf("\nCOMPLETED AFTER SOME SIMULATIONS");
	string aOutput = LogBroker(m_BestBroker, " BEST OF THE BEST", true);
	Log(aOutput, true, "Best");


	PrintBroker("Broker\\", m_BestBroker, to_string(tSimDepth));



//
//#pragma region MAINLOOP
//	Log("~~~~~~~~~STARTING NEW RUN~~~~~~~~~~~~~~~" + to_string(tSimDepth), true, "Log");
//	for (int y = 0; (y < 20) && (aIdleLoopCount < 1); y++)
//	{
//		printf("\nRunning with %i Brokers\n", aBrokerCount);
//		aIdleLoopCount++;
//		int aRange = 200;
//		for (int x = 0; (x < aMaxTarget); x += aBrokerCount)
//		{
//			aRange = floor(200 * ((aMaxTarget - x) / aMaxTarget));
//			clock_t aStartLoop = clock();
//			Broker* aBrokerList = new Broker[aBrokerCount];
//			ReworkBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, m_MarketCount, aRange);
//			m_Loader.RunBrokers(aBrokerList, aBrokerCount);
//			aTotalSimCount += aBrokerCount;
//			Broker aTempBroker;// = m_BestBroker;
//
//			aTempBroker = CalcDeviations(aBrokerList, aBrokerCount);
//			if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
//			{
//				aBestofTheBest.insert(aBestofTheBest.begin(), aTempBroker);
//				aTempBroker = CalcDeviations(aBestofTheBest.data(), aBestofTheBest.size());
//				if (aBestofTheBest.size() > 50)
//				{
//					std::sort(aBestofTheBest.begin(), aBestofTheBest.end(), [](Broker const& a, Broker const& b) { return a.m_BrokerScore > b.m_BrokerScore; });
//					aBestofTheBest.pop_back();
//				}
//
//				if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
//				{
//					m_BestBroker = aTempBroker;
//					LogBroker(m_BestBroker, " ", true);
//					printf("%i: ", aBestofTheBest.size());
//
//					x = (x * 0.75);
//					aIdleLoopCount = 0;
//					x = x - (x % 50000);
//				}
//				else
//				{
//					aBestofTheBest.erase(aBestofTheBest.begin());
//				}
//			}
//			else if (aBrokerCount == 1)
//			{
//				m_BestBroker = aTempBroker;
//			}
//
//			if ((x % 50000) == 0)
//			{
//				printf("%iK", x / 1000);
//			}
//			else
//			{
//				printf(".");
//			}
//			
//			clock_t aEndLoop = clock() - aStartLoop;
//			delete aBrokerList;
//		}
//
//
//		if (tBrokerCount != 1)
//		{
//			aRange = 500;
//			printf("\nRefining Results");
//			int aSettingNum = 0;
//			int aLoopCount = 0;
//			bool aLooping = true;
//			for (aLoopCount = 0; aLooping&&(aLoopCount<5); aLoopCount++)
//			{
//				aLooping = false;
//				for (int x = 0; (x < m_BestBroker.m_SettingsCount); x++)
//				{
//					clock_t aStartLoop = clock();
//					Broker* aBrokerList = new Broker[aBrokerCount];
//					Broker aBestofTheBestofTheBest = m_BestBroker;
//					RefineBrokerList(aBrokerList, &m_BestBroker, aBrokerCount, m_MarketCount, aRange, x);
//					m_Loader.RunBrokers(aBrokerList, aBrokerCount);
//					aTotalSimCount += aBrokerCount;
//
//					bool aNewBest = false;
//					Broker aTempBroker;
//					aTempBroker = CalcDeviations(aBrokerList, aBrokerCount);
//					if (aTempBroker.m_BrokerGuid != m_BestBroker.m_BrokerGuid)// aBestofTheBestofTheBest.m_BrokerNum)
//					{
//						aBestofTheBest.insert(aBestofTheBest.begin(), aTempBroker);
//						aTempBroker = CalcDeviations(aBestofTheBest.data(), aBestofTheBest.size());
//						if (aBestofTheBest.size() > 50)
//						{
//							std::sort(aBestofTheBest.begin(), aBestofTheBest.end(), [](Broker const& a, Broker const& b) { return a.m_BrokerScore > b.m_BrokerScore; });
//							aBestofTheBest.pop_back();
//						}
//
//						if (m_BestBroker.m_BrokerGuid != aTempBroker.m_BrokerGuid)
//						{
//							m_BestBroker = aTempBroker;
//							aNewBest = true;
//							LogBroker(m_BestBroker, "", true);
//							printf("%i-%i-%i", x, aLoopCount, aBestofTheBest.size());
//							aIdleLoopCount = 0;
//							aLooping = true;
//
//
//						}
//					}
//					else
//					{
//						aBestofTheBest.erase(aBestofTheBest.begin());
//					}
//
//					//if (!aNewBest)
//					{
//						if ((x % 10) == 0)
//						{
//							printf(" [%i]", x);
//						}
//						else
//						{
//							printf(".");
//						}
//					}
//					clock_t aEndLoop = clock() - aStartLoop;
//					delete aBrokerList;
//					aSettingNum++;
//				}
//			}
//		}
//		aBestofTheBest.clear();
//		aBestofTheBest.insert(aBestofTheBest.begin(), m_BestBroker);
//		LogBroker(m_BestBroker, "BEST OF RUN:" + to_string(y), false);
//	}
//	 
#pragma endregion
	 
}

vector<Broker> SimControl::NaturalSelection(vector<Broker> &tBrokerHerd, int tBrokerCount, int tRange, string tMutateType, int tMaxLoopCount)
{
	if (tBrokerCount % tBrokerHerd.size() != 0)
	{
		cout << "Shit";
	}
	cout << tMutateType<< " at range : " << tRange << endl;
	int aChildCount = tBrokerCount / tBrokerHerd.size();
	int aTopCount = 10000;
	vector<Broker> aFullList = tBrokerHerd;
	for (int aGeneration = 0; aGeneration < tMaxLoopCount; aGeneration++)
	{
		vector<Broker> aBrokerList;
		for (int x = 0; x < tBrokerHerd.size(); x++)
		{
			Broker* aTempBrokerList = new Broker[aChildCount];

			if (tMutateType == "RAND")
			{
				ReworkBrokerList(aTempBrokerList, &(tBrokerHerd)[x], aChildCount, m_MarketCount, tRange);
			}
			else if (tMutateType == "TARG")
			{
				RefineBrokerList(aTempBrokerList, &(tBrokerHerd)[x], aChildCount, m_MarketCount, tRange, aGeneration);
			}
			aBrokerList.insert(aBrokerList.begin(), aTempBrokerList, aTempBrokerList + aChildCount);


			delete aTempBrokerList;
		}
		if (aBrokerList.size() != tBrokerCount)
		{
			cout << "fuck";
		}
		m_Loader.RunBrokers(aBrokerList.data(), tBrokerCount);
		//CalcDeviations(aBrokerList.data(), aBrokerList.size());
		//std::sort(aBrokerList.begin(), aBrokerList.end(), [](Broker const& a, Broker const& b) { return a.m_BrokerScore > b.m_BrokerScore; });

		aFullList.insert(aFullList.begin(), aBrokerList.begin(), aBrokerList.end());
		CalcDeviations(aFullList.data(), aFullList.size());
		std::sort(aFullList.begin(), aFullList.end(), [](Broker const& a, Broker const& b) { return a.m_BrokerScore > b.m_BrokerScore; });

		if (aFullList.size() > aTopCount)
		{
			aFullList.erase(aFullList.begin() + aTopCount, aFullList.end());
		}

		if (aGeneration % 10 == 0)
		{
			if ((m_BestBroker.m_TotalProfit != aFullList[0].m_TotalProfit) || (m_BestBroker.m_ProfitPerShare != aFullList[0].m_ProfitPerShare) || (m_BestBroker.m_PercentReturn != aFullList[0].m_PercentReturn))
			{
				m_BestBroker = aFullList[0];
				LogBroker(m_BestBroker, " @ " + to_string(m_BestBroker.m_BrokerScore), true);
			}
		}

		for (int x = 0; x < tBrokerHerd.size(); x++)
		{
			tBrokerHerd[x] = aFullList[x];
		}
		cout << "."; 
		
		//cout << "X" << endl;
	}
	return tBrokerHerd;
}

void SimControl::UpdateBrokers(vector<std::string> tBrokerFileList, int tConcurentBrokers)
{
	BrokerLoader aLoader;
	int aBrokerCount = tBrokerFileList.size();

	cout << "Running with: " << m_TotalPriceCount << " price points" << endl;

	m_BestBroker.m_MarketCount = m_MarketCount;
	std::cout << fixed;

	Broker* aBrokerList = new Broker[aBrokerCount];

	for (int x = 0; x < tBrokerFileList.size(); x++)
	{
		cout << tBrokerFileList[x] << endl;
		aBrokerList[x] = aLoader.ParseXML(tBrokerFileList[x]);
	}

	m_Loader.RunBrokers(aBrokerList, aBrokerCount);
	CalcDeviations(aBrokerList, aBrokerCount);

	for (int x = 0; x < tBrokerFileList.size(); x++)
	{
		cout << tBrokerFileList[x] << endl;
		if (aBrokerList[x].m_BrokerGuid != 0)
		{
			PrintBroker("Broker\\Saved\\", aBrokerList[x], "UPDATE");
		}
		else
		{
			cout << aBrokerList[x].m_BrokerGuid << " HAS A BAD FILE"<< endl;
		}
	}

	delete aBrokerList;
}


void SimControl::PrintBroker(std::string tFilePath, Broker tPrintBroker,string tSuffix)
{
	string aOutput = LogBroker(tPrintBroker, " BEST OF THE BEST", true);
	//Log(aOutput, true, "Best");

	ptree aBrokerXML;

	ptree aSettingsNode;
	aSettingsNode = BrokerSettingsToNode(tPrintBroker);

	double aProfit = tPrintBroker.m_NetWorth - (tPrintBroker.m_BudgetPerMarket*m_MarketCount);
	ptree aBrokerDetails;
	aBrokerDetails.add("AlgorithmID", tPrintBroker.m_AlgorithmID);
	aBrokerDetails.add("BrokerGUID", tPrintBroker.m_BrokerGuid);
	aBrokerDetails.add("TotalProfit", aProfit);
	aBrokerDetails.add("ShareCount", tPrintBroker.m_TotalShareCount);
	aBrokerDetails.add("ProfitPerShare", aProfit / tPrintBroker.m_TotalShareCount);
	aBrokerDetails.add("PercentReturn", tPrintBroker.m_PercentReturn);

	aBrokerDetails.add_child("Settings", aSettingsNode);
	aBrokerXML.add_child("Broker.Details", aBrokerDetails);

	for (int x = 0; x<m_MarketCount; x++)
	{
		//aTempBroker.m_TotalShareCount = 0;
		MarketPrice* aMarketPriceDifference = new MarketPrice[m_MarketList[x].m_MarketPriceCount];
		m_Loader.CalcMarketDifferences(m_MarketList[x].PriceList, aMarketPriceDifference, &m_MarketList[x].m_MarketPriceCount);
		tPrintBroker.m_TotalInvestment = 0;
		ptree aMarketDisplay = m_Loader.DisplayBestBroker(&tPrintBroker, m_MarketList[x], aMarketPriceDifference, true); //defaulting to true. Feature to return to later
		aBrokerXML.add_child("Broker.MarketResult", aMarketDisplay);/*
																	aProfit += (aTempBroker.m_NetWorth - (aTempBroker.m_BudgetPerMarket));
																	aTotalShareCount += aTempBroker.m_TotalShareCount;*/
		delete aMarketPriceDifference;
	}
	
	string aSaveFile = to_string(tPrintBroker.m_BrokerGuid) + "__" + tSuffix + ".xml";
	write_xml(tFilePath+aSaveFile, aBrokerXML);

	/*
	if (tBrokerCount == 1)
	{
		write_xml("Broker\\Saved\\" + aSaveFile, aBrokerXML);
	}
	else
	{
		write_xml("Broker\\" + aSaveFile, aBrokerXML);
	}*/
}

void SimControl::ReworkBrokerList(Broker* tBrokerList, Broker *tBestBroker, int tBrokerCount, int tMarketCount, double tRange)
{
	tBrokerList[0] = *tBestBroker;
	int aModRange = 10000;
	double aMaxRange = 500;
	for (int x = 0; x < tBrokerCount; x++)
	{

		tBrokerList[x] = BrokerLoader::CreateDefaultBroker();
		tBrokerList[x].m_MarketCount = tMarketCount;
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
		tBrokerList[x] = BrokerLoader::CreateDefaultBroker();
		tBrokerList[x].m_MarketCount = tMarketCount;
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
//
//Broker SimControl::CalcDeviations(vector<Broker> tBrokerList, int tBrokerCount)
//{
//	Broker* aBrokerList = new Broker[tBrokerCount];
//	for (int x = 0; x < tBrokerCount; x++)
//	{
//		aBrokerList[x] = tBrokerList[x];
//	}
//	Broker aBest = CalcDeviations(aBrokerList, tBrokerCount);
//	delete aBrokerList;
//	return aBest;
//}

Broker SimControl::CalcDeviations(Broker* tBrokerList, int tBrokerCount)
{
	double aProfitPerShareMean = 0;
	double aTotalProfithMean = 0;
	double aPercentReturnMean = 0;
	for (int x = 0; x < tBrokerCount; x++)
	{
		Broker aTestBroker = tBrokerList[x];
		tBrokerList[x].m_BrokerScore = -50;
		tBrokerList[x].m_ProfitPerShare = 0;
		tBrokerList[x].m_TotalProfit = 0;
		tBrokerList[x].m_PercentReturn = 0;
		if ((tBrokerList[x].m_TotalShareCount > m_TotalPriceCount*0.1)&&(tBrokerList[x].m_TotalShareCount < m_TotalPriceCount*0.9))
		{
			tBrokerList[x].m_TotalProfit = tBrokerList[x].m_NetWorth - (tBrokerList[x].m_BudgetPerMarket * tBrokerList[x].m_MarketCount);
			tBrokerList[x].m_ProfitPerShare = tBrokerList[x].m_TotalProfit / (tBrokerList[x].m_TotalShareCount+1);
			//tBrokerList[x].m_PercentReturn = tBrokerList[x].m_TotalProfit / abs((tBrokerList[x].m_TotalInvestment + 1));


			double aAverageInvestment = tBrokerList[x].m_TotalInvestment / m_TotalPriceCount;
			tBrokerList[x].m_PercentReturn = tBrokerList[x].m_TotalProfit / (aAverageInvestment * (m_TotalPriceCount / ONEYEAR));
			


			if (tBrokerList[x].m_PercentReturn > 10 || tBrokerList[x].m_PercentReturn < -10)
			{
				tBrokerList[x].m_PercentReturn = 0;
			}
			else if (tBrokerList[x].m_PercentReturn < 0.001 && tBrokerList[x].m_PercentReturn > -0.001)
			{
				tBrokerList[x].m_PercentReturn = 0;
			}

			aTotalProfithMean += tBrokerList[x].m_TotalProfit;
			aProfitPerShareMean += tBrokerList[x].m_ProfitPerShare;

			aPercentReturnMean += tBrokerList[x].m_PercentReturn;
		}
	}
	aTotalProfithMean = aTotalProfithMean / tBrokerCount;
	aProfitPerShareMean = aProfitPerShareMean / tBrokerCount;
	aPercentReturnMean = aPercentReturnMean / tBrokerCount;

	double aProfitPerShareDev = 0;
	double atotalProfitDev = 0;
	double aPercentReturnDev = 0;

	for (int x = 0; x < tBrokerCount; x++)
	{
		aProfitPerShareDev += abs(aProfitPerShareMean - tBrokerList[x].m_ProfitPerShare);
		atotalProfitDev += abs(aTotalProfithMean - tBrokerList[x].m_TotalProfit);
		aPercentReturnDev += abs(aPercentReturnMean - tBrokerList[x].m_PercentReturn);
		if (aPercentReturnDev > 10000000000)
		{
			x = x;
		}

		//aPercentReturnMean += tBrokerList[x].m_PercentReturn;
	}
	aProfitPerShareDev = aProfitPerShareDev / tBrokerCount;
	atotalProfitDev = atotalProfitDev / tBrokerCount;
	aPercentReturnDev = aPercentReturnDev / tBrokerCount;
	
	Broker aBestBroker = tBrokerList[0];
	for (int x = 0; x < tBrokerCount; x++)
	{
		if (tBrokerList[x].m_TotalProfit != 0)
		{
			double aScore = 0;
			aScore += ((tBrokerList[x].m_TotalProfit - aTotalProfithMean) / atotalProfitDev);
			aScore += ((tBrokerList[x].m_ProfitPerShare - aProfitPerShareMean) / aProfitPerShareDev);

			double aPercentRet = (tBrokerList[x].m_PercentReturn - aPercentReturnMean) / aPercentReturnDev;
			if (aPercentRet > 4)
			{
				aPercentRet = 4;
			}
			aScore += aPercentRet;

			tBrokerList[x].m_BrokerScore = aScore;
/*
			int aUsableMarketCount = m_TotalPriceCount - (ONEYEAR*m_MarketCount);
			if ((tBrokerList[x].m_TotalShareCount > aUsableMarketCount / 20) && (tBrokerList[x].m_TotalShareCount < aUsableMarketCount / 2))
			{
			}
			else
			{
				tBrokerList[x].m_BrokerScore = aScore;
			}*/
		}
		

		if ((aBestBroker.m_BrokerScore) < tBrokerList[x].m_BrokerScore)
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
	aBrokerLog += "$pS" + to_string(tBroker.m_ProfitPerShare) + ",\t$" + to_string(tBroker.m_TotalProfit) + ",\t%R" + to_string(tBroker.m_PercentReturn) + ",\tB" + to_string(tBroker.m_Settings[0] * tBroker.m_Settings[1])
		+ ",\tS" + to_string(tBroker.m_Settings[2] * tBroker.m_Settings[3]) + ",\tSC" + to_string(tBroker.m_TotalShareCount) + ",\tNum" + to_string(tBroker.m_BrokerNum) + ",\tScore" + to_string(tBroker.m_BrokerScore)
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