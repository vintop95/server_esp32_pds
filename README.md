# Server ESP32-Progetto Programmazione di Sistema 2017/18
### Gruppo
- Gianluca D'Alleo
- Salvatore Di Cara
- Giorgio Pizzuto
- Vincenzo Topazio

**Client ESP32**: https://github.com/vintop95/client_esp32_pds

## Configurazione ambiente di sviluppo
Useremo qt per lo sviluppo del server con interfaccia grafica in C++
Scarica la versione open-source da https://www.qt.io/download  
e installa **SOLO** il componente MSVC2017 64BIT

Documentazione: http://doc.qt.io/qt-5/qtwidgets-index.html#  

## Commenti sul codice

### Server TCP
Mediante lo switch di compilazione (mediante define)
```
#define __WINDOWS
//#define __LINUX
```
è possibile creare un server funzionante o in Windows o in Linux a seconda della configurazione scelta

# Ulteriori informazioni
### Videolezione del progetto
LEZIONE 24 2017/18
https://video.polito.it/dl/ad1eec5c99f6f41650c415ac15604a74/5b9fece3/2018/2018_02GRSOV_0219089/Programmazione_di_sistema_lez_24/Programmazione_di_sistema_lez_24.mp4