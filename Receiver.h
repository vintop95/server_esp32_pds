#ifndef RECEIVER_H
#define RECEIVER_H

#include "main.h"

struct Record{
    std::string sender_mac;
    uint32_t timestamp;
    int8_t rssi;
    std::string hashed_pkt;
    std::string ssid;
};

class Receiver
{
public:
    //friend void to_json(json& j, const Record& r);
   // friend void from_json(const json& j, Record& r);
    Receiver();
};

#endif // RECEIVER_H
