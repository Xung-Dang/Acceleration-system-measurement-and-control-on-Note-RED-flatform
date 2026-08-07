//*************************************THƯ VIỆN**************************************
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D5,D6);
// THIẾT LẬP CHÂN LED
#define green D3
#define red D4
//***********************************THIẾT LẬP THÔNG TIN WIFI, MQTT VÀ TÊN TOPIC MUỐN GỬI LÊN CLOUD ***************
// Cập nhật thông tin
// Thông tin về wifi
#define ssid "Điện thoại Mi"
#define password "kido122000"
// Thông tin về MQTT Broker
#define mqtt_server "projectmain.cloud.shiftr.io" 
//TÊN TOPIC
#define mqtt_topic_pub "anglex"
#define mqtt_topic_pub1 "accx"
#define mqtt_topic_pub2 "accy"
#define mqtt_topic_pub3 "accz"
#define mqtt_topic_pub4 "temp"
#define mqtt_topic_pub5 "biencanhbao"

#define mqtt_user "projectmain"    
#define mqtt_pwd "7gwGvfUhzPX4lA4G"

const uint16_t mqtt_port = 1883; //Port của CloudMQTT

WiFiClient espClient;
PubSubClient client(espClient);
//*******************************************BIẾN****************************************************
long lastMsg = 0;
long lastMsg1 = 0;
char msg[50];
int value = 0;

//float bienaccx, bienaccy, bienaccz;

String inputString="",sendnano="",myttopic="";
bool stringComplete = false;
String inputString1="";
bool stringComplete1= false;
//long last1=0;
float biengoc=0, bienaccx=0,bienaccy=0,bienaccz=0,biennhietdo=0;
float biengoc_max=0,bienaccx_max=0,bienaccy_max=0,bienaccz_max=0, chedorung=0, chedogoc=0,dulieu=0;
int biencanhbao =0;


//*******************************************CHƯƠNG TRÌNH CHÍNH************************************
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  mySerial.begin(115200);
//  last1 = millis();
  pinMode( red, OUTPUT);
  pinMode( green, OUTPUT);
  
}
//******************************************************CHƯƠNG TRÌNH KẾT NỐI WIFI***************************
// Hàm kết nối wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//******************************CHƯƠNG TRÌNH NHẬN DỮ LIỆU, XỬ LÝ CHUỖI VÀ LƯU BIẾN*****************************

void callback(char* topic, byte* payload, unsigned int length) {
  
  String myTopic;
  myTopic = String(myTopic + topic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  myttopic=myTopic;
//////nhận dữ liệu
  char receivedChar[length];
  for (int i = 0; i < length; i++) {
    receivedChar[i] = (char)payload[i];
  }
  
  //XỬ LÝ CHUỖI VÀ LƯU BIẾN
  
  float abc = atof(receivedChar);
  // data.toFloat();
  Serial.println(abc);
  dulieu=abc;
  
  if(myTopic == "DO_AN/XUNG/SWITCH4"){
//    float abc = atof(receivedChar);
    chedorung = abc;
    Serial.print("chedorung: ");
    Serial.println(chedorung);
    
  }
  if(myTopic == "DO_AN/XUNG/RUNG/X"){
//    float abc = atof(receivedChar);
    bienaccx_max = abc;
    Serial.print("bienaccx_max: ");
    Serial.println(bienaccx_max);
  }
  if(myTopic == "DO_AN/XUNG/RUNG/Y"){
//    float abc = atof(receivedChar);
    bienaccy_max = abc;
    Serial.print("bienaccy_max: ");
    Serial.println(bienaccy_max);
  }
  if(myTopic == "DO_AN/XUNG/RUNG/Z"){
//  float abc = atof(receivedChar);
  bienaccz_max = abc;
  Serial.print("bienaccz_max: ");
  Serial.println(bienaccz_max);
  }
  if(myTopic == "DO_AN/XUNG/SWITCH3"){
//  float abc = atof(receivedChar);
  chedogoc = abc;
  Serial.print("chedogoc: ");
  Serial.println(chedogoc);
  }
  if(myTopic == "DO_AN/XUNG/CANHBAO"){
//  float abc = atof(receivedChar);
  biengoc_max = abc;
  Serial.print("biengoc_max: ");
  Serial.println(biengoc_max);
  }

}
//************************************KẾT NỐI LẠI MQTT BROKER VÀ NHẬN THÔNG TIN TỪ CLOUD************************
// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() {
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      
// NHẬN THÔNG TIN TỪ CLOUD

      client.subscribe("DO_AN/XUNG/SWITCH4");
      client.subscribe("DO_AN/XUNG/RUNG/X");
      client.subscribe("DO_AN/XUNG/RUNG/Y");
      client.subscribe("DO_AN/XUNG/RUNG/Z");
      client.subscribe("DO_AN/XUNG/SWITCH3");
      client.subscribe("DO_AN/XUNG/CANHBAO");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
//*****************************************CHƯƠNG TRÌNH VÒNG LẶP****************************************************
void loop() {
//ĐỌC UART
 Readuart();

//*************************************GỬI THÔNG TIN CẢM BIẾN VÀ CẢNH BÁO LÊN CLOUD*********************************
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Sau mỗi 2s sẽ thực hiện publish dòng hello world lên MQTT broker
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;

    client.publish(mqtt_topic_pub,String(biengoc).c_str());
   
    client.publish(mqtt_topic_pub1,String(bienaccx).c_str());
    
    client.publish(mqtt_topic_pub2,String(bienaccy).c_str());
   
    client.publish(mqtt_topic_pub3,String(bienaccz).c_str());
    
    client.publish(mqtt_topic_pub5,String(biencanhbao).c_str());
    
    client.publish(mqtt_topic_pub4,String(biennhietdo).c_str());
    
    delay(10);
     
  }
 //ĐỌC UART1
   Readuart1();
 //CHƯƠNG TRÌNH CẢNH BÁO
   led();

}
//*************************************************CHƯƠNG TRÌNH CON****************************************************
//**********************ĐỌC GIÁ TRỊ NHẬN ĐƯỢC TỪ ARDUINO, XỬ LÝ CHUỖI VÀ LƯU BIẾN***************************************

