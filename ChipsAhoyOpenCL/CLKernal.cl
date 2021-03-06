#pragma OPENCL EXTENSION cl_khr_fp64 : enable

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
	double m_Settings[56];
	double m_TotalInvestment;
	double m_Investment;
	double m_PercentReturn;
	int m_MarketCount;
	int m_SettingsCount;
	int m_ShareCount;
	int m_TotalShareCount;
	int m_BrokerNum;
	int m_AlgorithmID;
	int m_BrokerGuid;
} Broker;
typedef struct ConstQueueStruct {
	int m_Queue[100];
	int m_Front;
	int m_Back;

} ConstQueue;

ConstQueue CreateQueue()
{
	ConstQueue aQueue;
	aQueue.m_Front = 0;
	aQueue.m_Back = 0;
	for (int x=0; x<100; x++)
	{
		aQueue.m_Queue[x]=0;
	}
	return aQueue;
};

bool AddRear(ConstQueue *tConstQueue, int tTarget)
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
bool RemoveFront(ConstQueue *tConstQueue)
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

void kernel TestRun(global Broker* tBrokerList, global const MarketPrice* tMarketPriceList, global const int* tMarketPriceCount)
{
	Broker aSimBroker = tBrokerList[get_global_id(0)];
	aSimBroker.m_BrokerNum = get_global_id(0);
	aSimBroker.m_NetWorth = tMarketPriceList[0].m_Open.m_Price + get_global_id(0);
	tBrokerList[get_global_id(0)] = aSimBroker;
}
PriceAverage CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay)
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

void kernel LongTerm(global Broker* tBrokerList, global const MarketPrice* tMarketPriceList, global const int* tMarketPriceCount, global const MarketPrice* tMarketDifferenceList)
{

	Broker aSimBroker = tBrokerList[get_global_id(0)];
	aSimBroker.m_BrokerNum = get_global_id(0);
	aSimBroker.m_AlgorithmID = 9;
	aSimBroker.m_Investment = 0;
	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	Market aMarket;
	int aOldestStock = 0;

	int aMarketStockCount = 0;

	
	double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];

	double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

	if (aBuyPoint < aSellPoint)
	{
		aSellPoint = aBuyPoint;
	}

	for (int y = 1; y < tMarketPriceCount[0]; y++)
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
			if (y == tMarketPriceCount[0] - 1)
			{
				aSellEnd = true;//Sell End
			}

		}

		if (aBuy)
		{
			if (aSimBroker.m_Budget > tMarketPriceList[y].m_Close.m_Price)
			{
				aSimBroker.m_ShareCount++;
				aSimBroker.m_TotalShareCount++;
				aSimBroker.m_Budget -= tMarketPriceList[y].m_Close.m_Price;
				aSimBroker.m_Investment += tMarketPriceList[y].m_Close.m_Price;
				//aSimBroker.m_TotalInvestment += tMarketPriceList[y].m_Close.m_Price;
				aOldestStock = y;
			}
		}
		if (aSell)
		{

			if (aSimBroker.m_ShareCount > 0)
			{
				aSimBroker.m_Investment -= tMarketPriceList[y].m_Close.m_Price;
				//aSimBroker.m_TotalInvestment -= tMarketPriceList[y].m_Close.m_Price;
				aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price;
				aSimBroker.m_ShareCount--;
				aOldestStock = y;
			}
		}
		if (aSellOld)
		{
			aSimBroker.m_Investment = 0;
			//aSimBroker.m_TotalInvestment -= tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		if (aSellSplit)
		{
			//aSimBroker.m_TotalInvestment -= tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
			aSimBroker.m_Investment = 0;
		}
		if (aSellEnd)
		{
			//aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_NetWorth += aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}

		aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
	}
	
	tBrokerList[get_global_id(0)] = aSimBroker;

}

void kernel ShortTerm(global Broker* tBrokerList, global const MarketPrice* tMarketPriceList, global const int* tMarketPriceCount, global const MarketPrice* tMarketDifferenceList, global const int* tHoldDays)
{



	Broker aSimBroker = tBrokerList[get_global_id(0)];
	aSimBroker.m_BrokerNum = get_global_id(0);
	aSimBroker.m_AlgorithmID = 102;
	aSimBroker.m_Investment = 0;
	aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
	aSimBroker.m_ShareCount = 0;
	Market aMarket;
	int aOldestStock = 0;
	int aMarketStockCount = 0;

	
	double aBuyPoint = aSimBroker.m_Settings[0] * aSimBroker.m_Settings[1];
	
	double aSellPoint = aSimBroker.m_Settings[2] * aSimBroker.m_Settings[3];

	if (aBuyPoint < aSellPoint)
	{
		aSellPoint = aBuyPoint;
	}
	ConstQueue aPurchaseQueue = CreateQueue();


	for (int y = 1; y < tMarketPriceCount[0]; y++)
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
			if (y == tMarketPriceCount[0] - 1)
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
						aSimBroker.m_Investment -= tMarketPriceList[y].m_Close.m_Price;

						aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price;
						aSimBroker.m_ShareCount--;
					}
				}
				
			}
		}
		if (aSellSplit)
		{
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y - 1].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_ShareCount = 0;
			//Resetting the queue
			aPurchaseQueue = CreateQueue();
		}
		if (aSellEnd)
		{
			aSimBroker.m_Investment = 0;

			aSimBroker.m_Budget += tMarketPriceList[y].m_Close.m_Price * aSimBroker.m_ShareCount;
			aSimBroker.m_NetWorth += aSimBroker.m_Budget;
			aSimBroker.m_Budget = aSimBroker.m_BudgetPerMarket;
			aSimBroker.m_ShareCount = 0;
			aOldestStock = y;
		}
		aSimBroker.m_TotalInvestment += aSimBroker.m_Investment;
	}
	
	tBrokerList[get_global_id(0)] = aSimBroker;

}