/*
 * BeaconSniffer.cc
 *
 *  Created on: Sep 3, 2016
 *      Author: root
 */


#include "BeaconSniffer.h"
#include "PDUTypes_S.h"


BeaconSniffer::BeaconSniffer(myID& s) : id{s} {

}

//void BeaconSniffer::run(const std::string& iface) {
void BeaconSniffer::run(){
	config.set_promisc_mode(true);
	//bconfig.set_filter("type mgt subtype probereq");
	//config.set_filter("type mgt subtype beacon");
	config.set_rfmon(true);
	Sniffer sniffer {id.myIface, config};
	sniffer.sniff_loop(make_sniffer_handler(this, &BeaconSniffer::callback));
}

bool BeaconSniffer::callback(PDU& pdu) {
	PDU* receivedPDU = pdu.inner_pdu();

	//const Tins::PDU::PDUType abc = receivedPDU->pdu_type();
	// Dot11ProbeRequest& probe = pdu.rfind_pdu<Dot11ProbeRequest>();

	switch(receivedPDU->pdu_type()) {

	case(Tins::PDU::PDUType::DOT11_PROBE_REQ): {
		try {
			address_type addr1 = static_cast<Dot11ProbeRequest*>(receivedPDU)->addr1();
			address_type addr2 = static_cast<Dot11ProbeRequest*>(receivedPDU)->addr2();
			string ss = static_cast<Dot11ProbeRequest*>(receivedPDU)->ssid();
			if(ss.compare(id.mySSID)) {
				respond_to_addr = addr2;
				return false;
				//RespondToProbe(addr2);
				//cout << "I replied to " << addr2  << " - "<< ss << endl;
			}
			ssids_type::iterator it = probed_addys.find(addr2);
			if(it == probed_addys.end()) {
				probed_addys.insert(addr2);
				cout << "Probe Req " << addr1 << " - " << addr2 << " - " << ss << endl;
			}
		}
		catch(std::exception& e){
			//cout << e.what() << endl;
		}
	}

	case(Tins::PDU::PDUType::DOT11_PROBE_RESP): {
		try {
			address_type addr1 = static_cast<Dot11ProbeResponse*>(receivedPDU)->addr1();
			address_type addr2 = static_cast<Dot11ProbeResponse*>(receivedPDU)->addr2();
			string ss = static_cast<Dot11ProbeResponse*>(receivedPDU)->ssid();
			ssids_type::iterator it = response_addys.find(addr2);
			if(it == response_addys.end()) {
				response_addys.insert(addr2);
				cout << "Probe Rpl " << addr1 << " - " << addr2 << endl;
			}
		}
		catch(std::exception& e){
			//cout << e.what() << endl;
		}
	}

	case(Tins::PDU::PDUType::DOT11_BEACON) : {
		try {
			address_type addr1 = static_cast<Dot11Beacon*>(receivedPDU)->addr1();
			address_type addr2 = static_cast<Dot11Beacon*>(receivedPDU)->addr2();
			string ss = static_cast<Dot11Beacon*>(receivedPDU)->ssid();
			set<std::string>::iterator it = ssids.find(ss);
			if (it == ssids.end()) {
				ssids.insert(ss);
				cout << "Beacon    " << addr1 << " - " << addr2 << " - " << ss << endl;
			}
		}
		catch(std::exception& e){
			//cout << e.what() << endl;
		}
	}

	default: {
		if(Tins::PDU::PDUType::DOT11_BEACON == 1223)
			cout << "pdu type = " << PDUTypes_string[receivedPDU->pdu_type()] << endl;
	}
	}

	return true;
}

void BeaconSniffer::RespondToProbe() {


	try {


		Dot11ProbeResponse myResponse(respond_to_addr, id.myMac);
		// Let's add an ssid option
		myResponse.ssid(id.mySSID);
		myResponse.addr3(id.myMac); //need for BSSID
		//myResponse.addr4(id.myMac);
		// Our current channel is 8
		myResponse.ds_parameter_set(8);
		// This is our list of supported rates:
		myResponse.supported_rates({ 1.0f, 5.5f, 11.0f });

		// Encryption: we'll say we use WPA2-psk encryption
		myResponse.rsn_information(RSNInformation::wpa2_psk());
		myResponse.interval(100);
		Dot11Beacon::country_params  myCountry("US ",
				byte_array(1, 1), //first
				byte_array(1, 13), //num channels
				byte_array(1, 20)); //max power

		Dot11ManagementFrame::capability_information& cap = myResponse.capabilities();
		cap.ess(true);

		myResponse.country(myCountry);

		Dot11ManagementFrame::quiet_type myQuiet (0,0,0,0);

		myResponse.quiet(myQuiet);

		myResponse.tpc_report(200,0); //power, delay



		Dot11ManagementFrame::bss_load_type myLoad (0,255,0);
		myResponse.bss_load(myLoad);
        myResponse.seq_num(id.sequence++);
		if(id.sequence > 4094) id.sequence = 100;
		//MakeBeacon();
		RadioTap radio = RadioTap();
		radio.inner_pdu(myResponse);
		radio.antenna(2);
		radio.channel(Utils::channel_to_mhz(8),0x00a0);

		radio.send(sender,id.nIface);

		cout << " responded to  " << respond_to_addr << " as " << id.myMac << endl;

	}
	catch(std::exception& e){
		cout << e.what() << endl;
	}

}

