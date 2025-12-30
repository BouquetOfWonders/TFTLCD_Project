#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define sd_cs  5

#include <UTFTGLUE.h>
UTFTGLUE myGLCD(0,13, 12, 33, 32, 15);

#include <WiFi.h>

// add more if need be
const char *ssid1 = "YourSSID";
const char *password1 = "YourPassword";

NetworkServer server(80);

void setup()
{ 
  //Setup the LCD 480/320
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.clrScr();

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  //Setup Serial
  Serial.begin(115200);
  //Setup SD card
  Serial.print(F("Initializing SD card..."));
  myGLCD.clrScr();
  myGLCD.print("Initializing SD card", CENTER, 120);
  if (!SD.begin(sd_cs)) {
    Serial.println(F("failed!"));
    myGLCD.clrScr();
    myGLCD.print("SD card Fail", CENTER, 120);
    myGLCD.print("Power Cycle the Device", CENTER, 160);
    myGLCD.print("Check SD Card", CENTER, 180);
    while(1){}
  }
  Serial.print(F("Fine"));
// Setup Wifi Server
  Serial.print("Connecting to ");
  Serial.println(ssid1);

  WiFi.begin(ssid1, password1);

  myGLCD.clrScr();
  myGLCD.print("Connecting to", CENTER, 120);
  myGLCD.print(ssid1, CENTER, 160);

    for(int i; i < 20; i++){
      delay(500);
      Serial.print(".");
      if(WiFi.status() == WL_CONNECTED){
        break;
      }
    }
    

  if(WiFi.status() != WL_CONNECTED){
    myGLCD.clrScr();
    myGLCD.print("Starting Offline", CENTER, 120);
    delay(1000);
    myGLCD.clrScr();
      }
  else{
    myGLCD.clrScr();
    myGLCD.print("Connected", CENTER, 120);
    delay(1000);
    myGLCD.clrScr();
  }
  server.begin();
  Serial.println(WiFi.localIP());
  printImage("/Logo.txt", 109, 70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(WiFi.localIP().toString(), CENTER, 20);
}
void loop(){
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<a href=\"/Clr\">Clear Screen</a><br>");
            
            File root = SD.open("/");

              File FileInstance = root.openNextFile();
              while(FileInstance){
                String FileName = FileInstance.name();
                  if(FileName.endsWith(".Image")){
                    String ClientPrint = "<a href=\"";
                    ClientPrint += FileName;
                    ClientPrint += "\">";
                    ClientPrint += FileName;
                    ClientPrint += "</a><br>";
                    client.print(ClientPrint);
                  }
                  FileInstance = root.openNextFile();
                }
                root.close();
                FileInstance.close();
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /Clr")) {
          myGLCD.clrScr();
           Serial.println(currentLine);
        }
        if (currentLine.endsWith(".Image")) {
          String Directory = currentLine.substring(currentLine.indexOf("/"));
          char ConversionArray[Directory.length() + 1];
          Directory.toCharArray(ConversionArray, Directory.length() + 1);
          printImage(ConversionArray, -1, 0);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void printImage(char *Directory, int x, int y){
  File Image = SD.open(Directory);
  if(Image){
    String xImgstr = Image.readStringUntil('\n');
    String yImgstr = Image.readStringUntil('\n');
    int xImg = xImgstr.toInt();
    int yImg = yImgstr.toInt();
    if(x == -1){
      x = 240 - (xImg/2);
      y = 160 - (yImg/2);
    }
    int i = 0;
    int j = 0;
    while(j < yImg){
      i = 0;
      while(i < xImg){
        String R = Image.readStringUntil(' ');
        String G = Image.readStringUntil(' ');
        String B = Image.readStringUntil('\n');
        myGLCD.setColor (R.toInt(), G.toInt(), B.toInt());
        myGLCD.drawPixel(x + i, y + j);
        i++;
      }
      j++;
    }
    Image.close();
  }
  
}
