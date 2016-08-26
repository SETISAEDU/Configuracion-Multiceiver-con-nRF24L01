/*  ----------------------------------------------------------------
    SETISAEDU
    Codigo para la configuracion MultiCeiver con NRF24L01.
    Codigo del PTX1 (Alarma con activacion por final de carrera(pulsador) )
  --------------------------------------------------------------------
*/
#include <SPI.h> //libreria para el uso del modulo SPI
#include "nRF24L01.h" //Libreria del modulos nRF24L01+
#include "RF24.h"

RF24 radio(9, 10); //Declaracion de los pines de control CE y CSN para el modulo, se define el objeto "radio"

/* Lista de direcciones de los transmisores 
direccion1 = 0x7878787878LL (Este nodo) 
direccion1 = 0xB3B4B5B6F1LL 
direccion3 = 0xB3B4B5B6CDLL 
direccion4 = 0xB3B4B5B6A3LL 
direccion5 = 0xB3B4B5B60FLL 
direccion6 = 0xB3B4B5B605LL
*/
const uint64_t direccion1 = 0x7878787878LL; // En esta parte LL significa LongLong para asegurar el dato de 64bits

int Alarm_out =4; //Pin de salida de la alarma
int Alarm_modo =5; //Pin indicador de alarma Armada o Desarmada

bool A_state=0; //Variable de estado logico de la salida del la alarma
bool Alarma=false,modo=true; //Variable de estado de la alarma, variable de modo de actividad
bool armado=1; /*Variable que se usa para discriminar las peticiones de interrupcion 
                *que se hagan mientras la alarma esta Desarmada
                */
void setup()

{

  pinMode(10, OUTPUT); //Iniciar como salida el pin de control CSN
  radio.begin(); //Inicio del modulo nRF24L01+
  //radio.setRetries(15, 15); //Configuracion del numero de intentos
  radio.setPALevel(RF24_PA_LOW);  // Configuracion en modo de baja potencia
  radio.setChannel(100); // Apertura del canal de comunicacion
    //Apertura de la linea de comunicacion, direccion 2
  radio.openReadingPipe(0, direccion1); 
  radio.startListening(); // iniciamos en modo de escucha

  //Configuracion de pines de salida de alarma e indicador
  pinMode(Alarm_out, OUTPUT);
  pinMode(Alarm_modo, OUTPUT);
  digitalWrite(Alarm_out,LOW);
  digitalWrite(Alarm_modo,HIGH); // La alarma inicia activa
  
  //Declaracion de interrupciones de el pulsadores y el sensor 
  pinMode(2, INPUT); //Disparador de la alarmar (Final de carrera, PIR, IR, Luz)
  pinMode(3, INPUT); //Boton de apagado de alarma
  attachInterrupt(digitalPinToInterrupt(2), Alarma_On, RISING);
  attachInterrupt(digitalPinToInterrupt(3), Alarma_Off, RISING);

}

void loop()
{
       radio.startListening(); //Se inicia en modo de escucha para poder recibir ordenes del maestro
      //Espera de peticiones
      if (radio.available())      // Comprobacion de datos recibidos
      {
        char op; //operacion
        bool done = false;
      
        while (!done)   // Espera hasta recibir algo
        {
          radio.read( &op, sizeof(op) );
          if (op > 0)done = true;
          delay(20);            
        }
        //Acciones
        if (op == 'a') //Apagar alarma, pero siempre esta armada
        {
          Alarma = false;
          digitalWrite(Alarm_out,LOW);
        } else if (op == 'O') //Armar la alarma, modo On
        {
          digitalWrite(Alarm_out,LOW);
          digitalWrite(Alarm_modo,HIGH);
          armado=0; //Se pone a cero para obviar las interrupciones
          interrupts();
          delay(100);//Espera aque se den las llamadas a interrupcion pendientes
          armado=1; //Reactivar las acciones de las interrupciones
          Alarma = false;
          modo=true; //Poner modo en Alarma Armada
        } else if (op == 'o') //Desarmar la alarma,modo Off
        {
          digitalWrite(Alarm_out,LOW);
          digitalWrite(Alarm_modo,LOW);
          noInterrupts();
          Alarma = false;
          modo=false; //Poner modo en Alarma Desarmada
        } 
      }

                                           //Verificacion si la alarma a sido disparada y
      if((Alarma==true) &&  (modo==true))  // si esta se encuentra armada (modo = true)                             
      {
        A_state=!A_state;  //Cambio de estado de la variable que controla la salida
        digitalWrite(Alarm_out,A_state); //Escribir el estado en la salida
        delay(500); //Espera para que el cambio de estados sea apreciable
      }
}

//Rutinas de interrupcion
void Alarma_On() //Alarma disparada
  {
    if((armado==1)&&(Alarma==false)) //Verificacion si se deben obviar o no las peticiones de interrupcion
    {
    radio.stopListening(); //Paramos el modo de escucha para poder escribir
    Alarma = true; //Se enciende la funcion de la alarma
    char fun = 'A';
    radio.openWritingPipe(direccion1); //Se abre el puerto de escritura
    radio.write(&fun, 1); //Se envia la notificacion de que la alarma fue activada.
    //delay(200);
    }
  }

void Alarma_Off() //Boton de apagado de alarma disparado
  {               
    //Verificacion si se deben obviar o no las peticiones de interrupcion
    if((armado==1)&&(Alarma==true)) // y a la vez, sí la la alarma esta encendida
    {
    radio.stopListening(); //Paramos el modo de escucha para poder escribir
    Alarma = false; //Se apaga la funcion de la alarma
    digitalWrite(Alarm_out,LOW); //Se pone la salida apagada
    char fun = 'D'; //Letra para informar que la alarma se desactivo manualmente
    radio.openWritingPipe(direccion1); //Se abre el puerto de escritura
    radio.write(&fun, 1); //Se envia la notificacion de que la alarma fue activada.
    //delay(200);
    }
  }
