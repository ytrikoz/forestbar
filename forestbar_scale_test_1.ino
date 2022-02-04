//31 october 2015 
// added clean command reverse 5 second
// added wtc - Water Clean command 10 second each
// added rtc - Reverse Water Clean command 10 second each
// added cancel - Cancel all commands.
// Manual:
// Com port send command ^^RA1000## - relay 1 for 1 second
// clean , rtc = voltage inverters  and activate pump
// wtc activate relay 
//^^RA19,RB10##,RC100##

#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;


HX711 scale;

float cal1 = 0.0;
float cal2 = 0.0;
float bre = 0.0;
float data2 = -10.0;
//float calibration_factor = -1955;
float calibration_factor = -1800;
boolean done = false;
boolean emerg = false;
///HX711 scale(12, 13);

//RM,RN voltage inverters
#define RELAY_ON 0
#define RELAY_OFF 1
int datax;
enum Commands : uint8_t

{
  RA = 14,
  RB = 10,
  RC = 8,
  RD = 6, 
  RE = 16,
  RF = 4,
  RG = 5,
  RH = 17,
  RI = 7, 
  RJ = 9,
  RK = 11,
  RL = 15,
  RM = 18,//power
  RN = 19 //power
};
struct Mapping
{
  String cmd;
  int id;
};

const struct Mapping map2[] = { { "RA", RA },{ "RB", RB },{ "RC", RC },{ "RD", RD },{ "RE", RE },
{ "RF", RF },{ "RG", RG },{ "RH", RH },{ "RI", RI },{ "RJ", RJ },{ "RK", RK },{ "RL", RL },{ "RM", RM },{ "RN", RN } };

String arr[] = { "RA","RB","RC","RD","RE","RF","RG","RH","RI","RJ","RK","RL","RM","RN" };
int number_of_relays = sizeof(arr) / sizeof(arr[0]);
String readString, data;
int data1, val;

//int RI,RJ,RK,RL,RM,RN,RO,RP;
String inputString = "";
boolean stringComplete = false;
boolean stringStart = false;
int ledPin = 13;                 // LED pin 13
unsigned long currentTime;
unsigned long loopTime;

int GetIdByCmd(const String &cmd)
{
  int id = -1;

  for (unsigned i = 0, len = (sizeof(map2) / sizeof(map2[0])); i < len; ++i)
  {
    if (map2[i].cmd == cmd)
    {
      id = map2[i].id;
      break;
    }
  }
  return id;
}

void serialEvent() {
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '^')
    {
      inputString = "";
      stringStart = true;
    }
    if (stringStart == true && stringComplete != true)
    {
      inputString += inChar;
    }
    if (inChar == '#')
    {
      stringComplete = true;
    }
  }
  if (stringComplete)
  {
    if ((inputString.startsWith("^")) && (inputString.endsWith("#")))
    {

    if(inputString.indexOf("ready") >=0)
{
     Serial.println("iamready");
     Serial.println("iamready");
     Serial.println("iamready");
      inputString = "";
      goto drom;
      
}     

//cancel
    if(inputString.indexOf("cancel") >=0)
{
      for (int x = 0; x < (number_of_relays - 2); x++) 
      {   
        digitalWrite(GetIdByCmd(arr[x]), RELAY_OFF);
      }
    digitalWrite(18, RELAY_OFF);
    digitalWrite(19, RELAY_OFF);

      inputString = "";
      goto drom;
}

//clean - Reverse Clean command - 5 second each
    if(inputString.indexOf("clean") >=0)
{
  Serial.println("Clean start");
    digitalWrite(18, RELAY_ON);
    digitalWrite(19, RELAY_ON);
    delay (500);
      for (int z = 0; z < (number_of_relays - 2); z++) 
      {   
        digitalWrite(GetIdByCmd(arr[z]), RELAY_ON);
        delay (5000);
        digitalWrite(GetIdByCmd(arr[z]), RELAY_OFF);
        delay (500);
      }
    digitalWrite(18, RELAY_OFF);
    delay (300);
    digitalWrite(19, RELAY_OFF);
      inputString = "";
      goto drom;
}

//rtc - Reverse Water Clean command 10 sec each
      if(inputString.indexOf("rtc") >=0)
{
   Serial.println("Reverse Water Clean start");
    digitalWrite(18, RELAY_ON);
    digitalWrite(19, RELAY_ON);
    delay (500);
      for (int x = 0; x < (number_of_relays - 2); x++) 
      {
        digitalWrite(GetIdByCmd(arr[x]), RELAY_ON);
        // Serial.println("GetIdByCmd(arr[x])", "relay is on");
        delay (10000);
        digitalWrite(GetIdByCmd(arr[x]), RELAY_OFF);
        delay (500);
         // Serial.println("GetIdByCmd(arr[x])", "relay is off");
      }
    digitalWrite(18, RELAY_OFF);
    digitalWrite(19, RELAY_OFF);
    inputString = "";
    goto drom;
}

//added wtc - Water Clean command 10 sec each
      if(inputString.indexOf("wtc") >=0)
{
   Serial.println("wtc start");
    for (int x = 0; x < (number_of_relays - 2); x++) 
  {
    int so=GetIdByCmd(arr[x]);
    digitalWrite(GetIdByCmd(arr[x]), RELAY_ON);
      Serial.print("\t relay is on , pin = ");
      Serial.println(so);//display value of integer i   
    delay (10000);
    digitalWrite(GetIdByCmd(arr[x]), RELAY_OFF);
    delay (500);
      Serial.print("\t relay is off , pin = ");
      Serial.println(so);//display value of integer i
  }
      inputString = "";
      goto drom;
}

      inputString = inputString.substring(0, inputString.length() - 1); //cut last 1 char #
      inputString.remove(0, 1); //cut first 1 char ^
      inputString = (inputString + ",");
      //Serial.print ("start count");  
      int Tag1 = 0;
      for (int i = 0; i<number_of_relays; i++)
      {
        int Tag = inputString.indexOf("R", +((inputString.indexOf("R") + Tag1)));
        Tag1 = inputString.indexOf(",", Tag + 1);
        if (Tag < 0)
        {
          i = 0;
          break;
        }

        data = inputString.substring(Tag, Tag + 2);
        data1 = inputString.substring(Tag + 2, Tag1).toInt();
//        scale.set_scale();
//        scale.tare(); //Reset the scale to 0
  scale.set_scale(calibration_factor);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

        
        start((data), (data1));
        
        if (emerg) { break; }
      }
      Serial.println("Completed");
      delay(100);
      Serial.println("Completed");
      delay(100);
      Serial.println("Completed");
      delay(100);
      Serial.println("Completed");
      delay(100);
      Serial.println("Completed");
      delay(100);
      Serial.println("Forestbar Ready");

    }
    drom:
    inputString = "";
    stringComplete = false;
    stringStart = false;
  }
}

