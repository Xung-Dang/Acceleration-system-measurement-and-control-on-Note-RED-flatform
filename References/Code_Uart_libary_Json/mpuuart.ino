
// Include Wire Library for I2C
#include <Wire.h>
//#include <SPI.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial mySerial(10,11);
////Display
//#define OLED_RESET 4
////Adafruit_SSD1306 display(OLED_RESET);
//
//// OLED I2C bus address
//#define OLED_address  0x3c

Adafruit_MPU6050 mpu;


//Variables for Gyroscope
int gyro_x, gyro_y, gyro_z;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
boolean set_gyro_angles;

long acc_x, acc_y, acc_z, acc_total_vector;
float angle_roll_acc, angle_pitch_acc;

float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
float angle_pitch_output, angle_roll_output;

// Setup timers and temp variables
long loop_timer;
int temp;

// Display counter
//int displaycount = 0;
//float old_value_factor=0.5; 
//float new_value_factor = 1 - old_value_factor;
//int sample_cnt = 100;

void setup() {
    Serial.begin(115200);
    mySerial.begin(115200);


  //Start I2C
  Wire.begin();

//  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
//  display.clearDisplay();   // clears the screen and buffer 

 
  //Setup the registers of the MPU-6050                                                       
  setup_mpu_6050_registers(); 

  
  //Read the raw acc and gyro data from the MPU-6050 1000 times                                          
  for (int cal_int = 0; cal_int < 100 ; cal_int ++){                  
    read_mpu_6050_data(); 
    //Add the gyro x offset to the gyro_x_cal variable                                            
    gyro_x_cal += gyro_x;
    //Add the gyro y offset to the gyro_y_cal variable                                              
    gyro_y_cal += gyro_y; 
    //Add the gyro z offset to the gyro_z_cal variable                                             
    gyro_z_cal += gyro_z; 
    //Delay 3us to have 250Hz for-loop                                             
    delay(3);                                                          
  }

  // Divide all results by 1000 to get average offset
  gyro_x_cal /= 100;                                                 
  gyro_y_cal /= 100;                                                 
  gyro_z_cal /= 100;
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);

  // Init Timer 
  loop_timer = micros();                                               
}
StaticJsonBuffer<1000> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
void loop(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
//   accx = a.acceleration.x-0.82;
//   accy = a.acceleration.y-0.02;
//   accz = a.acceleration.z-1.40;
//   Temp = temp.temperature;

//  
//  Serial.println(a.acceleration.x);
//// Serial.print(",");
// Serial.println(a.acceleration.y);
//// Serial.print(",");
// Serial.println(a.acceleration.y);
  // Get data from MPU-6050
  read_mpu_6050_data();
     
  //Subtract the offset values from the raw gyro values
  gyro_x -= gyro_x_cal;                                                
  gyro_y -= gyro_y_cal;                                                
  gyro_z -= gyro_z_cal;                                                
         
  //Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  
  //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_pitch += gyro_x * 0.0000611;  
  //Calculate the traveled roll angle and add this to the angle_roll variable
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians                                
  angle_roll += gyro_y * 0.0000611; 
                                     
  //If the IMU has yawed transfer the roll angle to the pitch angle
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);
  //If the IMU has yawed transfer the pitch angle to the roll angle               
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               
  
  //Accelerometer angle calculations
  
  //Calculate the total accelerometer vector
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z)); 
   
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  //Calculate the pitch angle
  angle_pitch_acc = angle_pitch_acc * 0.95 + asin( (float)acc_y/acc_total_vector )* 57.296*0.05; 
  //Calculate the roll angle      
  angle_roll_acc = angle_roll_acc * 0.95 + asin((float)acc_x/acc_total_vector)* -57.296 *0.05;
  //Accelerometer calibration value for pitch
  angle_pitch_acc -= 0.0;
  //Accelerometer calibration value for roll                                              
  angle_roll_acc -= 0.0;                                               

  if(set_gyro_angles){ 
  
  //If the IMU has been running 
  //Correct the drift of the gyro pitch angle with the accelerometer pitch angle                      
    //angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;
   
    angle_pitch = angle_pitch * 0.5 + angle_pitch_acc * 0.5; 
    //Correct the drift of the gyro roll angle with the accelerometer roll angle    
    //angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004; 
    angle_roll = angle_roll *  0.5 + angle_roll_acc * 0.5;       
  }
  else{ 
    //IMU has just started  
    //Set the gyro pitch angle equal to the accelerometer pitch angle                                                           
    angle_pitch = angle_pitch_acc;
    //Set the gyro roll angle equal to the accelerometer roll angle                                       
    angle_roll = angle_roll_acc;
    //Set the IMU started flag                                       
    set_gyro_angles = true;                                            
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;
  //Take 90% of the output roll value and add 10% of the raw roll value 
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1; 
  //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop  
  
  // Print to Serial Monitor   
  //Serial.print(" | Angle  = "); Serial.println(angle_pitch_output);
//  Serial.print(accx);
//  Serial.print(",");
//  Serial.print(accy);
//  Serial.print(",");
//  Serial.print(accz);
    Serial.println(a.acceleration.x);
// Serial.print(",");
 Serial.println(a.acceleration.y);
// Serial.print(",");
 Serial.println(a.acceleration.y);
 Serial.println(angle_pitch_output);
 delay(100);
  // Increment the display counter
//  displaycount = displaycount +1;
  
//  if (displaycount > 10) {
//     // text display tests
//   display.display();
////  delay(2000); // Pause for 2 seconds
//
//  // Clear the buffer
//  display.clearDisplay();
//  display.setTextSize(1.2);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  //Display ACC
//  display.println("      MPU6050");
//  display.print("AngleX:");
//  display.print(angle_pitch_output);
//  display.print(" T:");
//  display.println(Temp);
//
////  display.println(acc_x);
////  display.print(", Y: ");
////  display.println(angle_roll_output);
//
//  display.print(" ax: ");
//  display.print(accx);
//
//  display.print("  ay: ");
//  display.println(accy);
//
//  display.print("   az: ");
//  display.print(accz);
//  display.println(" m/s^2");
//  
//    
//  displaycount = 0;
//  
//  }


 while(micros() - loop_timer < 4000); 
 //Reset the loop timer                                
 loop_timer = micros();
 
   root["hum"] = a.acceleration.x;
   root["hi"] = angle_pitch_output;
   root["temp"] = a.acceleration.y;
   root["k"] = a.acceleration.z;
   root["g"] =  temp.temperature;
   
  //cu phai gui di
  if (mySerial.available() > 0)
  {
    root.printTo(mySerial);
  }
}

