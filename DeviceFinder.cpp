#include "DeviceFinder.h"

DeviceFinder::DeviceFinder(int n): ESPNo(n)
{
    for(int i=0; i<ESPNo; ++i){
        QString newName = "ESP" + QString::number(i);
        esp.insert(
           newName,
           ESP32(newName) );
    }
}

void DeviceFinder::setESPPos(QString devName, float xpos, float ypos)
{
    esp.find(devName).value().setPos(xpos, ypos);
}

void DeviceFinder::pushRecord(Record r)
{
    //check if hash content already exists
    //controllerei prima il sender_mac per
    //vedere se il dispositivo è stato trovato almeno una volta
    records.push_back(r);
}
