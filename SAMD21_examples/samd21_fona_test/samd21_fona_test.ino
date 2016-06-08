/*
	Example of use SAMD21 with FONA808 
	Ricardo Mena C
	ricardo@crcibernetica.com
	http://crcibernetica.com
	
*/

/***************************************************
  This is an example for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963
  ----> http://www.adafruit.com/products/2468
  ----> http://www.adafruit.com/products/2542

  These cellular modules use TTL serial to communicate, 2 pins are
  required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*
THIS CODE IS STILL IN PROGRESS!

Open up the serial console on the Arduino at 115200 baud to interact with FONA

Note that if you need to set a GPRS APN, username, and password scroll down to
the commented section below at the end of the setup() function.
*/
#include "Adafruit_FONA.h"

//---------FONA network-------------
#define FONA_RX 1
#define FONA_TX 0
#define FONA_RST 2
#define FONA_KEY 3
#define FONA_PS 4
#define FONA_VIO 5
//-----------------------------------

// this is a large buffer for replies
char replybuffer[255];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the Hardwareserial line

//ATENTION!!!!
//You could use this example with sparkfun samd21 dev or mini board
//as well with feather M0, just comment SPRKD21 definition
#define SPRKD21//Comment this line for use with Feather M0

#ifdef SPRKD21
	#define fonaserial Serial1
	#define serial SerialUSB
#else
	#define fonaserial Serial
	#define serial Serial1
#endif

// Hardware serial is also possible!
//  Hardwareserial *fonaserial = &serial1;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

uint8_t type;

void setup() {
  while (!serial);

  pinMode(FONA_KEY, OUTPUT);
  pinMode(FONA_PS, INPUT);
  pinMode(FONA_VIO, OUTPUT);
  digitalWrite(FONA_VIO, HIGH);


  serial.begin(115200);
  serial.println(F("FONA basic test"));
  serial.println(F("Initializing....(May take 3 seconds)"));

  fonaserial.begin(4800);
  if (! fona.begin(fonaserial)) {
    serial.println(F("Couldn't find FONA"));
    while (1);
  }
  type = fona.type();
  serial.println(F("FONA is OK"));
  serial.print(F("Found "));
  switch (type) {
    case FONA800L:
      serial.println(F("FONA 800L")); break;
    case FONA800H:
      serial.println(F("FONA 800H")); break;
    case FONA808_V1:
      serial.println(F("FONA 808 (v1)")); break;
    case FONA808_V2:
      serial.println(F("FONA 808 (v2)")); break;
    case FONA3G_A:
      serial.println(F("FONA 3G (American)")); break;
    case FONA3G_E:
      serial.println(F("FONA 3G (European)")); break;
    default: 
      serial.println(F("???")); break;
  }
  
  // Print module IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    serial.print("Module IMEI: "); serial.println(imei);
  }

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
 //fona.setGPRSNetworkSettings(F(""), F(""), F(""));

  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);

  printMenu();
}

