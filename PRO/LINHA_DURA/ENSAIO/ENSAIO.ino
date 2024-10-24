// Definição das Constantes e suas respectivas portas
const int motorE = 3;   // Ativação Motor Esquerda
const int motorD = 11;  // Ativação Motor Direita
const int FMD = 5;      // Direção do motor D - Frente
const int TMD = 4;      // Direção do motor D - Tras
const int FME = 8;      // Direção do motor E - Frente
const int TME = 7;      // Direção do motor E - Tras
const int VE = 60;      // Velocidade base do motor E
const int VD = 60;      // Velocidade base do motor D

#include <QTRSensors.h>
QTRSensors qtr;

int Kp = 35;  // Constante proporcional

// Função para parar os motores
void Parar() {
  analogWrite(motorE, 0);
  analogWrite(motorD, 0);
}

// Função para mover o robô para frente
void AndarFrente(int velocidadeE, int velocidadeD) {
  //digitalWrite(FME, HIGH);  // Ativa o motor esquerdo para frente
  digitalWrite(TME, LOW);   // Desativa motor esquerdo para tras
  digitalWrite(FMD, HIGH);  // Ativa motor direito para frente
  digitalWrite(TMD, LOW);   // Desativa motor direito para tras

  analogWrite(motorE, velocidadeE);
  analogWrite(motorD, velocidadeD);
  if(velocidadeE > 0){
    digitalWrite(FME, HIGH);
    digitalWrite(TME, LOW);
  }else{
    digitalWrite(FME, LOW);
    digitalWrite(TME, HIGH);
  }
  if(velocidadeD != 0){
    digitalWrite(FMD, HIGH);
    digitalWrite(TMD, LOW);
  }else{
    digitalWrite(FMD, LOW);
    digitalWrite(TMD, HIGH);

  }
}

// Função para controlar os motores com base no erro
void controlMotors(int16_t error) {
  // Ajuste proporcional para as velocidades dos motores
  int16_t adjustment = Kp * error;

  int16_t VelocidadeME = VE - adjustment;  // Ajusta a velocidade do motor esquerdo
  int16_t VelocidadeMD = VD + adjustment;  // Ajusta a velocidade do motor direito

  // Limita as velocidades para valores válidos 
  VelocidadeME = constrain(VelocidadeME, 0, VE);//75 IDEAL
  VelocidadeMD = constrain(VelocidadeMD, 0, VD);

  // Move o robô para frente com as velocidades ajustadas
  AndarFrente(VelocidadeME, VelocidadeMD);
}

void setup() {
  // Configuração dos pinos dos motores
  pinMode(motorE, OUTPUT);
  pinMode(motorD, OUTPUT);
  pinMode(FMD, OUTPUT);
  pinMode(TMD, OUTPUT);
  pinMode(FME, OUTPUT);
  pinMode(TME, OUTPUT);

  // Inicia a comunicação serial para monitoramento
  Serial.begin(9600);

  // Inicializa os sensores QTR-8D com os pinos correspondentes
  qtr.setTypeRC(); 
  qtr.setSensorPins((const uint8_t[]){A0 ,A1 ,A2 ,A3 ,A4 ,A5}, 6);

  // Calibra os sensores automaticamente
  Serial.println("Calibrando os sensores... Coloque o robô sobre a linha!");
  for (uint8_t i = 0; i < 150; i++) {  // Calibração durante 5 segundos
    qtr.calibrate();
    delay(20);
  }
  Serial.println("Calibração concluída!");
}

void loop() {
  // Leitura dos sensores após a calibração automática
  uint16_t sensors[6];
  int16_t position = qtr.readLineBlack(sensors); // Lê a linha preta

  // Exibe as leituras dos sensores no monitor serial
  Serial.print("Leituras dos sensores: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(sensors[i]);
    if (i < 5) {
      Serial.print(", ");
    }
  }
  Serial.println();

  // Calcula o "erro" da posição da linha
  int16_t error = position - 2500;  // Ajuste o valor central conforme o seu setup

  // Exibe a posição e o erro no monitor serial
  //Serial.print("Posição: ");
  //Serial.print(position);
  Serial.print(" | Erro: ");
  Serial.println(error);

  // Controle proporcional dos motores baseado no erro
  controlMotors(error);
  
  delay(15);
}

  // Exibe os valores dos sensores no monitor serial para depuração