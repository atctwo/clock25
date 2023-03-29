#include "wifi.h"
#include "rtc.h"
#include "settings.h"
#include "pins.h"
#include "log.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncUDP.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG_TAG "ntp"
#define NTP_TIMESTAMP_DELTA 2208988800

AsyncUDP ntp_server;
uint32_t ntp_server_reference_time = 0;

void get_time_from_ntp()
{
    if (is_wifi_connected())
    {
        logi(LOG_TAG, "Setting time using NTP");

        // get ntp settings
        std::string ntp_server_1 = get_setting("<ntp>", "NTP Server", "pool.ntp.org");
        long gmt_offset   = std::stoi(get_setting("<ntp>", "GMT Offset", "0"));
        long dst_offset   = std::stoi(get_setting("<ntp>", "DST Offset", "3600"));
        std::string tz    = get_setting("<ntp>", "POSIX Timezone", "GMT0BST,M3.5.0/1,M10.5.0");
        logi(LOG_TAG, "server=%s, gmt=%d, dst=%d", ntp_server_1.c_str(), gmt_offset, dst_offset);

        // get time from ntp
        configTime(gmt_offset, dst_offset, ntp_server_1.c_str());

        // set timezone (if a timezone has been specified)
        if (!tz.empty()) {
            logi(LOG_TAG, "setting timezone to %s", tz.c_str());
            setenv("TZ", tz.c_str(), 1);
            tzset();
        }

        // get time and write to rtc
        struct tm timeinfo;
        getLocalTime(&timeinfo);
        DateTime dt(timeinfo.tm_year - 100, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        set_rtc_time(dt);

        // update ntp server reftime
        ntp_server_reference_time = dt.unixtime() + NTP_TIMESTAMP_DELTA;

        logi(LOG_TAG, "time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    else logw(LOG_TAG, "Can't use NTP client because i'm not connected to wifi");
}

// from https://github.com/DennisSc/PPS-ntp-server/blob/master/src/main.cpp
uint64_t DateTimeToNtp64(DateTime dt) 
{
    uint64_t ntpts;

    ntpts = (((uint64_t)dt.unixtime()) << 32);
    ntpts |= (uint64_t)(0 * 4294.967296);

    return (ntpts);
}

void ntp_server_callback(AsyncUDPPacket packet)
{
    // store the time the request arrived at the server
    uint32_t ntp_time_org = get_rtc_time().unixtime() + NTP_TIMESTAMP_DELTA;

    USBSerial.print("UDP Packet Type: ");
    USBSerial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
    USBSerial.print(", From: ");
    USBSerial.print(packet.remoteIP());
    USBSerial.print(":");
    USBSerial.print(packet.remotePort());
    USBSerial.print(", To: ");
    USBSerial.print(packet.localIP());
    USBSerial.print(":");
    USBSerial.print(packet.localPort());
    USBSerial.print(", Length: ");
    USBSerial.print(packet.length());
    USBSerial.print(", Data: ");
    USBSerial.write(packet.data(), packet.length());
    USBSerial.println();
    //reply to the client
    // packet.printf("Got %u bytes of data", packet.length());

    // create NTP response packet
    // copy the request
    char response[packet.length()];
    strcpy(response, (const char *)packet.data());

    // set ntp mode
    // 00...... Leap Indicator      no leap second adjustment
    // ..100... NTP Version         version 4
    // .....100 Packet Mode         server response
    response[0] = 0b00100100;

    // set stratum level
    response[1] = 15;            // lowest level of stratum because idk what to use

    // set poll interval - maximum interval between NTP messages in seconds
    // response[2] = 0;

    // set clock precision
    // response[3] = 0;

    // set root delay
    // response[4] = 0;
    // response[5] = 0;
    // response[6] = 0;
    // response[7] = 0;

    // set root dispersion
    // response[8] = 0;
    // response[9] = 0;
    // response[10] = 0;
    // response[11] = 0;

    // set reference identifier
    std::string ntp_server_1 = get_setting("<ntp>", "NTP Server", "pool.ntp.org");
    struct hostent *host_details = gethostbyname(ntp_server_1.c_str());
    if (host_details) 
    {
        // store the first ip address
        struct in_addr **addr_list = (struct in_addr**) host_details->h_addr_list;
        uint32_t addr = addr_list[0]->s_addr;
        response[12] = (addr >> (0)) & 0xff;
        response[13] = (addr >> (8)) & 0xff;
        response[14] = (addr >> (16)) & 0xff;
        response[15] = (addr >> (24)) & 0xff;
        USBSerial.printf("%d.", (addr >> (0)) & 0xff);
        USBSerial.printf("%d.", (addr >> (8)) & 0xff);
        USBSerial.printf("%d.", (addr >> (16)) & 0xff);
        USBSerial.printf("%d\n", (addr >> (24)) & 0xff);
    } else 
    {
        logw(LOG_TAG, "NTP Server: failed to resolve hostname\n");
    }
    
    // set reference timestamp - time the system clock was last set
    // bytes 16, 17, 18, 19, 20, 21, 22, 23
    USBSerial.println(ntp_server_reference_time);
    response[16] = (ntp_server_reference_time >> (24)) & 0xff;
    response[17] = (ntp_server_reference_time >> (16)) & 0xff;
    response[18] = (ntp_server_reference_time >> (8)) & 0xff;
    response[19] = (ntp_server_reference_time >> (0)) & 0xff;
    response[20] = 0;
    response[21] = 0;
    response[22] = 0;
    response[23] = 0;

    // set originate timestamp - time the request departed the client - leave as-is
    // bytes 24, 25, 26, 27, 28, 29, 30, 31 
    for (int i = 24; i < 32; i++)
    {
        response[i] = packet.data()[i+16];
    }

    // set recieve timestamp - time the request arrived at the server
    // bytes 32, 33, 34, 35, 36, 37, 38, 39
    response[32] = (ntp_time_org >> (24)) & 0xff;
    response[33] = (ntp_time_org >> (16)) & 0xff;
    response[34] = (ntp_time_org >> (8)) & 0xff;
    response[35] = (ntp_time_org >> (0)) & 0xff;
    response[36] = 0;
    response[37] = 0;
    response[38] = 0;
    response[39] = 0;


    // set transit timestamp - time the reply left the server
    // bytes 40, 41, 42, 43, 44, 45, 46, 47
    uint32_t ntp_time_xmt = get_rtc_time().unixtime() + NTP_TIMESTAMP_DELTA;
    response[40] = (ntp_time_xmt >> (24)) & 0xff;
    response[41] = (ntp_time_xmt >> (16)) & 0xff;
    response[42] = (ntp_time_xmt >> (8)) & 0xff;
    response[43] = (ntp_time_xmt >> (0)) & 0xff;
    response[44] = 0;
    response[45] = 0;
    response[46] = 0;
    response[47] = 0;

    // send response
    packet.write((uint8_t *)response, packet.length());
}

bool setup_ntp_server()
{
    logi(LOG_TAG, "Setting up NTP server");

    if (!WiFi.isConnected()) {
        logw(LOG_TAG, "Failed to setup NTP server - wifi disconnected");
        return false;
    }

    // listen on UDP port 123
    if (ntp_server.listen(123)) {
        
        ntp_server.onPacket(ntp_server_callback);
        MDNS.addService("ntp", "udp", 123);

    } else {
        logw(LOG_TAG, "Failed to setup NTP server - failed to listen to port 123");
        return false;
    }

    return true;
}

void check_ntp_server()
{

}
