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

 An initial prototype was then constructed on a solderless breadboard. Various breakout boards, a TQFP chip adapter, and a basic LD33V power supply were used.
<figure>
    <figcaption>First Prototype>/figcaption>
    <img src="Images/20230211_151912.jpg" width="356" height="200">
</figure>
 
 The first prototype had numerous problems, mainly caused by the poor connections between the breadboard plates and the component legs. This resulted in a very unreliable supply of current and voltage as some connections, in parallel in series respectively, were barely touching and provided high resistance. Ultimately, this caused the malfunction of the wifi module, which required a high current and stable voltage. The components were all soldered onto perfboard to fix this. As can be seen, the wifi module problem was rectified.
<figure>
    <figcaption>Second Prototype>/figcaption>
    <img src="Images/20230216_144904.jpg" width="356" height="200">
</figure>
