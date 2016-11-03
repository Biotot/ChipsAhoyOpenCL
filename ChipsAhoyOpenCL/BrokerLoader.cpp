#include "BrokerLoader.h"
#include <stdio.h>
using namespace std;
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


std::vector<std::string> BrokerLoader::LoadSavedBrokers(std::string tFileName)
{
	std::vector<std::string> aBrokerList;

	ptree aBrokerTree;
	std::ifstream aFileStream(tFileName);
	read_xml(aFileStream, aBrokerTree);
	BOOST_FOREACH(ptree::value_type const& aValue, aBrokerTree.get_child("BrokerList"))
	{
		if (aValue.first == "Broker")
		{
			std::string aFilePath = aValue.second.get<std::string>("FilePath");
			if (aFilePath != "StoredBrokers.xml")
			{
				//cout << aFilePath + " LOADING " << endl;
				aBrokerList.push_back(aFilePath);
			}
		}
	}

	return aBrokerList;
}

Broker BrokerLoader::CreateDefaultBroker()
{
	Broker aRetBroker;

	aRetBroker.m_Budget = 0;
	aRetBroker.m_BudgetPerMarket = 1000000;
	aRetBroker.m_ProfitPerShare = 0;
	aRetBroker.m_NetWorth = 0;
	aRetBroker.m_TotalProfit = 0;
	aRetBroker.m_BrokerScore = 0;
	aRetBroker.m_SettingsCount = 56;
	aRetBroker.m_TotalShareCount = 0;
	aRetBroker.m_TotalInvestment = 0;

	return aRetBroker;
}

Broker BrokerLoader::ParseXML(std::string tFileName)
{
	ptree aBrokerTree;
	std::ifstream aFileStream("Broker\\"+tFileName);
	read_xml(aFileStream, aBrokerTree);
	Broker aBroker;
	bool aFailure = false;
	BOOST_FOREACH(ptree::value_type const& aValue, aBrokerTree.get_child("Broker")) 
	{
		
			
		//aBroker.m_BrokerGuid = aValue.second.get<int>("BrokerGUID");
		if (aValue.first == "Details") 
		{
			///aBroker.m_AlgorithmID = aValue.second.get<int>("AlgorithmID");
			aBroker = CreateDefaultBroker();
			int aSettingCount = 0;
			aBroker.m_BrokerGuid = aValue.second.get<int>("BrokerGUID");
			aBroker.m_AlgorithmID = aValue.second.get<int>("AlgorithmID");
			ptree aSettingsNode = aValue.second.get_child("Settings");
			for (int x=0; x<aBroker.m_SettingsCount; x++)
			{
				try
				{
					string aSettingString = aSettingsNode.get<string>("Setting" + std::to_string(x));

					int aVal = -10;
					aVal = atof(aSettingString.c_str());
					if (aVal != INT_MAX)
					{
						aBroker.m_Settings[x] = aVal;// aSettingsNode.get<double>("Setting" + std::to_string(x));
					}
					else
					{
						aFailure = true;
						cout << aBroker.m_BrokerGuid << " HAS A BAD FILE : SETTINGS"<<x << endl;
						aBroker.m_BrokerGuid = 0;
					}
				}
				catch (int e)
				{
					aFailure = true;
					cout << aBroker.m_BrokerGuid << " HAS A BAD FILE" << endl;
					aBroker.m_BrokerGuid = 0;
				}
			}
		}
	}



	return aBroker;
}