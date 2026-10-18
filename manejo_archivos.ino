#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Definición del pin Chip Select (CS)
#define SD_CS 5

// Función recursiva para buscar archivos de audio
void listarArchivosDeAudio(File dir, int nivel) {
  while (true) {
    File entrada = dir.openNextFile();
    if (!entrada) {
      // No hay más archivos en este directorio
      break;
    }

    if (entrada.isDirectory()) {
      // Ignorar directorios ocultos de sistema (ej. macOS/Windows)
      String dirName = String(entrada.name());
      if (!dirName.startsWith(".") && !dirName.startsWith("System Volume Information")) {
        listarArchivosDeAudio(entrada, nivel + 1);
      }
    } else {
      String nombreArchivo = String(entrada.name());
      nombreArchivo.toLowerCase();
      
      // Filtrar únicamente los formatos requeridos para el Walkman
      if (nombreArchivo.endsWith(".mp3") || nombreArchivo.endsWith(".flac")) {
        Serial.print("AUDIO: /");
        Serial.print(entrada.name());
        Serial.print("  |  Tamaño: ");
        Serial.print(entrada.size() / 1024);
        Serial.println(" KB");
      }
    }
    entrada.close();
  }
}

void setup() {
  Serial.begin(115200);
  // Pequeño retardo para dar tiempo a que se abra el Monitor Serie
  delay(1000); 

  Serial.println("\n--- Proyecto: Walkman Hi-Fi ---");
  Serial.println("Fase 1: Inicializando tarjeta SD...");

  // Iniciar la comunicación con el módulo SD usando el pin CS
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR: Fallo al montar la tarjeta SD.");
    Serial.println("Verifica las conexiones SPI y la alimentación.");
    return;
  }

  uint8_t tipoTarjeta = SD.cardType();
  if (tipoTarjeta == CARD_NONE) {
    Serial.println("ERROR: No se detectó ninguna tarjeta SD física en el módulo.");
    return;
  }

  Serial.println("Tarjeta SD inicializada correctamente.");
  Serial.println("Escaneando archivos MP3 y FLAC en la memoria...\n");

  File raiz = SD.open("/");
  listarArchivosDeAudio(raiz, 0);
  raiz.close();
  
  Serial.println("\nEscaneo completado.");
}

void loop() {
  // En esta fase solo validamos la lectura de directorios y filtrado.
}