void Readuart1()
{
  while (Serial.available())
{
  char inChar1= (char)Serial.read();
//  char inChar= (char)Serial.read();
  inputString1 += inChar1;

  if (inChar1 == 'F')
  {
    stringComplete1 = true;
  }

  if (stringComplete1)
  {

    mySerial.println(inputString1);
    mySerial.flush();
    inputString1 ="";
    stringComplete1 = false;
  }   
}
}


void Readuart()
{
  while (mySerial.available())
{
  char inChar= (char)mySerial.read();
//  char inChar= (char)Serial.read();
  inputString += inChar;

  if (inChar == 'F')
  {
    stringComplete = true;
  }

  if (stringComplete)
  {
//    Serial.print("Data nhan: ");
//    Serial.println(inputString);

    xulyonoff(String(inputString));
    caidat(String(inputString));
    
    inputString ="";
    stringComplete = false;
  }   
}
}
void xulyonoff(String inputString)
{
  }
void caidat(String inputString)
{
 int TimA, TimB, TimC, TimD, TimE, TimF = -1;
 TimA = inputString.indexOf("A"); 
 TimB = inputString.indexOf("B"); 
 TimC = inputString.indexOf("C"); 
 TimD = inputString.indexOf("D");
 TimE = inputString.indexOf("E"); 
 TimF = inputString.indexOf("F");  

 if (TimA >=0 && TimB >=0)
 {
  String DulieuAB ="";
  DulieuAB=inputString.substring(TimA+1,TimB);
   biengoc = DulieuAB.toFloat();
  
// Serial.print("biengoc: ");
// Serial.println(biengoc);

 
 }
  if (TimB >=0 && TimC >=0)
 {
  String DulieuBC ="";
  DulieuBC=inputString.substring(TimB+1,TimC);
   bienaccx = DulieuBC.toFloat();

//  Serial.print("bienaccx: ");
//  Serial.println(bienaccx);
 }
 if (TimC >=0 && TimD >=0)
 {
  String DulieuCD ="";
  DulieuCD=inputString.substring(TimC+1,TimD);
   bienaccy = DulieuCD.toFloat();

//  Serial.print("bienaccy: ");
//  Serial.println(bienaccy);
 }
  if (TimD >=0 && TimE >=0)
 {
  String DulieuDE ="";
  DulieuDE=inputString.substring(TimD+1,TimE);
   bienaccz = DulieuDE.toFloat();
//
//  Serial.print("bienaccz: ");
//  Serial.println(bienaccz);
 }
  if (TimE >=0 && TimF >=0)
 {
  String DulieuEF ="";
  DulieuEF=inputString.substring(TimE+1,TimF);
   biennhietdo = DulieuEF.toFloat();

//  Serial.print("biennhietdo: ");
//  Serial.println(biennhietdo);
 }

}
//***************************************CHƯƠNG TRÌNH CẢNH BÁO VÀ LƯU BIẾN CẢNH BÁO****************************************************

void led()
{
if (chedorung==1&&chedogoc==0){
  if (bienaccx>bienaccx_max+1||bienaccy>bienaccy_max+1||bienaccz>bienaccz_max+1){
   digitalWrite(red,HIGH);
   digitalWrite(green,LOW);
   biencanhbao=1;
  }
  else {
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);
  biencanhbao=2;
  }
 }
 else if (chedorung==0&&chedogoc==1){
    if (biengoc-0.99>biengoc_max){
     digitalWrite(red,HIGH);
     digitalWrite(green,LOW);
     biencanhbao = 3;
    }
    else {
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
     biencanhbao = 4;
    }
 }
 else {
 digitalWrite(red,LOW);
 digitalWrite(green,LOW);
 biencanhbao = 0;
 }

}
