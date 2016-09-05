/*
 * myMac.h
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */

#ifndef MYID_H_
#define MYID_H_

#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <unistd.h>


using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

using namespace Tins;


class myID {
public:
	myID(std::string x, std::string y, HWAddress<6> z);
	const std::string myIface;
	const std::string mySSID;
	const HWAddress<6> myMac;
	NetworkInterface nIface{myIface};
	unsigned short sequence = 0;


};

#endif /* MYID_H_ */
