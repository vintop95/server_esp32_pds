/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "devicefinder.h"

#include <limits>
#include <cmath>

DeviceFinder* DeviceFinder::instance;

/**
 * @brief Constructor of DeviceFinder
 * Initializes the ESP32 devices calling them for each n
 * ESPn (ESP0, ESP1, ESP2...)
 */
DeviceFinder::DeviceFinder():
    db("server_esp32_pds.sqlite3")
{
    // in order to let the command of printing
    // from the gui
    setWindow(MainWindow::getInstance());

    connect(pWin, &MainWindow::logCurrDev,
            this, &DeviceFinder::logCurrentDevices);
}

DeviceFinder* DeviceFinder::getInstance(espMapPtr_t list, QString dbPath)
{
    if (instance == nullptr){
        instance = new DeviceFinder();  
    }
    instance->init(list, dbPath);

    return instance;
}

void DeviceFinder::init(espMapPtr_t list, QString dbPath)
{
    if(dbPath != "server_esp32_pds.sqlite3"){
        db.setPath(dbPath);
    }
    if(!list.isNull() && esp32s.isNull()){
        esp32s = list;
        //inizializza vettore interazioni
        for(auto espName : esp32s->keys()){
            espInteracted.insert(espName, false);

        }
        writeLog("HO INIZIALIZZATO VETTORE INTERAZIONI", QtWarningMsg);
        for(auto esp:espInteracted.keys()){
                writeLog("SCHEDA " + esp + " ha interagito: " + (espInteracted.value(esp) ? "true" : "false"), QtWarningMsg);
        }

        // setta per la prima volta il timestamp che definisce
        // l'inizio della finestra di ascolto
        lastTimestamp = QDateTime::currentDateTime().toTime_t();
        // generatePackets();
    }

}

/**
 * @brief Initialize the chart in the window
 * Sets a timer that is called every chartPeriod
 * and calls updateChart()
 */
void DeviceFinder::setChartUpdateTimer()
{
    connect(&chartUpdateTimer, &QTimer::timeout,
    [=](){
        // The flag is used when devices_in_window is updated
        // When the timer of 5 minutes is called, we must clear the
        // devices continously found in the last 5 minutes
        hasTimerReset = true;
        int nOfDevices = this->countCurrentDevices();
        devices_in_window.clear();

        if (pWin != nullptr){
            pWin->getChart()->updateChart(nOfDevices);
        }
    });
    chartUpdateTimer.setInterval(CHART_PERIOD_MS);
    chartUpdateTimer.start();
}
//return true if the passed string is an allowed ESP name, specified in the settings file
bool DeviceFinder::isAllowedESPName(QString name)
{
    auto it = esp32s->find(name);
    return it!=esp32s->end();
}

/**
 * @brief Sets the windows containing the chart and
 * initializes the chart
 */
void DeviceFinder::setWindow(MainWindow *w)
{
    pWin = w;
    setChartUpdateTimer();
}

void DeviceFinder::resetInteractionsWithEsp()
{
    for(auto it=espInteracted.begin(); it!=espInteracted.end(); it++){
        it.value() = false;
    }
}


/**
 * @brief Pushes the packets received from ClientHandler in a list
 * and checks if the packets with the same HASH are received from
 * all the esp32 clients, if yes add the device detected to a list
 *
 * @param Packet to push
 *
 */
void DeviceFinder::pushPacketInBuffer(Packet p)
{

    writeLog("#DeviceFinder");
    writeLog("Received pkt\n" + p.toString());
    Logger::saveCsv(p);
    if(isAllowedESPName(p.espName)){
        packets.push_back(p);
    }
}

//TODO: finire
bool DeviceFinder::insertBufferedPacketsIntoDB(QString espName)
{
    setEspInteracted(espName);

    bool res = db.insertPackets(packets);
    if(res){//if insertion in database was succesfull we can clear the vector
        packets.clear();
        if(canStartPacketProcessing()){
            writeLog("CLEAR PLOT");
            pWin->getAreaPlot()->clearDevices();
            processLocationsFromPackets();//and buffer devices
            insertBufferedDevicesIntoDB();
        }
        return true;
    }else{
        writeLog("ERROR IN INSERTING " + QString::number(packets.size()) + " PACKETS TO DATABASE. "
                 "We keep them for the next try.", QtWarningMsg);
        return false;
    }
}
//return true if it has been contacted by all the ESPs
bool DeviceFinder::canStartPacketProcessing()
{
    for(auto truth_value:espInteracted){
        if(!truth_value)
            return false;
    }
    return true;
}

