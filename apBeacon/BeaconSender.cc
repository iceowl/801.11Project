/*
 * BeaconSender.cc
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */


#include <tins/utils.h>
#include "BeaconSender.h"



using namespace Utils;

BeaconSender::BeaconSender(myID& t_id) : id {t_id} {

	sender = new PacketSender {id.nIface,0,0};
	MakeBeacon();
	MakeRadio();


	// The beacon's ready to be sent!b

}

void BeaconSender::MakeBeacon() {

	beacon = new Dot11Beacon;

	Dot11Beacon::tim_type  myTim(0, 0, 0, byte_array(1));

	Dot11Beacon::country_params  myCountry("US ",
			                     	 	 	     byte_array(1, 1), //first
			                                     byte_array(1, 13), //num channels
			                                     byte_array(1, 20)); //max power


	// Make this a broadcast frame. Note that Dot11::BROADCAST
	// is just the same as "ff:ff:ff:ff:ff:ff"
	beacon->addr1(Dot11::BROADCAST);
	// We'll set the source address to some arbitrary address
	beacon->addr2(id.myMac);
	// Set the bssid, to the same one as above
	beacon->addr3(id.myMac); // need for BSSID
	beacon->addr4(id.myMac);

	// Let's add an ssid option
	beacon->ssid(id.mySSID);
	// Our current channel is 8
	beacon->ds_parameter_set(8);
	// This is our list of supported rates:
	beacon->supported_rates({ 1.0f, 5.5f, 11.0f });

	beacon->interval(100);


	beacon->country(myCountry);
	beacon->tim(myTim);

	// Encryption: we'll say we use WPA2-psk encryption
	beacon->rsn_information(RSNInformation::wpa2_psk());

	Dot11ManagementFrame::capability_information& cap = beacon->capabilities();
	cap.ess(true);
	cap.radio_measurement(true);
	cap.ibss(false);
	cap.privacy(false);
	cap.cf_poll(false);
	cap.cf_poll_req(false);
	cap.immediate_block_ack(false);
	cap.delayed_block_ack(false);
	//cap.channel_agility(false);  //some of these commented out make it impossible to be seen by an iBookPro
	//cap.spectrum_mgmt(false);
	//cap.pbcc(false);
	//cap.short_preamble(false);
	//Dot11ManagementFrame::bss_load_type myLoad (0,255,0);
	//beacon->bss_load(myLoad);
	beacon->seq_num(id.sequence++);
	if(id.sequence > 4095) id.sequence = 100;

}

void BeaconSender::MakeRadio() {

	radio = new RadioTap;
	radio->inner_pdu(beacon);
	radio->antenna(2);
	radio->channel(Utils::channel_to_mhz(8),0x00a0);




}


void BeaconSender::run(){



	while(1){
		sequence_lock.lock();
		try {
			sender->send(*radio,id.nIface);
		}
		catch(std::exception& e){
			std::cout<< "couldn't send beacon - " << e.what() << endl;
		}


		beacon->seq_num(id.sequence++);
		if(id.sequence > 4094) id.sequence = 100;
		sequence_lock.unlock();
		usleep(102400); //102.4 milliseconds
		//MakeBeacon();
		//MakeRadio();
	}
	return;
}


