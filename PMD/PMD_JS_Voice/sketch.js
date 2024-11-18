/*
 * 👋 Hello! This is an ml5.js example made and shared with ❤️.
 * Learn more about the ml5.js project: https://ml5js.org/
 * ml5.js license and Code of Conduct: https://github.com/ml5js/ml5-next-gen/blob/main/LICENSE.md
 *
 * This example demonstrates Sound classification using SpeechCommands18w
 */

/* Read about this at: https://docs.ml5js.org/#/reference/sound-classifier
*/

// Initialize a sound classifier method with SpeechCommands18w model. A callback needs to be passed.
let classifier;

// Initialize MQTT client at a broker and topics
let client = mqtt.connect('wss://public:public@public.cloud.shiftr.io');
let topic_foo = "10061C686A14_ALM"
let topic_alarm = "3C6105324EAC_ALM"; 
let topic2 = "10061C686A14_ACK"
let topic_ack = "3C6105324EAC_ACK"
let message_alarm4 = 'a4Message From p5 on line. abcdefghijklmnopqrstuvwxyz012345678'  //Works

let macUSA1 = "3C61053DF08C"
let macUSA2 = "3C6105324EAC"
let macUSA3 = "3C61053DF63C"
let macUSA4 = "10061C686A14"
let macUSA5 = "FCB467F4F74C"

// Array containing the 18 words of SpeechCommands18w
let words = [
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "up",
  "down",
  "left",
  "right",
  "go",
  "stop",
  "yes",
  "no",
];

// Variable for displaying the results on the canvas
let predictedWord = "";

function preload() {
  // Options for the SpeechCommands18w model, the default probabilityThreshold is 0
  let options = { probabilityThreshold: 0.7 };
  // Load SpeechCommands18w sound classifier model
  classifier = ml5.soundClassifier("SpeechCommands18w", options);
}

function setup() {
  createCanvas(650, 450);
  setupMQTT (); 
  // Classify the sound from microphone in real time
  classifier.classifyStart(gotResult);
}

function draw() {
  background(250);
  // Call function for displaying background words
  displayWords();

  // Once the model outputs results start displaying the predicted word on the canvas
  if (predictedWord !== "") {
//    fill(211, 107, 255);
    fill(211, 32, 32);
    textAlign(CENTER, CENTER);
    textSize(48);
//    text(predictedWord, width / 2, 90);
    text(predictedWord, width / 2, 140);
  }else {
    fill(255);
    textAlign(CENTER, CENTER);
    textSize(48);
    text("                 ", width / 2, 140);
  }
}

// Function to display the 18 words on the canvas
function displayWords() {
  textAlign(CENTER, CENTER);
  //textSize(32);
  textSize(40);
  fill(0);
  text("PMD Voice", width / 2, 20);
  fill(96);   
  textSize(18);
  text("(Testing Krake Alarms) Rev 0.1", width / 2, 50);
  textSize(32);
  text("Say one of these words!", width / 2, 80);
  textSize(18);
  text("zero-five for alarms ", width / 2, 105);

  textSize(32);
  let x = 125;
  let y = 150;
  // Words appear in 3 columns of 6 rows
  for (let i = 0; i < words.length; i++) {
    fill(158);
    text(words[i], x, y+25);
    y += 50;
    if ((i + 1) % 6 === 0) {
      x += 200;
      y = 150;
    }
  }
}

//Setup MQTT client for Krake
function setupMQTT (){
// client.subscribe(topic_ack);
client.subscribe((macUSA1 + "_ACK"));
client.subscribe((macUSA2 + "_ACK"));
client.subscribe((macUSA3 + "_ACK"));
client.subscribe((macUSA4 + "_ACK"));
client.subscribe((macUSA5 + "_ACK"));

// client.publish(topic_alarm, message_alarm4);
// console.log("P5 Web page Shifter_client_MQTT publised message to" + topic_ack);
//console.log(topic_ack);

client.on('message', function (topic_ack, message2) { // message is Buffer
  console.log("Got a subscription message  from topic" + topic_ack);
  console.log(message2.toString());
});

// client.end();

}

//Send an alarm message 
function publish_to_Krake(results){
  client.publish((macUSA1 + "_ALM"), results);
  client.publish((macUSA2 + "_ALM"), results);
  client.publish((macUSA3 + "_ALM"), results);
  client.publish((macUSA4 + "_ALM"), results);
  client.publish((macUSA5 + "_ALM"), results);
console.log("Publised " + results);
  
}


// A function to run when we get any errors and the results
function gotResult(results) {
  // The results are in an array ordered by confidence
//  console.log(results[0].label);
  // Load the first label to the text variable displayed on the canvas
  predictedWord = results[0].label;
  
  if (results[0].label == "zero"){
  publish_to_Krake("a0No message")      
  }
  if (results[0].label == "one") {
    publish_to_Krake("a1Information")  
  }
  if (results[0].label == "two") {
    publish_to_Krake("a2Problem")  
  }
  if (results[0].label == "three") {
    publish_to_Krake("a3Warn")  
  }
  if (results[0].label == "four") {
    publish_to_Krake("a4Critical")  
  }
  if (results[0].label == "five") {
    publish_to_Krake("a5Panic")  
  }
  if (results[0].label == "six") {
    publish_to_Krake("h")  //Help
  }
    if (results[0].label == "seven") {
    publish_to_Krake("s")  //Mute
  }
    if (results[0].label == "eight") {
    publish_to_Krake("u")  //unMute
  }
    if (results[0].label == "yes") {
    publish_to_Krake("u")  //unMute
  }
    if (results[0].label == "no") {
    publish_to_Krake("s")  //Mute
  }

  //Publish an MQTT here for results 
//  publish_to_Krake("a0no message")
  
}