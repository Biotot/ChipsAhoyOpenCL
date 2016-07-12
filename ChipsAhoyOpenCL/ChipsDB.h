#pragma once
#include <afxwin.h> 
#include <iostream>
#include "Structs.h"
#include <vector>

#include <atlstr.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <string>

#include <windows.h>

#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

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

