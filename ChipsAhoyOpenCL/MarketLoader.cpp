#include "MarketLoader.h"
//using namespace std;

MarketLoader::MarketLoader()
{
}

MarketLoader::MarketLoader(string tMarketName, int tYear, int tMarketNum)
{

	string aMarketCommand = tMarketName;
	int aIndex = tMarketName.find('^');
	if (aIndex != std::string::npos)
	{
		(aMarketCommand).insert(aIndex, "^");
	}


	//system("E:\\Development\\ChocolateChips\\ChocolateChips\\bin\\Debug\\DataDump.bat");
	cout << "Loading: " << tMarketName << "\t";
	string aCommand;
	if (tYear == 0)
	{
		aCommand = "DataDump.bat \"" + aMarketCommand + "\"";
	}
	else
	{
		aCommand = "DataDumpVar.bat \"" + aMarketCommand + "\" " + to_string(tYear);
	}


	string aFileName = "Markets\\"+ tMarketName + to_string(tYear) + ".csv";
	ifstream aCheckFile(aFileName.c_str());
	if (!aCheckFile.good())
	{
		system(aCommand.c_str());
	}
	else
	{
		struct tm *aFileTime;
		struct stat attrib;
		stat(aFileName.c_str(), &attrib);
		aFileTime = localtime(&(attrib.st_mtime));
		time_t now;
		time(&now);
		int aFileDay = aFileTime->tm_yday;
		int aCurrentDay = localtime(&now)->tm_yday;
		if (aFileDay != aCurrentDay)
		{
			system(aCommand.c_str());
		}
	}

	aCheckFile.close();
	ifstream aFile(aFileName.c_str());
	string aStringLine;
	int aLineCount = 0;
	vector<MarketPrice> aMarketPriceVector;
	//toss out first line of the file
	getline(aFile, aStringLine);
	while (getline(aFile, aStringLine)&&(tMarketNum > aLineCount))
	{
		MarketPrice aTempPrice = CreatePrice(aStringLine);
		aMarketPriceVector.push_back(aTempPrice);
		aLineCount++;
	}
	string aMarketFileName = tMarketName + to_string(tMarketNum);
	for (int x = 0; x < 15; x++)
	{
		if (x < aMarketFileName.length())
		{
			m_Market.m_MarketName[x] = aMarketFileName[x];
		}
		else
		{
			m_Market.m_MarketName[x] = ' ';
		}
	}
	m_Market.m_MarketPriceCount = aLineCount;
	m_Market.PriceList = new MarketPrice[aLineCount];
	for (int x = 0; !aMarketPriceVector.empty(); x++)
	{
		m_Market.PriceList[x] = aMarketPriceVector.back();
		aMarketPriceVector.pop_back();
	}

	for (int x = ONEYEAR; x < m_Market.m_MarketPriceCount; x++)
	{
		//set all to 0s
		PriceAverage OpenSums = m_Market.PriceList[x].m_Open;
		PriceAverage HighSums = m_Market.PriceList[x].m_High;
		PriceAverage LowSums = m_Market.PriceList[x].m_Low;
		PriceAverage CloseSums = m_Market.PriceList[x].m_Close;
		PriceAverage VolumeSums = m_Market.PriceList[x].m_Volume;
		for (int y = (x - ONEYEAR); y < x; y++)
		{
			OpenSums.m_Avg1Year += m_Market.PriceList[y].m_Open.m_Price;
			HighSums.m_Avg1Year += m_Market.PriceList[y].m_High.m_Price;
			LowSums.m_Avg1Year += m_Market.PriceList[y].m_Low.m_Price;
			CloseSums.m_Avg1Year += m_Market.PriceList[y].m_Close.m_Price;
			VolumeSums.m_Avg1Year += m_Market.PriceList[y].m_Volume.m_Price;
			if ((x - y) < SIXMONTH)
			{
				OpenSums.m_Avg6Month += m_Market.PriceList[y].m_Open.m_Price; 
				HighSums.m_Avg6Month += m_Market.PriceList[y].m_High.m_Price;
				LowSums.m_Avg6Month += m_Market.PriceList[y].m_Low.m_Price;
				CloseSums.m_Avg6Month += m_Market.PriceList[y].m_Close.m_Price;
				VolumeSums.m_Avg6Month += m_Market.PriceList[y].m_Volume.m_Price;
				if ((x - y) < THREEMONTH)
				{
					OpenSums.m_Avg3Month += m_Market.PriceList[y].m_Open.m_Price;
					HighSums.m_Avg3Month += m_Market.PriceList[y].m_High.m_Price;
					LowSums.m_Avg3Month += m_Market.PriceList[y].m_Low.m_Price;
					CloseSums.m_Avg3Month += m_Market.PriceList[y].m_Close.m_Price;
					VolumeSums.m_Avg3Month += m_Market.PriceList[y].m_Volume.m_Price;
					if ((x - y) < THIRTYDAY)
					{
						OpenSums.m_Avg30Day += m_Market.PriceList[y].m_Open.m_Price;
						HighSums.m_Avg30Day += m_Market.PriceList[y].m_High.m_Price;
						LowSums.m_Avg30Day += m_Market.PriceList[y].m_Low.m_Price;
						CloseSums.m_Avg30Day += m_Market.PriceList[y].m_Close.m_Price;
						VolumeSums.m_Avg30Day += m_Market.PriceList[y].m_Volume.m_Price;
						if ((x - y) < FIFTEENDAY)
						{
							OpenSums.m_Avg15Day += m_Market.PriceList[y].m_Open.m_Price;
							HighSums.m_Avg15Day += m_Market.PriceList[y].m_High.m_Price;
							LowSums.m_Avg15Day += m_Market.PriceList[y].m_Low.m_Price;
							CloseSums.m_Avg15Day += m_Market.PriceList[y].m_Close.m_Price;
							VolumeSums.m_Avg15Day += m_Market.PriceList[y].m_Volume.m_Price;
							if ((x - y) < FIVEDAY)
							{
								OpenSums.m_Avg5Day += m_Market.PriceList[y].m_Open.m_Price;
								HighSums.m_Avg5Day += m_Market.PriceList[y].m_High.m_Price;
								LowSums.m_Avg5Day += m_Market.PriceList[y].m_Low.m_Price;
								CloseSums.m_Avg5Day += m_Market.PriceList[y].m_Close.m_Price;
								VolumeSums.m_Avg5Day += m_Market.PriceList[y].m_Volume.m_Price;
							}
						}
					}
				}
			}
		}
		LoadAverages(&(m_Market.PriceList[x]), &OpenSums, &HighSums, &LowSums, &CloseSums, &VolumeSums);
		m_LastMarket = m_Market.PriceList[x];
	}
	//m_LastMarket = &m_Market.PriceList[m_Market.m_MarketPriceCount];
	cout << m_Market.m_MarketPriceCount << " Records Loaded." << endl;
}


