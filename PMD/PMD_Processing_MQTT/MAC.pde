//String PROG_NAME = "MAC_address_Processing";
//String VERSION = "V0.1 ";
//String PROJECT_URL = ""; 
//color myBackground = 0;

// Author: Forrest Lee Erickson
// Date: 20241209
// Summary: Find and add to title the MAC address of the computer on which the sketch is running.
// License: Dedicated to the public domain.
// Warranty: This sketch is designed to kill you and render the earth uninhabitable but is not guaranteed to do so.

/*  Started from but modified
AI Overview
Learn more
To find the MAC address in Processing (which uses Java underneath), 
you can use the NetworkInterface class from the java.net package to
access the network interfaces and retrieve their hardware addresses 
(MAC addresses) as a byte array, then convert each byte to a
hexadecimal string to display it properly
*/

import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

String theMAC = "default";

void getNetworkInterface() {
  try {
    Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
    while (interfaces.hasMoreElements()) {
      NetworkInterface networkInterface = interfaces.nextElement();
      byte[] mac = networkInterface.getHardwareAddress();
      if (mac != null) {
        printMacAddress(mac);
      }
    }
  } 
  catch (SocketException e) {
    println("Error getting network interfaces: " + e.getMessage());
    theMAC = ("Error getting network interfaces: " + e.getMessage());
  }
}

void printMacAddress(byte[] mac) {
  StringBuilder macString = new StringBuilder();
  for (int i = 0; i < mac.length; i++) {
    String hex = String.format("%02X", mac[i] & 0xFF);
    macString.append(hex).append("-");
  }
  // Remove the trailing hyphen
  macString.deleteCharAt(macString.length() - 1);
  println("MAC Address: " + macString.toString());
//  theMAC = ("MAC Address: " + macString.toString());
  theMAC = macString.toString();
}



//void setup() {
////  surface.setTitle(PROG_NAME + " Ver:" + VERSION);
//  size(700, 360);
//  noStroke();    //disables drawing outlines
//  background (myBackground);
//  frameRate(24);
  
//  //Add this to a sketch to set the MAC into the draw() title
//  getNetworkInterface();
//  surface.setTitle(PROG_NAME + " Ver:" + VERSION + "MAC: " + theMAC);

//}

//void draw() {
//  background(0);
//  fill(255);
//  textSize(25);
//  text("MAC: ", 10, 30);
//  text(theMAC, 75, 30);
//}
