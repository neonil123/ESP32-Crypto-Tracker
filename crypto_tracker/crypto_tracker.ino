
// ----------------------------
// Standard Libraries
// ----------------------------
#include <Wire.h>
#include "DFRobot_LCD.h"


DFRobot_LCD lcd(16,2);  //16 characters and 2 lines of show

#include <WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------
char ssid[] = "*********";       // your network SSID (name)
char password[] = "*******";  // your network key

// For Non-HTTPS requests
// WiFiClient client;

// For HTTPS requests
WiFiClientSecure client;


// Just the base of the URL you want to connect to
#define TEST_HOST "api.coingecko.com"


// Root cert of server we are connecting to
// Baltimore CyberTrust Root - Expires 12 May 2025
// (FYI, from a security point of view you should not trust certs from other people, including me!)
const char *server_cert = "-----BEGIN CERTIFICATE-----\n"
                                  "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
                                  "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
                                  "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
                                  "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
                                  "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
                                  "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
                                  "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
                                  "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
                                  "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
                                  "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
                                  "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
                                  "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
                                  "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
                                  "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
                                  "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
                                  "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
                                  "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
                                  "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
                                  "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
                                  "-----END CERTIFICATE-----\n";


    float ethereum_usd; // 3961.66
    float ethereum_eur; // 3261.73

    float bitcoin_usd; // 48924
    float bitcoin_eur; // 40281
    
    float cardano_usd; // 48924
    float cardano_eur; // 40281

    unsigned long millis1=0;



void setup() {
  pinMode(27,OUTPUT);
  digitalWrite(27,HIGH);
  Serial.begin(115200);

    lcd.init();   
    delay(200);
    lcd.setCursor(0,0);
    lcd.print("Crypto Budilnik");
    delay(1000);
    lcd.clear();
    

  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  lcd.setCursor(0,0);
  lcd.print("Connecting WIFI");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IP address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);
  lcd.clear();
  //--------

  // Checking the cert is the best way on an ESP32
  // This will verify the server is trusted.
  client.setCACert(server_cert);

  // If you don't want to verify the server
  // Unlike the fingerprint method of the ESP8266 which expires frequently
  // the cert lasts years, so I don't see much reason to ever
  // use this on the ESP32
  // client.setInsecure();

  makeHTTPRequest();
}

void makeHTTPRequest() {

  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    lcd.setCursor(0,0);
    lcd.print("Connection failed ");
    delay(3000);
    return;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/api/v3/simple/price?ids=ethereum%2Cbitcoin%2Ccardano&vs_currencies=usd%2Ceur"); // %2C == ,
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  //delay(100);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }

  // This is probably not needed for most, but I had issues
  // with the Tindie api where sometimes there were random
  // characters coming back before the body of the response.
  // This will cause no hard to leave it in
  // peek() will look at the character, but not take it off the queue
  while (client.available() && client.peek() != '{')
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  //  // While the client is still availble read each
  //  // byte and print to the serial monitor
  //  while (client.available()) {
  //    char c = 0;
  //    client.readBytes(&c, 1);
  //    Serial.print(c);
  //  }

  //Use the ArduinoJson Assistant to calculate this:

  //StaticJsonDocument<192> doc;
  DynamicJsonDocument doc(192); //For ESP32/ESP8266 you'll mainly use dynamic.

  DeserializationError error = deserializeJson(doc, client);

  if (!error) {
    ethereum_usd = doc["ethereum"]["usd"]; // 3961.66
    ethereum_eur = doc["ethereum"]["eur"]; // 3261.73

    bitcoin_usd = doc["bitcoin"]["usd"]; // 48924
    bitcoin_eur = doc["bitcoin"]["eur"]; // 40281
    
    cardano_usd = doc["cardano"]["usd"]; // 48924
    cardano_eur = doc["cardano"]["eur"]; // 40281

    Serial.print("ethereum_usd: ");
    Serial.println(ethereum_usd);
    Serial.print("ethereum_eur: ");
    Serial.println(ethereum_eur);

    Serial.print("cardano_usd: ");
    Serial.println(cardano_usd);
    Serial.print("cardano_eur: ");
    Serial.println(cardano_eur);

    Serial.print("bitcoin_usd: ");
    Serial.println(bitcoin_usd);
    Serial.print("bitcoin_eur: ");
    Serial.println(bitcoin_eur);
    
  } else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
client.stop();

}

void loop() {


if(millis()-millis1 > 10000){
    makeHTTPRequest();
    millis1=millis();
}
 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ethereum: ");
    lcd.setCursor(0,1);
    lcd.print("$ ");
    lcd.print(ethereum_usd);
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("cardano: ");
    lcd.setCursor(0,1);
    lcd.print("$ ");
    lcd.print(cardano_usd);
    delay(2000);
  
    lcd.setCursor(0,0);
    lcd.print("bitcoin: ");
    lcd.setCursor(0,1);
    lcd.print("$ ");
    lcd.print(bitcoin_usd);
    delay(2000);
    
    if(cardano_usd <= 1.14){
      digitalWrite(27,LOW);
    }
    else{
       digitalWrite(27,HIGH);
    }
    


}
