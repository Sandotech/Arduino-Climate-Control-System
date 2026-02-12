#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ===== CONFIGURACIÓN HARDWARE =====
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== PINES =====
const int fanPin = 8;
const int buzzerPin = 9;

// ===== VARIABLES DE ESTADO =====
bool monitoringActive = true;
bool fanActive = false;
bool alertActive = false;
bool highTempTriggered = false; // Nueva variable para controlar disparo único

// ===== TIMING (MILLIS) =====
unsigned long previousMillis = 0;
const long interval = 2000; // Intervalo de lectura (2s)

void setup()
{
  Serial.begin(9600);
  dht.begin();

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Listo");
  delay(1000);
  lcd.clear();

  // Inicializar Pines
  pinMode(fanPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop()
{
  unsigned long currentMillis = millis();

  // 1️⃣ LEER COMANDOS (Siempre activo, sin delay)
  if (Serial.available() > 0)
  {
    char command = Serial.read();
    processCommand(command);
  }

  // 2️⃣ MONITOREO SENSORES (Cada x segundos)
  if (monitoringActive)
  {
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      readAndSendData();
    }
  }
}

void processCommand(char command)
{
  switch (command)
  {
  case 'M':
    monitoringActive = !monitoringActive;
    showLCDStatus("Monitoreo", monitoringActive);
    break;
  case 'V':
    fanActive = !fanActive;
    digitalWrite(fanPin, fanActive ? HIGH : LOW);
    showLCDStatus("Ventilador", fanActive);
    break;
  case 'A':
    alertActive = !alertActive;
    digitalWrite(buzzerPin, alertActive ? HIGH : LOW);
    showLCDStatus("Alerta", alertActive);
    break;
  }
}

void readAndSendData()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Error,Error");
    return;
  }

  // Definir Umbrales
  float umbralAlto = 30.0;  // Temperatura para ACTIVAR
  float umbralReset = 28.0; // Temperatura para RESETEAR (Histéresis)

  // ===== CONTROL AUTOMÁTICO DE SEGURIDAD =====
  if (t >= umbralAlto)
  {
    // Solo activamos si NO hemos disparado ya la alerta en este ciclo
    if (!highTempTriggered)
    {
      highTempTriggered = true;

      fanActive = true;
      digitalWrite(fanPin, HIGH);

      alertActive = true;
      digitalWrite(buzzerPin, HIGH);
    }
  }
  // Solo reseteamos el trigger si la temperatura baja MUCHO (debajo del umbral de reset)
  // Esto evita que pequeñas oscilaciones (29.9 -> 30.0) reactiven la alarma apagada
  else if (t < umbralReset)
  {
    highTempTriggered = false;
  }

  // Actualizar LCD Local
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t, 1);
  lcd.print("C ");
  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(h, 1);
  lcd.print("% ");

  // Enviar a PC
  // Formato: 24.50,60.00
  Serial.print(t);
  Serial.print(",");
  Serial.println(h);
}

void showLCDStatus(String component, bool state)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(component + ":");
  lcd.setCursor(0, 1);
  // Manteniendo los strings en español como se solicitó
  lcd.print(state ? "ACTIVADO" : "DESACTIVADO");
  delay(1000); // Pequeña pausa visual solo al cambiar estados
  lcd.clear();
}