void DeviceFinder::setEspInteracted(QString espName)
{
    if(isAllowedESPName(espName)){
        espInteracted[espName] = true;
    }
}

void DeviceFinder::processLocationsFromPackets()
{
    writeLog("LOCATIONS PROCESSING FUNCTION CALLED! WORK IN PROGRESS...", QtInfoMsg);
    //TODO: leggi dalla tabella 'Packets' i dati, aggregali e inseriscili nella tabella 'Location'.
    //Ricordati anche di aggiornare l'interfaccia grafica
    //altro giro altra corsa:
    for(auto esp:espInteracted.keys()){
            writeLog("SCHEDA " + esp + " ha interagito: " + (espInteracted.value(esp) ? "true" : "false"), QtWarningMsg);
    }
    resetInteractionsWithEsp();
    writeLog("HO RESETTATO", QtWarningMsg);
    for(auto esp:espInteracted.keys()){
            writeLog("SCHEDA " + esp + " ha interagito: " + (espInteracted.value(esp) ? "true" : "false"), QtWarningMsg);
    }
    //riempiamo la mappa tramite una query al database
    avgRssiMap_t avgRssiMap = db.calculateAvgRssi(getEspNo(), lastTimestamp);
    //iteriamo per ogni dispositivo
    for(auto deviceMac:avgRssiMap.keys()){
        //iteriamo per ogni entry (elemento del vector) relativa a quel dispositivo
        //definiamo un riferimento al vector per avere un alias più semplice
        const QVector<QPair<QString, double>>& avgRssiVector = avgRssiMap.value(deviceMac);
        //if we have more than 2 boards

        QPointF position;

        // SE IL NUMERO DI ESP32 È ALMENO 3 FACCIAMO TRILATERAZIONE
        if(getEspNo() >= 3){
            //prendiamo le posizioni delle 3 schedine più vicine a quel dispositivo
            QPointF firstEspPos = esp32s->value(avgRssiVector[0].first).getPos();
            QPointF secondEspPos = esp32s->value(avgRssiVector[1].first).getPos();
            QPointF thirdEspPos = esp32s->value(avgRssiVector[2].first).getPos();
            //prendiamo le distanze stimate da quelle schedine al dispositivo
            double distFromFirstEsp = calculateDistance(avgRssiVector[0].second);
            double distFromSecondEsp = calculateDistance(avgRssiVector[1].second);
            double distFromThirdEsp = calculateDistance(avgRssiVector[2].second);
            writeLog("DISPOSITIVO " + deviceMac + " SI STIMA A:\n" +
                     QString::number(distFromFirstEsp) + " da (" + QString::number(firstEspPos.rx()) + "; " + QString::number(firstEspPos.ry()) + ")\n" +
                     QString::number(distFromSecondEsp) + " da (" + QString::number(secondEspPos.rx()) + "; " + QString::number(secondEspPos.ry()) + ")\n" +
                     QString::number(distFromThirdEsp) + " da (" + QString::number(thirdEspPos.rx()) + "; " + QString::number(thirdEspPos.ry()) + ")\n",
                     QtWarningMsg);
            position = trilateration(firstEspPos, secondEspPos, thirdEspPos,
                                                      distFromFirstEsp, distFromSecondEsp, distFromThirdEsp);
            writeLog("POSSIBILE POSIZIONE: (" + QString::number(position.rx()) + "; " + QString::number(position.ry())+ ")",
                     QtWarningMsg);


        }else{//if we have 2 boards (MINIMUM ALLOWED)
            //prendiamo le posizioni delle 2 schedine più vicine a quel dispositivo (cioè le uniche)
            QPointF firstEspPos = esp32s->value(avgRssiVector[0].first).getPos();
            QPointF secondEspPos = esp32s->value(avgRssiVector[1].first).getPos();
            //prendiamo le distanze stimate da quelle schedine al dispositivo
            double distFromFirstEsp = calculateDistance(avgRssiVector[0].second);
            double distFromSecondEsp = calculateDistance(avgRssiVector[1].second);
            writeLog("DISPOSITIVO " + deviceMac + " SI STIMA A:\n" +
                     QString::number(distFromFirstEsp) + " da (" + QString::number(firstEspPos.rx()) + "; " + QString::number(firstEspPos.ry()) + ")\n" +
                     QString::number(distFromSecondEsp) + " da (" + QString::number(secondEspPos.rx()) + "; " + QString::number(secondEspPos.ry()) + ")\n",
                     QtWarningMsg);
            position = bilateration(firstEspPos, secondEspPos,
                                            distFromFirstEsp, distFromSecondEsp);
            if(isnan(position.rx())){
                writeLog("Le due circonferenze non si intersecano", QtWarningMsg);
            }
            writeLog("POSSIBILE POSIZIONE: (" +
                     QString::number(position.rx()) + "; " + QString::number(position.ry())+ ")",
                     QtWarningMsg);
        }


        pushDeviceInBuffer( Device(deviceMac, position, lastTimestamp) );

    }

    //aggiorniamo il timestamp per la prossima finestra
    lastTimestamp = QDateTime::currentDateTime().toTime_t();
    //db.test_2();
}

