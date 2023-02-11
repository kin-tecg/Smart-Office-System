#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <ThingSpeak.h> 

#define DHTPIN 12          // D6
#define DHTTYPE DHT11     // DHT 11
#define pump 13
#define BLYNK_PRINT Serial
#define BUZZER 15

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WiFiClient client; 
int led =5;
int led1 = 4;
int led2 = 14;
int number;
int relay = 0;
WidgetLED correct(V1); 
WidgetLED wrong(V2); 
int Gas;
char ssid[] = "MY HOME";
char password[] = "kairos21";
char auth[] = "-SeUO0VbFnMLBq2gxSyVxKbNHc_DO4gO"; //Blynk token
unsigned long myChannelNumber = 1378113;  
const char * myWriteAPIKey = "MP56BLB98KRN5PFJ";  
uint8_t temperature, humidity; 

BLYNK_WRITE(V0){

 String textIn = param.asStr(); //receive text
 Serial.print(textIn + "\n"); 
 number = textIn.toInt(); // convert string to integer
 Serial.print ("i received "); 
 Serial.println(number); 
 delay(1); 
  }

 
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V7, Gas);
}
 
void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode (led2, OUTPUT);
  pinMode(A0, INPUT);
  pinMode (relay, OUTPUT);
  pinMode (BUZZER, OUTPUT);
  pinMode(pump, OUTPUT);
  Blynk.begin(auth, ssid, password);
 
  dht.begin();
  delay(10);  
  Serial.println();  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(ssid);  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED)  
  {  
   delay(500);  
   Serial.print(".");  
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
 
  Serial.println(WiFi.localIP());  
  ThingSpeak.begin(client);   
  timer.setInterval(1000L, sendSensor);
}
 
void loop()
{
  Blynk.run();
  timer.run();
  static boolean data_state = false;  
  temperature = dht.readTemperature();  
  humidity = dht.readHumidity();  
  Gas = analogRead(A0);

  if (Gas >= 600){
    digitalWrite(led2, HIGH);
    digitalWrite(BUZZER,HIGH);
    delay(5000);
    digitalWrite (pump, HIGH);
    Serial.print("Gas is :");  
    Serial.print(Gas);  
    Serial.println(" %");
    Serial.println("WARNING WARNING");  
  }
  else{
    digitalWrite(led2, LOW);
    digitalWrite(BUZZER,LOW);
    delay(5000);
    digitalWrite (pump, LOW);
    Serial.print("Gas is :");  
    Serial.print(Gas);  
    Serial.println(" %");
    Serial.println("safe"); 
  }

  
  if ( humidity >=75){
  digitalWrite(led, HIGH);
  Serial.print("Humidity is :");  
  Serial.print(humidity);  
  Serial.println(" %");
  Serial.println("WARNING WARNING");  
  }
  else{
  digitalWrite(led, LOW);
  Serial.print("Humidity is :");  
  Serial.print(humidity);  
  Serial.println(" %");
  Serial.println("safe");  
  }
  
if ( temperature>=30){
  digitalWrite(led1, HIGH);
  Serial.print("Temp is :");  
  Serial.print(temperature);  
  Serial.println(" C");
  Serial.println("WARNING WARNING");  
  }
  else{
  digitalWrite(led1, LOW);
  Serial.print("Temp is :");  
  Serial.print(temperature);  
  Serial.println(" C");
  Serial.println("safe");  
  }

  if( data_state )  
  {  
   ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);  
   data_state = false;  
  }  
  else   {  
   ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);  
   data_state = true;  
  }  

  ThingSpeak.writeField (myChannelNumber, 3, Gas, myWriteAPIKey);

  if (number == 12345) //password
  {
  Serial.println("Correct Password"); 
  digitalWrite(relay, HIGH); 
  correct.on(); 
  wrong.off();
} else {
  Serial.println("Incorrect Password ... !!!");
  delay(1000); 
  digitalWrite(relay, LOW); 
  correct.off(); 
  wrong.on();
}

  
  delay(1000);
}
