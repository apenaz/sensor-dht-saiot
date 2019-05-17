/**
 * Exemplo de código para plataforma
 * SaIoT usando o protocolo MQTT. Nesse exemplo foi usado a
 * biblioteca para dispositivos IoT se comunicarem com a
 * plataforma.
 * Autores:
 * Patricio Oliveira patricio@bct.ect.ufrn.br
 * Danielly Costa daniellycosta97@ufrn.edu.br
 * Ricardo Cavalcanti ricavalcanti@bct.ect.ufrn.br
 *
 * */

 #include <Arduino.h>
 #include <SaIoTDeviceLib.h>

 // Sensores
 #include <Wire.h>
 #include <Adafruit_Sensor.h>
 #include <DHT_U.h>

#define DHTPIN            D5         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
void dhtDisplaySensorDetails(void);
void dhtValue(void);

SaIoTDeviceLib sensorDht("sensores","40206","ricardo@email.com");
SaIoTSensor sensorDht_t("t1","temperatura"," C","number");
SaIoTSensor sensorDht_u("u1","umidade"," %","number");

#define timeToSend 1000

WiFiClient espClient;
void callback(char* topic, byte* payload, unsigned int length);
String senha = "12345678910";
unsigned long tDecorrido;
String hora ="";

void setup(){
  Serial.begin(115200);
  Serial.println("INICIO");
  delay(1000);

  dht.begin();
  dhtDisplaySensorDetails();

  sensorDht.addSensor(sensorDht_t);
  sensorDht.addSensor(sensorDht_u);
  
  sensorDht.preSetCom(espClient, callback);
  sensorDht.startDefault(senha);

	tDecorrido = millis();
}

void loop(){
	if( ((millis() - tDecorrido)) >= timeToSend ){
    hora = SaIoTCom::getDateNow();
    dhtValue();

		tDecorrido = millis();
	}
	sensorDht.handleLoop();
}

void callback(char* topic, byte* payload, unsigned int length){
  String payloadS;
  Serial.print("Topic: ");
  Serial.println(topic);
  for (unsigned int i=0;i<length;i++) {
    payloadS += (char)payload[i];
  }
  if(strcmp(topic,sensorDht.getSerial().c_str()) == 0){
    Serial.println("SerialLog: " + payloadS);
  }
}


void dhtDisplaySensorDetails(void){
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
}
void dhtValue(void){
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    sensorDht_t.sendData(event.temperature,hora);
    Serial.println(" *C");
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    sensorDht_u.sendData(event.relative_humidity,hora);
    Serial.println("%");
  }
}