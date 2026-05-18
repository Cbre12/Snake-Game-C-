# Arduino Snake Game (LED Matrix)

A Snake game built on Arduino using an 8x8 LED matrix (MAX7219), joystick controls, and potentiometers for speed and brightness control.

## Features
- Joystick-controlled snake movement
- Food spawning with collision safety
- Self-collision game over logic
- Screen wrap-around movement
- Adjustable speed via potentiometer
- Adjustable brightness via potentiometer
- Runs on 8x8 LED matrix

## Hardware Requirements
- Arduino Uno (or compatible board)
- 8x8 LED Matrix (MAX7219 driver)
- Analog joystick module
- 2 potentiometers
- Jumper wires

## Controls
- Joystick: Move snake (up, down, left, right)
- Potentiometer 1: Adjust snake speed
- Potentiometer 2: Adjust LED brightness

## Required Library
- LedControl (for MAX7219 LED matrix)

Install via Arduino Library Manager:
LedControl

## How It Works
- Snake is stored in a fixed-size array
- Movement updates the head and shifts the body
- Food spawns randomly in empty spaces
- Collision with itself ends the game
- LED matrix updates every frame

## Setup Instructions
1. Wire components according to the pin configuration in the code
2. Install the LedControl library
3. Upload the code to the Arduino
4. Play the game on the LED matrix
