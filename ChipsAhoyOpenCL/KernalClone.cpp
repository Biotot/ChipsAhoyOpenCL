#include "KernalClone.h"
using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;

KernalClone::KernalClone()
{
}
KernalClone::~KernalClone()
{
}


PriceAverage KernalClone::CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay)
{
	PriceAverage aRet;
	aRet.m_Price = tCurrentDay.m_Price - tPrevDay.m_Price;
	aRet.m_Avg5Day = tCurrentDay.m_Avg5Day - tPrevDay.m_Avg5Day;
	aRet.m_Avg15Day = tCurrentDay.m_Avg15Day - tPrevDay.m_Avg15Day;
	aRet.m_Avg30Day = tCurrentDay.m_Avg30Day - tPrevDay.m_Avg30Day;
	aRet.m_Avg3Month = tCurrentDay.m_Avg3Month - tPrevDay.m_Avg3Month;
	aRet.m_Avg6Month = tCurrentDay.m_Avg6Month - tPrevDay.m_Avg6Month;
	aRet.m_Avg1Year = tCurrentDay.m_Avg1Year - tPrevDay.m_Avg1Year;

	aRet.m_Price = (aRet.m_Price / tCurrentDay.m_Price) * 100;
	aRet.m_Avg5Day = (aRet.m_Avg5Day / tCurrentDay.m_Avg5Day) * 100;
	aRet.m_Avg15Day = (aRet.m_Avg15Day / tCurrentDay.m_Avg15Day) * 100;
	aRet.m_Avg30Day = (aRet.m_Avg30Day / tCurrentDay.m_Avg30Day) * 100;
	aRet.m_Avg3Month = (aRet.m_Avg3Month / tCurrentDay.m_Avg3Month) * 100;
	aRet.m_Avg6Month = (aRet.m_Avg6Month / tCurrentDay.m_Avg6Month) * 100;
	aRet.m_Avg1Year = (aRet.m_Avg1Year / tCurrentDay.m_Avg1Year) * 100;

	return aRet;
}