MarketLoader::~MarketLoader()
{
}

MarketPrice MarketLoader::CreatePrice(string tPriceString)
{
	MarketPrice aPrice;
	string aValue;
	istringstream aLine(tPriceString);
	vector<string> aValueList;
	while (getline(aLine, aValue, ','))
	{
		aValueList.push_back(aValue);
	}

	string aTemp = aValueList.at(0);
	for (int x = 0; x < 15; x++)
	{
		if (x < aTemp.length())
		{
			aPrice.m_Timestamp[x] = aTemp[x];
		}
		else
		{
			aPrice.m_Timestamp[x] = '|';
		}
	}

	aPrice.m_Open.m_Price = stod(aValueList.at(1));
	aPrice.m_Open.m_Avg5Day = 0; aPrice.m_Open.m_Avg15Day = 0;
	aPrice.m_Open.m_Avg30Day = 0; aPrice.m_Open.m_Avg3Month = 0;
	aPrice.m_Open.m_Avg6Month = 0; aPrice.m_Open.m_Avg1Year = 0;
	aPrice.m_High.m_Price = stod(aValueList.at(2));
	aPrice.m_High.m_Avg5Day = 0; aPrice.m_High.m_Avg15Day = 0;
	aPrice.m_High.m_Avg30Day = 0; aPrice.m_High.m_Avg3Month = 0;
	aPrice.m_High.m_Avg6Month = 0; aPrice.m_High.m_Avg1Year = 0;
	aPrice.m_Low.m_Price = stod(aValueList.at(3));
	aPrice.m_Low.m_Avg5Day = 0; aPrice.m_Low.m_Avg15Day = 0;
	aPrice.m_Low.m_Avg30Day = 0; aPrice.m_Low.m_Avg3Month = 0;
	aPrice.m_Low.m_Avg6Month = 0; aPrice.m_Low.m_Avg1Year = 0;
	aPrice.m_Close.m_Price = stod(aValueList.at(4));
	aPrice.m_Close.m_Avg5Day = 0; aPrice.m_Close.m_Avg15Day = 0;
	aPrice.m_Close.m_Avg30Day = 0; aPrice.m_Close.m_Avg3Month = 0;
	aPrice.m_Close.m_Avg6Month = 0; aPrice.m_Close.m_Avg1Year = 0;
	aPrice.m_Volume.m_Price = stod(aValueList.at(5));
	aPrice.m_Volume.m_Avg5Day = 0; aPrice.m_Volume.m_Avg15Day = 0;
	aPrice.m_Volume.m_Avg30Day = 0; aPrice.m_Volume.m_Avg3Month = 0;
	aPrice.m_Volume.m_Avg6Month = 0; aPrice.m_Volume.m_Avg1Year = 0;

	return aPrice;
}

