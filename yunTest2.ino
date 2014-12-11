#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Adafruit_NeoPixel.h>
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;
byte pixelValues[NUMPIXELS];
bool pixelIncrements[NUMPIXELS];
byte minValue = 1;
byte maxValue = 30;
int delayRetarget = 40;
String colorScheme = "blue";

void setup() {
  ring.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  // Bridge startup
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  
  initializeRandomValuesAndIncrements();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();
  // There is a new client?
  if (client) {
    // Process request
    process(client);
    // Close connection and free resources.
    client.stop();
  }
  //delay(50); // Poll every 50ms
  retargetValuesAndIncrements();
}

void initializeRandomValuesAndIncrements() {
  for(int i=0;i<NUMPIXELS;i++){
    pixelValues[i] = random(minValue, maxValue+1);
    pixelIncrements[i] = random(2);
  }
}

void retargetValuesAndIncrements() {
  for(int i=0;i<NUMPIXELS;i++){
    
    if (pixelIncrements[i]) {
      if (pixelValues[i] < maxValue) {
        pixelValues[i]++;
      }
      else if (pixelValues[i] == maxValue) {
        pixelIncrements[i] = !pixelIncrements[i];
        pixelValues[i]--;
      }
    }
    else {
      if (pixelValues[i] > minValue) {
        pixelValues[i]--;
      }
      else if (pixelValues[i] <= minValue) {
        pixelIncrements[i] = !pixelIncrements[i];
        pixelValues[i]++;
      }
    }
    
    if (colorScheme == "white") ring.setPixelColor(i, ring.Color(0,pixelValues[i],0));
    else if (colorScheme == "blue") ring.setPixelColor(i, ring.Color(pixelValues[i],pixelValues[i],pixelValues[i]));
    //Serial.println(pixels.getPixelColor(11));
  }
  ring.show(); // This sends the updated pixel color to the hardware.
  delay(delayRetarget);
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  if (command == "makeittrickle") {
    minValue = 1;
    maxValue = 255;
    delayRetarget = 1;
    colorScheme = "blue";
    initializeRandomValuesAndIncrements();
  }
  else if (command == "makeitrain") {
    minValue = 1;
    maxValue = 255;
    delayRetarget = 1;
    colorScheme = "white";
    initializeRandomValuesAndIncrements();
  }
}
