
String PROG_NAME = "PMD_Processing_MQTT";
String VERSION = "V0.25 ";
String PROJECT_URL = "https://github.com/PubInv/krake/tree/main/PMD/PMD_Processing_MQTT"; 
String BROKER_URL = "mqtt://public:public@public.cloud.shiftr.io";

// File: PMD_Processing_MQTT 
// Author: Forrest Lee Erickson
// Date: 20241006
// Date: 20241016 Background change color on send and receive MQTT. Subscribe to 20240421_USA4.
// Date: 20241017 Set title on window.
// Date: 20241024 Publish to a Krake model and serial number. Works with "FT_PMD_MQTT ""V0.7 "
// Date: 20241031 Begin publishing and subscriving messages in the GPAD API format of 20241031. Need a KRAKE which can parse GPAD API for this.
// Date: 20241101 Saved and renamed from FT_processingPMD_MQTT.  Fixed error on KRAKE_DTA_TOPIC[] from KRAKE_20240421_LEB1_ALM to KRAKE_20240421_LEB1_ALM
// Date: 20241113 topics based on MAC address for US1-US5 assemblies. Rev 7.
// Date: 20241115 Time stamp received messages. Rev 8.
// Date: 20241119 Rev 0.9. Update with three LEB MAC addresses. 
// Make a mac to SerialNumber dictionary for easier reading of the console.
// Date: 20241123 Rev 0.10. Update with another USA MAC addresses. 
// Date: 20241130 Rev 0.11. Format source. Add PROJECT_URL and display it in draw(). Update background in draw().  
// Date: 20241130 Rev 0.12. Move void keyPressed() to UserInput tab.  
// Date: 20241130 Rev 0.13. WinkInProcessingPMD to simulate an LED heart beat.
// Date: 20241130 Rev 0.14. Set MQTT WILL message(s).
// Date: 20241209 Rev 0.19. Add log file name into draw() window.
// Date: 20241209 Rev 0.20. Save screens on client connect or lost.
// Date: 20241209 Rev 0.21.  Add MAC address to the sketch title.
// Date: 20241209 Rev 0.22.  Add MAC address to the MQTT Username. Display Broker in draw window.
// Date: 20241221 Rev 0.23.  Add LEB4 and MockingKrake MAC addresses.
// Date: 20241221 Rev 0.24.  Make the the broker button color same as the visualizer.
// Date: 20250102 Rev 0.25.  Sent an 'i' command.


// Description:
// Pseude Medical Device in Processing. 
// This program is a development tool for the Krake(TM) wirless alarm device.
// This example sketch connects to the public shiftr.io instance and sends a message on every keystroke.
// You can find the broker visulization here: https://public.cloud.shiftr.io/.
//
// Note: If you're running the sketch via the Android Mode you need to set the INTERNET permission
// in Android > Sketch Permissions.
// Started from MQTT library example by Joël Gähwiler
// https://github.com/256dpi/processing-mqtt

/* 
 #define LICENSE "GNU Affero General Public License, version 3 "
 #define ORIGIN "USA"
 */

String KRAKE_DTA_TOPIC[] = {"3C61053DC954_ALM", "3C61053DF08C_ALM", "3C6105324EAC_ALM", "3C61053DF63C_ALM", "10061C686A14_ALM", "FCB467F4F74C_ALM", 
  "CCDBA730098C_ALM", "CCDBA730BFD4_ALM", "CCDBA7300954_ALM", "A0DD6C0EFD28_ALM", "KRAKE_20240421_LEB5_ALM", "A0B765F51E28_ALM" }; //Publish to a Krake data topic for ALARMs.

String KRAKE_ACK_TOPIC[] = {"3C61053DC954_ACK", "3C61053DF08C_ACK", "3C6105324EAC_ACK", "3C61053DF63C_ACK", "10061C686A14_ACK", "FCB467F4F74C_ACK", 
  "CCDBA730098C_ACK", "CCDBA730BFD4_ACK", "CCDBA7300954_ACK", "A0DD6C0EFD28_ACK", "KRAKE_20240421_LEB5_ACK","A0B765F51E28_ACK" }; //Subscribe to a Krake ack.

StringDict mac_to_NameDict = new StringDict();
void setupDictionary() {
  mac_to_NameDict.set("3C61053DF08C_ACK", "20240421_USA1");
  mac_to_NameDict.set("3C6105324EAC_ACK", "20240421_USA2");
  mac_to_NameDict.set("3C61053DF63C_ACK", "20240421_USA3");
  mac_to_NameDict.set("10061C686A14_ACK", "20240421_USA4");
  mac_to_NameDict.set("FCB467F4F74C_ACK", "20240421_USA5");
  mac_to_NameDict.set("CCDBA730098C_ACK", "20240421_LEB1");
  mac_to_NameDict.set("CCDBA730BFD4_ACK", "20240421_LEB2");
  mac_to_NameDict.set("CCDBA7300954_ACK", "20240421_LEB3");
  mac_to_NameDict.set("A0DD6C0EFD28_ACK", "20240421_LEB4");
//  mac_to_NameDict.set("A0DD6C0EFD??_ACK", "20240421_LEB5");
  mac_to_NameDict.set("A0B765F51E28_ACK", "MockingKrake_LEB");
  mac_to_NameDict.set("3C61053DC954_ACK", "Not Homework2, Maryville TN");
}//end setup mac_to_NameDict

