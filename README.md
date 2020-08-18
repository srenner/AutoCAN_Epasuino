# AutoCAN_Epasuino

**EPAS** + ard**UINO** = epasuino. Pronounced "eh-puh-sween-oh." Or however you want. Speed sensitive power steering for automobiles.

## Overview
Typical aftermarket electric power assist steering (EPAS) kits for automobiles use an adjustment knob to tailor how much power assist to provide to the steering system. The user turns the knob up in parking lots, and turns it down on the highway, or selects a middle ground that's ok for every scenario, but rarely ideal. epasuino uses the speed of the vehicle to adjust the amount of assist. **epasuino does not replace the power steering ECU**. It connects to the existing ECU and partially automates the user input.

## User Interface
The user will use a 6 way rotary switch to control the base amount of steering assitance.

| Knob Position | 0mph | 30mph | 60mph | Setting Name |
|---------------|------|-------|-------|--------------|
| 1             | 0%   | 0%    | 0%    | Manual       |
| 2             | 75%  | 25%   | 0%    | Firm         |
| 3             | 100% | 25%   | 25%   | Sport+       |
| 4             | 100% | 50%   | 50%   | Sport        |
| 5             | 100% | 100%  | 50%   | Touring      |
| 6             | 100% | 100%  | 100%  | Comfort      |

## Hardware details
* [SparkFun AST-CAN485 Dev Board](https://www.sparkfun.com/products/14483)
* 100k ohm digital potentiometer
* 1 Pole 6 Throw Band Channel Rotary Switch Selector
