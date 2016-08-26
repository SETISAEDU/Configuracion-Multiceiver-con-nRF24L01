/*  ----------------------------------------------------------------
    SETISAEDU
    Codigo para la configuracion MultiCeiver con NRF24L01.
    Codigo del PRX (Receptor Maestro y controlador de las alarmas y operaciones)
  --------------------------------------------------------------------
*/

#include <SPI.h> // Libreria de SPI para la comunicacion con el modulo nRF24L01
//Librerias para el control del modulo nRF
#include <nRF24L01.h> 
#include <RF24.h>


RF24 radio(9, 10);//Declaracion de los pines de control CE y CSN para el modulo, se define "radio"

//Se crean las 6 direcciones posibles para las Pipes P0 - P5; 
//Las direcciones es un numero de 40bits para este caso se usa uint_64_t de 64bits
const uint64_t direccion[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };
                             // En esta parte LL significa LongLong para asegurar el dato de 64bits
//Variables de estado de las alarmas
bool Alarma1 = false, Alarma2 = false, Alarma3 = false;

void setup()
{

  Serial.begin(115200);  //Se inicia el puerto serial
  Serial.println("Iniciando modulo receptor");
  Serial.println();

  radio.begin(); //Inicio del modulo nRF24L01+
//  radio.setRetries(15, 15);// Cinfiguracion del numero maximo de reintentos
  radio.setPALevel(RF24_PA_MAX);  // Se configura a la maxima potencia
  radio.setChannel(100); // Apertura del canal especifico
  
  //Apertura de las lineas de comunicacion con un maximo de 6 direcciones
  radio.openReadingPipe(0, direccion[0]);
  radio.openReadingPipe(1, direccion[1]);
  radio.openReadingPipe(2, direccion[2]);
  //para el caso de tenes mas modulos
  radio.openReadingPipe(3, direccion[3]); 
  radio.openReadingPipe(4, direccion[4]);
  radio.openReadingPipe(5, direccion[5]);

  radio.startListening(); //Se inicia en modo de escucha, a la espera de alertas de las alarmas
}