/**
 * @brief Push a new device into the list
 *
 * @param Device to add
 *
 * @todo save in packet the actual position device
 */
void DeviceFinder::pushDeviceInBuffer(Device d)
{
    writeLog("#DeviceFinder");
    writeLog("Added/updated device " + d.sender_mac);

    // TODO: save in packet the actual position device

    // If there is already an item with the key, that
    // item's value is replaced with value.
    devices.insert(d.sender_mac, d);
    pWin->getAreaPlot()->addDevice(d.pos.x(),d.pos.y());
}

void DeviceFinder::updateDevicesInWindow() {
    writeLog("Update Devices in window", QtInfoMsg);

    // currentDevices contains all the mac of the devices detected
    // in the last cycle of about 1 minute
    QSet<QString> currentDevices;
    for (auto device: devices) {
        currentDevices.insert(device.sender_mac);
    }

    // Se il timer dei 5 minuti è appena scattato, dobbiamo riempire
    // devices_in_window con tutti i device rilevati nell'ultimo ciclo di 1 minuto
    if (hasTimerReset) {
        writeLog("START POPULATE DEVICES_IN_WINDOW", QtInfoMsg);
        // Aggiungiamo i device rilevati in un vettore temporaneo che
        // contiene i device rilevati continuativamente nella finestra.
        for (auto device: devices) {
            devices_in_window.insert(device.sender_mac);
        }
        hasTimerReset = false;
    } else {
        // Nel prossimo ciclo avremo soltanto i device rilevati continuativamente
        // a partire dall'ultima volta che il timer di 5 minuti è scattato
        writeLog("INTERSECT", QtInfoMsg);
        devices_in_window = devices_in_window.intersect(currentDevices);
    }

    writeLog("devices_in_window content:", QtDebugMsg);
    for (auto device: devices_in_window) {
        writeLog(device);
    }


    writeLog("devices content BEFORE:", QtDebugMsg);
    for (auto device: devices) {
        writeLog(device.sender_mac);
    }

    // Puliamo il buffer dei devices relativo al ciclo di 1 minuto appena terminato
    devices.clear();

    writeLog("devices content AFTER:", QtDebugMsg);
    for (auto device: devices) {
        writeLog(device.sender_mac);
    }
}

bool DeviceFinder::insertBufferedDevicesIntoDB() {

    bool res = db.insertDevices(devices.values());
    if(res){//if insertion in database was succesfull we can clear the vector
        updateDevicesInWindow();
        return true;
    }else{
        writeLog("ERROR IN INSERTING " + QString::number(devices.size()) + " DEVICES TO DATABASE. "
                 "We keep them for the next try.", QtWarningMsg);
        return false;
    }
}

/**
 * @brief a log of the current devices
 *
 * @todo: perfezionare
 */
