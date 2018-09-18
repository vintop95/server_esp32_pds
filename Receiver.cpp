#include "Receiver.h"

Receiver::Receiver()
{

}

//{"sender_mac":"", "timestamp":0, "rssi":0, "hashed_pkt":"", "ssid":""}
//void to_json(json& j, const Record& r) {
//        j = json{{"sender_mac", r.sender_mac},
//                 {"timestamp", r.timestamp},
//                 {"rssi", r.rssi},
//                 {"hashed_pkt", r.hashed_pkt},
//                 {"ssid", r.ssid}};
//}

//void from_json(const json& j, Record& r) {
//        r.sender_mac = j.at("sender_mac").get<std::string>();
//        r.timestamp = j.at("timestamp").get<uint32_t>();
//        r.rssi = j.at("rssi").get<int8_t>();
//        r.hashed_pkt = j.at("hashed_pkt").get<std::string>();
//        r.ssid = j.at("ssid").get<std::string>();
//}
