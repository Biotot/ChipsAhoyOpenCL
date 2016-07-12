#pragma comment(lib, "x64\\OpenCL.lib")
//using namespace std;

typedef struct PriceAverageStruct {
	double m_Price;
	double m_Avg5Day;
	double m_Avg15Day;
	double m_Avg30Day;
	double m_Avg3Month;
	double m_Avg6Month;
	double m_Avg1Year;
} PriceAverage;
typedef struct MarketPriceStruct {
	char m_Timestamp[15];
	PriceAverage m_Open;
	PriceAverage m_High;
	PriceAverage m_Low;
	PriceAverage m_Close;
	PriceAverage m_Volume;
} MarketPrice;
typedef struct MarketStruct {
	char m_MarketName[15];
	int m_MarketPriceCount;
	MarketPrice* PriceList;
} Market;
typedef struct BrokerStruct {
	double m_Budget;
	double m_NetWorth;
	double m_TotalProfit;
	double m_ProfitPerShare;
	double m_BudgetPerMarket;
	double m_BrokerScore;
	double m_Settings[50];
	int m_MarketCount;
	int m_SettingsCount;
	int m_ShareCount;
	int m_TotalShareCount;
	int m_BrokerNum;
	int m_AlgorithmID;
	int m_BrokerGuid;
} Broker;

void kernel TestRun(global Broker* tBrokerList, global const MarketPrice* tMarketPriceList, global const int* tMarketPriceCount)
{
	Broker aSimBroker = tBrokerList[get_global_id(0)];
	aSimBroker.m_BrokerNum = get_global_id(0);
	aSimBroker.m_NetWorth = tMarketPriceList[0].m_Open.m_Price + get_global_id(0);
	tBrokerList[get_global_id(0)] = aSimBroker;
}
void kernel FullRun(global Broker* tBrokerList, global const MarketPrice* tMarketPriceList, global const int *tMarketPriceCount)
{
	Broker aSimBroker = tBrokerList[get_global_id(0)];
	aSimBroker.m_BrokerNum = get_global_id(0);
	aSimBroker.m_AlgorithmID = 2;


	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	Market aMarket;
	int aOldestStock = 0;
	aSimBroker.m_NetWorth = 0;

	int aMarketStockCount = 0;
	for(int y=0; y<tMarketPriceCount[0];y++)
	{
		bool aValid = true;
		if (tMarketPriceList[y].m_Open.m_Price==0.0)
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
			if (aValid&&((tMarketPriceList[y-1].m_Close.m_Price/tMarketPriceList[y].m_Open.m_Price)>1.9))
			{
				aSimBroker.m_ShareCount += aSimBroker.m_ShareCount;
				aSimBroker.m_Budget += tMarketPriceList[y-1].m_Close.m_Price * aSimBroker.m_ShareCount;
				aSimBroker.m_ShareCount = 0;
				aOldestStock = y;
				aValid = false;
			}

			if (aOldestStock + 30 < y)
			{
				aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount;
				aSimBroker.m_ShareCount = 0;
				aOldestStock = y;
				aValid = false;
			}
		}
		
		if (aValid)
		{
			double aBuyPoint = aSimBroker.m_Settings[0]*aSimBroker.m_Settings[1];

			double aSellPoint = aSimBroker.m_Settings[2]*aSimBroker.m_Settings[3];
		
			double aDecisionPoint = 0.0;
			PriceAverage aDOpen;
			aDOpen.m_Price = tMarketPriceList[y].m_Open.m_Price - tMarketPriceList[y - 1].m_Open.m_Price;
			aDOpen.m_Avg5Day = tMarketPriceList[y].m_Open.m_Avg5Day - tMarketPriceList[y - 1].m_Open.m_Avg5Day;
			aDOpen.m_Avg15Day = tMarketPriceList[y].m_Open.m_Avg15Day - tMarketPriceList[y - 1].m_Open.m_Avg15Day;
			aDOpen.m_Avg30Day = tMarketPriceList[y].m_Open.m_Avg30Day - tMarketPriceList[y - 1].m_Open.m_Avg30Day;
			aDOpen.m_Avg3Month = tMarketPriceList[y].m_Open.m_Avg3Month - tMarketPriceList[y - 1].m_Open.m_Avg3Month;
			aDOpen.m_Avg6Month = tMarketPriceList[y].m_Open.m_Avg6Month - tMarketPriceList[y - 1].m_Open.m_Avg6Month;
			aDOpen.m_Avg1Year = tMarketPriceList[y].m_Open.m_Avg1Year - tMarketPriceList[y - 1].m_Open.m_Avg1Year;
			aDecisionPoint += aDOpen.m_Price*aSimBroker.m_Settings[4];
			aDecisionPoint += aDOpen.m_Avg5Day*aSimBroker.m_Settings[5];
			aDecisionPoint += aDOpen.m_Avg15Day*aSimBroker.m_Settings[6];
			aDecisionPoint += aDOpen.m_Avg30Day*aSimBroker.m_Settings[7];
			aDecisionPoint += aDOpen.m_Avg3Month*aSimBroker.m_Settings[8];
			aDecisionPoint += aDOpen.m_Avg6Month*aSimBroker.m_Settings[9];
			aDecisionPoint += aDOpen.m_Avg1Year*aSimBroker.m_Settings[10];
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
		
			aDecisionPoint += aDOpen.m_Avg5Day-aDOpen.m_Avg15Day*aSimBroker.m_Settings[32];
			aDecisionPoint += aDOpen.m_Avg15Day-aDOpen.m_Avg30Day*aSimBroker.m_Settings[33];
			aDecisionPoint += aDHigh.m_Avg5Day-aDHigh.m_Avg15Day*aSimBroker.m_Settings[34];
			aDecisionPoint += aDHigh.m_Avg15Day-aDHigh.m_Avg30Day*aSimBroker.m_Settings[35];
			aDecisionPoint += aDLow.m_Avg5Day-aDLow.m_Avg15Day*aSimBroker.m_Settings[36];
			aDecisionPoint += aDLow.m_Avg15Day-aDLow.m_Avg30Day*aSimBroker.m_Settings[37];
			aDecisionPoint += aDClose.m_Avg5Day-aDClose.m_Avg15Day*aSimBroker.m_Settings[38];
			aDecisionPoint += aDClose.m_Avg15Day-aDClose.m_Avg30Day*aSimBroker.m_Settings[39];
		
			aDecisionPoint += aDOpen.m_Avg5Day-aDClose.m_Avg5Day*aSimBroker.m_Settings[40];
			aDecisionPoint += aDOpen.m_Avg15Day-aDClose.m_Avg15Day*aSimBroker.m_Settings[41];

			aDecisionPoint += aDHigh.m_Avg5Day-aDLow.m_Avg5Day*aSimBroker.m_Settings[42];
			aDecisionPoint += aDHigh.m_Avg15Day-aDLow.m_Avg15Day*aSimBroker.m_Settings[43];

			if (aDecisionPoint > aBuyPoint)
			{
				if (aSimBroker.m_Budget > tMarketPriceList[y].m_Open.m_Price)
				{
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
					aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price;
					aSimBroker.m_ShareCount--;
					aOldestStock = y;
				}
				if (y == tMarketPriceCount[0]-1)
				{
					aSimBroker.m_Budget += tMarketPriceList[y].m_Open.m_Price * aSimBroker.m_ShareCount;
					aSimBroker.m_ShareCount = 0;
					aOldestStock = y;
					aValid = false;
				}
			}
		}

	}
	
	
	tBrokerList[get_global_id(0)] = aSimBroker;
}