/*
  Código criado para o robô seguidor de linha (LINHA DURA) da equipe Phantom Bee.
  Contribuintes - 
  Gabriel Amorim        Matrícula:  1230121318
  Bruno Verduc          Matrícula:  1230106373
*/

//////////////////////////////////////////////////////////////
//PORTAS/PARAMETROS E TESTES DO MOTOR(N20) / PONTE H(MX1616)//
//////////////////////////////////////////////////////////////
int CONTA_CURVA = 0;
int CONTA_VOLTA = 0;
//************************************************************

//////////////////////////////////////////////////////////////
//PORTAS/PARAMETROS E TESTES DO MOTOR(N20) / PONTE H(MX1616)//
//////////////////////////////////////////////////////////////
#define AIN1                5           //Motor da esquerda tras
#define AIN2                4           //Motor da esquerda frente
#define BIN1                8           //Motor da direita para tras
#define BIN2                7           //Motor da direita para frente
#define VELOCIDADE_MOTORA   11          //PORTA11
#define VELOCIDADE_MOTORB   3           //PORTA3
#define ESPERA              5           //TEMPO DE ESPERA PADRÃO (5)
#define VELOCIDADE_RETA     46          //48
#define ACERTO_RETA         30          //30
#define VELOCIDADE_CURVA    65        //60
#define ACERTO_CURVA        0           //0
#define FATOR_DIFERENCIAL_LINHA 500
int VELOCIDADE_OPERANTE_RETA = VELOCIDADE_RETA;
int VELOCIDADE_OPERANTE_CURVA = ACERTO_RETA;
#define LINHA               "PRETA"    //Linha BRANCA ou PRETA
int ultima_curva    = 0;
int linha                   =       0;
int fundo                   =       0;
int STATUS                  =       0;       //(0) PARADO (1)EM MOVIMENTO
bool curva                  =       false;
int tempo_curva             =       0;


int count = 0;

void moveLeft(int direction, int speed){
  
  analogWrite(VELOCIDADE_MOTORA, speed);
  if(direction == 1){ // gira anti-horário
    //STATUS = 1;
    digitalWrite(AIN1, HIGH); 
    digitalWrite(AIN2, LOW);
    Serial.println("Motor A - Frente"); 
  }else if(direction == 2){ // gira horário
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    Serial.println("Motor A - Trás");
  }else if(direction == 0 || speed == 0){ // Para motor
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    Serial.println("Motor A - PARADO");
  }
}

void moveRight(int direction, int speed){
  analogWrite(VELOCIDADE_MOTORB, speed);
  if(direction == 1){ // gira anti-horário
    //STATUS = 1;
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    Serial.println("Motor B - Frente");
  }else if(direction == 2){ // gira horário
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    Serial.println("Motor B - Trás");
  }else if(direction == 0 || speed == 0){ // Para motor
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    Serial.println("Motor B - PARADO");
  }
}
/*
void stop(int ultima){
  if(STATUS == 1){
    if(ultima == 1){
      moveLeft(2, VELOCIDADE_RETA);
      moveRight(1, VELOCIDADE_RETA);
      delay(ESPERA*30);
      STATUS = 0;
      ultima_curva = 0;
    }else if(ultima == 2){
      moveRight(2, VELOCIDADE_RETA);
      moveLeft(1, VELOCIDADE_RETA);
      delay(ESPERA*30);
      STATUS = 0;
      ultima_curva = 0;
    }
  }
*/

  void stop(int ultima){
    if(STATUS == 1){
      if(ultima == 1){
        Serial.println("");
        moveLeft(2, VELOCIDADE_RETA);
        moveRight(1, VELOCIDADE_CURVA);
        delay(ESPERA*100);
        STATUS = 1;
        //ultima_curva = 0;
      }else if(ultima == 2){
        
        moveRight(2, VELOCIDADE_RETA);
        moveLeft(1, VELOCIDADE_CURVA);
        delay(ESPERA*10);
        STATUS = 1;
        //ultima_curva = 0;
      }else if(ultima == 0){
        moveLeft(2, VELOCIDADE_RETA);
        moveRight(2, VELOCIDADE_RETA);
        delay(ESPERA*10);
        STATUS = 0;
        ultima_curva = 0;
      }
    }
  
    analogWrite(VELOCIDADE_MOTORA, 0);
    analogWrite(VELOCIDADE_MOTORB, 0);
    Serial.println("Solicitando a parada");
    delay(ESPERA);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    delay(ESPERA * 2);
  }
  

