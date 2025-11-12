import serial
import requests
import json
import time
from openai import OpenAI

# === CONFIGURACIÓN ===
PORT = "COM4"  # Ajusta tu puerto
BAUD = 9600
THINGSPEAK_API_KEY = "57ZRJ6EBXJRBM9NE"
OPENAI_API_KEY = "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"

# Inicializar cliente de OpenAI
client = OpenAI(api_key=OPENAI_API_KEY)

def enviar_a_thingspeak(humedad, ph):
    url = "https://api.thingspeak.com/update"
    payload = {
        "api_key": THINGSPEAK_API_KEY,
        "field1": humedad,
        "field2": ph
    }
    r = requests.post(url, data=payload)
    if r.status_code == 200:
        print(f"Datos enviados: humedad={humedad}%, pH={ph:.2f}")
    else:
        print(f"Error enviando datos ({r.status_code})")

def obtener_tip_chatgpt(humedad, ph):
    prompt = f"""
Eres un asistente agrícola experto. Analiza los siguientes valores y da un consejo breve y útil, estos son aji tipo Capsicum pubescens:
- Humedad del suelo: {humedad}%
- pH del suelo: {ph}

Da sugerencias sobre el tratado de la tierra o el riego, o di que esta bien si es asi.
"""
    try:
        response = client.chat.completions.create(
            model="gpt-5",
            messages=[{"role": "user", "content": prompt}],
        )
        tip = response.choices[0].message.content.strip()
        print("Consejo de ChatGPT:", tip)
    except Exception as e:
        print("Error al consultar ChatGPT:", e)

def main():
    ser = serial.Serial(PORT, BAUD, timeout=2)
    print("Esperando datos del Arduino...")

    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if not line:
                continue
            if line.startswith("{") and line.endswith("}"):
                data = json.loads(line)
                humedad = data["humedad"]
                ph = data["ph"]

                enviar_a_thingspeak(humedad, ph)
                obtener_tip_chatgpt(humedad, ph)

                time.sleep(20)
            else:
                print("→", line)

        except Exception as e:
            print("Error:", e)
            time.sleep(5)

if __name__ == "__main__":
    main()
