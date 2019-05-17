# SaIoTDevicelib
Biblioteca genérica para gerenciamento de dispositivos IoT

## Conteúdo

- Gerenciamento de *WiFi* (WiFiManager);
- Manipulação de JSONs;
- Suporte a diferentes tipos de dispositivos (Analógicos, Digitais, Acumuladores, dentre outros);
- Suporte ao protocolo [MQTT](https://www.ibm.com/developerworks/br/library/iot-mqtt-why-good-for-iot/index.html).

## Como usar

### Incluir  no projeto

1. Para usar a biblioteca basta, primeiramente, fazer o [download do código](https://github.com/daniellycosta/SaiotDevice), extrair e copiar para a pasta de bibliotecas do seu projeto;
2. Deve-se também, fazer o *download* das seguintes bibliotecas (para comunicação): [WiFiManager](https://github.com/apenaz/WiFiManager) e [PubSubClient](https://github.com/knolleary/pubsubclient).
3. Incluir no arquivo fonte as instruções:

```c
#include <SaIoTDeviceLib.h>
```

### Escopo Global

1. No **escopo de varáveis globais**, deve-se instanciar um objeto tipo **WiFiClient**, declarar a função de callback (caso seja usado o protocolo MQTT) e declarar um objeto do tipo **SaIoTDeviceLib** com seus respectivos atributos. Para isso, é preciso definir três valores do tipo `String`, são eles: o nome do dispositivo (`deviceName`), o serial de identificação do dispositivo (`deviceSerial`) e o email (`emailUser`) de uma conta de usuário válida no SaIoT de quem irá cadastrar o dispositivo em questão, respectivamente.

```c++
WiFiClient espClient;
void callback(char* topic, byte* payload, unsigned int length);
SaIoTDeviceLib myDevice(deviceName, deviceSerial, emailUser);
```

2. Deve-se inicializar todos os controladores e sensores que fazem parte do dispositivo a ser criado utilizando, respectivamente, as classes **SaIoTController** e **SaIoTSensor**.

- Para um **controlador** deve-se passar três parâmetros do tipo **Strings**, necessariamente nesta sequência: uma **key** (dado para ser usado como referência daquele controlador), uma **tag** (apenas para identificação), e a **class** do controlador (que indica como ele será renderizado na interface de usuário). Uma forma alternativa é passar todo o seu JSON de configuração. A utilização desse segundo construtor é uma boa alternativa para caso seu controlador tenha mais atributos. Saiba mais detalhes sobre os [controladores aqui](/blog/2018/09/18/controladores.html).

```c++
SaIoTController myController(controllerKey, controllerTag, controllerType);
SaIoTController myController("{\"key\":\"controllerKeyJSON\",\"tag\":\"controllerTagJSON\",\"class\":\"onoff\"}");
```

Exemplo de configuração de um controlador para acionamento de algo:

```json
{
  "key": "on/off",
  "tag": "simpleToggle",
  "class":"onoff"
}
```

- Para um **sensor** deve-se passar como parâmetros **4 Strings**, necessariamente nesta sequência: uma **key** (dado para ser usado como referência daquele sensor), uma **tag**, a **unidade de medida** (para o dado a ser mensurado, como por exemplo Litros) e o **tipo** do dado (Boolean, String, Point, Number); assim como para o controlador, há uma forma alternativa de construtor no qual deve-se ser passado todo o seu JSON de configuração.

```c++
SaIoTSensor mySensor(sensorKey,sensorTag,dataUnit,dataType);
SaIoTSensor mySensor("{\"key\":\"hidro01\",\"tag\":\"hidrometro_01\",\"unit\":\"Litros\",\"type\":\"number\"}");
```

Exemplo de configuração de um sensor:

```json
{
  "key": "hidro01",
  "tag": "hidrometro_01",
  "unit":"Litros",
  "type":"number"
}
```

### Setup

1. Na função **Setup** é preciso associar os **Sensores** e **Controladores**, previamente instanciados, com o **Device**, utilizando os seguintes métodos da classe SaIoTDeviceLib, para **cada** controlador e sensor:

```c++
myDevice.addController(myController);
myDevice.addSensor(mySensor);
```

2. Iniciando a parte de comunicação, o objeto do tipo **WiFiClient** e a função de **Callback** devem ser passadas.

```c++
myDevice.preSetCom(espClient, callback);
```

 A função de **Callback** deverá ter a mesma estrutura do exemplo a seguir (Embora exemplificada aqui, esta função deve ser definida no Escopo Global).

```c++
void callback(char* topic, byte* payload, unsigned int length){
  String payloadS;

  for (unsigned int i=0; i < length; i++) {
    payloadS += (char)payload[i];
  }

  if(strcmp(topic,myDevice.getSerial().c_str()) == 0){
    // insira aqui o que o device deve fazer ao receber algo nesse tópico
  }

  if(strcmp(topic,(myDevice.getSerial()+myController.getKey()).c_str()) == 0){
    // insira aqui o que o device deve fazer ao receber algo nesse tópico
  }

  if(strcmp(topic,(myDevice.getSerial()+mySensor.getKey()).c_str()) == 0){
    // insira aqui o que o device deve fazer ao receber algo nesse tópico
  }

  // inclua outras condições para todos os controllers e sensores
}
```

**OBS.:** Para enviar dados do sensor, o método a seguir, da classe **SaIoTSensor**, deverá ser chamado passando o valor (double), a data e hora do dispositivo.

```c++
double valor;
String dataHora = "2018-09-20 11:20:55";
mySensor.sendData(valor, dataHora);
```

A dataHora deverá ser uma String no seguinte padrão, sem as aspas:
"YYYY-MM-DD hh:mm:ss".

3.  Iniciar a comunicação com o servidor chamando o seguinte método, passando uma **String** com a **senha do usuário**.

```c++
myDevice.startDefault(senhaDoUsuario);
```

### Loop

1. Por último, é necessário chamar o método handleLoop no escopo da função **Loop**.

```c++
bool handleLoop(void);
```

### Exemplo
Um exemplo de uso pode ser consultado na pasta [**examples**](https://github.com/daniellycosta/SaiotDevice/tree/master/examples) do repositório oficial da biblioteca. Nele é feito um Device com apenas um controlador e um sensor, onde é possível também se ter uma dica de como configurar o tempo de envio dos dados do sensor.

**OBS.:** Após importar a [PubSubClient](https://github.com/knolleary/pubsubclient) no segundo passo descrito em "Incluir no Projeto", é preciso alterar o valor "MQTT_MAX_PACKET_SIZE" no arquivo "PubSubClient.h" para  próximo de 500 a depender da quantidade de informações a serem enviadas.

```c++
#define  MQTT_MAX_PACKET_SIZE  500
```
**OBS.:** Caso seja a primeira conexão, o dispositivo entrará no modo AP. Para conectar, basta então, acessar a rede gerada pelo microcontrolador e configurá-la. No mais, o dispositivo tentará se reconectar com a rede salva anteriormente.
