#include "stdlib.h"
#include "pcapplusplus/PcapLiveDeviceList.h"
#include "pcapplusplus/PlatformSpecificUtils.h"
/**
* A struct for collecting packet statistics
*/

static bool onPacketArrivesBlockingMode(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);
struct PacketStats
{
    int totalCount;
    
    /**
    * Clear all stats
    */
    void clear() 
    {
        totalCount = 0;
    }

    /**
    * C'tor
    */
    PacketStats() { clear(); }

    /**
    * Collect stats from a packet
    */
    void consumePacket(pcpp::Packet& packet)
    {
       totalCount++;  
    }

    /**
    * Print stats to console
    */
    void printToConsole()
    {
	printf("%d", totalCount); 
    }
};


int main(){

// IPv4 address of the interface we want to sniff
std::string interfaceIPAddr = "10.176.150.92";

// find the interface by IP address
pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
if (dev == NULL)
{
    printf("Cannot find interface with IPv4 address of '%s'\n", interfaceIPAddr.c_str());
    exit(1);
}

//open device
if (!dev->open())
{
    printf("Cannot open device\n");
    exit(1);
}

PacketStats stats;
// clear stats
stats.clear();


// set filter
pcpp::PortFilter portFilter(502, pcpp::SRC_OR_DST);
//pcpp::ProtoFilter protocolFilter(pcpp::TCP);

pcpp::AndFilter andFilter;
andFilter.addFilter(&portFilter);
//andFilter.addFilter(&protocolFilter);

dev->setFilter(andFilter);

// start capturing in blocking mode. Give a callback function to call to whenever a packet is captured, the stats object as the cookie and a 10 seconds timeout
dev->startCaptureBlockingMode(onPacketArrivesBlockingMode, &stats, 1);

// thread is blocked until capture is finished

stats.printToConsole();
}

/**
* a callback function for the blocking mode capture which is called each time a packet is captured
*/
static bool onPacketArrivesBlockingMode(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
    // extract the stats object form the cookie
    PacketStats* stats = (PacketStats*)cookie;

    // parsed the raw packet
    pcpp::Packet parsedPacket(packet);

    // collect stats from packet
    stats->consumePacket(parsedPacket);

    // return false means we don't want to stop capturing after this callback
    return false;
}

