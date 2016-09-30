#pragma once
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
	double m_Settings[51];
	int m_MarketCount;
	int m_SettingsCount;
	int m_ShareCount;
	int m_TotalShareCount;
	int m_BrokerNum;
	int m_AlgorithmID;
	int m_BrokerGuid;
} Broker;