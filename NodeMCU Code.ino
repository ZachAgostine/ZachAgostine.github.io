/*----------------------------------------------------------------------------------------------------Libraries---------------------------------------------------------------------
Purpose: Include the libraries and information needed to connect to wifi and to the Blynk app.
Inputs: No descrete inputs.  Libraries are used to run the code in setup() and loop().
Output: No descrete outputs. Variables are used in setup().
Use: Copy your Blynk server token into array auth[] as a string.  Enter network name as a string into array ssid[] and password into pass[].  If network is open, set password to "".
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = ""; //auth token for my Blynk server
char ssid[] = ""; //WiFi network name
char pass[] = ""; //WiFi network password. If network is open, use char pass[] = "";



/*-------------------------------------------------------------------------Setup--------------------------------------------------------------------------------------------
Purpose: Run the functions requred to connect to Blynk via local wifi network.
Inputs: Global variables are passed to Blynk.begin.  The esp8266 receivs input from the local network to establish a connection and receive commands from Blynk.
Outputs: Blynk.run allows user to send a HIGH signal to pin A0 on the NODE MCU board via a Blynk instruction.  In the main code, pin A0 is electrically connected to pin 7, which is set as digital input. 
Use: Upload to NODE MCU board.  Apply 5V to Vin (include path for GND) and allow 3-4 seconds for board to connect to network.  Red LED will turn on when it is connected to your Blynk server. 

*/
void setup() {
  Blynk.begin(auth, ssid, pass); //establish network connection and connection to Blynk server.

}


void loop() {
  Blynk.run(); //allows user to control pins via Blynk or IoT.

}