void printMenu(void) {
  serial.println(F("-------------------------------------"));
  serial.println(F("[?] Print this menu"));
  serial.println(F("[a] read the ADC 2.8V max (FONA800 & 808)"));
  serial.println(F("[b] read the Battery V and % charged"));
  serial.println(F("[C] read the SIM CCID"));
  serial.println(F("[U] Unlock SIM with PIN code"));
  serial.println(F("[i] read RSSI"));
  serial.println(F("[n] get Network status"));
  serial.println(F("[v] set audio Volume"));
  serial.println(F("[V] get Volume"));
  serial.println(F("[H] set Headphone audio (FONA800 & 808)"));
  serial.println(F("[e] set External audio (FONA800 & 808)"));
  serial.println(F("[T] play audio Tone"));
  serial.println(F("[P] PWM/Buzzer out (FONA800 & 808)"));

  // FM (SIM800 only!)
  serial.println(F("[f] tune FM radio (FONA800)"));
  serial.println(F("[F] turn off FM (FONA800)"));
  serial.println(F("[m] set FM volume (FONA800)"));
  serial.println(F("[M] get FM volume (FONA800)"));
  serial.println(F("[q] get FM station signal level (FONA800)"));

  // Phone
  serial.println(F("[c] make phone Call"));
  serial.println(F("[A] get call status"));
  serial.println(F("[h] Hang up phone"));
  serial.println(F("[p] Pick up phone"));

  // SMS
  serial.println(F("[N] Number of SMSs"));
  serial.println(F("[r] Read SMS #"));
  serial.println(F("[R] Read All SMS"));
  serial.println(F("[d] Delete SMS #"));
  serial.println(F("[s] Send SMS"));
  serial.println(F("[u] Send USSD"));
  
  // Time
  serial.println(F("[y] Enable network time sync (FONA 800 & 808)"));
  serial.println(F("[Y] Enable NTP time sync (GPRS FONA 800 & 808)"));
  serial.println(F("[t] Get network time"));

  // GPRS
  serial.println(F("[G] Enable GPRS"));
  serial.println(F("[g] Disable GPRS"));
  serial.println(F("[l] Query GSMLOC (GPRS)"));
  serial.println(F("[w] Read webpage (GPRS)"));
  serial.println(F("[W] Post to website (GPRS)"));

  // GPS
  if ((type == FONA3G_A) || (type == FONA3G_E) || (type == FONA808_V1) || (type == FONA808_V2)) {
    serial.println(F("[O] Turn GPS on (FONA 808 & 3G)"));
    serial.println(F("[o] Turn GPS off (FONA 808 & 3G)"));
    serial.println(F("[L] Query GPS location (FONA 808 & 3G)"));
    if (type == FONA808_V1) {
      serial.println(F("[x] GPS fix status (FONA808 v1 only)"));
    }
    serial.println(F("[E] Raw NMEA out (FONA808)"));
  }
  
  serial.println(F("[S] create serial passthru tunnel"));
  serial.println(F("-------------------------------------"));
  serial.println(F(""));

}
void loop() {
  serial.print(F("FONA> "));
  while (! serial.available() ) {
    if (fona.available()) {
      serial.write(fona.read());
    }
  }

  char command = serial.read();
  serial.println(command);


  switch (command) {
    case '?': {
        printMenu();
        break;
      }

    case 'a': {
        // read the ADC
        uint16_t adc;
        if (! fona.getADCVoltage(&adc)) {
          serial.println(F("Failed to read ADC"));
        } else {
          serial.print(F("ADC = ")); serial.print(adc); serial.println(F(" mV"));
        }
        break;
      }

    case 'b': {
        // read the battery voltage and percentage
        uint16_t vbat;
        if (! fona.getBattVoltage(&vbat)) {
          serial.println(F("Failed to read Batt"));
        } else {
          serial.print(F("VBat = ")); serial.print(vbat); serial.println(F(" mV"));
        }


        if (! fona.getBattPercent(&vbat)) {
          serial.println(F("Failed to read Batt"));
        } else {
          serial.print(F("VPct = ")); serial.print(vbat); serial.println(F("%"));
        }

        break;
      }

    case 'U': {
        // Unlock the SIM with a PIN code
        char PIN[5];
        flushserial();
        serial.println(F("Enter 4-digit PIN"));
        readline(PIN, 3);
        serial.println(PIN);
        serial.print(F("Unlocking SIM card: "));
        if (! fona.unlockSIM(PIN)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    case 'C': {
        // read the CCID
        fona.getSIMCCID(replybuffer);  // make sure replybuffer is at least 21 bytes!
        serial.print(F("SIM CCID = ")); serial.println(replybuffer);
        break;
      }

    case 'i': {
        // read the RSSI
        uint8_t n = fona.getRSSI();
        int8_t r;

        serial.print(F("RSSI = ")); serial.print(n); serial.print(": ");
        if (n == 0) r = -115;
        if (n == 1) r = -111;
        if (n == 31) r = -52;
        if ((n >= 2) && (n <= 30)) {
          r = map(n, 2, 30, -110, -54);
        }
        serial.print(r); serial.println(F(" dBm"));

        break;
      }

    case 'n': {
        // read the network/cellular status
        uint8_t n = fona.getNetworkStatus();
        serial.print(F("Network status "));
        serial.print(n);
        serial.print(F(": "));
        if (n == 0) serial.println(F("Not registered"));
        if (n == 1) serial.println(F("Registered (home)"));
        if (n == 2) serial.println(F("Not registered (searching)"));
        if (n == 3) serial.println(F("Denied"));
        if (n == 4) serial.println(F("Unknown"));
        if (n == 5) serial.println(F("Registered roaming"));
        break;
      }

    /*** Audio ***/
    case 'v': {
        // set volume
        flushserial();
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          serial.print(F("Set Vol [0-8] "));
        } else {
          serial.print(F("Set Vol % [0-100] "));
        }
        uint8_t vol = readnumber();
        serial.println();
        if (! fona.setVolume(vol)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    case 'V': {
        uint8_t v = fona.getVolume();
        serial.print(v);
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          serial.println(" / 8");
        } else {
          serial.println("%");
        }
        break;
      }

    case 'H': {
        // Set Headphone output
        if (! fona.setAudio(FONA_HEADSETAUDIO)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        fona.setMicVolume(FONA_HEADSETAUDIO, 15);
        break;
      }
    case 'e': {
        // Set External output
        if (! fona.setAudio(FONA_EXTAUDIO)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }

        fona.setMicVolume(FONA_EXTAUDIO, 10);
        break;
      }

    case 'T': {
        // play tone
        flushserial();
        serial.print(F("Play tone #"));
        uint8_t kittone = readnumber();
        serial.println();
        // play for 1 second (1000 ms)
        if (! fona.playToolkitTone(kittone, 1000)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    /*** FM Radio ***/

    case 'f': {
        // get freq
        flushserial();
        serial.print(F("FM Freq (eg 1011 == 101.1 MHz): "));
        uint16_t station = readnumber();
        serial.println();
        // FM radio ON using headset
        if (fona.FMradio(true, FONA_HEADSETAUDIO)) {
          serial.println(F("Opened"));
        }
        if (! fona.tuneFMradio(station)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("Tuned"));
        }
        break;
      }
    case 'F': {
        // FM radio off
        if (! fona.FMradio(false)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }
    case 'm': {
        // Set FM volume.
        flushserial();
        serial.print(F("Set FM Vol [0-6]:"));
        uint8_t vol = readnumber();
        serial.println();
        if (!fona.setFMVolume(vol)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }
    case 'M': {
        // Get FM volume.
        uint8_t fmvol = fona.getFMVolume();
        if (fmvol < 0) {
          serial.println(F("Failed"));
        } else {
          serial.print(F("FM volume: "));
          serial.println(fmvol, DEC);
        }
        break;
      }
    case 'q': {
        // Get FM station signal level (in decibels).
        flushserial();
        serial.print(F("FM Freq (eg 1011 == 101.1 MHz): "));
        uint16_t station = readnumber();
        serial.println();
        int8_t level = fona.getFMSignalLevel(station);
        if (level < 0) {
          serial.println(F("Failed! Make sure FM radio is on (tuned to station)."));
        } else {
          serial.print(F("Signal level (dB): "));
          serial.println(level, DEC);
        }
        break;
      }

    /*** PWM ***/

    case 'P': {
        // PWM Buzzer output @ 2KHz max
        flushserial();
        serial.print(F("PWM Freq, 0 = Off, (1-2000): "));
        uint16_t freq = readnumber();
        serial.println();
        if (! fona.setPWM(freq)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    /*** Call ***/
    case 'c': {
        // call a phone!
        char number[30];
        flushserial();
        serial.print(F("Call #"));
        readline(number, 30);
        serial.println();
        serial.print(F("Calling ")); serial.println(number);
        if (!fona.callPhone(number)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("Sent!"));
        }

        break;
      }
    case 'A': {
        // get call status
        int8_t callstat = fona.getCallStatus();
        switch (callstat) {
          case 0: serial.println(F("Ready")); break;
          case 1: serial.println(F("Could not get status")); break;
          case 3: serial.println(F("Ringing (incoming)")); break;
          case 4: serial.println(F("Ringing/in progress (outgoing)")); break;
          default: serial.println(F("Unknown")); break;
        }
        break;
      }
      
    case 'h': {
        // hang up!
        if (! fona.hangUp()) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    case 'p': {
        // pick up!
        if (! fona.pickUp()) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("OK!"));
        }
        break;
      }

    /*** SMS ***/

    case 'N': {
        // read the number of SMS's!
        int8_t smsnum = fona.getNumSMS();
        if (smsnum < 0) {
          serial.println(F("Could not read # SMS"));
        } else {
          serial.print(smsnum);
          serial.println(F(" SMS's on SIM card!"));
        }
        break;
      }
    case 'r': {
        // read an SMS
        flushserial();
        serial.print(F("Read #"));
        uint8_t smsn = readnumber();
        serial.print(F("\n\rReading SMS #")); serial.println(smsn);

        // Retrieve SMS sender address/phone number.
        if (! fona.getSMSSender(smsn, replybuffer, 250)) {
          serial.println("Failed!");
          break;
        }
        serial.print(F("FROM: ")); serial.println(replybuffer);

        // Retrieve SMS value.
        uint16_t smslen;
        if (! fona.readSMS(smsn, replybuffer, 250, &smslen)) { // pass in buffer and max len!
          serial.println("Failed!");
          break;
        }
        serial.print(F("***** SMS #")); serial.print(smsn);
        serial.print(" ("); serial.print(smslen); serial.println(F(") bytes *****"));
        serial.println(replybuffer);
        serial.println(F("*****"));

        break;
      }
    case 'R': {
        // read all SMS
        int8_t smsnum = fona.getNumSMS();
        uint16_t smslen;
        int8_t smsn;

        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          smsn = 0; // zero indexed
          smsnum--;
        } else {
          smsn = 1;  // 1 indexed
        }

        for ( ; smsn <= smsnum; smsn++) {
          serial.print(F("\n\rReading SMS #")); serial.println(smsn);
          if (!fona.readSMS(smsn, replybuffer, 250, &smslen)) {  // pass in buffer and max len!
            serial.println(F("Failed!"));
            break;
          }
          // if the length is zero, its a special case where the index number is higher
          // so increase the max we'll look at!
          if (smslen == 0) {
            serial.println(F("[empty slot]"));
            smsnum++;
            continue;
          }

          serial.print(F("***** SMS #")); serial.print(smsn);
          serial.print(" ("); serial.print(smslen); serial.println(F(") bytes *****"));
          serial.println(replybuffer);
          serial.println(F("*****"));
        }
        break;
      }

    case 'd': {
        // delete an SMS
        flushserial();
        serial.print(F("Delete #"));
        uint8_t smsn = readnumber();

        serial.print(F("\n\rDeleting SMS #")); serial.println(smsn);
        if (fona.deleteSMS(smsn)) {
          serial.println(F("OK!"));
        } else {
          serial.println(F("Couldn't delete"));
        }
        break;
      }

    case 's': {
        // send an SMS!
        char sendto[21], message[141];
        flushserial();
        serial.print(F("Send to #"));
        readline(sendto, 20);
        serial.println(sendto);
        serial.print(F("Type out one-line message (140 char): "));
        readline(message, 140);
        serial.println(message);
        if (!fona.sendSMS(sendto, message)) {
          serial.println(F("Failed"));
        } else {
          serial.println(F("Sent!"));
        }

        break;
      }

    case 'u': {
      // send a USSD!
      char message[141];
      flushserial();
      serial.print(F("Type out one-line message (140 char): "));
      readline(message, 140);
      serial.println(message);

      uint16_t ussdlen;
      if (!fona.sendUSSD(message, replybuffer, 250, &ussdlen)) { // pass in buffer and max len!
        serial.println(F("Failed"));
      } else {
        serial.println(F("Sent!"));
        serial.print(F("***** USSD Reply"));
        serial.print(" ("); serial.print(ussdlen); serial.println(F(") bytes *****"));
        serial.println(replybuffer);
        serial.println(F("*****"));
      }
    }

    /*** Time ***/

    case 'y': {
        // enable network time sync
        if (!fona.enableNetworkTimeSync(true))
          serial.println(F("Failed to enable"));
        break;
      }

    case 'Y': {
        // enable NTP time sync
        if (!fona.enableNTPTimeSync(true, F("pool.ntp.org")))
          serial.println(F("Failed to enable"));
        break;
      }

    case 't': {
        // read the time
        char buffer[23];

        fona.getTime(buffer, 23);  // make sure replybuffer is at least 23 bytes!
        serial.print(F("Time = ")); serial.println(buffer);
        break;
      }


    /*********************************** GPS (SIM808 only) */

    case 'o': {
        // turn GPS off
        if (!fona.enableGPS(false))
          serial.println(F("Failed to turn off"));
        break;
      }
    case 'O': {
        // turn GPS on
        if (!fona.enableGPS(true))
          serial.println(F("Failed to turn on"));
        break;
      }
    case 'x': {
        int8_t stat;
        // check GPS fix
        stat = fona.GPSstatus();
        if (stat < 0)
          serial.println(F("Failed to query"));
        if (stat == 0) serial.println(F("GPS off"));
        if (stat == 1) serial.println(F("No fix"));
        if (stat == 2) serial.println(F("2D fix"));
        if (stat == 3) serial.println(F("3D fix"));
        break;
      }

    case 'L': {
        // check for GPS location
        char gpsdata[120];
        fona.getGPS(0, gpsdata, 120);
        if (type == FONA808_V1)
          serial.println(F("Reply in format: mode,longitude,latitude,altitude,utctime(yyyymmddHHMMSS),ttff,satellites,speed,course"));
        else 
          serial.println(F("Reply in format: mode,fixstatus,utctime(yyyymmddHHMMSS),latitude,longitude,altitude,speed,course,fixmode,reserved1,HDOP,PDOP,VDOP,reserved2,view_satellites,used_satellites,reserved3,C/N0max,HPA,VPA"));
        serial.println(gpsdata);

        break;
      }

    case 'E': {
        flushserial();
        if (type == FONA808_V1) {
          serial.print(F("GPS NMEA output sentences (0 = off, 34 = RMC+GGA, 255 = all)"));
        } else {
          serial.print(F("On (1) or Off (0)? "));
        }
        uint8_t nmeaout = readnumber();

        // turn on NMEA output
        fona.enableGPSNMEA(nmeaout);

        break;
      }

    /*********************************** GPRS */

    case 'g': {
        // turn GPRS off
        if (!fona.enableGPRS(false))
          serial.println(F("Failed to turn off"));
        break;
      }
    case 'G': {
        // turn GPRS on
        if (!fona.enableGPRS(true))
          serial.println(F("Failed to turn on"));
        break;
      }
    case 'l': {
        // check for GSMLOC (requires GPRS)
        uint16_t returncode;

        if (!fona.getGSMLoc(&returncode, replybuffer, 250))
          serial.println(F("Failed!"));
        if (returncode == 0) {
          serial.println(replybuffer);
        } else {
          serial.print(F("Fail code #")); serial.println(returncode);
        }

        break;
      }
    case 'w': {
        // read website URL
        uint16_t statuscode;
        int16_t length;
        char url[80];

        flushserial();
        serial.println(F("NOTE: in beta! Use small webpages to read!"));
        serial.println(F("URL to read (e.g. www.adafruit.com/testwifi/index.html):"));
        serial.print(F("http://")); readline(url, 79);
        serial.println(url);

        serial.println(F("****"));
        if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
          serial.println("Failed!");
          break;
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

            // serial.write is too slow, we'll write directly to serial register!
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
            UDR0 = c;
#else
            serial.write(c);
#endif
            length--;
            if (! length) break;
          }
        }
        serial.println(F("\n****"));
        fona.HTTP_GET_end();
        break;
      }

    case 'W': {
        // Post data to website
        uint16_t statuscode;
        int16_t length;
        char url[80];
        char data[80];

        flushserial();
        serial.println(F("NOTE: in beta! Use simple websites to post!"));
        serial.println(F("URL to post (e.g. httpbin.org/post):"));
        serial.print(F("http://")); readline(url, 79);
        serial.println(url);
        serial.println(F("Data to post (e.g. \"foo\" or \"{\"simple\":\"json\"}\"):"));
        readline(data, 79);
        serial.println(data);

        serial.println(F("****"));
        if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
          serial.println("Failed!");
          break;
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
            UDR0 = c;
#else
            serial.write(c);
#endif

            length--;
            if (! length) break;
          }
        }
        serial.println(F("\n****"));
        fona.HTTP_POST_end();
        break;
      }
    /*****************************************/

    case 'S': {
        serial.println(F("Creating serial TUBE"));
        while (1) {
          while (serial.available()) {
            delay(1);
            fona.write(serial.read());
          }
          if (fona.available()) {
            serial.write(fona.read());
          }
        }
        break;
      }

    default: {
        serial.println(F("Unknown command"));
        printMenu();
        break;
      }
  }
  // flush input
  flushserial();
  while (fona.available()) {
    serial.write(fona.read());
  }

}

void flushserial() {
  while (serial.available())
    serial.read();
}

char readBlocking() {
  while (!serial.available());
  return serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //serial.print(c);
  }
  serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true) {
    if (buffidx > maxbuff) {
      //serial.println(F("SPACE"));
      break;
    }

    while (serial.available()) {
      char c =  serial.read();

      //serial.print(c, HEX); serial.print("#"); serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;

        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }

    if (timeoutvalid && timeout == 0) {
      //serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
