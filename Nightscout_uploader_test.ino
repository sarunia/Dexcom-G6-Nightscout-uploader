#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include "time.h"

const char *AP_SSID = "brakdostepu";                  // WiFi SSID
const char *AP_PWD = "malinowykrul1977comeback";      // WiFi password
const char* ntpServer = "0.pl.pool.ntp.org";          // NTP server to request epoch time (for example Poland zone)

unsigned long long epochTime;                         // Variable to save current epoch time

unsigned long long getTime()                          // Function that gets current epoch time
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    return(0);
  }
  time(&now);
  return (((unsigned long long)now) * 1000);
}

WiFiMulti wifiMulti;
 
void setup()
{
  Serial.begin(9600);
  wifiMulti.addAP(AP_SSID, AP_PWD);
  configTime(0, 0, ntpServer);
}
 
void loop()
{
  if (wifiMulti.run() == WL_CONNECTED)
  {
    epochTime = (unsigned long long)getTime();
    Serial.print("Epoch Time: ");
    Serial.println((unsigned long long)epochTime);
    postDataToServer();
  }
  delay(60000);
}
 
void postDataToServer()
{
  Serial.println("Posting JSON data to server...");
  if (wifiMulti.run() == WL_CONNECTED)                                //if WiFi connected
  {
    HTTPClient https;   
    https.begin("https://testowycgm.herokuapp.com/api/v1/entries");   //https://your-NS-site.herokuapp.com/api/v1/entries
    https.addHeader("Content-Type", "application/json");         
    StaticJsonDocument<200> doc;
    doc["device"] = "Test-G6-upload";                                 //Dexcom G6 device name
  doc["secret"] = "d6026bb45e7efd38de82680c75d31cf7f7a6a1e3";         //API secret encoded to SHA-1 http://www.sha1-online.com/
    doc["sgv"] = 112;                                                 //BG value
    doc["date"] = epochTime;                                          //epoch time in miliseconds https://www.epochconverter.com/clock
    doc["direction"] = "Flat";                   
    String requestBody;
    serializeJson(doc, requestBody);
    int httpResponseCode = https.POST(requestBody);
    if(httpResponseCode>0)                                            //if site response
    {
      String response = https.getString();                       
      Serial.println(httpResponseCode);   
      Serial.println(response);
    }
  }
}
