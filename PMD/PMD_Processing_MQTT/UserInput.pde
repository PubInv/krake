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
    
    for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
      int keyIndex = -1;
//      if (key >= 'A' && key <= 'Z') {
      if (key == 'Z') {
        //        MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:UpperCase";
        MessageFromProcessing_PMD = "a5Lee's Browser Not Responding. Might be Internet Provider Problem";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'A' ) {
        String theTimeStamp = "";
        theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) + " " ; //time stamp
        MessageFromProcessing_PMD = "a1 Lee has left Discord, " + theTimeStamp +id ;
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
        println("");
        println(MessageFromProcessing_PMD);
        println("");
        
      } else if (key == 'b' ) {
        MessageFromProcessing_PMD = "a2Lee is back in Discord";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'i' ) {
        MessageFromProcessing_PMD = "i";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'j' ) {  //Request information in JSON format.
        MessageFromProcessing_PMD = "j";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'l' ) {
        String theTimeStamp = "";
        theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) + " " ; //time stamp
        MessageFromProcessing_PMD = "a5Lee is in Discord, " + theTimeStamp ;
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'm' ) {
        MessageFromProcessing_PMD = "a2Lee's browser not responding. Giving up.";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'n' ) {    //For Nagham
        String theTimeStamp = "";
        theTimeStamp = str(year())+ String.format("%02d", month())+ String.format("%02d", day())+ "_"+ String.format("%02d", hour())+ String.format("%02d", minute())+ String.format("%02d", second()) + " " ; //time stamp
        MessageFromProcessing_PMD = "a5Nagham is in Discord, " + theTimeStamp ;
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'r' ) {
        MessageFromProcessing_PMD = "a1Lee's closing browser to restart it.";
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
      myBackground = color(0, 16, 0);
    }// end of for
  } else { //Client not connected so do nothing
    print(str(year())+str(month())+str(day())+"_"+str(hour())+str(minute())+str(second())+ " ");
    println("Client not connected. keyPressed() do nothing.");
    myBackground = color(16, 0, 0);
  }
}
