
#include <NMEA2000_CAN.h>
#include "BluetoothSerial.h"
//Repeater needs ESP32 chip supporting Bluetooth Classic (BR/EDR) 

// for chip id
uint32_t chipId = 0;
char deviceName[30];


#define ESP32_CAN_TX_PIN GPIO_NUM_5  // Set according to your ESP32 connections
#define ESP32_CAN_RX_PIN GPIO_NUM_4  // Set according ----


#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void LedOn(unsigned long OnTime);
void UpdateLedState();

// declare nmea200 handle function
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg);

void setup() {
  Serial.begin(115200);
  
  //-------------------------------------------------------------------------
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  // Set the desired PIN code and device name
  char chipIdstring[10];
  sprintf(chipIdstring, "%lu", chipId);
 


//SERIAL BLUETOOTH START
  sprintf(deviceName, "BUMBOX-%lu", chipIdstring);
 
  SerialBT.begin(deviceName);  // Bluetooth device name
  SerialBT.setPin("1234"); // not working - will use simple pairing



  // Set product information
  NMEA2000.SetProductInformation("1001",                     // Manufacturer's Model serial code
                                 100,                     // Manufacturer's product code
                                 "BUMBOX",               // Manufacturer's Model ID
                                 "1.0.0.1 (2023-12-26)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2019-07-07)"   // Manufacturer's Model version
  );
  // // Set device information
  NMEA2000.SetDeviceInformation(100,  // Unique number. Use e.g. Serial number.
                                130,     // Device function=Analog to NMEA 2000 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                25,      // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                777     // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  );



  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("\nThe device started, now you can pair it with bluetooth!");

  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(250);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenOnly);

  //NMEA2000.SetN2kCANSendFrameBufSize(250);
  NMEA2000.SetForwardStream(&SerialBT);
  //NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.SetMsgHandler(HandleNMEA2000Msg); // can comment if no led needed
  NMEA2000.Open();
}
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
void loop() {

  UpdateLedState(); //remove or comment if do not like or need led flashing

  NMEA2000.ParseMessages();
}

#define LedOnTime 2
#define LedBlinkTime 50
unsigned long TurnLedOffTime = 0;
unsigned long TurnLedOnTime = millis() + LedBlinkTime;

void LedOn(unsigned long OnTime) {
  if (LED_BUILTIN != 0xff) digitalWrite(LED_BUILTIN, HIGH);
  TurnLedOffTime = millis() + OnTime;
  TurnLedOnTime = 0;
}

void UpdateLedState() {
  if (TurnLedOffTime > 0 && millis() > TurnLedOffTime) {
    digitalWrite(LED_BUILTIN, LOW);
    TurnLedOffTime = 0;
  }
}

void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  //blink led when data arrives
  // add you logic to do smth with data
  //N2kMsg.Print(&Serial); //log to serial // commnt on production
  LedOn(LedOnTime);
}
