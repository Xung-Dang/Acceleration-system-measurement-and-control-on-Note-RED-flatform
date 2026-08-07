//******************************************************THƯ VIỆN*************************
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//***********************************THIẾT LẬP THÔNG TIN WIFI, MQTT VÀ TÊN TOPIC MUỐN GỬI VÀ NHẬN TƯ CLOUD ***************
#define mqtt_server "projectmain.cloud.shiftr.io"
#define mqtt_user "projectmain"
#define mqtt_pwd "7gwGvfUhzPX4lA4G"
// các topic mà esp nhận dữ liệu
#define sub_dc "DO_AN/LINH/SWITCH1"
#define sub_led "DO_AN/LINH/SWITCH2"
#define sub_ctl "anglex"

#define pub_dc "DO_AN/LINH/TOCDO"
#define pub_led "DO_AN/LINH/DOSANG"
#define sub_stop "biencanhbao"
#define Led D4
#define PWM D5
#define encoder D6
const uint16_t mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
//*****************************************BIẾN*******************************************
long lastMsg = 0;
char msg[50];
int value = 0;
int reset_dc =1;
int reset_led =1;
float biengoc;
int controlMotor;
int controlLed;
// thông tin wifi
#define ssid "wifi_iot"
#define password "123456780"
// khia báo hàm
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
float receiveData(unsigned int l, char a[]);
//---------dem encoder--------
unsigned long previousMillis = 0;
const long interval = 3000; // setup tốc độ đọc encoder
void demxung(); // hàm ngắt đếm encoder
int soxung = 0; // lấy giá trị encoder
// biến tính toán tốc độ và độ sáng
float toc_do = 0;
float do_sang =0;
//*****************************************CHƯƠNG TRÌNH CHÍNH***************************
void setup() {
Serial.begin(9600);
pinMode(Led,OUTPUT);
pinMode(PWM,OUTPUT);
pinMode(encoder,INPUT);
attachInterrupt(encoder, demxung, RISING);
setup_wifi();
analogWriteFreq(1000);
client.setServer(mqtt_server, mqtt_port); 
client.setCallback(callback);
}
//******************************************************CHƯƠNG TRÌNH VÒNG LẶP***************************
void loop() {
  // Kiểm tra kết nối
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  // khởi động thì sẽ tắt đèn và động cơ
  if((controlMotor-reset_dc)==-1) {analogWrite(PWM,0);client.publish(pub_dc, String(0.0).c_str());reset_dc=0;}
  if((controlLed-reset_led)==-1) {analogWrite(Led,255);client.publish(pub_led, String(0.0).c_str());reset_led=0;}
  int z;
  //***************************************CHƯƠNG TRÌNH ĐIỀU KHIỂN LED VÀ GỬI GIÁ TRỊ TỐC ĐỘ ĐẾN CLOUD ***************************
  if(controlMotor==true){ // ct điều khiển động cơ
    z = biengoc*3;
    analogWrite(PWM,z);
    //------ct doc encoder
    if(currentMillis - previousMillis >= interval){ // xác định 3s
      Serial.println(currentMillis - previousMillis);
      previousMillis = currentMillis;
      toc_do = (soxung*20)/231; // tính toán số vòng quay
      soxung = 0;
      // gửi giá trị tốc độ động cơ
      client.publish(pub_dc, String(toc_do).c_str());
      reset_dc =1;
      }
    }
    //***************************CHƯƠNG TRÌNH ĐIỀU KHIỂN LED VÀ GỬI GIÁ TRỊ ĐỘ SÁNG ĐẾN CLOUD***************************
    if(controlLed == true){ // chương trình điều khiển led
      z = biengoc*3;
      analogWrite(Led,(254-z));
      delay(1000);
      do_sang = (254-z)/2.5;
      // gửi giá trị độ sáng Led
      client.publish(pub_led, String(100.0-do_sang).c_str()); 
      reset_led=1;
    }
}
//********************************************NGẮT ĐỌC ENCODER***************************
ICACHE_RAM_ATTR void demxung() {
 soxung++;
}

//*************************************CHƯƠNG TRÌNH KẾT NỐI WIFI***************************
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
//*********************************CHƯƠNG TRÌNH NHẬN DỮ LIỆU VÀ XỬ LÝ CHUỖI***************************
void callback(char* topic, byte* payload, unsigned int length) {
  String myTopic;
  myTopic = String(myTopic + topic); // ép kiểu dữ liệu của topic thành String
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char receivedChar[length];
  for (int i = 0; i < length; i++) {
    receivedChar[i] = (char)payload[i]; // nhập dữ liệu kiểu char
  }
  String data;
  data = String(receivedChar + data);
  Serial.println(data);
  // phân loại topic
  if(myTopic == sub_ctl){
    float abc = atof(receivedChar); // biến dữ liệu từ array char sang float
    biengoc = abc;
  }
  if(myTopic == sub_dc){
    float abc = atof(receivedChar);// biến dữ liệu từ array char sang float
    controlMotor = abc;
    Serial.print("bien control motor: ");
    Serial.println(controlMotor);
  }
  if(myTopic == sub_led){
    float abc = atof(receivedChar);// biến dữ liệu từ array char sang float
    controlLed = abc;
    Serial.print("bien control led: ");
    Serial.println(controlLed);
  }
}

//*********************************CHƯƠNG TRÌNH KẾT NỐI LẠI MQTT VÀ NHẬN GIÁ TRỊ TỪ CLOUD***************************
void reconnect() {
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // các topic subcribe
      client.subscribe(sub_ctl);
      client.subscribe(sub_dc);
      client.subscribe(sub_led);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
//--------------convert arraychar to float------------------
// float receiveData(unsigned int l, char a[]){
//   float value=0;
//   int z = 0;
//   for (int i = 0; i < l; i++)
//   {
//     if((char)a[l-i-1]== '.'){
//      // Serial.println("dau cham vi tri: ");
//      // Serial.println(i);
//       value = value*pow(10,-i);
//       //Serial.println(value);
//       z = -i-1;
//     }
//     else if ((char)a[l-i-1]== '-')
//     {
//       value = -value;
//     }
//     else{
//     value += (float)((char)a[l-i-1]-'0')*pow(10,i+z);
//     //Serial.println(value);
//     }
//   }
//   //Serial.println(value);
//   return value;
// }
