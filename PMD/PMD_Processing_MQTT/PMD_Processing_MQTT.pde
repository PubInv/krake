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

// Pseude Medical Device in Processing. 
// This program is a development tool for the Krake(TM) wirless alarm device.
// This example sketch connects to the public shiftr.io instance and sends a message on every keystroke.
// You can find the broker visulization here: https://public.cloud.shiftr.io/.
//
// Note: If you're running the sketch via the Android Mode you need to set the INTERNET permission
// in Android > Sketch Permissions.
//
// by Joël Gähwiler
// https://github.com/256dpi/processing-mqtt

/* 
 #define LICENSE "GNU Affero General Public License, version 3 "
 #define ORIGIN "USA"
 */

String PROG_NAME = "FT_processingPMD_MQTT";
String VERSION = "V0.8 ";

//String KRAKE_DTA_TOPIC[] = {"KRAKE_20240421_USA1_ALM", "KRAKE_20240421_USA2_ALM", "KRAKE_20240421_USA3_ALM", "KRAKE_20240421_USA4_ALM", "KRAKE_20240421_USA5_ALM", 
//  "KRAKE_20240421_LEB1_ALM", "KRAKE_20240421_LEB2_ALM", "KRAKE_20240421_LEB_ALM3", "KRAKE_20240421_LEB4_ALM", "KRAKE_20240421_LEB5_ALM" }; //Publish to a Krake data topic for ALARMs.

String KRAKE_DTA_TOPIC[] = {"3C61053DF08C_ALM", "3C6105324EAC_ALM", "3C61053DF63C_ALM", "10061C686A14_ALM", "FCB467F4F74C_ALM", 
  "KRAKE_20240421_LEB1_ALM", "KRAKE_20240421_LEB2_ALM", "KRAKE_20240421_LEB_ALM3", "KRAKE_20240421_LEB4_ALM", "KRAKE_20240421_LEB5_ALM" }; //Publish to a Krake data topic for ALARMs.


//String KRAKE_ACK_TOPIC[] = {"KRAKE_20240421_USA1_ACK", "KRAKE_20240421_USA2_ACK", "KRAKE_20240421_USA3_ACK", "KRAKE_20240421_USA4_ACK", "KRAKE_20240421_USA5_ACK", 
//  "KRAKE_20240421_LEB1_ACK", "KRAKE_20240421_LEB2_ACK", "KRAKE_20240421_LEB3_ACK", "KRAKE_20240421_LEB4_ACK", "KRAKE_20240421_LEB5_ACK" }; //Subscrive to a Krake ack.
String KRAKE_ACK_TOPIC[] = {"3C61053DF08C_ACK", "3C6105324EAC_ACK", "3C61053DF63C_ACK", "10061C686A14_ACK", "FCB467F4F74C_ACK",
"KRAKE_20240421_LEB1_ACK", "KRAKE_20240421_LEB2_ACK", "KRAKE_20240421_LEB3_ACK", "KRAKE_20240421_LEB4_ACK", "KRAKE_20240421_LEB5_ACK" }; //Subscrive to a Krake ack.


String thePayload = "";

import mqtt.*;

MQTTClient client;
String MessageFromProcessing_PMD = "";

class Adapter implements MQTTListener {
  void clientConnected() {
    println("client connected");

    for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
      client.subscribe(KRAKE_ACK_TOPIC[i]);
    }

// Old formats for topics from KRAKE from firmware: FT_PMD_MQTT.ino VERSION "V0.7 "
    client.subscribe("/mello");
    client.subscribe("PMD_LB1");
    client.subscribe("PMD_LB2");
    client.subscribe("PMD_LB3");
    client.subscribe("PMD_LB4");
    client.subscribe("PMD_LB5");
    client.subscribe("20240421_LEB1");
    client.subscribe("20240421_LEB2");
    client.subscribe("20240421_LEB3");
    client.subscribe("20240421_LEB4");
    client.subscribe("20240421_LEB5");

    client.subscribe("PMD_Austin1");    

    client.subscribe("20240421_USA1");    
    client.subscribe("20240421_USA2");    
    client.subscribe("20240421_USA3");    
    client.subscribe("20240421_USA4");    
    client.subscribe("20240421_USA5");    
    //    client.subscribe("KRAKE_DTA_TOPIC");
  }

  void messageReceived(String topic, byte[] payload) {
    thePayload = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) ; //time stamp
//    thePayload = thePayload + " " + "Recd msg: " + topic + " - " + new String(payload);
    thePayload = thePayload + " " + "Msg_recd: " + topic + " - " + new String(payload);
    println(thePayload);
    background(0); //Set background on messageReceived.
  }

  void connectionLost() {
    println("connection lost");
    background(128,0,0); //Set background on
  }
}

Adapter adapter;

void setup() {
  surface.setTitle(PROG_NAME + " Ver:" + VERSION);

  adapter = new Adapter();
  client = new MQTTClient(this, adapter);
  //  client.connect("mqtt://public:public@public.cloud.shiftr.io", "processing");
  client.connect("mqtt://public:public@public.cloud.shiftr.io", "Lee's processing");

  size(700, 360);
  //disables drawing outlines
  noStroke();
  //background(255, 4, 255);
  background(64);
  frameRate(24);

  MessageFromProcessing_PMD = "Nothing published Yet";
}//end setup()

void draw() {
  //Need a heart beat message
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
}//end draw()


/* Keyboard Event handler for single key.
Sets digits prefixed with an "a" for alarm.
Alpha as is.
Suppresses all other keys
Publishes to all devices, aka topics, KRAKE_DTA_TOPIC[i]
*/
void keyPressed() {
  for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
    int keyIndex = -1;
    if (key >= 'A' && key <= 'Z') {
      MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:UpperCase";
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    } else if (key >= 'a' && key <= 'z') {
      MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:LowerCase";
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    } else if (key >= '0' && key <= '9') {     //Alarms by number pressed.
      keyIndex = key - '0';                                                           //Offset the numerical ASCII down to an int.
      MessageFromProcessing_PMD = "MessageFromProcessing_PMD:" + " " + (int(key)-48);
      //Form the GPAD API compatible message for KRAKE Topic.  Need a for loop for all Krakes.
      MessageFromProcessing_PMD = "a" + (int(key)-48) + "MessageFromProcessing_PMD:" + (int(key)-48);
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    }
  }// end of for 
  background(0, 16, 0); //Set background on sent message. 
}