void DeviceFinder::logCurrentDevices()
{
    writeLog("+++ DEVICES FOUND: +++", QtInfoMsg);
    if(devices.isEmpty()){
        writeLog("NONE", QtInfoMsg);
    }
    for(auto d : devices){
        writeLog(d.toString(), QtInfoMsg);
    }
    writeLog("+++++++++++++++++", QtInfoMsg);
}

/**
 * @brief Counts the number of current devices in the area
 *
 * @return number of current devices in the area
 *
 * @todo completare
 */
int DeviceFinder::countCurrentDevices()
{
    return devices_in_window.size();
}

int DeviceFinder::getEspNo()
{
    return esp32s->size();
}


// https://forums.estimote.com/t/determine-accurate-distance-of-signal/2858/2
// https://gist.github.com/eklimcz/446b56c0cb9cfe61d575
// https://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing/20434019#20434019
/**
 * @brief It calculates the distance between the ESP32 client and the device
 * using the rssi value
 *
 * @param rssi
 *
 * @return distance
 *
 * @todo completare
 */
double DeviceFinder::calculateDistance(double rssi) {

    // TX-power-level == RSSI at 1m distance
    // esp txPower
    // https://www.esp32.com/viewtopic.php?t=5359

    if (rssi >= 0) {
        return -1.0;
    }

    double txCalibratedPower = -59; //hard coded power value. Usually ranges between -59 to -65
    double ratio_db = txCalibratedPower - rssi;
    double ratio_linear =  pow(10, ratio_db / 10);

    double r = sqrt(ratio_linear);
    return r;
}

/**
 * @brief Auxiliary struct, for sorting packets by their rssi
 */
struct sort_rssi_desc
{
    inline bool operator() (const Packet& r1, const Packet& r2)
    {
        //writeLog(QString::number(r1.rssi) + ">" + QString::number(r2.rssi) + "?");
        return ( r1.rssi > r2.rssi );
    }
};

/**
 * @brief It calculates the position of the device identified by the
 * sender_mac in the Packet
 *
 * https://trello.com/c/X2EOGoxG
 * HOW IT SHOULD WORK WITH SQL:
 * SQL Query returns (sender_mac, espName, avgRssi)
 * for all sender_mac
 *  we take the packets that has the same sender_mac (and hashed_pkt)
 *  and we calculate the position with avgRssi
 * we push all the devices with their position in the 'device' table
 * (or in a QVector<Device>)
 *
 *  last_ts = the timestamp of the last time we calculated the positions
 *  of devices
 *
 * @param Packet containing the device
 * it's the last received for calculation purposes from the last board
 *
 * @return Position in space of the device detected
 *
 * @todo completare
 */
/*QPointF DeviceFinder::calculatePosition(Packet lastPacket)
{
    writeLog("#DeviceFinder");

    QVector<Packet> packetsOrdered;
    for(Packet genericPacket : packets){
        auto it = esp32s->find(genericPacket.espName);
        bool exists = (it != esp32s->end());
        if(exists && genericPacket == lastPacket){
            packetsOrdered.push_back(genericPacket);
        }
    }

    // take the 3 packets with the most powerful rssi
    std::sort(packetsOrdered.begin(), packetsOrdered.end(), sort_rssi_desc());

    QList<ESP32> espChosen;
    writeLog("PRINTING PACKETS ORDERED FOR RSSI");
    for(Packet packetOrdered : packetsOrdered){
        writeLog(packetOrdered.toString());

        ESP32 espToPush("");
        auto it = esp32s->find(packetOrdered.espName);
        bool exists = (it != esp32s->end());
        if(exists){
            espToPush = esp32s->value(packetOrdered.espName, ESP32(""));
            espChosen.push_back(espToPush);
        }
    }


//    // DEBUG: test di trilateration
//    QPointF pos = trilateration(QPointF(0,0), QPointF(2,0), QPointF(0,2),
//            1.0,
//            sqrt(5.0),
//            1.0);

    // check if espChosen contains all the esp needed
    QPointF pos;

    if(espChosen.size() <= 1){
        pos.setX(std::numeric_limits<double>::quiet_NaN());
        pos.setY(std::numeric_limits<double>::quiet_NaN());
    }else if(espChosen.size() == 2){
        // TODO: how to return two points?
        std::pair<QPointF,QPointF> pair = bilateration(
                espChosen[0].getPos(), espChosen[1].getPos(),
                calculateDistance(packetsOrdered[0].rssi),
                calculateDistance(packetsOrdered[1].rssi));
        pos = pair.first;
    }else{
        pos = trilateration(espChosen[0].getPos(), espChosen[1].getPos(), espChosen[2].getPos(),
                        calculateDistance(packetsOrdered[0].rssi),
                        calculateDistance(packetsOrdered[1].rssi),
                        calculateDistance(packetsOrdered[2].rssi));
    }

    // delete the existing packet used for find the position
    packets.removeAll(lastPacket);

    lastTimestamp = QDateTime::currentDateTime().toTime_t();

    return pos;
}*/