String MessageFromProcessing_PMD = "";  // The MQTT message first part.
String thePayload = "";  // The MQTT received.
boolean clientStatusChanged = false;

import java.util.TimeZone; 
import mqtt.*;

MQTTClient client;

class Adapter implements MQTTListener {
  //This appears to auto reconnect
  void clientConnected() {
    String theTimeStamp = "";
    theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) + " " ; //time stamp
    theTimeStamp = theTimeStamp + "MQTT clientConnected" ;
    println(theTimeStamp);  
    appendTextToFile(myLogFileName, "MQTT clientConnected");
    clientStatusChanged = true; //TO flag save of draw() window.

    mqttBrokerIsConnected = true;
    for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
      client.subscribe(KRAKE_ACK_TOPIC[i]);
      //client.setWill(String topic, String payload);
      client.setWill(KRAKE_ACK_TOPIC[i]+"/will", KRAKE_ACK_TOPIC[i]+" Has disconnected.");
    }//end for i
  }// end clientCOnnect

  void messageReceived(String topic, byte[] payload) {
    thePayload = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) ; //time stamp
    thePayload = thePayload + " " + "Msg_recd: " + mac_to_NameDict.get(topic) + " - " + new String(payload);
    println(thePayload);
    myBackground = color(0, 0, 0);
    mqttBrokerIsConnected = true;
  }

  void connectionLost() {
    String theTimeStamp = "";
    theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) + " " ; //time stamp
    theTimeStamp = theTimeStamp + "MQTT Client Connection lost" ;
    println(theTimeStamp);  
    appendTextToFile(myLogFileName, "MQTT Client Connection lost");
    clientStatusChanged = true;  //TO flag save of draw() window.
    myBackground = color(128, 0, 0);
    mqttBrokerIsConnected = false;
  }
}//end Class Adapter

Adapter adapter;
boolean mqttBrokerIsConnected = false;

color myBackground = color(64, 64, 64);  //Start grey
color shiftrBackground = color(0,171,92);  //Shifter.io


boolean overButton = false;

void setup() {
  getNetworkInterface();
  surface.setTitle(PROG_NAME + " Ver:" + VERSION + "MAC: " + theMAC);
  size(900, 360);
  noStroke();    //disables drawing outlines
  background (myBackground);
  frameRate(24);

  //Start up logging system
  String startTime = (str(year()) + str(month()) +str(day()) +"_" + str(hour()) + str(minute()) + str(second()) );
  myLogFileName = (startTime + "_" + myLogFileName);
  appendTextToFile(myLogFileName, ("Your log is born."));

  setupDictionary(); //for MAC to serial numbers. 

  adapter = new Adapter();
  client = new MQTTClient(this, adapter);

  //client.connect(BROKER_URL, USERNAME);    //  BROKER_URL and name
  client.connect(BROKER_URL, PROG_NAME + "_" + theMAC);    //  BROKER_URL and name
  MessageFromProcessing_PMD = "Nothing published Yet"; //An intial message for the draw()

}//end setup()

void draw() {
  background(myBackground);
  //A heart beat LED
  updateLED(); //Set the LED color
  circle(width -20, 12, 20); //draw the LED.  
    checkOverButton();
  //Text on draw window
  fill(255);
  textSize(25);
  text("PMD_Message: ", 10, 30);
  textSize(15);
  fill(55, 10, 252);
  text(MessageFromProcessing_PMD, 200, 30);
  textSize(20);
  fill(200);
  text("Alarms, press digits 0-9, s, u, h", 10, 60);
  fill(252, 10, 55);
  text(thePayload, 10, 100);

  if (mqttBrokerIsConnected) {
    fill(200);
    text("mqttBrokerIsConnected", 10, 150);
  } else {
    fill(252, 10, 55);
    text("mqttBroker NOT Connected", 10, 150);
  }
  
//  checkOverButton();

  //Footer
  textSize(10);
  fill(200);
  text("Broker Name: " + BROKER_URL , 10, height-30);
  text("myLogFileName: " + myLogFileName, 10, height - 20); 
  text("PROJECT_URL: " + PROJECT_URL, 10, height - 10);
  //end of Footer

  //Save the screen when MQTT connection events
  if (clientStatusChanged) {                
    String theTimeStamp = "";
    theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()); //time stamp
    save("./data/" + theTimeStamp+"_clientEvent.png");  
    clientStatusChanged = false;
  }
}//end draw()
