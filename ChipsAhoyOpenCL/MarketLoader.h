#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <ctime>
#include <sstream>
#include "Structs.h"
#include <vector>

using namespace std;

#define FIVEDAY 5
#define FIFTEENDAY 15
#define THIRTYDAY 20
#define THREEMONTH 60
#define SIXMONTH 125
#define ONEYEAR 250



class MarketLoader
{
public:
	MarketLoader();
	MarketLoader(string tMarketName, int tYear, int tMarketCount);
	~MarketLoader();
	Market m_Market;
	MarketPrice m_LastMarket;
	MarketPrice CreatePrice(string tPriceString);
	void LoadAverages(MarketPrice *tTargetPrice, PriceAverage *tOpen, PriceAverage *tHigh, PriceAverage *tLow, PriceAverage *tClose, PriceAverage *tVolume);
};

