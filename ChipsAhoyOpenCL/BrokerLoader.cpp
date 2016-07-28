#include "BrokerLoader.h"


using boost::property_tree::ptree;
using boost::property_tree::read_xml;
using boost::property_tree::xml_writer_settings;
//using boost::FOREACH;

BrokerLoader::BrokerLoader()
{
}


BrokerLoader::~BrokerLoader()
{
}


std::vector<Broker> BrokerLoader::LoadSavedBrokers(std::string tFileName)
{
	std::vector<Broker> aBrokerList;

	ptree aBrokerTree;
	std::ifstream aFileStream(tFileName);
	read_xml(aFileStream, aBrokerTree);
	BOOST_FOREACH(ptree::value_type const& aValue, aBrokerTree.get_child("BrokerList"))
	{
		if (aValue.first == "Broker")
		{
			std::string aFilePath = aValue.second.get<std::string>("FilePath");
			aBrokerList.push_back(ParseXML(aFilePath));
		}
	}

	return aBrokerList;
}

Broker BrokerLoader::ParseXML(std::string tFileName)
{
	ptree aBrokerTree;
	std::ifstream aFileStream("Broker\\Saved\\"+tFileName);
	read_xml(aFileStream, aBrokerTree);
	Broker aBroker;
	// traverse pt
	std::vector<Broker> ans;
	BOOST_FOREACH(ptree::value_type const& aValue, aBrokerTree.get_child("Broker")) 
	{
		
			
		//aBroker.m_BrokerGuid = aValue.second.get<int>("BrokerGUID");
		if (aValue.first == "Details") 
		{
			aBroker.m_BrokerGuid = aValue.second.get<int>("BrokerGUID");
			aBroker.m_AlgorithmID = aValue.second.get<int>("AlgorithmID");
			aBroker.m_AlgorithmID = aValue.second.get<int>("AlgorithmID");

			aBroker.m_Budget = 0;
			aBroker.m_BudgetPerMarket = 10000;
			aBroker.m_ProfitPerShare = 0;
			aBroker.m_NetWorth = 0;
			aBroker.m_TotalProfit = 0;
			aBroker.m_BrokerScore = 0;
			aBroker.m_SettingsCount = 50;
			aBroker.m_TotalShareCount = 0;
			int aSettingCount = 0;
			ptree aSettingsNode = aValue.second.get_child("Settings");
			for (int x=0; x<50; x++)
			{
				aBroker.m_Settings[x] = aSettingsNode.get<double>("Setting" + std::to_string(x));
			}
		}
	}



	return aBroker;
}