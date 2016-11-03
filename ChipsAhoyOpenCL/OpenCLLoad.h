#pragma once
//#pragma comment(lib, "lib\\x64\\OpenCL.lib")
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
	cl::CommandQueue m_Queue;
	cl::Context m_Context;
	cl::Program m_Program;
	string m_KernalCode;
	cl::Buffer m_BrokerBuffer;
	vector<cl::Buffer> m_MarketBufferList;
	vector<cl::Buffer> m_MarketDifferenceBufferList;
	vector<cl::Buffer> m_CountBufferList;
	OpenCLLoad();
	void Setup(int tPlatform, int tDevice);
	~OpenCLLoad();
	void RunBrokers(Broker* tBrokerList, int tBrokerCount);
	void CalcMarketDifferences(const MarketPrice* tMarketPriceList, MarketPrice* tMarketChanges, const int *tMarketPriceCount);
	void TestRun(Broker* tBrokerList, int tBrokerCount, Market* tMarketList, int tMarketCount);
	boost::property_tree::ptree LogLongTermBroker(Broker *tBroker, Market tMarket, MarketPrice* tMarketDifferenceList, bool tDisplayActions);
	boost::property_tree::ptree LogShortTermBroker(Broker *tBroker, Market tMarket, MarketPrice* tMarketDifferenceList, bool tDisplayActions, int tHoldDays);
	void Log(string tMessage, string tMarketName, bool tAppend=true);
	PriceAverage CalcDPrice(PriceAverage tCurrentDay, PriceAverage tPrevDay);
	void LoadMarkets(Market* tMarketList, int tMarketCount, int tBrokerCount);
	boost::property_tree::ptree CreateNode(string tBrokerAction, string tTimeStamp, double tBudget, double aPrice, int tShareCount, double tInvestment, double tValue);
};