/* Revised to use UBlox native binary protocol to engage NMEA */
#include "configuration.h"
#include "gps.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <TinyGPS++.h>

HardwareSerial gpsSerial(GPS_SERIAL_NUM);

SFE_UBLOX_GNSS myGNSS;

TinyGPSPlus tGPS;

void gps_time(char* buffer, uint8_t size) {
  snprintf(buffer, size, "%02d:%02d:%02d", tGPS.time.hour(), tGPS.time.minute(), tGPS.time.second());
}

void gps_end(void) {
  gpsSerial.end();
}

void gps_setup(boolean first_init) {
  static boolean serial_ready = false;
  if (serial_ready) {
    gpsSerial.updateBaudRate(GPS_BAUDRATE);
  } else {
    gpsSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    gpsSerial.setRxBufferSize(2048);  // Default is 256
    serial_ready = true;
  }
  // Drain any waiting garbage
  while (gpsSerial.read() != -1)
    ;

  // myGNSS.enableDebugging();

  bool changed_speed = false;  // Assume we're already at the right speed

  // Check all the possible GPS bitrates to get in sync
  do {
    gpsSerial.updateBaudRate(GPS_BAUDRATE);  // Try the desired speed first
    if (myGNSS.begin(gpsSerial)) {
      Serial.println("GPS connected.");
      break;
    }

    // Well, wasn't where we expected it
    changed_speed = true;

    // Serial.println("Trying 115200...");
    gpsSerial.updateBaudRate(115200);
    if (myGNSS.begin(gpsSerial)) {
      Serial.println("GPS found at 115200 baud");
      myGNSS.setSerialRate(GPS_BAUDRATE);
      continue;
    }

    // Serial.println("Trying 9600...");
    gpsSerial.updateBaudRate(9600);
    if (myGNSS.begin(gpsSerial)) {
      Serial.println("GPS found at 9600 baud");
      myGNSS.setSerialRate(GPS_BAUDRATE);
      continue;
    }

    // Serial.println("Trying 38400...");
    gpsSerial.updateBaudRate(38400);
    if (myGNSS.begin(gpsSerial)) {
      Serial.println("GPS found at 38400 baud");
      myGNSS.setSerialRate(GPS_BAUDRATE);
      continue;
    }

    // Serial.println("Trying 57600...");
    gpsSerial.updateBaudRate(57600);
    if (myGNSS.begin(gpsSerial)) {
      Serial.println("GPS found at 57600 baud");
      myGNSS.setSerialRate(GPS_BAUDRATE);
      continue;
    }

    Serial.println("Could not connect to GPS. Retrying all speeds...");
  } while (1);

  // Configure NMEA messages only once, save to flash
  if (first_init) {
    myGNSS.setUART1Output(COM_TYPE_NMEA);  // We do want NMEA

    myGNSS.setNavigationFrequency(2);  // Produce X solutions per second

#if 0
  // On the Neo6M, these are all off by default anyway:
  myGNSS.disableNMEAMessage(UBX_NMEA_DTM, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GAQ, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GBQ, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GBS, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GLQ, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GNQ, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GNS, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GPQ, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GRS, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GST, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_TXT, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_VLW, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_ZDA, COM_PORT_UART1);
#endif

    // Disable these messages that are on after factory reset
    myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
    myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1);
    myGNSS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
    myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
    
    myGNSS.enableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);   // For Speed
    myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1);   // For Time & Location & SV count
  }

  if (first_init || changed_speed)
    myGNSS.saveConfiguration();  // Save the current settings to flash and BBR
}

void gps_full_reset(void) {
  Serial.println("Resetting GPS...");
  myGNSS.factoryReset();
  delay(5000);
  Serial.println("Reconfiguring GPS...");
  gps_setup(true);
  delay(1000);
  // Drain any waiting garbage
  while (gpsSerial.read() != -1)
    ;
  // gps_passthrough();
  // ESP.restart();
}

void gps_passthrough(void) {
  Serial.println("GPS Passthrough forever...");
  while (1) {
    if (gpsSerial.available())
      Serial.write(gpsSerial.read());
    if (Serial.available())
      gpsSerial.write(Serial.read());
  }
}

void gps_loop(boolean print_it) {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    if (print_it)
      Serial.print(c);
    tGPS.encode(c);
  }
}