void setup_mpu_6050_registers(){

  //Activate the MPU-6050
  
  //Start communicating with the MPU-6050
  Wire.beginTransmission(0x68); 
  //Send the requested starting register                                       
  Wire.write(0x6B);  
  //Set the requested starting register                                                  
  Wire.write(0x00);
  //End the transmission                                                    
  Wire.endTransmission(); 
                                              
  //Configure the accelerometer (+/-8g)
  
  //Start communicating with the MPU-6050
  Wire.beginTransmission(0x68); 
  //Send the requested starting register                                       
  Wire.write(0x1C);   
  //Set the requested starting register                                                 
  Wire.write(0x10); 
  //End the transmission                                                   
  Wire.endTransmission(); 
                                              
  //Configure the gyro (500dps full scale)
  
  //Start communicating with the MPU-6050
  Wire.beginTransmission(0x68);
  //Send the requested starting register                                        
  Wire.write(0x1B);
  //Set the requested starting register                                                    
  Wire.write(0x08); 
  //End the transmission                                                  
  Wire.endTransmission(); 
                                              
}


void read_mpu_6050_data(){ 

  //Read the raw gyro and accelerometer data

  //Start communicating with the MPU-6050                                          
  Wire.beginTransmission(0x68);  
  //Send the requested starting register                                      
  Wire.write(0x3B);
  //End the transmission                                                    
  Wire.endTransmission(); 
  //Request 14 bytes from the MPU-6050                                  
  Wire.requestFrom(0x68,14);    
  //Wait until all the bytes are received                                       
  while(Wire.available() < 14);
  
  //Following statements left shift 8 bits, then bitwise OR.  
  //Turns two 8-bit values into one 16-bit value                                       
  acc_x = Wire.read()<<8|Wire.read();                                  
  acc_y = Wire.read()<<8|Wire.read();                                  
  acc_z = Wire.read()<<8|Wire.read();                                  
  temp = Wire.read()<<8|Wire.read();                                   
  gyro_x = Wire.read()<<8|Wire.read();                                 
  gyro_y = Wire.read()<<8|Wire.read();                                 
  gyro_z = Wire.read()<<8|Wire.read();                                 
}
