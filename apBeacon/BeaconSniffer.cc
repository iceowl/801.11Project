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
    bool m;
    //const Tins::PDU::PDUType abc = receivedPDU->pdu_type();
    // Dot11ProbeRequest& probe = pdu.rfind_pdu<Dot11ProbeRequest>();

    switch(receivedPDU->pdu_type()) {

        case(Tins::PDU::PDUType::DOT11_PROBE_REQ): {
            try {
                address_type addr1 = static_cast<Dot11ProbeRequest*>(receivedPDU)->addr1();
                address_type addr2 = static_cast<Dot11ProbeRequest*>(receivedPDU)->addr2();
                string ss = static_cast<Dot11ProbeRequest*>(receivedPDU)->ssid();
                if((! (m = ss.compare(id.mySSID)))|| addr1.is_broadcast()){
                    if(!m) cout << "I was addressed directly ************** " << endl;
                    respond_to_addr = addr2;
                    return false;
                    //RespondToProbe(addr2);
                    //cout << "I replied to " << addr2  << " - "<< ss << endl;
                } else {
                    //cout << "Probe Req " << addr1 << " - " << addr2 << " - " << ss << endl;
                }
            }
            catch(std::exception& e){
                //something should go here
            }
        }

        case(Tins::PDU::PDUType::DOT11_PROBE_RESP): {
            try {
                address_type addr1 = static_cast<Dot11ProbeResponse*>(receivedPDU)->addr1();
                address_type addr2 = static_cast<Dot11ProbeResponse*>(receivedPDU)->addr2();
                string ss = static_cast<Dot11ProbeResponse*>(receivedPDU)->ssid();
                cout << "Probe Rpl " << addr1 << " - " << addr2 << endl;
            }
            catch(std::exception& e){
                //cout << e.what() << endl;
            }
        }

        case(Tins::PDU::PDUType::DOT11_BEACON) : {
            try {
                address_type addr1 = static_cast<Dot11Beacon *>(receivedPDU)->addr1();
                address_type addr2 = static_cast<Dot11Beacon *>(receivedPDU)->addr2();
                string ss = static_cast<Dot11Beacon *>(receivedPDU)->ssid();
                set<std::string>::iterator it = ssids.find(ss);
                if (it == ssids.end()) {
                    ssids.insert(ss);
                    cout << "Beacon    " << addr1 << " - " << addr2 << " - " << ss << endl;
                }
                //cout << "Beacon    " << addr1 << " - " << addr2 << " - " << ss << endl;

            }
            catch(std::exception& e){
                //cout << e.what() << endl;
            }
        }

        case PDU::RAW:break;
        case PDU::ETHERNET_II:break;
        case PDU::IEEE802_3:break;
        case PDU::RADIOTAP:break;
        case PDU::DOT11:break;
        case PDU::DOT11_ACK:break;
        case PDU::DOT11_ASSOC_REQ:break;
        case PDU::DOT11_ASSOC_RESP:break;
        case PDU::DOT11_AUTH:break;
        case PDU::DOT11_BLOCK_ACK:break;
        case PDU::DOT11_BLOCK_ACK_REQ:break;
        case PDU::DOT11_CF_END:break;
        case PDU::DOT11_DATA:break;
        case PDU::DOT11_CONTROL:break;
        case PDU::DOT11_DEAUTH:break;
        case PDU::DOT11_DIASSOC:break;
        case PDU::DOT11_END_CF_ACK:break;
        case PDU::DOT11_MANAGEMENT:break;
        case PDU::DOT11_PS_POLL:break;
        case PDU::DOT11_REASSOC_REQ:break;
        case PDU::DOT11_REASSOC_RESP:break;
        case PDU::DOT11_RTS:break;
        case PDU::DOT11_QOS_DATA:break;
        case PDU::LLC:break;
        case PDU::SNAP:break;
        case PDU::IP:break;
        case PDU::ARP:break;
        case PDU::TCP:break;
        case PDU::UDP:break;
        case PDU::ICMP:break;
        case PDU::BOOTP:break;
        case PDU::DHCP:break;
        case PDU::EAPOL:break;
        case PDU::RC4EAPOL:break;
        case PDU::RSNEAPOL:break;
        case PDU::DNS:break;
        case PDU::LOOPBACK:break;
        case PDU::IPv6:break;
        case PDU::ICMPv6:break;
        case PDU::SLL:break;
        case PDU::DHCPv6:break;
        case PDU::DOT1Q:break;
        case PDU::PPPOE:break;
        case PDU::STP:break;
        case PDU::PPI:break;
        case PDU::IPSEC_AH:break;
        case PDU::IPSEC_ESP:break;
        case PDU::PKTAP:break;
        case PDU::MPLS:break;
        case PDU::UNKNOWN:break;
        case PDU::USER_DEFINED_PDU:break;
    }

    return true;
}

void BeaconSniffer::RespondToProbe() {


    try {

        sequence_lock.lock();


        Dot11ProbeResponse myResponse(respond_to_addr, id.myMac);
        // Let's add an ssid option
        myResponse.ssid(id.mySSID);
        myResponse.addr3(id.myMac); //need for BSSID
        //myResponse.addr4(id.myMac);
        // Our current channel is 8
        myResponse.ds_parameter_set(id.channel);
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

        timespec tp;
        myResponse.timestamp(clock_gettime(CLOCK_MONOTONIC,&tp));

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

        sequence_lock.unlock();

        cout << " responded to  " << respond_to_addr << " as " << id.myMac << endl;


    }
    catch(std::exception& e){
        cout << e.what() << endl;
    }

}

