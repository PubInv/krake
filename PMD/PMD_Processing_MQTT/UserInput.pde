/*Mouse code
 
 */

//Broker Button
int buttonHorz = 50;
int buttonVert = 200;


//Broker Button
void checkOverButton() {
  //int buttonHorz = 50;
  //int buttonVert = 160;
  //int arrowHorz = 155;
  //int arrowVert = 85;
  int arrowHorz = buttonHorz +50 ;
  int arrowVert = buttonVert + 25;

  if (overButton == true) {
    fill(255);
    stroke(32);
  } else {
    //    fill(32);
    fill(shiftrBackground);  //Background at shiftr.io
    stroke(0);
    //noFill();
  }

  //Draw button
  //  rect(105, 60, 75, 75);
  //  stroke(128);
  rect(buttonHorz, buttonVert, 75, 75);
  fill(255);
  //  line(135, 105, 155, 85);
  line(arrowHorz-20, arrowVert + 30, arrowHorz, arrowVert);
  //  line(140, 85, 155, 85);
  line(arrowHorz - 15, arrowVert, arrowHorz, arrowVert);
  //  line(155, 85, 155, 100);
  line(arrowHorz, arrowVert, arrowHorz, arrowVert + 15);
  text("Broker View", buttonHorz+10, buttonVert);
}

void mousePressed() {
  if (overButton) {
    link("http://www.shiftr.io/try");
  }
}

void mouseMoved() {
  checkButtons();
}

void mouseDragged() {
  checkButtons();
}

void checkButtons() {
  if (mouseX > buttonHorz && mouseX < buttonHorz +75 && mouseY > buttonVert && mouseY <buttonVert+75) {
    overButton = true;
  } else {
    overButton = false;
  }
}

/* Keyboard Event handler for single key.
 Sets digits prefixed with an "a" for alarm.
 Alpha as is.
 Suppresses all other keys
 Publishes to all devices, aka topics, KRAKE_DTA_TOPIC[i]
 */
void keyPressed() {

  if (mqttBrokerIsConnected) {         
    TimeZone tz = TimeZone.getDefault();
    String id = tz.getID();
    String theTimeStamp = "";
    theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ " "+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second())+ " " + id  ; //time stamp

    for (int i = 0; i < KRAKE_MAC.length; i++) { //Need a for loop for all Krakes.
      int keyIndex = -1;
      if (key == 'Z') {
        MessageFromProcessing_PMD = "a5Lee's Browser Not Responding. Might be Internet Provider Problem";
      } else if (key == 'a' ) {
        MessageFromProcessing_PMD = "a1 Lee has left Discord, " + theTimeStamp ;
      } else if (key == 'b' ) {
        MessageFromProcessing_PMD = "a5 Lee is back in Discord" + theTimeStamp;
      } else if (key == 'c' ) {
        MessageFromProcessing_PMD = "a1 Nagham has left Discord, " + theTimeStamp ;
      } else if (key == 'd' ) {
        MessageFromProcessing_PMD = "a5 Nagam is back in Discord" + theTimeStamp;
      } else if (key == 'e' ) {
        MessageFromProcessing_PMD = "a1 Robert has left Discord, " + theTimeStamp ;
      } else if (key == 'f' ) {
        MessageFromProcessing_PMD = "a5 Robert is back in Discord" + theTimeStamp;
      } else if (key == 'i' ) {
        MessageFromProcessing_PMD = "i";
      } else if (key == 'j' ) {  //Request information in JSON format.
        MessageFromProcessing_PMD = "j";
      } else if (key == 'l' ) {
        MessageFromProcessing_PMD = "a5Lee is in Discord, " + theTimeStamp ;
      } else if (key == 'm' ) {
        MessageFromProcessing_PMD = "a2Lee's browser not responding. Giving up."  + theTimeStamp;
      } else if (key == 'n' ) {    //For Nagham
        MessageFromProcessing_PMD = "a5Nagham is in Discord, " + theTimeStamp ;
      } else if (key == 'r' ) {
        MessageFromProcessing_PMD = "a1Lee's closing browser to restart it."  + theTimeStamp;
      } else if (key == 't' ) {
        MessageFromProcessing_PMD = "a1Test with CR and LF\r\n"  + theTimeStamp;
      } else if (key == 'X' ) { //Exit the MQTT broker
        exit();
      } else if (key == 'x' ) { //Exit the MQTT broker
        MessageFromProcessing_PMD = "a1 PMD disconnecting MQTT broker."  + theTimeStamp;
        client.publish(KRAKE_MAC[i]+"_ALM", MessageFromProcessing_PMD);
        disconnectMQTTBroker = true;  // Not working
        println("Exit PMT");
        //        exit();
        exitProgram = true;
      } else if (key >= 'a' && key <= 'z') {
        MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:LowerCase" + theTimeStamp;
      } else if (key >= '0' && key <= '9') {     //Alarms by number pressed.
        keyIndex = key - '0';                                                           //Offset the numerical ASCII down to an int.
        MessageFromProcessing_PMD = "MessageFromProcessing_PMD:" + " " + (int(key)-48);
        //Form the GPAD API compatible message for KRAKE Topic.  
        MessageFromProcessing_PMD = "a" + (int(key)-48) + "MessageFromProcessing_PMD:" + (int(key)-48)+ " " + theTimeStamp;
      }

      if (mqttBrokerIsConnected) {
        client.publish(KRAKE_MAC[i]+"_ALM", MessageFromProcessing_PMD);
      }

      myBackground = color(0, 16, 0);
    }// end of for
  } else { //Client not connected so do nothing
    print(str(year())+str(month())+str(day())+"_"+str(hour())+str(minute())+str(second())+ " ");
    println("Client not connected. keyPressed() do nothing.");
    myBackground = color(16, 0, 0);
  }
}
