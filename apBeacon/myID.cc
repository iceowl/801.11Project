/*
 * myMac.cc
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */

#include "myID.h"

myID::myID(std::string x, std::string y, HWAddress<6> z, short chan) : myIface{x}, mySSID{y}, myMac{z}, channel{chan} {
	sequence = 100;
};


