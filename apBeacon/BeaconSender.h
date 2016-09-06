/*
 * BeaconSender.h
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */

#ifndef BEACONSENDER_H_
#define BEACONSENDER_H_

#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <unistd.h>

#include "myID.h"


using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

using namespace Tins;


class BeaconSender {
public:
	const std::string myIface;
	BeaconSender(myID& t_id);
	void MakeBeacon();
	void MakeRadio();
	void run();
    myID& id;
    //PacketSender *sender;
	PacketSender sender {id.nIface,0,0};
    Dot11Beacon *beacon;
    RadioTap *radio;

};



#endif /* BEACONSENDER_H_ */
