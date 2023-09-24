#include "AppWeb.h"

#include <Arduino.h>
#include <FFat.h>
#include <Update.h>
#include <WebServer.h>
#include <cJSON.h>

#include "configuration.h"
#include "credentials.h"

// mapper
extern float min_dist_moved;
extern unsigned int stationary_tx_interval_s;
extern boolean never_rest;
extern unsigned int rest_wait_s;
extern unsigned int rest_tx_interval_s;
extern unsigned int sleep_wait_s;
extern unsigned int sleep_tx_interval_s;

// gps
extern unsigned int gps_lost_wait_s;
extern unsigned int gps_lost_ping_s;

// lorawan
extern uint8_t lorawan_sf;

// deadzone
extern double deadzone_lat;
extern double deadzone_lon;
extern double deadzone_radius_m;

// screen
extern int screen_idle_off_s;
extern int screen_menu_timeout_s;

// battery
extern float battery_low_voltage;

extern void mapper_save_prefs(void);
extern void lorawan_save_prefs(void);
extern void deadzone_save_prefs(void);
extern void screen_save_prefs(void);

static String array2string(const uint8_t *array, size_t len, bool lsb);
static void string2array(const char *str, uint8_t *array, bool lsb);

static String array2string(const uint8_t *array, size_t len, bool lsb) {
  // char *temp = (char *)malloc(len * 2 + 1);
  char temp[len * 2 + 1] = {0};
  size_t count = 0;

  if (lsb) {
    for (size_t i = 0; i < len; i++) {
      count += snprintf(&temp[count], len * 2, "%02X", array[len - i - 1]);
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      count += snprintf(&temp[count], len * 2, "%02X", array[i]);
    }
  }

  return String(temp);
}

static void string2array(const char *str, uint8_t *array, bool lsb) {
  size_t len = strlen(str) / 2;
  char h;
  char l;

  for (size_t i = 0; i < len; i++) {
    h = str[2 * i];
    l = str[2 * i + 1];

    h = h - 0x30;
    if (h > 9) {
      h -= 7;
    }

    l = l - 0x30;
    if (l > 9) {
      l -= 7;
    }

    if (lsb) {
      array[len - i - 1] = h * 16 + l;
    } else {
      array[i] = h * 16 + l;
    }
  }
}

AppWeb::AppWeb() {}

AppWeb::~AppWeb() {}

void AppWeb::begin() {
  bool rlst = FFat.begin(true);
  if (rlst) {
    Serial.println("FS initialized.");
  } else {
    Serial.println("FS initialization failed.");
    return;
  }

  server = new WebServer(80);

  server->addHandler(new FunctionRequestHandler(getStatus, nullptr, "/api/v1/status", HTTP_GET));

  server->addHandler(new FunctionRequestHandler(getConfig, nullptr, "/api/v1/config", HTTP_GET));
  server->addHandler(new FunctionRequestHandler(postConfig, nullptr, "/api/v1/config", HTTP_POST));
  server->addHandler(new FunctionRequestHandler(postUpdate, postUpdateUpload, "/api/v1/update", HTTP_POST));

  server->serveStatic("/", FFat, "/www/index.html");
  server->serveStatic("/index.html", FFat, "/www/index.html");
  server->serveStatic("/favicon.ico", FFat, "/www/favicon.ico");
  server->serveStatic("/assets/axios-c0bebe37.js", FFat, "/www/assets/axios-c0bebe37.js");
  server->addHandler(new FunctionRequestHandler(
      [](WebServer &server) {
        File dataFile = FFat.open("/www/assets/index-6b705773.js.gz", "rb");
        if (!dataFile) {
          return;
        }
        if (server.streamFile(dataFile, "application/javascript") != dataFile.size()) {
          Serial.println("Sent less data than expected!");
        }
        dataFile.close();
      },
      nullptr, "/assets/index-6b705773.js", HTTP_GET));
  server->addHandler(new FunctionRequestHandler(
      [](WebServer &server) {
        File dataFile = FFat.open("/www/assets/index-036f9c55.css.gz", "rb");
        if (!dataFile) {
          return;
        }
        if (server.streamFile(dataFile, "text/css") != dataFile.size()) {
          Serial.println("Sent less data than expected!");
        }
        dataFile.close();
      },
      nullptr, "/assets/index-036f9c55.css", HTTP_GET));
  server->serveStatic("/assets/Info-03968bcc.js", FFat, "/www/assets/Info-03968bcc.js");
  server->serveStatic("/assets/Info-14de03fa.css", FFat, "/www/assets/Info-14de03fa.css");
  server->serveStatic("/assets/Settings-c775545a.js", FFat, "/www/assets/Settings-c775545a.js");
  server->serveStatic("/assets/Upgrade-1211ce82.js", FFat, "/www/assets/Upgrade-1211ce82.js");
  server->begin();
}

