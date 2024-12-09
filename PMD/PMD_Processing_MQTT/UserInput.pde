/*Mouse code
 
 */

void checkOverButton() {
  int buttonHorz = 105;
  int buttonVert = 60;

  if (overButton == true) {
    fill(255);
  } else {
    fill(25);
  }
//  rect(105, 60, 75, 75);
  rect(buttonHorz, buttonVert, 75, 75);
//  line(135, 105, 155, 85);
  line(buttonHorz+30, buttonVert+45, buttonHorz+50, 85);
//  line(140, 85, 155, 85);
  line(buttonHorz+35, buttonVert+25, buttonHorz+50, buttonVert+25);
//  line(155, 85, 155, 100);
  line(buttonHorz+50, buttonVert+25, buttonHorz+50, 100);
}

void mousePressed() {
  if (overButton) { 
    //    link("http://www.processing.org");
    //    link("chrome.exe%20shiftr.io");
    //    link("chrome.exe%20http://www.shiftr.io");
    link("http://www.shiftr.io");
  }
}

void mouseMoved() { 
  checkButtons();
}

void mouseDragged() {
  checkButtons();
}

void checkButtons() {
  if (mouseX > 105 && mouseX < 180 && mouseY > 60 && mouseY <135) {
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
    for (int i = 0; i < KRAKE_DTA_TOPIC.length; i++) {
      int keyIndex = -1;
      if (key >= 'A' && key <= 'Z') {
        //        MessageFromProcessing_PMD = key + "MessageFromProcessing_PMD:UpperCase";
        MessageFromProcessing_PMD = "a5Lee's Browser Not Responding. Might be Internet Provider Problem";

        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'b' ) {
        MessageFromProcessing_PMD = "a2Lee has restart it. Back in Discord";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'l' ) {
        MessageFromProcessing_PMD = "a5Lee's Browser AGAIN Not Responding.";
        client.publish(KRAKE_DTA_TOPIC[i], MessageFromProcessing_PMD);
      } else if (key == 'm' ) {
        MessageFromProcessing_PMD = "a2Lee's browser not responding. Giving up.";
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
