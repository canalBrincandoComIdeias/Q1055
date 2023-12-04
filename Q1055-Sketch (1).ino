//Definição do uso dos pinos do Arduino
#define pinMotor1A     11 //vd
#define pinMotor1B     10 //az
#define pinMotor2A     9  //br
#define pinMotor2B     6  //cz

#define pinSensorLinD  4  //cz 
#define pinSensorLinE  5  //br

#define pinDistEcho    2  //am
#define pinDistTrigger 3  //lj 

#define pinBtRx        8  //vd
#define pinBtTx        7  //az

//Biblioteca para comunicação com módulo Bluetooth
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(pinBtRx, pinBtTx); // RX, TX

//Declaração das funções programadas abaixo
void enviaPulso();
void medeDistancia();

//Declaração das variaveis para Sensor de Distancia
volatile unsigned long  inicioPulso = 0;
volatile float          distancia   = 0;
volatile int            modo        = -1;

//Declaração das variaveis para Controle do Motor
int velocidade = 0;
bool sentido = 1;
int estado = 0;

//Configuração do projeto no Arduino
void setup() {
  //Configura o estado das portas do Arduino
  pinMode(pinMotor1A, OUTPUT);
  pinMode(pinMotor1B, OUTPUT);
  pinMode(pinMotor2A, OUTPUT);
  pinMode(pinMotor2B, OUTPUT);

  pinMode(pinSensorLinD, INPUT);
  pinMode(pinSensorLinE, INPUT);

  pinMode(pinDistEcho, INPUT);
  pinMode(pinDistTrigger, OUTPUT);

  //Desliga a porta de controle do sensor de distancia
  digitalWrite(pinDistTrigger, LOW);

  //Inicia a comunicação serial para monitorar o que está acontecendo pelo monitor serial
  Serial.begin(9600);

  //Inicia a cominicação serial com o módulo bluetooth
  bluetooth.begin(9600);

  //CONFIGURA A INTERRUPÇÃO PARA FUNCIONAMENTO DO SENSOR DE DISTANCIA
  attachInterrupt(digitalPinToInterrupt(pinDistEcho), medeDistancia, CHANGE); 
}

void loop() {
char recebido;

  //Teste do Bluetooth
  if (bluetooth.available()) {
    recebido = bluetooth.read();
    Serial.write(recebido);
    Serial.print("- ");
  } else {
    //Serial.print(" - ");
  }

  //Teste dos Sensores de Linha
  //Serial.print("SD:");
  //Serial.print(digitalRead(pinSensorLinD));
  
  //Serial.print(" SE:");
  //Serial.print(digitalRead(pinSensorLinE));

  //Teste do Sensor de Distancia
  //ENVIA O COMANDO PARA O MÓDULO LER A DISTANCIA
  enviaPulso();
  
  //A RESPOSTA DA DISTANCIA VEM POR INTERRUPÇÃO, SÓ PRECISA ESPERAR ALGUNS MILISSEGUNDOS
  delay(25); // TEMPO DE RESPOSTA APÓS A LEITURA
  //Serial.print(" Dist:");
  //Serial.print(distancia);
  //Serial.println("cm");

  //Logica de controle de velocidade e sentido de rotação dos motores
  /*
  if (sentido) {
    velocidade += 2;
    if (velocidade >= 255) {
       velocidade = 255;
       sentido = 0;
       estado++;
       if (estado > 5) estado = 0;
    }
  } else {
    velocidade -= 2;
    if (velocidade <= 0) {
       velocidade = 0;
       sentido = 1;
       estado++;
       if (estado > 5) estado = 0;
    }
  }
  */

  //Controle dos motores
  switch (estado) {
    case 0: //Para frente
       analogWrite(pinMotor1A, velocidade);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, velocidade);
       analogWrite(pinMotor2B, 0);
       break;
    case 1: //Para tras
       analogWrite(pinMotor1A, 0);
       analogWrite(pinMotor1B, velocidade);
       analogWrite(pinMotor2A, 0);
       analogWrite(pinMotor2B, velocidade);
       break;
    case 2: //Para direito
       analogWrite(pinMotor1A, velocidade);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, 0);
       analogWrite(pinMotor2B, 0);
       break;
    case 3: //Para esquerdo
       analogWrite(pinMotor1A, 0);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, velocidade);
       analogWrite(pinMotor2B, 0);
       break;
    case 4: //Parado
       digitalWrite(pinMotor1A, LOW);
       digitalWrite(pinMotor1B, LOW);
       digitalWrite(pinMotor2A, LOW);
       digitalWrite(pinMotor2B, LOW);
       break;
  }

  delay(20);
}

//PROGRAMAÇÃO DAS FUNÇÕES

//Envia um pulso para o sensor de distância
void enviaPulso(){
  // ENVIA O SINAL PARA O MÓDULO INICIAR O FUNCIONAMENTO
  digitalWrite(pinDistTrigger, HIGH);
  // AGUARDAR 10 uS PARA GARANTIR QUE O MÓDULO VAI INICIAR O ENVIO
  delayMicroseconds(10);
  // DESLIGA A PORTA PARA FICAR PRONTO PARA PROXIMA MEDIÇÃO
  digitalWrite(pinDistTrigger, LOW);
  // INDICA O MODO DE FUNCIONAMENTO (AGUARDAR PULSO)
  modo = 0;
}

//Calcula a distância detectada
//A interrupção será acionada quando o reflexo do pulso for recebido e então executa esta função.
void medeDistancia(){
  switch (modo) {
    case 0: {
        inicioPulso = micros();
        modo = 1;
        break;
      }
    case 1: {
        distancia = (float)(micros() - inicioPulso) / 58.3; // distancia em CM
        inicioPulso = 0;
        modo = -1;
        break;
      }
  }
}
