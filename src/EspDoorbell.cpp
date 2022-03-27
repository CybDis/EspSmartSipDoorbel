#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include "sip.h"
#include "user_settings.h"

//#define DEBUGSIP

// SIP constants
char caSipIn[2048];
char caSipOut[2048];
Sip aSip(caSipOut, sizeof(caSipOut));
uint millisWhenFinished = 0;

// MQTT constants
WiFiClient wifiClient;
HADevice device;
HAMqtt mqtt(wifiClient, device);
HABinarySensor doorbell("doorbell", true);
//HASensor battery("Battery");

void RingPhone()
{
  Serial.println(String(millis()) + " - Initializing SIP...");
  aSip.Udp.begin(sipport);
  aSip.Init(sipip, sipport, local_IP, sipport, sipuser, sippasswd, ringSeconds);
  Serial.println(String(millis()) + " - Dialing ...");
  aSip.Dial(sipdialnr, sipdialtext);
  Serial.println(String(millis()) + "- Finished Dialing. Ringing for " + String(ringSeconds) + " seconds.");
  Serial.println("+++");
}

void SenqMqtt()
{ 
    Serial.println("Conneting to MQTT broker...");
    
    // Unique ID must be set!
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));
    device.setName(mqttDeviceName);
    device.setManufacturer("github.com/cybdis");
    device.setModel("Doorbell 1.0");
    device.setSoftwareVersion("1.0.0");
    
    doorbell.setName("Doorbell");

    // battery.setName("Battery");
    // battery.setDeviceClass("battery");
    // battery.setUnitOfMeasurement("%");

    mqtt.begin(mqttBroker, mqttPort, mqttUser, mqttPass);
    mqtt.loop(); // connects to server

    //battery.setValue(24); // not yet supported, maybe some  soldering to do first

    Serial.println("Sending MQTT signal...");
    doorbell.setState(true);
    mqtt.loop(); // processes messages
    doorbell.setState(false);
    mqtt.loop(); // processes messages
    
    Serial.println("MQTT state sent.");
}

void setup()
{
  Serial.begin(115200);
  Serial.println(String(millis()) + " - Configuring WiFi to static IP " + String(local_IP));

  IPAddress staticIp;
  staticIp.fromString(local_IP);
  if (!WiFi.config(staticIp, gateway, subnetmask, primaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.hostname(wifihostname);

  Serial.print(String(millis()) + " - Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // SSID und WLAN-Key eintragen

  int i = 0;
  for (i = 0; i < 100; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
      break;
    delay(50);
    Serial.print(".");
  }
  Serial.println();

  // without connection go to sleep
  if (i >= 100)
  {
    Serial.print("Cannot connect to WiFi - going to sleep!");
    delay(500);
    ESP.deepSleep(0);
    return;
  }
  
  Serial.print(String(millis()) + " - Connected, IP address: ");
  Serial.println(WiFi.localIP());

  RingPhone();
  SenqMqtt();
}

void loop(void)
{
  int packetSize = aSip.Udp.parsePacket();
  if (packetSize > 0)
  {
    caSipIn[0] = 0;
    packetSize = aSip.Udp.read(caSipIn, sizeof(caSipIn));
    if (packetSize > 0)
    {
      caSipIn[packetSize] = 0;
#ifdef DEBUGSIP
      IPAddress remoteIp = aSip.Udp.remoteIP();
      Serial.printf("\r\n----- read %i bytes from: %s:%i ----\r\n", (int)packetSize, remoteIp.toString().c_str(), aSip.Udp.remotePort());
      // Serial.print(caSipIn);
      Serial.printf("----------------------------------------------------\r\n");
#endif
    }
  }
  aSip.HandleUdpPacket((packetSize > 0) ? caSipIn : 0);

  if (!aSip.IsBusy() && millisWhenFinished == 0)
  {
    millisWhenFinished = millis();
    Serial.println(String(millis()) + " - Ringing done.");
    Serial.println("+++");
  }

  // wait for deep sleep until ringing stops and SIP session finishes
  if (millisWhenFinished && (millis() - millisWhenFinished) > ringSeconds * 1000)
  {
    Serial.println(String(millis()) + " - Going to sleep. Zzz.");
    Serial.println("+++");
    ESP.deepSleep(0);
  }
}
