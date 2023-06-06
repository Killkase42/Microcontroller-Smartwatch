# Microcontroller-Smartwatch
 A WiFi-enabled, MCU driven smartwatch with environmental sensors and LiPO charging and protection circuits. It is built around the ATSAMD21G18A microcontroller.

# Initial Planning
  The original objective was to create a basic smartwatch with a standalone MCU, LCD screen, wifi capabilities, and various sensors. The ATSAMD21G18A was selected as the foundation of the project due to its excellent memory, high processing speed, low power consumption, small size, and versitility. The Nina-W102 was selected as the wifi module because of its ease of use and wealth of documentation. The other components were selected because they also had much supporting information to aid in their use.
  
-  [MCU (ATSAMD21G18A)](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/Atmel-42181-SAM-D21_Datasheet.pdf)
-  [WiFi Module (Nina-W102)](https://content.u-blox.com/sites/default/files/NINA-W10_DataSheet_UBX-17065507.pdf)
-  [Sensor Module (MS8607-02BA01)](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS8607-02BA01%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS8607-02BA01_B3.pdf%7FCAT-BLPS0018)
-  OLED Screen (SH1106I2C)
-  [LiPO Protection IC (FS312F-G)](https://pdf1.alldatasheet.com/datasheet-pdf/view/1132811/FORTUNE/FS312F-G.html)
-  [LiPO Charge IC (TP4056)](https://dlnmh9ip6v2uc.cloudfront.net/datasheets/Prototyping/TP4056.pdf)
-  [Power Supply IC (MT3608)](https://www.olimex.com/Products/Breadboarding/BB-PWR-3608/resources/MT3608.pdf)

# First Prototype
 Drawings were made of each components and its respective connections. 
<figure>
    <figcaption>Microcontroller</figcaption>
    <img src="Images/20230507_133534.jpg" width="203" height="200">
</figure>

<figure>
    <figcaption>Wifi Module</figcaption>
    <img src="Images/20230507_133614.jpg" width="185" height="200">
</figure>

<figure>
    <figcaption>Sensor Module</figcaption>
    <img src="Images/20230507_133515.jpg" width="293" height="200">
</figure>

<figure>
    <figcaption>OLED Screen</figcaption>
    <img src="Images/20230507_133628.jpg" width="162" height="200">
</figure>


<figure>
    <figcaption>LiPO ICs</figcaption>
    <img src="Images/20230507_133550.jpg" width="304" height="200">
</figure>

 An initial prototype was then constructed on a solderless breadboard. Various breakout boards, a TQFP chip adapter, and a basic LD33V power supply were used. Code for a clock, stopwatch, timer, the sensors, and wifi module was developed.
<figure>
    <figcaption>Design on Breadboard</figcaption>
    <img src="Images/20230211_151912.jpg" width="356" height="200">
</figure>
 
 The first prototype had numerous problems, mainly caused by the poor connections between the breadboard plates and the component legs. This resulted in a very unreliable supply of current and voltage as some connections, in parallel in series respectively, were barely touching and provided high resistance. Ultimately, this caused the malfunction of the wifi module, which required a high current and stable voltage. The components were all soldered onto perfboard to fix this. As can be seen, the wifi module problem was rectified.
<figure>
    <figcaption>Design on Perfboard</figcaption>
    <img src="Images/20230216_144904.jpg" width="356" height="200">
</figure>

All modules now functioned and software development would continue on a second prototype.

# Second Prototype

First, a proper schematic was drawn.
<figure>
    <figcaption>First Schematic</figcaption>
    <img src="Images/Schematic_Microcontroller Smartwatch Schematic 2_2023-05-11.png" width="282" height="200">
</figure>

Next, a PCB was designed. It was double-sided because there was not enough space to route all connections on one side. Spaces were left to screw on the OLED screen and insert a standard NATO watch strap.
<figure>
    <figcaption>First PCB</figcaption>
    <img src="Images/firstPCB.PNG" width="190" height="200">
</figure>

All components, the PCB, and the stencil were then purchased. 

The stencil was cut, placed over one of the PCBs, and then solder paste applied overtop of it.
<figure>
    <figcaption>Stencil and Solder Paste</figcaption>
    <img src="Images/20230520_213814.jpg" width="200" height="200">
</figure>

All SMD components were then placed on the board. The solder paste was reflowed via bottom heating. A pot with sand in it, used to evenly disperse heat, was used for bottom heating.
<figure>
    <figcaption>Bottom Heating</figcaption>
    <img src="Images/20230520_225355.jpg" width="200" height="200">
</figure>

The remaining THT components and the battery were then soldered to the board using a soldering iron.
<figure>
    <figcaption>Nearly-finished Board</figcaption>
    <img src="Images/20230522_143414.jpg" width="200" height="200">
</figure>

The OLED screen was then soldered atop thhe board, but did not display anything when the circuit was powered through the battery or an external 3.3 volts. Clearly, there was a big problem with the circuit. Every part of the design was reviewed and another blank PCB probed. The following potential causes were identified:

- PCB Stencil was initially not properly secured to the PCB, leading to improper solder paste application, so the new attachment method in the above picture was devised. The stencil was not properly cleaned before this second application, though, and some holes were blocked by old paste. This may have lead to improper application.
- Too much sand was used in the bottom-heating process. This was due to the excessive heat-up time, during which the MCU may have broken.
- When the reflow was complete, several components did not have enough solder connecting them to the board. This lead to extensive rework and possible damage of the MCU.

Ultimately, it was concluded that improper assembley left the circuit inoperable. This was fixed by rebuilding an identical second version which worked.
