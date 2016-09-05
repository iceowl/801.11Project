
#include "BeaconSniffer.h"
#include "BeaconSender.h"
#include <thread>

#include "myID.h"

using namespace Tins;

static void SenderWrapper (BeaconSender* sb);

void SenderWrapper(BeaconSender* sb){
	sb->run();
}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " <<* argv << " <interface>" << endl;
        return 1;
    }



    string interface = argv[1];

    myID mID {interface, "CoffeeTime", "00:c0:ca:8f:4c:cf"};
    BeaconSender bSender {mID};
    //bSender.run();//only for testing purposes
    std::thread t(SenderWrapper,&bSender);
    t.detach();
    BeaconSniffer bSniffer {mID};
    //BeaconSniffer *bSniffer = new BeaconSniffer(interface); //the old old way
    while(1) {
    	bSniffer.run();
    	bSniffer.RespondToProbe();
    }
}
