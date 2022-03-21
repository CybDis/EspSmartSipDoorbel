//------------------------------------------------
// WiFi Configuration
const char* ssid            = "AreaBlue";
const char* password        = "8998968153047257";
const char* wifihostname    = "ESP-Klingel";

// Network Configuration
const char *local_IP        = "192.168.178.220";  // use static IP for performance reasons
IPAddress gateway(192, 168, 178, 1);              // usally your router IP
IPAddress subnetmask(255, 255, 255, 0);           // 
IPAddress primaryDNS(192, 168, 178, 1);           // usally your router IP

// SIP Configuration
const char *sipip           = "192.168.178.1";  // Fritzbox or other sip host
const char *sipuser         = "SIP-USER";       // add new SIP device for doorbell
const char *sippasswd       = "PASSWORD";       // 
int         sipport         = 5060;             // default: 5060
// SIP Dial Configuration
const char *sipdialnr       = "**9";            // Broadcast: **9  
const char *sipdialtext     = "DOOR RINGING";	// Text displayed
int         ringSeconds     = 6;                // seconds to ring phone

// MQTT Configuration
const char *mqttDeviceName  = "Doorbell";       // Can be changed, but avoid spaces or special characters!
IPAddress   mqttBroker      = IPAddress(192,168,178,2);
const char *mqttUser        = "MQTTUSER";       // mqtt username, empty if not required
const char *mqttPass        = "MQTTPASS";       // mqtt password, empty if not required
int         mqttPort        = 1883;             // default: 1883

//------------------------------------------------