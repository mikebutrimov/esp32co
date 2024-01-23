#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include "MHZ19.h"
#include "DHT.h"

#include "index.h"  


#define NUMBER_OF_SAMPLES 1000
#define SAMPLE_RATE_MS 60000
#define SENSORS_READ_RATE_MS 10000

#define CHUNK_SIZE 17 
int log_cycle = 0;


#define DHTPIN 4
#define DHTTYPE DHT22 
#define displayCS 32    
#define displayDC 25   
#define displayReset 33  

 
#define displayCLK 26
#define displayDATA 27

#define BAUDRATE 9600  
MHZ19 myMHZ19; 
DHT dht(DHTPIN, DHTTYPE);


HardwareSerial mySerial(2);
unsigned long getDataTimer = 0;

struct _air {
  int co2 = 0;
  int temp = 0;
  float dht_temp = 0;
  float dht_humidity = 0;
};


_air sample_buffer[NUMBER_OF_SAMPLES];

volatile _air air;

U8G2_ST7565_ERC12864_ALT_F_4W_SW_SPI  display(U8G2_R0, displayCLK, displayDATA ,displayCS, displayDC,displayReset);

WebServer server(80);

//WIFI SSID and PASSWORD
const char* ssid = "SSID";
const char* password = "PASSWORD";

//===============================================================
// Handlers
//===============================================================
void handleRoot() {
 String s = MAIN_page;
 server.send(200, "text/html", s);
}
 
void handleCO2() {
 int a = air.co2;
 String value = String(a);
 server.send(200, "text/plane", value);
}

void handleTemp() {
 int a = air.temp;
 String value = String(a);
 server.send(200, "text/plane", value); 
}

void handleDHTTemp() {
 float a = air.dht_temp;
 String value = String(a);
 server.send(200, "text/plane", value); 
}

void handleDHTHumidity() {
 float a = air.dht_humidity;
 String value = String(a);
 server.send(200, "text/plane", value); 
}

void handleLog() {

  // Yes, for the sake of server.send method accepting string, i abuse string instance recreating
  // instead of more careful buffer manipulations
  // it is esp32 in home grade application
  // can afford cos i can
  String log_buffer_string = "";

  for (int i =log_cycle +1 ; i < NUMBER_OF_SAMPLES + log_cycle; i++){
    log_buffer_string +=(String)sample_buffer[i%NUMBER_OF_SAMPLES].co2 + ";"+(String)sample_buffer[i%NUMBER_OF_SAMPLES].dht_humidity+";"+(String)sample_buffer[i%NUMBER_OF_SAMPLES].dht_temp+"\n";
  }
  server.send(200, "text/plane", log_buffer_string); 
}

int readCO(){
  int CO2;
  CO2 = myMHZ19.getCO2();           
  air.co2 = CO2;     
  return CO2;         
}

int readTemp() {
  int8_t Temp;
  Temp = myMHZ19.getTemperature(); 
  air.temp = Temp;
  return Temp; 
}

void readDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  air.dht_temp = t;
  air.dht_humidity = h;
}




//===============================================================
//  Display functions
//===============================================================

void printIp() {
     display.firstPage();
  do {
    String ip =  WiFi.localIP().toString();
    int str_len = ip.length() + 1; 

    char ip_arr[str_len];
    ip.toCharArray(ip_arr, str_len);

    display.setFont(u8g2_font_6x13_mr);
    display.drawStr(0,12, ip_arr);
  } while ( display.nextPage() );

}

void printAir() {
     display.firstPage();
  do {

    int CO2 = air.co2;
    String CO2_string = String(CO2)+"ppm";

    int str_len = CO2_string.length() + 1; 

    char co_str[str_len];
    CO2_string.toCharArray(co_str, str_len);


    int TEMP = air.temp;
    String TEMP_string = String(TEMP) + "C";
    str_len = TEMP_string.length() + 1; 

    char temp_str[str_len];
    TEMP_string.toCharArray(temp_str, str_len);

    String dht_t_s = String(air.dht_temp) + "C";
    String dht_h_s = String(air.dht_humidity) + "%";

    str_len = dht_t_s.length() + 1; 
    char dht_temp_str[str_len];
    dht_t_s.toCharArray(dht_temp_str, str_len);

    str_len = dht_h_s.length() + 1; 
    char dht_humidity_str[str_len];
    dht_h_s.toCharArray(dht_humidity_str, str_len);

    display.setFont(u8g2_font_6x13_mr);
    display.drawStr(0,32, "Air->");
    display.drawStr(40,32, co_str);
    display.drawStr(90,32, temp_str);
    display.drawStr(0,48, "DHT->");
    display.drawStr(40,48, dht_humidity_str);
    display.drawStr(90,48, dht_temp_str);

  } while ( display.nextPage() );

}
//===============================================================


int log_timer = 0;

//===============================================================
// Setup
//===============================================================

void setup(void){

  pinMode(DHTPIN, INPUT_PULLUP);
  
  
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print(".");
    }
    
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 



//----------------------------------------------------------------
 
  server.on("/", handleRoot);     
  server.on("/CO2", handleCO2);
  server.on("/TEMP", handleTemp);
  server.on("/DHT_TEMP", handleDHTTemp);
  server.on("/DHT_HUMIDITY", handleDHTHumidity);
  server.on("/log", handleLog);
 
  server.begin();                 
  Serial.println("HTTP server started");

  display.begin(); 
  display.setContrast(90);
  printIp();
  dht.begin();

  Serial.println("Starting CO 2 sensor");
  mySerial.begin(BAUDRATE);                               
  myMHZ19.begin(mySerial);                                

  myMHZ19.autoCalibration(false);   
  Serial.println("Started CO 2 sensor");


  readCO();
  readTemp();
  readDHT();
  sample_buffer[log_cycle].co2 = air.co2;
  sample_buffer[log_cycle].temp = air.temp;
  sample_buffer[log_cycle].dht_temp = air.dht_temp;
  sample_buffer[log_cycle].dht_humidity = air.dht_humidity;
  
}

void loop(void){
  log_cycle = log_cycle%NUMBER_OF_SAMPLES;

  if (millis() - log_timer >= SAMPLE_RATE_MS) {
      // {air.co2, air.temp, air.dht_temp, air.dht_humidity};
      sample_buffer[log_cycle].co2 = air.co2;
      sample_buffer[log_cycle].temp = air.temp;
      sample_buffer[log_cycle].dht_temp = air.dht_temp;
      sample_buffer[log_cycle].dht_humidity = air.dht_humidity;
      log_cycle++;
      log_timer = millis();
  }

  
  server.handleClient();
  printIp();
  printAir();
  if (millis() - getDataTimer >= SENSORS_READ_RATE_MS) {
    readCO();
    readTemp();
    readDHT();
    getDataTimer = millis();
  }
}