void sex(int data1, String data)
{ //start func sex
      Serial.print("data1: " );
      Serial.println(data1);
      Serial.print("data: " );
      Serial.println(data);
  
  char buffer[50];
  int test = 0; //value for increment for remove weight check
  cal1 = 0;
  while (!done || !emerg)
  {
    if ((cal1 < ((float)data1)) && (done == false)) { //cal1 0.0,data1 - ml input

      scale.power_up(); //turn up weight sensor
      scale.set_scale(calibration_factor);

      /////cal1 = (scale.get_units(5));
      cal1 = (scale.get_units(1));
      //Serial.print ("scale.get_units: ");
      //Serial.println (scale.get_units(10), 1);
      //Serial.print ("cal1: ");
      //Serial.println (cal1);
      //cal1 = scale.get_units(10);// get value
      char temp[15];
      dtostrf(cal1, 4, 1, temp);
      Serial.print("temp value: ");
      Serial.println(temp);


      sprintf(buffer, "current:%s need:%d", temp, data1);
      Serial.println(buffer);
      Serial.println("----------------");
      //info value float data2=-10.0;
      //bre = scale.get_units(5) - cal1;
      bre = ((scale.get_units(1)) - cal1);
      Serial.print("bre(differs from latest weight): ");
      Serial.println(bre);
      if ( ( (cal1 < data2) || (bre  < data2) )  && ((done == false) && (emerg == false)) ) { //emergency stop, weight lower than -10.0//edited 2021
        test++; //increment counter of weight lower
        //if (test >= 3) {
        bre = ((scale.get_units(1)) - cal1);
        if (bre < data2){
        for (int x = 0; x < number_of_relays; x++) {
            digitalWrite(GetIdByCmd(arr[x]), RELAY_OFF);
            digitalWrite(ledPin, LOW);
          }
          done = true;
          emerg = true;
          Serial.print("We set emerg true why?");
          Serial.print("BRE:");
          Serial.print(bre);
          Serial.print("data2:");
          Serial.print(data2);
          break;
        }
      }
    }
    //if ((cal1 >= ((float)data1)) && (done == false)) //mod 2021
    if ( ( (cal1  >= (float)data1)) && (done == false)) { //start if (liquid is enough)
      digitalWrite(GetIdByCmd(data), RELAY_OFF);
      digitalWrite(ledPin, LOW);
      done = true;
      break;
    } //end if (liquid is enough)
  }//end sex function
}

void start(String data, int data1)
{
  scale.tare();
  Serial.println(data);
  Serial.println(data1);
  Serial.println("-------");
  digitalWrite(ledPin, HIGH);
  digitalWrite(GetIdByCmd(data), RELAY_ON);
  done = false;
  emerg = false;
  sex(data1, data);
  Serial.println("---xxx----");
  return;
}

void setup() {

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //scale.set_scale();
  scale.set_scale(calibration_factor);                      // this value is obtained by calibrating the scale with known weights; see the README for details

  scale.tare();
  //sleep for delay to boot 60sec
  delay(60000);
  Serial.begin(57600);
  //-------( Initialize Pins so relays are inactive at reset)----
  for (int x = 0; x < number_of_relays; x++) {
    digitalWrite(GetIdByCmd(arr[x]), RELAY_OFF);
  }
  //---( THEN set pins as outputs )----  
  for (int x = 0; x < number_of_relays; x++) {
    pinMode(GetIdByCmd(arr[x]), OUTPUT);
  }

  delay(500); //Check that all relays are inactive at Reset
  pinMode(ledPin, OUTPUT);
  inputString.reserve(200);
  Serial.println("Forestbar Ready"); // so I can keep track of what is loaded
}

void loop() {
}
