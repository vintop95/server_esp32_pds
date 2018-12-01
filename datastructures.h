/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QObject>
#include <QPointF>

/**
 * Packet received from the ESP32 devices
 * rssi: -35 MOLTO VICINO, -100 MOLTO LONTANO
 *
 * @todo: change operator==
 */
class Packet{
public:
    Packet() {}
    Packet(QString pkt, QString mac):
        sender_mac(mac), hashed_pkt(pkt) {}
    QString sender_mac;
    quint32 timestamp;
    qint8 rssi;
    QString hashed_pkt;
    QString ssid;
    QString espName;

    QString toString(){
        QString str;
        str.append("ESP: ");
        str.append(espName);
        str.append(", MAC: ");
        str.append(sender_mac);
        str.append(", timestamp: ");
        str.append(QString::number(timestamp));
        str.append(", rssi: ");
        str.append(QString::number(rssi));
        str.append(", ssid: ");
        str.append(ssid);
        str.append(", hashed_pkt: ");
        str.append(hashed_pkt);
        return str;
    }

    // TODO: check if is OK
    bool operator==(const Packet& r2) const
    {
        return
        (sender_mac == r2.sender_mac) &&
        //(timestamp == r2.timestamp) && //timestamp could be different
        //(rssi == r2.rssi) &&
        (hashed_pkt == r2.hashed_pkt)
        //(ssid == r2.ssid)
        ;
    }
};

/**
 * Devices detected in the area
 */
class Device{
public:
    Device():pos(0,0) {}
    Device(QString mac, QPointF p):
        sender_mac(mac), pos(p) {}

    QString toString(){
        QString str = "MAC: ";
        str.append(sender_mac);
        str.append(", pos: (");
        str.append(QString::number(pos.x()));
        str.append(", ");
        str.append(QString::number(pos.y()));
        str.append(")");
        return str;
    }

    QString sender_mac; //id of device
    QPointF pos;
};

/**
 * Client ESP32
 */
class ESP32{
private:
    QString name;
    QPointF pos;
public:
    ESP32(QString n, QPointF p = QPointF(0,0)):
    name(n), pos(p){ }

    QString getName() const{
        return name;
    }

    qreal getX() const{
        return pos.x();
    }
    qreal getY() const{
        return pos.y();
    }
    QPointF getPos() const{
        return pos;
    }
    void setPos(qreal xpos, qreal ypos){
        pos = QPointF(xpos, ypos);
    }
};

typedef QSharedPointer<QMap<QString, ESP32>> espMapPtr_t;

#endif // RECORD_H