void TesteMotores(){
  moveLeft(1, VELOCIDADE_RETA);
  stop(0);
  moveLeft(2, VELOCIDADE_RETA);
  stop(0);
  moveRight(1, VELOCIDADE_RETA);
  stop(0);
  moveRight(2, VELOCIDADE_RETA);
  stop(0);
  moveLeft(1, VELOCIDADE_CURVA);
  moveRight(1, VELOCIDADE_CURVA);
  stop(0);
  moveLeft(2, VELOCIDADE_CURVA);
  moveRight(2, VELOCIDADE_CURVA);
  stop(0);
  stop(0);
}

/////////////////////////////////////////////////////////////
///////////PARAMETROS E PORTAS DOS SENSORES QRE-8D///////////
/////////////////////////////////////////////////////////////
#include <QTRSensors.h>
#define QTD_Sensors 4
#define FATOR_CALIBRACAO 100
QTRSensorsRC qtra((unsigned char[]) {A2, A3, A4, A5}, QTD_Sensors);



// Valores mínimos e máximos dos sensores para calibração
unsigned int qtr_reading_min[QTD_Sensors];
unsigned int qtr_reading_max[QTD_Sensors];

bool calibrated = false; // Flag para verificar se a calibração foi concluída

void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(VELOCIDADE_MOTORA, OUTPUT);
  pinMode(VELOCIDADE_MOTORB, OUTPUT);
  //TesteMotores();

  pinMode(A0, INPUT);               //ESQUERDA  CURVA
  pinMode(A1, INPUT);               //DIREITA   PARTIDA/CHEGADA

  // Se a calibração ainda não foi feita, realiza a calibração
  /*
  if (!calibrated) {
    if(LINHA == "PRETA"){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Iniciando a calibração...");
      delay(2000); // Tempo para o usuário posicionar o sensor

      // Calibra com o sensor sobre a superfície clara (fundo)
      Serial.println("Calibrando fundo branco...");
      delay(5000);
      digitalWrite(LED_BUILTIN, HIGH);
      for (int i = 0; i < QTD_Sensors; i++) {
        qtr_reading_min[i] = 1023; // Valor máximo possível de uma leitura analógica
      }


      for (int i = 0; i < FATOR_CALIBRACAO; i++) {
        unsigned int readings[QTD_Sensors];
        qtra.read(readings);
        for (int j = 0; j < QTD_Sensors; j++) {

          if (readings[j] < qtr_reading_min[j]) {
            qtr_reading_min[j] = readings[j];
          }
        }
        delay(20); // Atraso para estabilização

      }
      digitalWrite(LED_BUILTIN, LOW);

      Serial.println("Calibrando linha preta...");
      delay(5000);
      digitalWrite(LED_BUILTIN, HIGH);
      for (int i = 0; i < QTD_Sensors; i++) {
        qtr_reading_max[i] = 0; // Valor mínimo possível de uma leitura analógica
      }

      for (int i = 0; i < FATOR_CALIBRACAO; i++) {
        unsigned int readings[QTD_Sensors];
        qtra.read(readings);
        for (int j = 0; j < QTD_Sensors; j++) {
          if (readings[j] > qtr_reading_max[j]) {
            qtr_reading_max[j] = readings[j];
          }
        }
        delay(20); // Atraso para estabilização
      }
      digitalWrite(LED_BUILTIN, LOW);

      Serial.println("Calibração concluída.");
      delay(2500);
      Serial.println("Valores mínimos:");
      for (int i = 0; i < QTD_Sensors; i++) {
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(qtr_reading_min[i]);
      }

      Serial.println("Valores máximos:");
      for (int i = 0; i < QTD_Sensors; i++) {
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(qtr_reading_max[i]);
      }

      calibrated = true; // Define a flag de calibração como verdadeira
      digitalWrite(LED_BUILTIN, HIGH);
      delay(2500);        // Tempo para o usuário ver os resultados da calibração
      linha = 200;        // intensidade de recepção da linha
      fundo = 500;        // ... ... ... ... ... ... do fundo

    }
  }
  */
  Serial.println("Iniciando calibração automática");
  for (uint8_t i = 0; i < QTD_Sensors; i++){
    //qtra.calibratedMinimumOn[i] = 48;
    qtr_reading_min[i] = 20;
    //qtra.calibratedMaximumOn[i] = 2500;
    qtr_reading_max[i] = 2500;
    calibrated = true;
    Serial.print("Calibrando: ");
    Serial.println(i);
    delay(ESPERA);
  }
  Serial.println("Finalizando calibracao");
}

