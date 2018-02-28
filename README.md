# Interceptor
Code and info for a Gas Pedal Interceptor for use with Open Pilot - Now superceded by Comma Pedal! 

### Explanation
The interceptor is used to provide stop-and-go capability on Open Pilot supported cars that don't currently support it. 

A gas pedal interceptor is a device that is placed between an electronic gas pedal and the ECU (Engine Control Unit). It detects the voltage from the gas pedal which represents the pedal's position. It then writes that same voltage back out to the ECU. When Open Pilot takes control, Open Pilot sends the gas position instead.

### History
When Comma originally setup Open Pilot on their Acura, they created their own interceptor to give it stop-and-go capability. However, it was considered a hack, therefore no plans for making interceptors were published.

Comma believes ECU hacks are a more elegant solution to the stop-and-go problem. However skills are required to accomplish such hacks, so it is likely many cars will never get one. This is where the interceptor comes in. When asked about this problem, Comma said their plans for an intercepter weren't ever going to be in any shape to publish, but that if we wanted to build one from scratch, we could create a Slack channel and make it happen. 

Much of the code Comma uses on the Acura still exists in Open Pilot. This means it shouldn't be too difficult to integrate into it. In fact, when asked, George said "It’s probably a 20 line change."

# UDATE: On February 27, 2018 COMMA ANNOUNCED THEIR OWN INTERCEPTOR! THE COMMA PEDAL!
Now that the Pedal exists, this project will flounder. But I'll at least keep the information around. 

### How it works
A prototype is now built using the hardware listed below. It is essentially an Arduino Uno connected to a CAN Bus module and a Digital to Analog Converter. These then connect to the gas pedal, the ECU and the CAN bus.  

The gas pedals for most modern cars output 0 to 5 volts representing pedal position. We feed the 0-5V into the Arduino's analog-to-digital ports. We then send the voltage back out to the ECU using the D-to-A chip. There are actually two redundant 5V analog signals, so we have to capture and send the voltage twice. 

Open Pilot communicates using the CAN bus. So we must send the pedal position to Open Pilot via CAN, and read back OP's gas commands from CAN as well. 

### Status
The prototype is built and installed in a Rav4. It currently handles all gas pedal relaying just fine. It can also read and write on the CAN bus. We're using the CAN id's Comma originally used.

We had to add the ID's to fingerprints.py and the dbc file. 

#### To Do
* Search OP code to find out what it needs from the Interceptor, and excactly what it will send.
* Alter OP code to use the interceptor
* Alter Arduino code.

### Hardware
Included in this repository is a diagram used to make a circuit board to hold the D-to-A and the pins for easy removal.

We used standard breadboarding jumper wires instead of trying to purchase the correct connectors for the gas pedal. That means poking male jumper wires into the female holes of the connector, and sliding female jumper wires into the male pins of the conector. It holds surprisingly well. We then removed the single Dupont ends from the jumper wires and added multi-conductor ends. This isn't a good permanent solution, but it works well for prototyping. 

Finally, the CAN module we used is not ideal. We could have probably done away with the circuit board if we would have instead used the CAN Shield in the list below. We've since purchased one, but not built with it yet. Another down-side to the one we used is that it required a custom Arduino library. The Can Shield probably doesn't. 



#### D-to-A chip: Microchip MCP4822
https://www.digikey.com/product-detail/en/microchip-technology/MCP4822-E-P/MCP4822-E-P-ND/951465

#### CAN Module I used:
https://www.amazon.com/gp/product/B015W4D9WY/ref=oh_aui_detailpage_o01_s01?ie=UTF8&psc=1

#### CAN Arduino Shield that I should have probably used instead:
https://www.amazon.com/gp/product/B06XWQ4WF9/ref=oh_aui_detailpage_o05_s00?ie=UTF8&psc=1

#### OBDII splitter:
https://www.amazon.com/gp/product/B017IBP1MK/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1



#### Altered Arduino library for the Can Module (This library is altered to use the slower clock on the specific module, generic library should be fine for the shield):
https://github.com/Flori1989/MCP2515_lib
