#include <Arduino.h>
#include <Wire.h>
#include <PID_v1.h>

#define ADDRESS             0x58                    // Address of MD04
#define SOFTREG             0x07                    // Byte to read software
#define CMDBYTE             0x00                    // Command byte
#define SPEEDBYTE           0x02                    // Byte to write to speed register
#define TEMPREG             0x04                    // Byte to read temperature
#define CURRENTREG          0x05          

int k;

//Sens du courant
byte direct = 2; 

//Courant
float i_ini=1.90;
int i_mot = 190;
float i=i_ini;

//Hauteurs
float h_0=75;
float h;

//Variables d'asservissement 
double consigne, entree, sortie;
double Kp=0, Ki=10, Kd=0;

PID myPID(&entree, &sortie, &consigne, Kp, Ki, Kd, DIRECT);


void setup() {
  Serial.begin(115200);
  Wire.begin();                                     // Start I2C connection
  consigne=75;
  myPID.SetMode(AUTOMATIC);
  delay(100);
  Serial.println(getData(SOFTREG));
  delay(1000);
}

void loop() {
//char k[5] ;
  if (Serial.available()){
    String inputStr = Serial.readStringUntil('\n'); // Lis jusqu'au retour à la ligne
    h = inputStr.toFloat();
    
    Serial.print("hauteur : ");
    Serial.println(h);
    entree=h;
    myPID.Compute();
    Serial.println(sortie);
    
  sortie=constrain(sortie,-100,250);
  Serial.print("erreur après correcteur :");
  Serial.println(sortie);
  sendData(SPEEDBYTE, sortie);             // Sets speed to i
  sendData(CMDBYTE, direct);          // Sets motor to direct, a value of 1 runs the motor forward and 2 runs backward
  showData(direct, sortie);
  delay(210);
    }
}

byte getData(byte reg){                 // function for getting data from MD03
  Wire.beginTransmission(ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(ADDRESS, 1);         // Requests byte from MD03
  while(Wire.available() < 1);          // Waits for byte to become available
  byte data = Wire.read();

  return(data);
}

void sendData(byte reg, byte val){      // Function for sending data to MD03
  Wire.beginTransmission(ADDRESS);      // Send data to MD03
    Wire.write(reg);                    // Command like Direction, Speed
    Wire.write(val);                    // Value for the command
  Wire.endTransmission();
}

void showData(byte valdirection, byte valSpeed) {
    Serial.print("Direction: ");
    Serial.print(valdirection);         // Prints current direction (1 or 2)
    Serial.print(" - Speed: ");
    Serial.print(valSpeed);             // Prints current Speed (0-250) tot Serial   
    Serial.println(".");                // Point at the end of the line and an Enter
}