ptree KernalClone::DisplayBestBroker(Broker *tBroker, Market tMarket)
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
	aSimBroker.m_ShareCount = 0;
	aSimBroker.m_NetWorth = 0;
	int aMarketStockCount = 0;

	ptree aMarketXML;
	aMarketXML.add("MarketActions.<xmlattr>.Name", string(tMarket.m_MarketName).substr(0, 15));
	aMarketXML.add("MarketActions.<xmlattr>.Depth", tMarket.m_MarketPriceCount);

	for (int y = 0; y < tMarket.m_MarketPriceCount; y++)
	{
		bool aValid = true;
		bool aBuy = false;
		bool aSell = false;
		bool aSellOld = false;
		bool aSellSplit = false;
		bool aSellEnd = false;

		if (tMarketPriceList[y].m_Open.m_Price == 0.0)
		{
			//Resetting the market
			aSellEnd = true;
			aValid = false;
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
			double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];

			double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

			double aDecisionPoint = 0.0;
			PriceAverage aDOpen;

			aDOpen = CalcDPrice(tMarketPriceList[y].m_Open, tMarketPriceList[y - 1].m_Open);
			/*
			aDOpen.m_Price = tMarketPriceList[y].m_Open.m_Price - tMarketPriceList[y - 1].m_Open.m_Price;
			aDOpen.m_Avg5Day = tMarketPriceList[y].m_Open.m_Avg5Day - tMarketPriceList[y - 1].m_Open.m_Avg5Day;
			aDOpen.m_Avg15Day = tMarketPriceList[y].m_Open.m_Avg15Day - tMarketPriceList[y - 1].m_Open.m_Avg15Day;
			aDOpen.m_Avg30Day = tMarketPriceList[y].m_Open.m_Avg30Day - tMarketPriceList[y - 1].m_Open.m_Avg30Day;
			aDOpen.m_Avg3Month = tMarketPriceList[y].m_Open.m_Avg3Month - tMarketPriceList[y - 1].m_Open.m_Avg3Month;
			aDOpen.m_Avg6Month = tMarketPriceList[y].m_Open.m_Avg6Month - tMarketPriceList[y - 1].m_Open.m_Avg6Month;
			aDOpen.m_Avg1Year = tMarketPriceList[y].m_Open.m_Avg1Year - tMarketPriceList[y - 1].m_Open.m_Avg1Year;
			*/
			aDecisionPoint += aDOpen.m_Price*aSimBroker.m_Settings[4];
			aDecisionPoint += aDOpen.m_Avg5Day*aSimBroker.m_Settings[5];
			aDecisionPoint += aDOpen.m_Avg15Day*aSimBroker.m_Settings[6];
			aDecisionPoint += aDOpen.m_Avg30Day*aSimBroker.m_Settings[7];
			aDecisionPoint += aDOpen.m_Avg3Month*aSimBroker.m_Settings[8];
			aDecisionPoint += aDOpen.m_Avg6Month*aSimBroker.m_Settings[9];
			aDecisionPoint += aDOpen.m_Avg1Year*aSimBroker.m_Settings[10];
			PriceAverage aDHigh;
			aDHigh = CalcDPrice(tMarketPriceList[y].m_High, tMarketPriceList[y - 1].m_High);
			/*
			aDHigh.m_Price = tMarketPriceList[y].m_High.m_Price - tMarketPriceList[y - 1].m_High.m_Price;
			aDHigh.m_Avg5Day = tMarketPriceList[y].m_High.m_Avg5Day - tMarketPriceList[y - 1].m_High.m_Avg5Day;
			aDHigh.m_Avg15Day = tMarketPriceList[y].m_High.m_Avg15Day - tMarketPriceList[y - 1].m_High.m_Avg15Day;
			aDHigh.m_Avg30Day = tMarketPriceList[y].m_High.m_Avg30Day - tMarketPriceList[y - 1].m_High.m_Avg30Day;
			aDHigh.m_Avg3Month = tMarketPriceList[y].m_High.m_Avg3Month - tMarketPriceList[y - 1].m_High.m_Avg3Month;
			aDHigh.m_Avg6Month = tMarketPriceList[y].m_High.m_Avg6Month - tMarketPriceList[y - 1].m_High.m_Avg6Month;
			aDHigh.m_Avg1Year = tMarketPriceList[y].m_High.m_Avg1Year - tMarketPriceList[y - 1].m_High.m_Avg1Year;
			*/
			aDecisionPoint += aDHigh.m_Price*aSimBroker.m_Settings[11];
			aDecisionPoint += aDHigh.m_Avg5Day*aSimBroker.m_Settings[12];
			aDecisionPoint += aDHigh.m_Avg15Day*aSimBroker.m_Settings[13];
			aDecisionPoint += aDHigh.m_Avg30Day*aSimBroker.m_Settings[14];
			aDecisionPoint += aDHigh.m_Avg3Month*aSimBroker.m_Settings[15];
			aDecisionPoint += aDHigh.m_Avg6Month*aSimBroker.m_Settings[16];
			aDecisionPoint += aDHigh.m_Avg1Year*aSimBroker.m_Settings[17];
			PriceAverage aDLow;
			aDLow = CalcDPrice(tMarketPriceList[y].m_Low, tMarketPriceList[y - 1].m_Low);
			/*
			aDLow.m_Price = tMarketPriceList[y].m_Low.m_Price - tMarketPriceList[y - 1].m_Low.m_Price;
			aDLow.m_Avg5Day = tMarketPriceList[y].m_Low.m_Avg5Day - tMarketPriceList[y - 1].m_Low.m_Avg5Day;
			aDLow.m_Avg15Day = tMarketPriceList[y].m_Low.m_Avg15Day - tMarketPriceList[y - 1].m_Low.m_Avg15Day;
			aDLow.m_Avg30Day = tMarketPriceList[y].m_Low.m_Avg30Day - tMarketPriceList[y - 1].m_Low.m_Avg30Day;
			aDLow.m_Avg3Month = tMarketPriceList[y].m_Low.m_Avg3Month - tMarketPriceList[y - 1].m_Low.m_Avg3Month;
			aDLow.m_Avg6Month = tMarketPriceList[y].m_Low.m_Avg6Month - tMarketPriceList[y - 1].m_Low.m_Avg6Month;
			aDLow.m_Avg1Year = tMarketPriceList[y].m_Low.m_Avg1Year - tMarketPriceList[y - 1].m_Low.m_Avg1Year;
			*/
			aDecisionPoint += aDLow.m_Price*aSimBroker.m_Settings[18];
			aDecisionPoint += aDLow.m_Avg5Day*aSimBroker.m_Settings[19];
			aDecisionPoint += aDLow.m_Avg15Day*aSimBroker.m_Settings[20];
			aDecisionPoint += aDLow.m_Avg30Day*aSimBroker.m_Settings[21];
			aDecisionPoint += aDLow.m_Avg3Month*aSimBroker.m_Settings[22];
			aDecisionPoint += aDLow.m_Avg6Month*aSimBroker.m_Settings[23];
			aDecisionPoint += aDLow.m_Avg1Year*aSimBroker.m_Settings[24];

			PriceAverage aDClose;
			aDClose = CalcDPrice(tMarketPriceList[y].m_Close, tMarketPriceList[y - 1].m_Close);
			/*
			aDClose.m_Price = tMarketPriceList[y].m_Close.m_Price - tMarketPriceList[y - 1].m_Close.m_Price;
			aDClose.m_Avg5Day = tMarketPriceList[y].m_Close.m_Avg5Day - tMarketPriceList[y - 1].m_Close.m_Avg5Day;
			aDClose.m_Avg15Day = tMarketPriceList[y].m_Close.m_Avg15Day - tMarketPriceList[y - 1].m_Close.m_Avg15Day;
			aDClose.m_Avg30Day = tMarketPriceList[y].m_Close.m_Avg30Day - tMarketPriceList[y - 1].m_Close.m_Avg30Day;
			aDClose.m_Avg3Month = tMarketPriceList[y].m_Close.m_Avg3Month - tMarketPriceList[y - 1].m_Close.m_Avg3Month;
			aDClose.m_Avg6Month = tMarketPriceList[y].m_Close.m_Avg6Month - tMarketPriceList[y - 1].m_Close.m_Avg6Month;
			aDClose.m_Avg1Year = tMarketPriceList[y].m_Close.m_Avg1Year - tMarketPriceList[y - 1].m_Close.m_Avg1Year;
			*/
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

		}

		if (aBuy)
		{
			if (aSimBroker.m_Budget > tMarketPriceList[y].m_Open.m_Price)
			{
				aSimBroker.m_ShareCount++;
				aSimBroker.m_TotalShareCount++;
				aSimBroker.m_Budget -= tMarketPriceList[y].m_Open.m_Price;
				aOldestStock = y;

				aInvestment += tMarketPriceList[y].m_Open.m_Price;
				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				string aAction = "Buy       ";
				ptree aNode = CreateNode(aAction, aTimestamp.substr(0, 15), aSimBroker.m_Budget, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
				aMarketXML.add_child("MarketActions.Action", aNode);
				Log("Buy: " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aSimBroker.m_Budget), aMarketName);
			}
		}
		if (aSell)
		{

			if (aSimBroker.m_ShareCount > 0)
			{
				string aTimestamp(tMarketPriceList[y].m_Timestamp);
				string aAction = "Sell      ";
				ptree aNode = CreateNode(aAction, aTimestamp.substr(0, 15), aSimBroker.m_Budget, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
				aMarketXML.add_child("MarketActions.Action", aNode);
				Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount), aMarketName);
				aInvestment -= tMarketPriceList[y].m_Open.m_Price;

				aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price;
				aSimBroker.m_ShareCount--;
				aOldestStock = y;
			}
		}
		if (aSellOld)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell OLD  ";
			ptree aNode = CreateNode(aAction, aTimestamp.substr(0, 15), aSimBroker.m_Budget, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
			aInvestment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		if (aSellSplit)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell SPLIT";
			ptree aNode = CreateNode(aAction, aTimestamp.substr(0, 15), aSimBroker.m_Budget, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
			aInvestment = 0;

			aSimBroker.m_ShareCount += aSimBroker.m_ShareCount;
			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		if (aSellEnd)
		{
			string aTimestamp(tMarketPriceList[y].m_Timestamp);
			string aAction = "Sell END  ";
			ptree aNode = CreateNode(aAction, aTimestamp.substr(0, 15), aSimBroker.m_Budget, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
			aMarketXML.add_child("MarketActions.Action", aNode);
			Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
			aInvestment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Open.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_NetWorth += aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
	}
	Log("Final: " + to_string(aSimBroker.m_Budget - aSimBroker.m_BudgetPerMarket), aMarketName);

	double aProfit = aSimBroker.m_NetWorth - aSimBroker.m_BudgetPerMarket;
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
	aMarketXML.add_child("MarketActions.Details", aMarketDetails);
	*tBroker = aSimBroker;
	return aMarketXML;

	//string aSaveFile = string(tMarket.m_MarketName).substr(0, 15) + to_string(aSimBroker.m_BrokerGuid) +  + ".xml";
	//write_xml("Broker\\"+aSaveFile, aMarketXML);
}

/*
aInvestment += tMarketPriceList[y].m_Open.m_Price;
string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Buy";
ptree aNode = CreateNode(aAction, aTimestamp, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);


string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell OLD";
ptree aNode = CreateNode(aAction, aTimestamp, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;


string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell SPLIT";
ptree aNode = CreateNode(aAction, aTimestamp, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y - 1].m_Close.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;

string aTimestamp(tMarketPriceList[y].m_Timestamp);
string aAction = "Sell END";
ptree aNode = CreateNode(aAction, aTimestamp, tMarketPriceList[y].m_Open.m_Price, aSimBroker.m_ShareCount, aInvestment, tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount);
aMarketXML.add_child("MarketActions.Action", aNode);
Log(aAction + ": " + aTimestamp.substr(0, 15) + " : " + to_string(tMarketPriceList[y].m_Open.m_Price) + " : " + to_string(aSimBroker.m_ShareCount) + " : " + to_string(aInvestment) + " : " + to_string((tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount)), aMarketName);
aInvestment = 0;


*/

void KernalClone::Log(string tMessage, string tMarketName, bool tAppend)
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

ptree KernalClone::CreateNode(string tBrokerAction, string tTimeStamp, double tBudget, double aPrice, int tShareCount, double tInvestment, double tValue)
{
	ptree aActionTree;
	aActionTree.add("<xmlattr>.Action", tBrokerAction);
	aActionTree.add("<xmlattr>.TimeStamp", tTimeStamp);
	aActionTree.add("<xmlattr>.Price", aPrice);
	aActionTree.add("<xmlattr>.ShareCount", tShareCount);
	aActionTree.add("<xmlattr>.Investment", tInvestment);
	aActionTree.add("<xmlattr>.Value", tValue);
	aActionTree.add("<xmlattr>.Budget", tBudget);
	return aActionTree;
}