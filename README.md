# Server ESP32-Progetto Programmazione di Sistema 2017/18
### Gruppo
- Gianluca D'Alleo
- Salvatore Di Cara
- Giorgio Pizzuto
- Vincenzo Topazio

**Client ESP32**: https://github.com/vintop95/client_esp32_pds  

### TODO
ICON TRAY: http://doc.qt.io/archives/qt-4.8/qt-desktop-systray-example.html


## Configurazione ambiente di sviluppo
Useremo qt per lo sviluppo del server con interfaccia grafica in C++
Scarica la versione open-source da https://www.qt.io/download  
e installa **SOLO** il componente MSVC2017 64BIT e Qt Charts

Documentazione: http://doc.qt.io/qt-5/qtwidgets-index.html#  
Qt Tutorial for beginners: https://www.youtube.com/watch?v=EkjaiDsiM-Q&list=PLS1QulWo1RIZiBcTr5urECberTITj7gjA

## Classi principali
Modello  
![aaa](https://github.com/vintop95/server_esp32_pds/blob/master/classModel.PNG)

 -------
```DeviceFinder```  
Gestisce l'analisi dei pacchetti inseriti da ```ClientHandler``` per trovare i dispositivi nell'area

```ClientHandler```  
Gestisce il client appena connesso ricevuto da ```Server``` e inserisce i pacchetti ricevuti in ```DeviceFinder```  
Inoltre usa ```DbManager``` per salvare i dati in modo persistente

```Server```  
Resta in ascolto dei client in ingresso e richiama ```ClientHandler``` per la gestione dei client appena connessi

```Record```  
Contiene le strutture dati per gestire i pacchetti e i dispositivi  

 -------
```DbManager```  
Gestisce l'interfaccia al database SQLite,

```Logger```  
Gestisce il logging per il debug, stampando in una finestra nella ```MainWindow``` i messaggi di log

```Settings```  
Gestisce le impostazioni dell'applicazione  

 -------
```MainWindow```  
Gestisce la finestra principale dell'applicazione

```Chart```  
Gestisce il grafico dei dispositivi trovati durante il tempo

```SettingsForm```  
Gestisce la modifica via GUI delle impostazioni del programma

```ESPWidget```  
Gestisce una riga della lista di ESP all'interno di SettingsForm


## Commenti sul codice

### Strutture dati
Sono utilizzate le strutture dati offerte dal framework qt, in sostanza le librerie standard con una Q davanti (es QString, QVector, ...)

### Signals e slots
Nella documentazione trovate cosa sono nel dettaglio  
connect(oggetto_sender, signal_triggerante,
        oggetto_receiver, slot_cioè_metodo_da_chiamare);

### Definizione classi 
- aggiungere macro Q_OBJECT all'inizio della classe
  per poter usare il meccanismo di signal e slots
  nelle classi derivate da qualsiasi classe che inizia
  con Q (sono tutte derivate dalla classe padre QObject)
  
# Ulteriori informazioni

## Altri file
```classModel.qmodel```  
File modificabile da Qt Creator per modellare le classi dell'applicazione

```server_esp32_pds.pro```  
File del progetto Qt Creator

### Videolezione del progetto
LEZIONE 24 2017/18