double norm (QPointF p) // get the norm of a vector
{
    return pow( pow(p.x(),2) + pow(p.y(),2), .5);
}

void printPoint(QPointF p, QString name)
{
    writeLog(name + ":(" + QString::number(p.x(), 'f', 2) + "," + QString::number(p.y(), 'f', 2) + ")");

}

void printValue(double v, QString name)
{
    writeLog(name + ":" + QString::number(v, 'f', 2));
}

QPointF DeviceFinder::trilateration(QPointF p1, QPointF p2, QPointF p3, double r1, double r2, double r3)
{
//    printPoint(p1, "p1");
//    printPoint(p2, "p2");
//    printPoint(p3, "p3");
//    printValue(r1, "r1");
//    printValue(r2, "r2");
//    printValue(r3, "r3");

    QPointF resultPose;
    //unit vector in a direction from point1 to point 2
    double p2p1Distance = pow(pow(p2.x()-p1.x(),2) + pow(p2.y()-   p1.y(),2),0.5);
//    printValue(p2p1Distance, "p2p1Distance");
    // Ex ed ey sono i versori della direzione, aux e aux2 sono solo delle variabili ausiliarie
    QPointF ex = {(p2.x()-p1.x())/p2p1Distance, (p2.y()-p1.y())/p2p1Distance};
//    printPoint(ex, "ex");
    QPointF aux = {p3.x()-p1.x(),p3.y()-p1.y()};
//    printPoint(aux, "aux");
    //signed magnitude of the x component
    double i = ex.x() * aux.x() + ex.y() * aux.y();
//    printValue(i, "i");
    //the unit vector in the y direction.
    QPointF aux2 = { p3.x()-p1.x()-i*ex.x(), p3.y()-p1.y()-i*ex.y()};
//    printPoint(aux2, "aux2");
    QPointF ey = { aux2.x() / norm(aux2), aux2.y() / norm(aux2) };
//    printPoint(ey, "ey");
    //the signed magnitude of the y component
    double j = ey.x() * aux.x() + ey.y() * aux.y();
//    printValue(j, "j");
    //coordinates
    double x = (pow(r1,2) - pow(r2,2) + pow(p2p1Distance,2))/ (2 * p2p1Distance);

//    printValue(x, "x");
    double y = (pow(r1,2) - pow(r3,2) + pow(i,2) + pow(j,2))/(2*j) - i*x/j;
//    printValue(y, "y");
    //result coordinates
    double finalX = p1.x()+ x*ex.x() + y*ey.x();
    double finalY = p1.y()+ x*ex.y() + y*ey.y();
    finalX = finalX<0?0:finalX;
    finalY = finalY<0?0:finalY;
    finalX = finalX>10?10:finalX;
    finalY = finalY>10?10:finalY;
    resultPose.setX(finalX);
    resultPose.setY(finalY);
//    printPoint(resultPose, "resultPose");
    return resultPose;

}

double distance(QPointF point1, QPointF point2){
    return pow(pow(point2.x()-point1.x(), 2) + pow(point2.y()-point1.y(), 2), 0.5);
}
QPointF normal(QPointF p1) {
    double length = sqrt(p1.x()*p1.x() + p1.y()*p1.y());
    return QPointF(p1.x()/length, p1.y()/length);
}
QPointF scale(QPointF p, double s) {
    return QPointF(p.x()*s, p.y()*s);
}
QPointF sub(QPointF p1, QPointF p2) {
    return QPointF(p1.x() - p2.x(), p1.y() - p2.y());
}
QPointF add(QPointF p1, QPointF p2) {
    return QPointF(p1.x() + p2.x(), p1.y() + p2.y());
}

