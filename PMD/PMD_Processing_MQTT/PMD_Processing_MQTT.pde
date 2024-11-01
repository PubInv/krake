// File: PMD_Processing_MQTT 
// Author: Forrest Lee Erickson
// Date: 20241006
// Date: 20241016 Background change color on send and receive MQTT. Subscribe to 20240421_USA4.
// Date: 20241017 Set title on window.
// Date: 20241024 Publish to a Krake model and serial number. Works with "FT_PMD_MQTT ""V0.7 "
// Date: 20241031 Begin publishing and subscriving messages in the GPAD API format of 20241031. Need a KRAKE which can parse GPAD API for this.
// Date: 20241101 Saved and renamed from FT_processingPMD_MQTT 

// Pseude Medical Device in Processing. 
// This example sketch connects to the public shiftr.io instance and sends a message on every keystroke.
// After starting the sketch you can find the client here: https://www.shiftr.io/try.
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
String VERSION = "V0.6 ";
// Set the topic for the Pseude Medical Device in Processing.
String PMD_DTA_TOPIC = "PROCESSING_PMD_USA1_DTA_TOPIC_USA_MARYVILLE";

//String KRAKE_DTA_TOPIC to Publish to in GPAD API form
// 20241031 Make the topics like that form of User manual "20240421_USA4_ALM" and "20240421_USA4_ACK" 
//where ALM is from PMD to KRAKE and ACK is from KRAKE to PMD

String KRAKE_DTA_TOPIC[] = {"KRAKE_20240421_USA1_ALM", "KRAKE_20240421_USA2_ALM", "KRAKE_20240421_USA3_ALM", "KRAKE_20240421_USA4_ALM", "KRAKE_20240421_USA5_ALM", 
  "KRAKE_20240421_LEB1", "KRAKE_20240421_LEB2", "KRAKE_20240421_LEB3", "KRAKE_20240421_LEB4", "KRAKE_20240421_LEB5" }; //Publish to a Krake data topic for ALARMs.

String KRAKE_ACK_TOPIC[] = {"KRAKE_20240421_USA1_ACK", "KRAKE_20240421_USA2_ACK", "KRAKE_20240421_USA3_ACK", "KRAKE_20240421_USA4_ACK", "KRAKE_20240421_USA5_ACK", 
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

    client.subscribe("PMD_Austin1");    

    client.subscribe("20240421_USA1");    
    client.subscribe("20240421_USA2");    
    client.subscribe("20240421_USA3");    
    client.subscribe("20240421_USA4");    
    client.subscribe("20240421_USA5");    


    //    client.subscribe("KRAKE_DTA_TOPIC");
  }

  void messageReceived(String topic, byte[] payload) {
    thePayload = "Received message: " + topic + " - " + new String(payload);
    //    println("new message: " + topic + " - " + new String(payload));
    println(thePayload);
    background(0); //Set background on messageReceived.
  }

  void connectionLost() {
    println("connection lost");
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

  client.publish(PMD_DTA_TOPIC, "FirstMessage PMD USA1");
  MessageFromProcessing_PMD = "FirstMessage PMD USA1";
}//end setup()

void draw() {
  //Need a heart beat message

  //Text on draw window
  fill(255);
  textSize(25);
  text("PMD_DTA_TOPIC: ", 10, 30);
  textSize(15);
  text(PMD_DTA_TOPIC, 225, 30);
  textSize(20);
  fill(200);
  text("Press digits 0-9", 10, 60);
  fill(252, 10, 55);
  
  text(MessageFromProcessing_PMD, 10, 80);
  fill(55, 10, 252);
  text(thePayload, 10, 100);
}//end draw()


void keyPressed() {
  for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
    int keyIndex = -1;
    if (key >= 'A' && key <= 'Z') {
//      keyIndex = key - 'A';
      MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:UpperCase";
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    } else if (key >= 'a' && key <= 'z') {
//      keyIndex = key - 'a';
      MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:LowerCase";
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    } else if (key >= '0' && key <= '9') {
      keyIndex = key - '0';                                                           //Offset the numerical ASCII down to an int.
      MessageFromProcessing_PMD = "MessageFromProcessing_PMD:" + " " + (int(key)-48);
      client.publish(PMD_DTA_TOPIC, MessageFromProcessing_PMD);


      //    float[] randoms = new float[100];
      //for (int i = 0; i < randoms.length; i++) {
      //  randoms[i] = random(100);
      //}

      //Form the GPAD API compatible message for KRAKE Topic.  Need a for loop for all Krakes.

      MessageFromProcessing_PMD = "a" + (int(key)-48) + "MessageFromProcessing_PMD:" + (int(key)-48);
      client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
    }
  }// end of for 


  background(0, 16, 0); //Set background on sent message.  } else {
}