void loop()
{
  byte pipeNum = 0; //Variable para leer que Modulo envio el dato
  byte dataRX = 0; //Variable para guardar el dato recibido

  while (radio.available(&pipeNum)) { //Verificar si hay datos de los esclavos
    radio.read(&dataRX, 1 ); //Se lee un byte, ya que hemos configurado solo en envio de una letra
    Serial.print("Recibido del transmisor: ");
    Serial.println(pipeNum + 1); // Se imprime el numero del transmisor que envio el dato
    
    if (dataRX == 'A') //Letra correspondiente al transmisor 1
    {
      Serial.println("********************************************************************");
      Serial.println("Aviso de alarma 1 activada!");
      Serial.println("presione A para apagar o realice lo manualmente desde el modulo 1");
      Serial.println("********************************************************************");
      Alarma1 = true; // Se activa la impresion de la alarma en el monitor serial
    } else if (dataRX == 'D') //Letra correspondiente al transmisor 1
    {
      Serial.println("********************************************************************");
      Serial.println("Alarma 1 desactivada manuealmente");
      Serial.println("********************************************************************");
      Alarma1 = false; // Se desactiva la impresion de la alarma en el monitor serial
    } else if (dataRX == 'B') //Letra correspondiente al transmisor 2
    {
      Serial.println("********************************************************************");
      Serial.println("Aviso de alarma 2 activada!");
      Serial.println("presione B para apagar o realice lo manualmente desde el modulo 2");
      Serial.println("********************************************************************");
      Alarma2 = true; // Se activa la impresion de la alarma en el monitor serial
    } else if (dataRX == 'E') //Letra correspondiente al transmisor 2
    {
      Serial.println("********************************************************************");
      Serial.println("Alarma 2 desactivada manuealmente");
      Serial.println("********************************************************************");
      Alarma2 = false; // Se activa la impresion de la alarma en el monitor serial
    } else if (dataRX == 'C') //Letra correspondiente al transmisor 3
    {
      Serial.println("********************************************************************");
      Serial.println("Aviso de alarma 3 activada!");
      Serial.println("presione C para apagar o realice lo manualmente desde el modulo 3");
      Serial.println("********************************************************************");
      Alarma3 = true; // Se activa la impresion de la alarma en el monitor serial
    } else if (dataRX == 'F') //Letra correspondiente al transmisor 3
    {
      Serial.println("********************************************************************");
      Serial.println("Alarma 3 desactivada manuealmente");
      Serial.println("********************************************************************");
      Alarma3 = false; // Se desactiva la impresion de la alarma en el monitor serial
    } else //En el caso de recibir un dato erroneo
    {
      Serial.println("Se recibio un dato erroneo");
    }
  }

  if (Serial.available())
  { 
    char data = Serial.read();
    int opc = Serial.parseInt();
    bool ok = 0;
    if (data == 'a' || data == 'A')
    {
      radio.stopListening(); // Paramos la escucha para poder escribir
      radio.openWritingPipe(direccion[0]); //Se abre el puerto de escritura para la direccion del modulo 1
      if(opc==0)
      {
        Serial.println("********************************************************************");
        Serial.print("Desactivando alarma 1");
        char fun = 'a';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de desactivacion   
      }else if(opc==1)
      {
        Serial.println("********************************************************************");
        Serial.print("Armando alarma 1");
        char fun = 'O';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de desactivacion 
      }else if(opc==2)
      {
        Serial.println("********************************************************************");
        Serial.print("Desarmando alarma 1");
        char fun = 'o';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de desactivacion 
      }else 
      {
        Serial.println("********************************************************************");
        Serial.println("Digite uno de los comandos validos:");
        Serial.println("A0: Desactivar alarma 1");
        Serial.println("A1: Armar alarma 1");
        Serial.println("A2: Desarmar alarma 1");
      }
      
      if (ok)
      {
        Serial.println(", eviado...");
        Alarma1 = false;
        radio.startListening(); //Se regresa al modo de escucha
      } else
      {
        Serial.println(", fallo en envio!");
      }
      Serial.println("********************************************************************");
    } else  if (data == 'b' || data == 'B')
    {
      radio.stopListening(); // Paramos la escucha para poder escribir
      radio.openWritingPipe(direccion[1]); //Se abre el puerto de escritura para la direccion del modulo 2
      if(opc==0)
      {
        Serial.println("********************************************************************");
        Serial.print("Desactivando alarma 2"); 
        char fun = 'b';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de desactivacion   
      }else if(opc==1)
      {
        Serial.println("********************************************************************");
        Serial.print("Armando alarma 2");
        char fun = 'O';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de Armado 
      }else if(opc==2)
      {
        Serial.println("********************************************************************");
        Serial.print("Desarmando alarma 2");
        char fun = 'o';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de Desarmado 
      }else 
      {
        Serial.println("********************************************************************");
        Serial.println("Digite uno de los comandos validos:");
        Serial.println("B0: Desactivar alarma 2");
        Serial.println("B1: Armar alarma 2");
        Serial.println("B2: Desarmar alarma 2");
       
      }
      
      if (ok)
      {
        Serial.println(", eviado...");
        Alarma2 = false;
        radio.startListening(); //Se regresa al modo de escucha
      } else
      {
        Serial.println(", fallo en envio!");
      }
      Serial.println("********************************************************************");
    } else  if (data == 'c' || data == 'C')
    {
      radio.stopListening(); // Paramos la escucha para poder escribir
      radio.openWritingPipe(direccion[2]); //Se abre el puerto de escritura para la direccion del modulo 3
      if(opc==0)
      {
        Serial.println("********************************************************************");
        Serial.print("Desactivando alarma 3"); 
        char fun = 'c';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de desactivacion   
      }else if(opc==1)
      {
        Serial.println("********************************************************************");
        Serial.print("Armando alarma 3");
        char fun = 'O';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de Armado 
      }else if(opc==2)
      {
        Serial.println("********************************************************************");
        Serial.print("Desarmando alarma 3");
        char fun = 'o';
        ok = radio.write(&fun, sizeof(fun)); //Envio de la funcion de Desarmado 
      }else 
      {
        Serial.println("********************************************************************");
        Serial.println("Digite uno de los comandos validos:");
        Serial.println("C0: Desactivar alarma 3");
        Serial.println("C1: Armar alarma 3");
        Serial.println("C2: Desarmar alarma 3");
       
      }
      
      if (ok)
      {
        Serial.println(", eviado...");
        Alarma3 = false;
        
      } else
      {
        Serial.println(", fallo en envio!");
      }
      Serial.println("********************************************************************");
    }
    radio.startListening(); //Se regresa al modo de escucha
     
  }
/*
  if (Alarma1 == true)
  {
    //acciones que se desean realizar al recibir la alerta de la alarma 1
  }
  if (Alarma2 == true)
  {
    //acciones que se desean realizar al recibir la alerta de la alarma 2
  }
  if (Alarma3 == true)
  {
    //acciones que se desean realizar al recibir la alerta de la alarma 3
  }

*/
  delay(200);
}


