
#include <InnovaS_Dactilar.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
bool time;
//define the cymbols on the buttons of the keypads
char hexaKeys[COLS][ROWS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
char numeros[10]={'0','1','2','3','4','5','6','7','8','9'};
byte rowPins[COLS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[ROWS] = {8, 9, 10, 11}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27,16,2);

// pin #2 - Entrada desde el Sensor  (cable verde)
// pin #3 - Salida de desde Arduino  (cable blanco)

SoftwareSerial MySerial(2, 3);

uint32_t ClaveSensorHuella = 0;
InnovaS_Dactilar MySensorDactilar = InnovaS_Dactilar(&MySerial, ClaveSensorHuella);

void setup()
{ 
    Serial.begin(9600);
    Serial.println("Sensor de Huella");
  
    //Setea la velocidad de comunicacion con el sensor de huella
    //Iniciar verificando los valores de 9600 y 57600
    MySensorDactilar.begin(57600);
    
    if (MySensorDactilar.VerificarClave()) {
      Serial.println("Sensor de Huella Encontrado :)  :) ");
    } else {
      Serial.println("No fue posible encontrar al sensor de Huella  :(  :( ");
      while (1);
    }
    lcd.init();
    lcd.backlight();
    lcd.print("Bienvenidos");
    delay(1500);
}

void loop()                     
{ 
  char customKey = customKeypad.getKey();
  switch (customKey){
    case 'A':
      lcd.clear();
      lcd.print("Ingrese su huella: ");
      for(int j = 0; j < 10; j++){
        BuscarID_Huella();
        delay(100);
      }
      break;
    case 'B':
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Ingrese Numero");
      lcd.setCursor(0,1);
      lcd.print("(1 -127):");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Y presione A");
      int my_id = 0;
      String number;
      bool aux=true;
      while (aux == true) {
        char c = customKeypad.getKey();
        for(int i = 0; i < 10 ; i++){
          if(c == numeros[i]){
            number += c;
            lcd.clear();
            lcd.print(number);
          }
        }
        if(number != ""){
          if(c == 'A'){
            int aux2 = number.toInt();
            if (aux2 > 0 && aux2 < 128){
              lcd.clear();
              SDACTILAR_Enrolar(aux2);
              lcd.print("Registro exitoso");
              aux = false;
            }else {
              lcd.clear();        
              lcd.setCursor(0,0);
              lcd.print("Numero equivocado");
              lcd.setCursor(0,1);
              lcd.print("Ingrese de nuevo");
              number = "";              
            }
          }
        }
      }
      break;
    default:
      break;
  }
}


boolean SDACTILAR_Enrolar(int id) 
{
  int p = -1;
  lcd.clear();
  lcd.print("Esperando una huella");
  
  while (p != SDACTILAR_OK) {
    p = MySensorDactilar.CapturarImagen();
    switch (p) {
    case SDACTILAR_OK:
      Serial.println(" ");
      Serial.println("Imagen Tomada");
      break;
    case SDACTILAR_D_NO_DETECTADO:
      Serial.print(".");
      //Serial.println("No se encuentra al dedo");
      break;
    case SDACTILAR_PAQUETE_IN_ERROR:
      Serial.println("Error al recibir el paquete");
      break;
    case SDACTILAR_IMG_ERROR:
      Serial.println("Error al determinar la imagen");
      break;
    default:
      Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
      break;
    }
  }

  // OK success!
  p = -1; 
  p = MySensorDactilar.GenerarImg2Tz(1);
  switch (p) {
    case SDACTILAR_OK:
      Serial.println("Imagen Convertida");
      break;
    case SDACTILAR_IMGCONFUSA:
      Serial.println("Image muy confusa");
      return false; 
    case SDACTILAR_PAQUETE_IN_ERROR:
      Serial.println("Paquetes Errados");
      return false; 
    case SDACTILAR_RASGOSERROR:
      Serial.println("No es posible detectar los rasgos caracteriticos");
      return false; 
    case SDACTILAR_IMGINVALIDA:
      Serial.println("Imagen invalida");
      return false; 
    default:
      Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
      return false; 
  }
//------------------------------------
  lcd.clear();
  lcd.print("Remover dedo");
  delay(2000);
  p = -1;
  while (p != SDACTILAR_D_NO_DETECTADO) {
    p = MySensorDactilar.CapturarImagen();
  }
  p = -1;
  lcd.clear();
  lcd.print("Poner nuevamente");
 
  while (p != SDACTILAR_OK) {
    p = MySensorDactilar.CapturarImagen();
    switch (p) {
    case SDACTILAR_OK:
      Serial.println(" ");
      Serial.println("Imagen Tomada");
      break;
    case SDACTILAR_D_NO_DETECTADO:
      Serial.print(".");
      //Serial.println("No se encuentra al dedo");
      break;
    case SDACTILAR_PAQUETE_IN_ERROR:
      Serial.println("Error al recibir el paquete");
      break;
    case SDACTILAR_IMG_ERROR:
      Serial.println("Error al determinar la imagen");
      break;
    default:
      Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
      break;
    }
  }
  // OK success!
  p = -1;
  p = MySensorDactilar.GenerarImg2Tz(2);
  switch (p) {
    case SDACTILAR_OK:
      Serial.println("Imagen Convertida");
      break;
    case SDACTILAR_IMGCONFUSA:
      Serial.println("Image muy confusa");
      return false; 
    case SDACTILAR_PAQUETE_IN_ERROR:
      Serial.println("Paquetes Errados");
      return false; 
    case SDACTILAR_RASGOSERROR:
      Serial.println("No es posible detectar los rasgos caracteriticos");
      return false; 
    case SDACTILAR_IMGINVALIDA:
      Serial.println("Imagen invalida");
      return false; 
    default:
      Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
      return false; 
  }
  // OK converted!
  p = -1;
  p = MySensorDactilar.CrearModelo();
  if (p == SDACTILAR_OK) 
  {
    lcd.clear();
    lcd.print("Registro exitoso");
  } else if (p == SDACTILAR_PAQUETE_IN_ERROR) {
    Serial.println("Error de comunicacion");
    return false; 
  } else if (p == SDACTILAR_ENROLL_MISMATCH) {
    lcd.clear();
    lcd.print("Error");
    return false; 
  } else {
    Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
    return false; 
  }   

  Serial.print("ID "); Serial.println(id);
  p = MySensorDactilar.GuardarModelo(id);
  if (p == SDACTILAR_OK) {
    Serial.println("EXITO - Huella Guardada!");
  } else if (p == SDACTILAR_PAQUETE_IN_ERROR) {
    Serial.println("Error de comunicacion");
    return false; 
  } else if (p == SDACTILAR_ERROR_UBICACION) {
    Serial.println("No se puede ubicar en la ubicacion asignada");
    return false; 
  } else if (p == SDACTILAR_ERROR_FLASH) {
    Serial.println("Error escribiendo en la flash");
    return false; 
  } else {
    Serial.print("Error Desconocido: 0x"); Serial.println(p, HEX);
    return false; 
  }   
  return false; 
}

void BuscarID_Huella() 
{
  int p = -1;
  p = MySensorDactilar.CapturarImagen();
  if (p != SDACTILAR_OK)  return;

  p = MySensorDactilar.GenerarImg2Tz();
  if (p != SDACTILAR_OK)  return;

  p = MySensorDactilar.BusquedaRapida();
  if (p != SDACTILAR_OK)  return;
  
  Serial.print("Encontrado ID #"); Serial.print(MySensorDactilar.Id_Dactilar); 
  Serial.print(" con seguridad: "); Serial.println(MySensorDactilar.Seguridad);
  if(MySensorDactilar.Seguridad != 0){
    lcd.clear();
    if(MySensorDactilar.Seguridad > 70){
      lcd.print("Ingreso exitoso");
      delay(500);
      Retiro();
    } else if(MySensorDactilar.Seguridad < 70){
      lcd.print("No reconoce la huella");
    }
  }
}

void Retiro(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese la cantidad");
  lcd.setCursor(0,1);
  lcd.print("a retirar");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Solo multiplos");
  lcd.setCursor(0,1);
  lcd.print("de 10");
  String number;
  bool aux=true;
  while (aux == true) {
    char c = customKeypad.getKey();
    for(int i = 0; i < 10 ; i++){
      if(c == numeros[i]){
        number += c;
        lcd.clear();
        lcd.print(number);
      }
    }
    if(number != ""){
      if(c == 'A'){
        int aux2 = number.toInt();
        if (aux2 % 10 == 0){
          lcd.clear();
          lcd.print("Registro exitoso");
          aux = false;
        }else {
          lcd.clear();        
          lcd.setCursor(0,0);
          lcd.print("Numero equivocado");
          lcd.setCursor(0,1);
          lcd.print("Ingrese de nuevo");
          number = "";              
        }
      }
    }
  }
}