void loop(){
  if (calibrated) {
    unsigned int readings[QTD_Sensors];
    qtra.read(readings);

    Serial.println("Leituras dos sensores:");
    for (int i = 0; i < QTD_Sensors; i++) {
      // Normaliza as leituras
      unsigned int normalized = map(readings[i], qtr_reading_min[i], qtr_reading_max[i], 0, 1000);
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(normalized);
    }
    
    //delay(ESPERA); // Atraso para visualização dos dados

    unsigned int  SENSOR_1          = map(readings[0], qtr_reading_min[0], qtr_reading_max[0], 0, 1000);
    unsigned int  SENSOR_2          = map(readings[1], qtr_reading_min[1], qtr_reading_max[1], 0, 1000);
    unsigned int  SENSOR_3          = map(readings[2], qtr_reading_min[2], qtr_reading_max[2], 0, 1000);
    unsigned int  SENSOR_4          = map(readings[3], qtr_reading_min[3], qtr_reading_max[3], 0, 1000);
    //unsigned int SENSOR_5         = map(readings[4], qtr_reading_min[4], qtr_reading_max[4], 0, 1000);
    //unsigned int SENSOR_6         = map(readings[5], qtr_reading_min[5], qtr_reading_max[5], 0, 1000);
    int           SENSOR_CURVA      = analogRead(A0);  
    int           SENSOR_CHEGADA    = analogRead(A1);  
    
    // Lógica para seguir a linha preta
  /*
    if(SENSOR_2 > 500 && SENSOR_3 > 500){
      STATUS = 1;
      //Serial.println("SIGA EM FRENTE");
      moveLeft(1, VELOCIDADE_RETA);
      moveRight(1, VELOCIDADE_RETA);
    }else if(SENSOR_2 < 500 && SENSOR_3 < 500){
      if(SENSOR_1 > 500){
        STATUS = 1;
        //Serial.println("VIRE A ESQUERDA");
        moveLeft(1, VELOCIDADE_CURVA);
        moveRight(1, VELOCIDADE_RETA);
      }else if(SENSOR_4 > 500){
        STATUS = 1;
        Serial.println("VIRE A DIREITA");
        moveLeft(1, VELOCIDADE_RETA);
        moveRight(1, VELOCIDADE_CURVA);
      }else{
        Serial.println("PARE");
        stop();
      }
      
    }else if(SENSOR_2 > 500 && SENSOR_3 < 500){
      STATUS = 1;
      //Serial.println("VIRE A ESQUERDA");
      moveLeft(1, VELOCIDADE_CURVA);
      moveRight(1, VELOCIDADE_RETA);
    }else if(SENSOR_2 < 500 && SENSOR_3 > 500){
      STATUS = 1;
      //Serial.println("VIRE A DIREITA");
      moveLeft(1, VELOCIDADE_RETA);
      moveRight(1, VELOCIDADE_CURVA);
    }

    if(SENSOR_CURVA > 1000 && SENSOR_CHEGADA > 1000){
      Serial.println("Cruzamento");
    }else if(SENSOR_CURVA > 1000 && SENSOR_CHEGADA > 1000){
      Serial.println("LATERAL ESQUERDA: CURVA");
    }else if(SENSOR_CURVA < 1000 && SENSOR_CHEGADA > 1000){

      Serial.println("LATERAL DIREITA: LARGADA / CHEGADA");
    }
  */



// Lógica para seguir a linha branca
    if(SENSOR_2 < FATOR_DIFERENCIAL_LINHA && SENSOR_3 < FATOR_DIFERENCIAL_LINHA){
      STATUS = 0;
      ultima_curva = 0;
      //Serial.println("SIGA EM FRENTE");
      moveLeft(1, VELOCIDADE_OPERANTE_RETA);
      moveRight(1, VELOCIDADE_OPERANTE_RETA);
    }else if(SENSOR_2 > FATOR_DIFERENCIAL_LINHA && SENSOR_3 > FATOR_DIFERENCIAL_LINHA){
      if(SENSOR_1 < FATOR_DIFERENCIAL_LINHA){
        STATUS = 1;
        ultima_curva = 1;
        //Serial.println("VIRE A ESQUERDA");
        moveLeft(1, VELOCIDADE_OPERANTE_CURVA);
        moveRight(1, VELOCIDADE_OPERANTE_RETA);
      }else if(SENSOR_4 < FATOR_DIFERENCIAL_LINHA){
        STATUS = 1;
        ultima_curva = 2;
        //Serial.println("VIRE A DIREITA");
        moveLeft(1, VELOCIDADE_OPERANTE_RETA);
        moveRight(1, VELOCIDADE_OPERANTE_CURVA);
      }else{
        Serial.println("PARE");
        stop(ultima_curva);
      }
      
    }else if(SENSOR_2 < FATOR_DIFERENCIAL_LINHA && SENSOR_3 > FATOR_DIFERENCIAL_LINHA){
      STATUS = 1;
      ultima_curva = 1;
      //Serial.println("VIRE A ESQUERDA");
      moveLeft(1, VELOCIDADE_OPERANTE_CURVA);
      moveRight(1, VELOCIDADE_OPERANTE_RETA);
    }else if(SENSOR_2 > FATOR_DIFERENCIAL_LINHA && SENSOR_3 < FATOR_DIFERENCIAL_LINHA){
      STATUS = 1;
      ultima_curva = 2;
      //Serial.println("VIRE A DIREITA");
      moveLeft(1, VELOCIDADE_OPERANTE_RETA);
      moveRight(1, VELOCIDADE_OPERANTE_CURVA);
    }

    if(SENSOR_CURVA < 1000 && SENSOR_CHEGADA < 1000){
      Serial.println("LATERAIS : CRUZAMENTO");
    }else if(SENSOR_CURVA > 1000 && SENSOR_CHEGADA < 1000){
      Serial.println("LATERAL ESQUERDA: CURVA");
      moveLeft(1, 0);
      moveRight(1, 0);
      delay(500);
      curva = true;
      
      VELOCIDADE_OPERANTE_RETA = VELOCIDADE_CURVA;
      VELOCIDADE_OPERANTE_CURVA = ACERTO_CURVA;
    }else if(SENSOR_CURVA < 1000 && SENSOR_CHEGADA > 1000){
      Serial.println("LATERAL DIREITA: LARGADA / CHEGADA");
      VELOCIDADE_OPERANTE_RETA = VELOCIDADE_CURVA;
      VELOCIDADE_OPERANTE_CURVA = ACERTO_CURVA;
    }
if(curva){
  tempo_curva++;
  if(tempo_curva > 200){
    curva = false;
  }
  Serial.print("Tempo de curva: ");
  Serial.println(tempo_curva);
}else{
  tempo_curva = 0;
  VELOCIDADE_OPERANTE_RETA = VELOCIDADE_RETA;
  VELOCIDADE_OPERANTE_CURVA = ACERTO_RETA;
}

    delay(ESPERA);
    //delay(1000);

  }
}
