#include "ChipsDB.h"

using namespace std;


using boost::property_tree::ptree;
using boost::property_tree::write_xml;
using boost::property_tree::xml_writer_settings;


ChipsDB::ChipsDB()
{
	ptree aMarketXML;
	aMarketXML.add("Market.<xmlattr>.version", "1.0");
	ptree aMarketStats;
	aMarketStats.add("MarketName", "test");
	aMarketStats.add("MarketLength", "testTest");
	aMarketXML.add_child("Market.Stats", aMarketStats);
	ptree aMarketTest;
	aMarketTest.add("MtName", "tst");
	aMarketTest.add("MarLength", "teTest");
	ptree aMarketTest2;
	aMarketTest2.add_child("Market.Stats.BUDDA", aMarketTest);
	aMarketTest2.add("MtNadfsame", "twgwgwst");
	aMarketTest2.add("MarLenqgfwgth", "teTggrest");
	aMarketXML.add_child("Market.ASDG", aMarketTest2);


	// Note that starting with Boost 1.56, the template argument must be std::string
	// instead of char
	write_xml("test.xml", aMarketXML);


}


ChipsDB::~ChipsDB()
{
	//do nothing
}



vector<Broker> ChipsDB::GetTopBrokers(int tBrokerCount, std::string tWhereClause)
{
	vector<Broker> aTopList;



	return aTopList;
}
bool ChipsDB::CheckThenInsert(MarketPrice tMarketPrice)
{
	bool aRet = false;



	return aRet;
}
bool ChipsDB::CheckThenInsert(Broker tBestBroker)
{
	bool aRet = false;


	return aRet;
}
bool ChipsDB::BrokerAction(std::string tPriceTimestamp, std::string tMarketName, int tBrokerGuid, std::string tAction)
{
	bool aRet = false;



	return aRet;
}