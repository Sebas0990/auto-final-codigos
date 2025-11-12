// === Configuración de pines ===
#define BOMBA_AGUA 4
#define BOMBA_SUBIR_PH 5
#define BOMBA_BAJAR_PH 6

#define SENSOR_HUMEDAD A2
#define SENSOR_PH A3

// === Umbrales ===
const int HUMEDAD_MIN = 60; // %
const float PH_MIN = 3.0;
const float PH_MAX = 10;

// === Temporizador ===
unsigned long lastRead = 0;
const unsigned long INTERVAL = 10000; // 20 s

void setup() {
  Serial.begin(9600);

  pinMode(BOMBA_AGUA, OUTPUT);
  pinMode(BOMBA_SUBIR_PH, OUTPUT);
  pinMode(BOMBA_BAJAR_PH, OUTPUT);

  digitalWrite(BOMBA_AGUA, LOW);
  digitalWrite(BOMBA_SUBIR_PH, LOW);
  digitalWrite(BOMBA_BAJAR_PH, LOW);

  Serial.println("Sistema de riego iniciado.");
}

void loop() {
  if (millis() - lastRead >= INTERVAL) {
    lastRead = millis();

    int humedad = leerHumedad();
    float ph = leerPH();

    controlBombas(humedad, ph);

    // 🔹 Enviar datos a la PC en formato JSON
    Serial.print("{\"humedad\":");
    Serial.print(humedad);
    Serial.print(",\"ph\":");
    Serial.print(ph, 2);
    Serial.println("}");
  }
}

int leerHumedad() {
  int raw = analogRead(SENSOR_HUMEDAD);
  return map(raw, 0, 1023, 100, 0);
}



float leerPH() {
  int raw = analogRead(SENSOR_PH);
  float voltaje = raw * (5.0 / 1023.0);
  return 7 + ((2.5 - voltaje) / 0.18);
}

void controlBombas(int humedad, float ph) {
  if (humedad < HUMEDAD_MIN) {
    Serial.println("Activando bomba de agua...");
    digitalWrite(BOMBA_AGUA, HIGH);
    delay(5000);
    digitalWrite(BOMBA_AGUA, LOW);
  }

  if (ph < PH_MIN) {
    Serial.println("Activando bomba para subir pH...");
    digitalWrite(BOMBA_SUBIR_PH, HIGH);
    delay(3000);
    digitalWrite(BOMBA_SUBIR_PH, LOW);
  }

  if (ph > PH_MAX) {
    Serial.println("Activando bomba para bajar pH...");
    digitalWrite(BOMBA_BAJAR_PH, HIGH);
    delay(3000);
    digitalWrite(BOMBA_BAJAR_PH, LOW);
  }
}
