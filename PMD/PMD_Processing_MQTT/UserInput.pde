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
      myBackground = color(0, 16, 0);
    }// end of for
  } else { //Client not connected so do nothing
    print(str(year())+str(month())+str(day())+"_"+str(hour())+str(minute())+str(second())+ " ");
    println("Client not connected. keyPressed() do nothing.");
    myBackground = color(16, 0, 0);
  }
}
