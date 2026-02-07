
String PROG_NAME = "PMD_Processing_MQTT";
String VERSION = "V0.32 ";
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
// Date: 20250106 Rev 0.26.  Simplify MAC to serial number management. Set a will for the "_ALM" topics.
// Date: 20250106 Rev 0.27.  Set LWIT PMD. Add x and X keys to disconnect and exit the sketch.
// Date: 20250107 Rev 0.28.  Add test 't' sends "a1Test with CR and LF\r\n" to reproduceing  Unexpected characters at the end of the alarm message, GPAD API 0.22 #131
// Date: 20250115 Rev 0.29.  Add test 'e' and 'f' for Robert Status in Discord.
// Date: 20250201 Rev 0.30.  Spaces to formate date and time and ID in message.
// Date: 20251225 Rev 0.32.  Publish with retain = true.  void client.publish(String topic, byte[] payload, int qos, boolean retained);





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

// Make MAC to Serial number association in this dictionary
StringDict mac_to_NameDict = new StringDict();
void setupDictionary() {
  
  
  mac_to_NameDict.set("F024F9F1B874", "KRAKE_LB0001");
  mac_to_NameDict.set("142B2FEB1F00", "KRAKE_LB0002");
  mac_to_NameDict.set("142B2FEB1C64", "KRAKE_LB0003");
  mac_to_NameDict.set("142B2FEB1E24", "KRAKE_LB0004");
  mac_to_NameDict.set("F024F9F1B880", "KRAKE_LB0005");
  mac_to_NameDict.set("F4650BC295C0", "KRAKE_LB0006");
  
  mac_to_NameDict.set("F4650BC0B52C", "KRAKE_US0006");
  mac_to_NameDict.set("ECC9FF7D8EE8", "KRAKE_US0005");
  mac_to_NameDict.set("ECC9FF7D8EF4", "KRAKE_US0004");
  mac_to_NameDict.set("ECC9FF7C8C98", "KRAKE_US0003");
  mac_to_NameDict.set("ECC9FF7D8F00", "KRAKE_US0002");
  mac_to_NameDict.set("ECC9FF7C8BDC", "KRAKE_US0001");
  mac_to_NameDict.set("3C61053DF08C", "20240421_USA1");
  mac_to_NameDict.set("3C6105324EAC", "20240421_USA2");
  mac_to_NameDict.set("3C61053DF63C", "20240421_USA3");
  mac_to_NameDict.set("10061C686A14", "20240421_USA4");
  mac_to_NameDict.set("FCB467F4F74C", "20240421_USA5");
  mac_to_NameDict.set("CCDBA730098C", "20240421_LEB1");
  mac_to_NameDict.set("CCDBA730BFD4", "20240421_LEB2");
  mac_to_NameDict.set("CCDBA7300954", "20240421_LEB3");
  mac_to_NameDict.set("A0DD6C0EFD28", "20240421_LEB4");
  mac_to_NameDict.set("10061C684D28", "20240421_LEB5");
  mac_to_NameDict.set("A0B765F51E28", "MockingKrake_LEB");
  mac_to_NameDict.set("3C61053DC954", "Not Homework2, Maryville TN");
}//end setup mac_to_NameDict

// Makes an array out of the dictionary keys. 
String KRAKE_MAC[] = {};
void make_KrakeArray() {
  for (String k : mac_to_NameDict.keys()) {
    //    println(k);
    KRAKE_MAC = append(KRAKE_MAC, k);
  }
} 

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
    theTimeStamp = theTimeStamp + "MQTT client Connected" ;
    println(theTimeStamp);  
    appendTextToFile(myLogFileName, "MQTT clientConnected");
    clientStatusChanged = true; //TO flag save of draw() window.

    mqttBrokerIsConnected = true;
    for (int i = 0; i < KRAKE_MAC.length; i++) {
      //client.subscribe("F024F9F1B880_ALM");// LB0005  Used to check LWT development.
      client.subscribe(KRAKE_MAC[i]+"_ACK");
      //      client.setWill(KRAKE_MAC[i]+"_ACK", KRAKE_MAC[i]+" Has disconnected.");
      //      client.setWill(KRAKE_MAC[i]+"_ACK", KRAKE_MAC[i]+" Has disconnected.");
      client.setWill(KRAKE_MAC[i]+"_ALM", "a1 LWIT PMD" + theMAC +" has disconnected.");
    }//end for i
  }// end clientCOnnect

  void messageReceived(String topic, byte[] payload) {    
    topic = topic.substring(0, 12); //Check shorten topic for a match to a MAC
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

//FLE This does not seam to work.
void disconnectMQTT() {
  //Set exit flag
  delay(1000);
  if (disconnectMQTTBroker)   client.disconnect();    //  the MQTT BROKER
  delay(1000);
  mqttBrokerIsConnected = false;
  disconnectMQTTBroker = false;
}

boolean exitProgram = false;

Adapter adapter;
boolean disconnectMQTTBroker = false;
boolean mqttBrokerIsConnected = false;

color myBackground = color(64, 64, 64);  //Start grey
color shiftrBackground = color(0, 171, 92);  //Shifter.io


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
  make_KrakeArray(); // Make an array of the MAC addresses, KRAKE_MAC[]

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

  //Footer
  textSize(10);
  fill(200);
  text("Broker Name: " + BROKER_URL, 10, height-30);
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
  
  if (disconnectMQTTBroker) {
    disconnectMQTT();
    disconnectMQTTBroker = false;
  }

  if (exitProgram == true) {
    println("exitProgram is true");
    exit();
  }//end draw()
}
