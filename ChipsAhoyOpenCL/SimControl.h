#pragma once
#include "Structs.h"
#include "OpenCLLoad.h"
#include "ChipsDB.h"
#include "BrokerLoader.h"
#include <atlstr.h>
#include <stdio.h>
#include <ctime>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
class SimControl
{
public:
	Broker m_BestBroker;
	Market* m_MarketList;
	int m_TotalPriceCount;
	OpenCLLoad m_Loader;

	ChipsDB m_DB;

	SimControl();
	~SimControl();
	void Run(int aSimDepth, int tBrokerCount);
	double GetTime();
	std::string LogBroker(Broker tBroker, std::string tMessage, bool tWrite);
	void Log(std::string tLog, bool tConsole, std::string tFileName);
	void ReworkBrokerList(Broker* tBrokerList, Broker *tBestBroker, int tBrokerCount, int tMarketCount, double tRange);
	void RefineBrokerList(Broker* tBrokerList, Broker *tBestBroker, int tBrokerCount, int tMarketCount, double tRange, int tSettingNum);
	Broker CalcDeviations(Broker* tBrokerList, int tBrokerCount);
	Broker CalcDeviations(std::vector<Broker> tBrokerList, int tBrokerCount);
	boost::property_tree::ptree BrokerSettingsToNode(Broker tBroker);
	//string LogBest(Broker tBroker);
	//string LogBroker(Broker tBroker);
};