void AppWeb::loop() {
  server->handleClient();
  delay(2);
}

void AppWeb::end() {
  server->stop();
}

void AppWeb::getStatus(WebServer &server) {
  cJSON *rspObject = NULL;
  cJSON *sysObject = NULL;
  cJSON *archObject = NULL;
  cJSON *memObject = NULL;
  cJSON *fsObject = NULL;
  cJSON *loraObject = NULL;

  rspObject = cJSON_CreateObject();
  if (rspObject == NULL) {
    goto OUT1;
  }

  sysObject = cJSON_CreateObject();
  if (sysObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "sys", sysObject);
  cJSON_AddStringToObject(sysObject, "model", "LILYGO T-Beam");
  cJSON_AddStringToObject(sysObject, "fw", APP_VERSION);
  cJSON_AddStringToObject(sysObject, "sdk", ESP.getSdkVersion());
  archObject = cJSON_CreateObject();
  if (archObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(sysObject, "arch", archObject);
  cJSON_AddStringToObject(archObject, "mfr", "Espressif");
  cJSON_AddStringToObject(archObject, "model", ESP.getChipModel());
  cJSON_AddNumberToObject(archObject, "revision", ESP.getChipRevision());
  if (!strncmp(ESP.getChipModel(), "ESP32-S3", strlen("ESP32-S3"))) {
    cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX7");
  } else if (!strncmp(ESP.getChipModel(), "ESP32-S2", strlen("ESP32-S2"))) {
    cJSON_AddStringToObject(archObject, "cpu", "XTensa® single-core LX7");
  } else if (!strncmp(ESP.getChipModel(), "ESP32-C3", strlen("ESP32-C3"))) {
    cJSON_AddStringToObject(archObject, "cpu", "RISC-V");
  } else if (!strncmp(ESP.getChipModel(), "ESP32", strlen("ESP32"))) {
    cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX6");
  }
  cJSON_AddNumberToObject(archObject, "freq", ESP.getCpuFreqMHz());

  memObject = cJSON_CreateObject();
  if (memObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "mem", memObject);
  cJSON_AddNumberToObject(memObject, "total", ESP.getHeapSize());
  cJSON_AddNumberToObject(memObject, "free", ESP.getFreeHeap());

  fsObject = cJSON_CreateObject();
  if (fsObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "fs", fsObject);
  cJSON_AddNumberToObject(fsObject, "total", FFat.totalBytes());
  cJSON_AddNumberToObject(fsObject, "used", FFat.usedBytes());
  cJSON_AddNumberToObject(fsObject, "free", FFat.totalBytes() - FFat.usedBytes());

  loraObject = cJSON_CreateObject();
  if (loraObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "lora", loraObject);
#if defined(CFG_eu868)
  cJSON_AddStringToObject(loraObject, "band", "EU868");
#elif defined(CFG_us915)
  cJSON_AddStringToObject(loraObject, "band", "US915");
#elif defined(CFG_au915)
  cJSON_AddStringToObject(loraObject, "band", "AU915");
#elif defined(CFG_as923)
  cJSON_AddStringToObject(loraObject, "band", "AS923");
#elif defined(CFG_kr920)
  cJSON_AddStringToObject(loraObject, "band", "KR920");
#elif defined(CFG_in866)
  cJSON_AddStringToObject(loraObject, "band", "IN866");
#else
  cJSON_AddStringToObject(loraObject, "band", "n/a");
#endif

#if defined(CFG_sx1276_radio)
  cJSON_AddStringToObject(loraObject, "radio", "sx1276");
#elif defined(CFG_sx1272_radio)
  cJSON_AddStringToObject(loraObject, "radio", "sx1272");
#else
  cJSON_AddStringToObject(loraObject, "radio", "n/a");
#endif

  server.send(200, "application/json", cJSON_Print(rspObject));
OUT:
  cJSON_Delete(rspObject);
OUT1:
  return;
}

void AppWeb::getConfig(WebServer &server) {
  cJSON *rspObject = NULL;
  cJSON *lorawanObject = NULL;
  cJSON *mapperObject = NULL;
  cJSON *deadzoneObject = NULL;
  cJSON *screenObject = NULL;
  cJSON *batteryObject = NULL;

  String deveui;
  String appeui;
  String appkey;

  rspObject = cJSON_CreateObject();
  if (rspObject == NULL) {
    goto OUT1;
  }

  lorawanObject = cJSON_CreateObject();
  if (lorawanObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "lorawan", lorawanObject);

  mapperObject = cJSON_CreateObject();
  if (mapperObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "mapper", mapperObject);

  deadzoneObject = cJSON_CreateObject();
  if (deadzoneObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "deadzone", deadzoneObject);

  screenObject = cJSON_CreateObject();
  if (screenObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "screen", screenObject);

  batteryObject = cJSON_CreateObject();
  if (batteryObject == NULL) {
    goto OUT;
  }
  cJSON_AddItemToObject(rspObject, "battery", batteryObject);

  // board
  cJSON_AddStringToObject(rspObject, "board", "tbeam 1.1");

  // lorawan
  cJSON_AddStringToObject(lorawanObject, "server", "helium");
  cJSON_AddNumberToObject(lorawanObject, "sf", lorawan_sf);
  cJSON_AddNumberToObject(lorawanObject, "ack", 0);
  // APPEUI
  deveui = array2string(DEVEUI, 8, true);
  appeui = array2string(APPEUI, 8, true);
  appkey = array2string(APPKEY, 16, false);
  cJSON_AddStringToObject(lorawanObject, "deveui", deveui.c_str());
  cJSON_AddStringToObject(lorawanObject, "appeui", appeui.c_str());
  cJSON_AddStringToObject(lorawanObject, "appkey", appkey.c_str());

  // mapper
  cJSON_AddNumberToObject(mapperObject, "minDistance", min_dist_moved);
  cJSON_AddNumberToObject(mapperObject, "stationaryTxInterval", stationary_tx_interval_s);
  cJSON_AddBoolToObject(mapperObject, "neverRest", never_rest);
  cJSON_AddNumberToObject(mapperObject, "restWaitTime", rest_wait_s);
  cJSON_AddNumberToObject(mapperObject, "restTxInterval", rest_tx_interval_s);
  cJSON_AddNumberToObject(mapperObject, "sleepWaitTime", sleep_wait_s);
  cJSON_AddNumberToObject(mapperObject, "sleepTxInterval", sleep_tx_interval_s);
  cJSON_AddNumberToObject(mapperObject, "lostWaitTime", gps_lost_wait_s);
  cJSON_AddNumberToObject(mapperObject, "lostPingTime", gps_lost_ping_s);

  // deadzone
  cJSON_AddNumberToObject(deadzoneObject, "lat", deadzone_lat);
  cJSON_AddNumberToObject(deadzoneObject, "lon", deadzone_lon);
  cJSON_AddNumberToObject(deadzoneObject, "radius", deadzone_radius_m);

  // screen
  cJSON_AddNumberToObject(screenObject, "offTime", screen_idle_off_s);
  cJSON_AddNumberToObject(screenObject, "menuTimeout", 5);

  // battery
  cJSON_AddNumberToObject(batteryObject, "lowVoltage", battery_low_voltage);

  server.send(200, "application/json", cJSON_Print(rspObject));
OUT:
  cJSON_Delete(rspObject);
OUT1:
  return;
}

void AppWeb::postConfig(WebServer &server) {
  cJSON *reqObject = NULL;
  cJSON *boardObject = NULL;
  cJSON *lorawanObject = NULL;
  cJSON *mapperObject = NULL;
  cJSON *deadzoneObject = NULL;
  cJSON *screenObject = NULL;
  cJSON *batteryObject = NULL;
  cJSON *tempObject = NULL;

  reqObject = cJSON_Parse(server.arg("plain").c_str());
  if (reqObject == NULL) {
    Serial.println("JSON parse error");
    Serial.print("payload: ");
    Serial.println(server.arg("plain"));
    goto OUT;
  }

  boardObject = cJSON_GetObjectItem(reqObject, "board");
  if (boardObject) {
    Serial.print("board: ");
    Serial.println(boardObject->valuestring);
  }

  lorawanObject = cJSON_GetObjectItem(reqObject, "lorawan");
  if (lorawanObject) {
    tempObject = cJSON_GetObjectItem(lorawanObject, "server");
    tempObject = cJSON_GetObjectItem(lorawanObject, "sf");
    lorawan_sf = tempObject->valueint;
    tempObject = cJSON_GetObjectItem(lorawanObject, "ack");
    tempObject = cJSON_GetObjectItem(lorawanObject, "deveui");
    string2array(tempObject->valuestring, DEVEUI, true);
    tempObject = cJSON_GetObjectItem(lorawanObject, "appeui");
    string2array(tempObject->valuestring, APPEUI, true);
    tempObject = cJSON_GetObjectItem(lorawanObject, "appkey");
    string2array(tempObject->valuestring, APPKEY, false);
  }

  mapperObject = cJSON_GetObjectItem(reqObject, "mapper");
  if (mapperObject) {
    tempObject = cJSON_GetObjectItem(mapperObject, "minDistance");
    min_dist_moved = tempObject->valuedouble;

    tempObject = cJSON_GetObjectItem(mapperObject, "stationaryTxInterval");
    stationary_tx_interval_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(mapperObject, "neverRest");
    never_rest = cJSON_IsTrue(tempObject);

    tempObject = cJSON_GetObjectItem(mapperObject, "restWaitTime");
    rest_wait_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(mapperObject, "restTxInterval");
    rest_tx_interval_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(mapperObject, "sleepWaitTime");
    sleep_wait_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(mapperObject, "sleepTxInterval");
    sleep_tx_interval_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(mapperObject, "lostWaitTime");
    gps_lost_wait_s = tempObject->valueint;
    tempObject = cJSON_GetObjectItem(mapperObject, "lostPingTime");
    gps_lost_ping_s = tempObject->valueint;
  }

  deadzoneObject = cJSON_GetObjectItem(reqObject, "deadzone");
  if (deadzoneObject) {
    tempObject = cJSON_GetObjectItem(deadzoneObject, "lat");
    deadzone_lat = tempObject->valuedouble;

    tempObject = cJSON_GetObjectItem(deadzoneObject, "lon");
    deadzone_lon = tempObject->valuedouble;

    tempObject = cJSON_GetObjectItem(deadzoneObject, "radius");
    deadzone_radius_m = tempObject->valuedouble;
  }
  screenObject = cJSON_GetObjectItem(reqObject, "screen");
  if (screenObject) {
    tempObject = cJSON_GetObjectItem(screenObject, "offTime");
    screen_idle_off_s = tempObject->valueint;

    tempObject = cJSON_GetObjectItem(screenObject, "menuTimeout");
  }

  batteryObject = cJSON_GetObjectItem(reqObject, "battery");
  if (batteryObject) {
    tempObject = cJSON_GetObjectItem(batteryObject, "lowVoltage");
    battery_low_voltage = tempObject->valuedouble;
  }

  server.send(201, "application/json", server.arg("plain"));
  mapper_save_prefs();
  lorawan_save_prefs();
  deadzone_save_prefs();
  screen_save_prefs();
OUT:
  return;
}

void AppWeb::postUpdate(WebServer &server) {
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
}

void AppWeb::postUpdateUpload(WebServer &server) {
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin()) {  // start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {  // true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  } else {
    Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
  }
}