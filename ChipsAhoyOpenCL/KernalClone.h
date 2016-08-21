#pragma once
#include "Structs.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include "Structs.h"
#include "MarketLoader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
class KernalClone
{
public:

	KernalClone();
	~KernalClone();


	void TestRun(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount);
	boost::property_tree::ptree DisplayBestBroker(Broker *tBroker, Market tMarket);
	void Log(string tMessage, string tMarketName, bool tAppend = true);
	PriceAverage CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay);
	boost::property_tree::ptree CreateNode(string tBrokerAction, string tTimeStamp, double tBudget, double aPrice, int tShareCount, double tInvestment, double tValue);

};

