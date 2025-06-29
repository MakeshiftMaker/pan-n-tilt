# Pan-Tilt System mit Joysticksteuerung

## Projektbeschreibung

Dieses Projekt implementiert ein einfaches **Pan-Tilt-System** mit zwei Schrittmotoren, die durch einen analogen **Joystick** gesteuert werden. Der Benutzer kann die **Neigung (Tilt)** und **Schwenkung (Pan)** über die Joystick-Achsen manuell steuern. Ein zusätzlicher Button erlaubt das **Zurücksetzen der Position** in die Mitte. Ebenso ist es möglich den Mechanismus per CLI Eingaben auszurichten.

Das Projekt basiert auf einem **ATmega32-Mikrocontroller** und verwendet eine benutzerdefinierte Hardware-Abstraktionsschicht (`avrhal`) zur Steuerung von Stepper-Motoren, ADC, USART und Joystick.

---

## Features

- **Ansteuerung von zwei Schrittmotoren** (Pan & Tilt) über eine einstellbare Taktfrequenz
- **Analoge Steuerung** der Bewegungsrichtung durch einen Joystick (X- & Y-Achse)
- **Tastergesteuertes Zurücksetzen** der Pan-Tilt-Position in die Ausgangslage
- **Ausrichtung per CLI** Eingabe ermöglicht genaue Ausrichtung des Mechanismus

---

## Aufbau

- **Mikrocontroller:** ATmega32  
- **Motorsteuerung:** Zwei Schrittmotoren
- **Joystick:** 2D Analog-Joystick (X, Y, Button)
- **Kommunikation:** USART (9600 Baud, 8N1)
- **Bibliotheken:** Eigene HAL (`avrhal`) für Stepper, ADC, Joystick, USART

---

## Abhängigkeiten

- `avr/io.h`, `util/delay.h` (AVR Libc)
- Eigene HAL-Dateien:
  - `avrhal/stepper.h`
  - `avrhal/joystick.h`
  - `avrhal/usart.h`
  - `avrhal/adc.h`
  - `utils/bit.h`

---

## Bedienung

1. **Joystick nach rechts/links bewegen:** Schwenkt den Motor (Pan)
2. **Joystick nach oben/unten bewegen:** Neigt den Motor (Tilt)
3. **Joystick drücken (Button):** Beide Motoren fahren automatisch in ihre Nullposition zurück
4. **USART-Monitor (z. B. `minicom`):** Zeigt Joystick-Werte und Motorstatus live an

