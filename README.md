# DTMF-Controlled Relay System made with the Seeeduino XIAO SAMD21

This project implements a DTMF (Dual-Tone Multi-Frequency) controlled relay system using a Seeeduino XIAO SAMD21 board. It activates a relay in response to a specific sequence of DTMF tones, making it suitable for remote control applications via audio signals, such as radio communication.

## Features

- Responds to a customizable sequence of DTMF tones
- Manual activation via a physical button
- Configurable activation duration
- Uses the MT8870 DTMF decoder chip
- Implements a cycling relay pattern when activated
- Designed for the compact Seeeduino XIAO SAMD21 board

## How It Works

1. Listens for a specific sequence of DTMF tones (default: 102*7777)
2. Activates a relay in a cycling pattern upon correct sequence
3. Relay alternates between ON and OFF states every 2 seconds
4. Customizable activation period (default: 60 seconds)
5. Physical button for manual trigger

## Hardware Requirements

- Seeeduino XIAO SAMD21 board
- MT8870 DTMF decoder
- Relay module
- Push button or Switch
- Appropriate wiring and power supply (I'm using the MEAN WELL MDR-20-5)

## Customization

Easily customize:
- DTMF activation sequence
- Pin assignments
- Activation duration
- Relay cycling pattern

## Potential Applications

- Remote control of devices via radio
- Security systems with audio-based activation
- Automated systems responding to phone-based commands

## License

This project is open-source and available under the MIT License. See the LICENSE file for details.

---

*Note: This README was created with the assistance of an AI language model (Claude) to provide a clear and concise description of the project.*
