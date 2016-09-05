/*
 * BeaconSniffer.h
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */

#ifndef BEACONSNIFFER_H_
#define BEACONSNIFFER_H_

#define TINS_HAVE_DOT11

#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <unistd.h>
#include <time.h>
#include "myID.h"


using std::set;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::runtime_error;

using namespace Tins;

class BeaconSniffer  {

public:
	BeaconSniffer(myID& t_id);
   	void RespondToProbe();
    void run();

private:
    typedef Dot11::address_type address_type;
    typedef set<address_type> ssids_type;
    bool callback(PDU& pdu);
    myID& id;
	SnifferConfiguration config;
	PacketSender sender {id.nIface,0,0};
    set<std::string> ssids;
    set<address_type> probed_addys;
    set<address_type> response_addys;
    set<address_type> responded_to_addys;
    address_type respond_to_addr;
};



#endif /* BEACONSNIFFER_H_ */
