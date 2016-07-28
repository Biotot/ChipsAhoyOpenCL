#pragma once
#pragma comment(lib, "lib\\x64\\OpenCL.lib")
#pragma comment(lib, "OpenCL.lib")
//#include <include/CL/cl.hpp>
#include <CL/cl.hpp>
#include "Structs.h"
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



using namespace std;
class OpenCLLoad
{
public:
	cl::Device m_Device;
	string m_KernalCode;
	OpenCLLoad();
	~OpenCLLoad();
	void RunBrokers(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount);
	void TestRun(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount);
	boost::property_tree::ptree DisplayBestBroker(Broker *tBroker, Market tMarket);
	void Log(string tMessage, string tMarketName, bool tAppend=true);
	PriceAverage CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay);
	boost::property_tree::ptree CreateNode(string tBrokerAction, string tTimeStamp, double tBudget, double aPrice, int tShareCount, double tInvestment, double tValue);
};