void MarketLoader::LoadAverages(MarketPrice *tTargetPrice, PriceAverage *tOpen, PriceAverage *tHigh, PriceAverage *tLow, 
	PriceAverage *tClose, PriceAverage *tVolume)
{
	tTargetPrice->m_Open.m_Avg5Day = tOpen->m_Avg5Day / FIVEDAY;
	tTargetPrice->m_Open.m_Avg15Day = tOpen->m_Avg15Day / FIFTEENDAY;
	tTargetPrice->m_Open.m_Avg30Day = tOpen->m_Avg30Day / THIRTYDAY;
	tTargetPrice->m_Open.m_Avg3Month = tOpen->m_Avg3Month / THREEMONTH;
	tTargetPrice->m_Open.m_Avg6Month = tOpen->m_Avg6Month / SIXMONTH;
	tTargetPrice->m_Open.m_Avg1Year = tOpen->m_Avg1Year / ONEYEAR;
				
	tTargetPrice->m_High.m_Avg5Day = tHigh->m_Avg5Day / FIVEDAY;
	tTargetPrice->m_High.m_Avg15Day = tHigh->m_Avg15Day / FIFTEENDAY;
	tTargetPrice->m_High.m_Avg30Day = tHigh->m_Avg30Day / THIRTYDAY;
	tTargetPrice->m_High.m_Avg3Month = tHigh->m_Avg3Month / THREEMONTH;
	tTargetPrice->m_High.m_Avg6Month = tHigh->m_Avg6Month / SIXMONTH;
	tTargetPrice->m_High.m_Avg1Year = tHigh->m_Avg1Year / ONEYEAR;
				
	tTargetPrice->m_Low.m_Avg5Day = tLow->m_Avg5Day / FIVEDAY;
	tTargetPrice->m_Low.m_Avg15Day = tLow->m_Avg15Day / FIFTEENDAY;
	tTargetPrice->m_Low.m_Avg30Day = tLow->m_Avg30Day / THIRTYDAY;
	tTargetPrice->m_Low.m_Avg3Month = tLow->m_Avg3Month / THREEMONTH;
	tTargetPrice->m_Low.m_Avg6Month = tLow->m_Avg6Month / SIXMONTH;
	tTargetPrice->m_Low.m_Avg1Year = tLow->m_Avg1Year / ONEYEAR;
				
	tTargetPrice->m_Close.m_Avg5Day = tClose->m_Avg5Day / FIVEDAY;
	tTargetPrice->m_Close.m_Avg15Day = tClose->m_Avg15Day / FIFTEENDAY;
	tTargetPrice->m_Close.m_Avg30Day = tClose->m_Avg30Day / THIRTYDAY;
	tTargetPrice->m_Close.m_Avg3Month = tClose->m_Avg3Month / THREEMONTH;
	tTargetPrice->m_Close.m_Avg6Month = tClose->m_Avg6Month / SIXMONTH;
	tTargetPrice->m_Close.m_Avg1Year = tClose->m_Avg1Year / ONEYEAR;
				
	tTargetPrice->m_Volume.m_Avg5Day = tVolume->m_Avg5Day / FIVEDAY;
	tTargetPrice->m_Volume.m_Avg15Day = tVolume->m_Avg15Day / FIFTEENDAY;
	tTargetPrice->m_Volume.m_Avg30Day = tVolume->m_Avg30Day / THIRTYDAY;
	tTargetPrice->m_Volume.m_Avg3Month = tVolume->m_Avg3Month / THREEMONTH;
	tTargetPrice->m_Volume.m_Avg6Month = tVolume->m_Avg6Month / SIXMONTH;
	tTargetPrice->m_Volume.m_Avg1Year = tVolume->m_Avg1Year / ONEYEAR;
}

