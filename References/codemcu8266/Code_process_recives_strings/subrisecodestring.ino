String myTopic;
  myTopic = String(myTopic + topic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  /////////////////////nhận dữ liệu
  char receivedChar[length];
  for (int i = 0; i < length; i++) {
    receivedChar[i] = (char)payload[i];
    //Serial.println(receivedChar);
    //Serial.println(receivedChar[i]);
  }
  // String data;
  // data = String(receivedChar + data);
  float abc = atof(receivedChar);
  // data.toFloat();
  Serial.println(abc);
  if(myTopic == "biengoc"){
    biengoc = abc;
    Serial.print("bien goc: ");
    Serial.println(biengoc);
  }
  if(myTopic == "bienX"){
    bienX = abc;
    Serial.print("bien goc X: ");
    Serial.println(bienX);
  }
  if(myTopic == "bienY"){
    bienX = abc;
    Serial.print("bien goc Y: ");
    Serial.println(bienX);
  }
  if(myTopic == "bienZ"){
    bienX = abc;
    Serial.print("bien goc Z: ");
    Serial.println(bienX);
  }
  if(myTopic == "controlMotor"){
    controlMotor = abc;
    Serial.print("bien control motor: ");
    Serial.println(controlMotor);
  }
  if(myTopic == "controlLed"){
    controlLed = abc;
    Serial.print("bien control led: ");
    Serial.println(controlLed);
  }
  ///////////////////////////////////////////////////nhan chuoi xu ly thu cong
  String myTopic;
  myTopic = String(myTopic + topic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  /////////////////////nhận dữ liệu
  char receivedChar[length];
  for (int i = 0; i < length; i++) {
    receivedChar[i] = (char)payload[i];
    //Serial.println(receivedChar);
    //Serial.println(receivedChar[i]);
  }
  String data;
  data = String(receivedChar + data);
  // // data.toFloat();
   Serial.println(data);
  if(myTopic == "biengoc"){
    float abc = atof(receivedChar);
    biengoc = abc;
    Serial.print("bien goc: ");
    Serial.println(biengoc);
  }
  if(myTopic == "bienX"){
    float abc = atof(receivedChar);
    bienX = abc;
    Serial.print("bien goc X: ");
    Serial.println(bienX);
  }
  if(myTopic == "bienY"){
    float abc = atof(receivedChar);
    bienY = abc;
    Serial.print("bien goc Y: ");
    Serial.println(bienY);
  }
  if(myTopic == "bienZ"){
    String abc;
    for(int i=0;i<length;i++){
      abc += receivedChar[i];
      //Serial.println(i);
    }
    if(abc == "hello"){
      Serial.println("da so sanh duoc chuoi hello");
    }
    bienZ = abc;
    Serial.print("bien goc Z: ");
    Serial.println(bienZ);
  }

  if(myTopic == "bienZ"){
    String abc;
    for(int i=0;i<length;i++){
      abc += receivedChar[i];
      //Serial.println(i);
    }
    if(abc == "hello"){
      Serial.println("da so sanh duoc chuoi hello");
    }
    bienZ = abc;
    Serial.print("bien goc Z: ");
    Serial.println(bienZ);
  }