QPair<QPointF, QPointF> DeviceFinder::bilaterationThatReturnsTwoPoints(QPointF p1, QPointF p2, double r1, double r2){
    QPointF P0(p1.x(), p1.y());
    QPointF P1(p2.x(), p2.y());

    double d, a, h;
    d = distance(P0, P1);
    a = (r1*r1 - r2*r2 + d*d)/(2*d);
    h = sqrt(r1*r1 - a*a);
    QPointF P2 = add( scale( sub(P1, P0), a/d), P0);

    double x3, y3, x4, y4;
    x3 = P2.x() + h*(P1.y() - P0.y())/d;
    y3 = P2.y() - h*(P1.x() - P0.x())/d;
    x4 = P2.x() - h*(P1.y() - P0.y())/d;
    y4 = P2.y() + h*(P1.x() - P0.x())/d;

    return QPair<QPointF, QPointF>(QPointF(x3, y3), QPointF(x4, y4));
}


// esempio:
// p1: (0,0), p2:(0,5), r1:2.5m, r2:3m
QPointF DeviceFinder::bilateration(QPointF p1, QPointF p2, double r1, double r2){
    QPointF P0(p1.x(), p1.y());
    QPointF P1(p2.x(), p2.y());

    double d, a, h; //esempi nei commenti
    d = distance(P0, P1); //5 metri
    a = (r1*r1 - r2*r2 + d*d)/(2*d); //6.25m - 9m + 25m = 22.25m -> 22.25/10=2.225m
    h = sqrt(r1*r1 - a*a);// ~2
    QPointF P2 = add( scale( sub(P1, P0), a/d), P0);

    // THIS is needed if we want to return 2 points
//    double x3, y3, x4, y4;
//    x3 = P2.x() + h*(P1.y() - P0.y())/d;
//    y3 = P2.y() - h*(P1.x() - P0.x())/d;
//    x4 = P2.x() - h*(P1.y() - P0.y())/d;
//    y4 = P2.y() + h*(P1.x() - P0.x())/d;
//    return QPair<QPointF, QPointF>(QPointF(x3, y3), QPointF(x4, y4));

    return P2;
}

/**
 * @brief A test function
 */
//genera dei pacchetti random
//viene chiamato dentro init
void DeviceFinder::generatePackets()
{
    //prima push packet e poi insert packets into DB
    Packet r;
    int numOfPackets = 10;
    int numOfDevices = 3;
    int numOfBoards = 5;
    QVector<QVector<Packet>> vec;
    for(int i=0; i<numOfBoards; i++){
        vec.push_back(QVector<Packet>());
    }
    QRandomGenerator rg(QTime::currentTime().msec());
    for(int n=0; n< numOfDevices; n++){
        //genero un mac fittizio
        int macNum = rg.bounded(10, 99);
        QString mac = QString::number(macNum);
        for(int j= 0; j< numOfPackets; j++){
            //genero un hash fittizio
            int hashNum = rg.bounded(100000, 999999);
            QString hash = QString::number(hashNum);
            for(int i=0; i<numOfBoards; ++i){
                //genero un rssi fittizio
                int rssi = rg.bounded(-90, -30);
                r.rssi = rssi;
                r.espName = "ESP" + QString::number(i);
                r.sender_mac = mac;
                r.timestamp = QDateTime::currentDateTime().toTime_t();//+30;
                r.hashed_pkt = hash;
                vec[i].push_back(r);
                //pushPacket(r);
                //logCurrentDevices();
            }
        }
    }
    // packets.clear();
    for(int i=0; i<numOfBoards; ++i){
        for(int j=0; j<vec[i].size(); j++){
            Packet p = vec[i][j];
            pushPacketInBuffer(p);
        }
        insertBufferedPacketsIntoDB("ESP"+QString::number(i));
    }
}
