# AutoCAN_Epasuino

**EPAS** + ard**UINO** = epasuino. Pronounced "eh-puh-sween-oh." Or however you want. Speed sensitive power steering for automobiles.

## Overview
Typical aftermarket electric power assist steering (EPAS) kits for automobiles use an adjustment knob to tailor how much power assist to provide to the steering system. The user turns the knob up in parking lots, and turns it down on the highway, or selects a middle ground that's ok for every scenario, but rarely ideal. epasuino uses the speed of the vehicle to adjust the amount of assist. **epasuino does not replace the power steering ECU**. It connects to the existing ECU and partially automates the user input.

This project has a secondary purpose of taking the speed sensor info and sending it on the CAN bus to a MegaSquirt ECU and my [carfuino performance computer](https://github.com/srenner/carfuino).

## User Interface
The user will use a 6 way rotary switch to control the base amount of steering assitance.

## Hardware details
* [SparkFun AST-CAN485 Dev Board](https://www.sparkfun.com/products/14483)
* 100k ohm digital potentiometer
* 1 Pole 6 Throw Band Channel Rotary Switch Selector
