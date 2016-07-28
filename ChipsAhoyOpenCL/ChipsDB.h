#pragma once
#include <iostream>
#include "Structs.h"
#include <vector>

#include <atlstr.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <string>

//#include <windows.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class ChipsDB
{
public:

	bool m_Connected;
	ChipsDB();
	~ChipsDB();


	std::vector<Broker> GetTopBrokers(int tBrokerCount, std::string tWhereClause);
	bool CheckThenInsert(MarketPrice tMarketPrice);
	bool CheckThenInsert(Broker tBestBroker);
	bool BrokerAction(std::string tPriceTimestamp, std::string tMarketName, int tBrokerGuid, std::string tAction);


};

