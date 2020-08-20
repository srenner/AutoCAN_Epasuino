# AutoCAN_Epasuino

**EPAS** + ard**UINO** = epasuino. Pronounced "eh-puh-sween-oh." Or however you want. Speed sensitive power steering for automobiles.

## Overview
Typical aftermarket electric power assist steering (EPAS) kits for automobiles use an adjustment knob to tailor how much power assist to provide to the steering system. The user turns the knob up in parking lots, and turns it down on the highway, or selects a middle ground that's ok for every scenario, but rarely ideal. epasuino uses the speed of the vehicle to adjust the amount of assist. **epasuino does not replace the power steering control module**. It connects to the existing control module and partially automates the user input.

## User Interface
The user will use a 6 way rotary switch to control the base amount of steering assitance. A System Message will be sent on the CAN bus every time the knob is turned to a different position. 

The EPAS control module will smooth out and delay large changes to the assist level, which makes precise speed-sensitive adjustments in the piggyback module essentially impossible to implement. Therefore, the algorithm has been simplified to be in 30mph and 25% increments.

| Knob Position | 0mph | 30mph | 60mph | Setting Name |
|---------------|------|-------|-------|--------------|
| 1             | 0%   | 0%    | 0%    | Manual       |
| 2             | 50%  | 25%   | 0%    | Firm         |
| 3             | 75%  | 25%   | 25%   | Sport+       |
| 4             | 100% | 50%   | 50%   | Sport        |
| 5             | 100% | 75%   | 50%   | Touring      |
| 6             | 100% | 100%  | 100%  | Comfort      |

## Hardware details
* [SparkFun AST-CAN485 Dev Board](https://www.sparkfun.com/products/14483)
* 100k ohm digital potentiometer
* 1 Pole 6 Throw Band Channel Rotary Switch Selector
