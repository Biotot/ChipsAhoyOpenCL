#pragma once
#include "Structs.h"
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

class BrokerLoader
{
public:
	BrokerLoader();
	~BrokerLoader();
	std::vector<Broker> LoadSavedBrokers(std::string tFileName);
	Broker ParseXML(std::string tFileName);
};

