/*
#include <Arduino.h>

UART xbeeSerial(8, 7);

const int TBuffer = 64; 
uint8_t buffer[TBuffer];  // Buffer para almacenar los bytes
int bufferIndex = 0;         // Posición actual en el buffer

void setup()
{
	Serial.begin(9600);
	xbeeSerial.begin(9600);
	Serial.println("Datos entrantes:");
}

void loop()
{
	//Acá si entran datos arranco a leer
	while (xbeeSerial.available() > 0)
	{
		// Leo cada byte
		uint8_t c = xbeeSerial.read();
		
		if (bufferIndex < TBuffer)
		{
			buffer[bufferIndex++] = c;
		}
		else	
		{
		    // Acá aviso si se llena el buffer
			Serial.println("Buffer lleno putito!");
		}

		delay(2);  //Agrupo datos
	}

	// Si recibimos datos (bufferIndex > 0) y no hay más datos llegando, procesamos
	if (bufferIndex > 0 && xbeeSerial.available() == 0)
	{
		Serial.print("Datos recibidos en HEX: ");
		for (int i = 0; i < bufferIndex; i++)
		{
			// Mostrar cada byte en formato hexadecimal de 2 cifras
			if (buffer[i] < 0x10) Serial.print("0");  // Para que siempre tenga dos dígitos
			Serial.print(buffer[i], HEX);
			Serial.print(" ");
		}
		Serial.println();

		// Limpiamos el buffer para la próxima tanda de datos
		bufferIndex = 0;
	}

	if (Serial.available() > 0)
	{
		xbeeSerial.write(Serial.read());
	}
}
*/

#include <Arduino.h>

UART xbeeSerial(8, 7);

uint8_t test_frame_1[] = {
	0x7E,0x00,0x10,0x90,0x00,0x13,0xA2,0x00,0x42,0x28,0xA1,0x3E,0xFF,0xFE,0xC1,0x48,0x4F,0x4C,0x41,0x8F
};
uint8_t test_frame_2[] = {
	0x7E,0x00,0x10,0x90,0x00,0x13,0xA2,0x00,0x42,0x28,0xA1,0x3E,0xFF,0xFE,0xC1,0x48,0x4F,0x4C,0x41,0x8F
};
uint8_t test_frame_3[] = {
	0x7E,0x00,0x10,0x90,0x00,0x13,0xA2,0x00,0x42,0x28,0xA1,0x3E,0xFF,0xFE,0xC1,0x48,0x4F,0x4C,0x41,0x8F
};
uint8_t test_frame_4[] = {
	0x7E,0x00,0x10,0x90,0x00,0x13,0xA2,0x00,0x42,0x28,0xA1,0x3E,0xFF,0xFE,0xC1,0x48,0x4F,0x4C,0x41,0x8F
};


// Estados de recepción
enum State
{
  WAIT_START,
  READ_LENGTH,
  READ_FRAME
};

State currentState = WAIT_START;

const int bufferSize = 256;
uint8_t buffer[bufferSize];
int bufferIndex = 0;

uint16_t frameLength = 0;  // Longitud indicada en el paquete
uint8_t lengthBytes[2];    // Bytes de longitud temporales
int lengthIndex = 0;       // Índice para los 2 bytes de longitud

void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.println("Esperando paquetes...");
}

void loop()
{
  while (xbeeSerial.available() > 0)
  {
    uint8_t incomingByte = xbeeSerial.read();

    switch (currentState)
    {
      case WAIT_START:
        if (incomingByte == 0x7E)
        {
          bufferIndex = 0;
          buffer[bufferIndex++] = incomingByte;
          currentState = READ_LENGTH;
          lengthIndex = 0;
        }
        break;

      case READ_LENGTH:
        lengthBytes[lengthIndex++] = incomingByte;
        buffer[bufferIndex++] = incomingByte;

        if (lengthIndex == 2)
        {
          frameLength = (lengthBytes[0] << 8) | lengthBytes[1];
          currentState = READ_FRAME;
        }
        break;

      case READ_FRAME:
        buffer[bufferIndex++] = incomingByte;

        if (bufferIndex == (frameLength + 4))  // +3 porque incluye Start(1) + Length(2)
        {
          Serial.print("Paquete completo (");
          Serial.print(bufferIndex);
          Serial.println(" bytes):");

          // Mostrar el buffer en HEX
          for (int i = 0; i < bufferIndex; i++)
          {
            if (buffer[i] < 0x10) Serial.print("0");
            Serial.print(buffer[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
          Serial.println();

          currentState = WAIT_START;  // Volvemos a esperar otro paquete
        }
        break;
    }
  }

  // Eco Serial-PC a xbeeSerial
  if (Serial.available() > 0)
  {
    xbeeSerial.write(Serial.read());
  }
}
