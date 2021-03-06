/*
  RFIDDB.h - HackPittsburgh RFID System DB
  Author: Marty McGuire <marty@hackpittsburgh.org>, Nov. 2009
  Released under CC3.0-BY-SA - http://creativecommons.org/licenses/by-sa/3.0/
  */

#include "WProgram.h"
#include "RFIDDB.h"
#include "../EEPROM/EEPROM.h"

RFIDDB::RFIDDB()
{
  numTags = EEPROM.read(0);
  if((numTags == 0) || (numTags > MAX_TAGS)){
    Serial.print("Bogus number of tags reported: ");
    Serial.println(numTags, DEC);
    numTags = 0;
  }
}

int RFIDDB::getNumTags()
{
  return numTags;
}

void RFIDDB::printTags()
{
  Serial.print("Printing ");
  Serial.print(numTags);
  Serial.println(" tags.");
  for(int i = 0; i < numTags; i++){
    Serial.print("Tag #");
    Serial.print(i + 1);
    Serial.print(": ");
    for(int j = 0; j < TAG_LENGTH; j++){
      char buffer;
      buffer = (char) EEPROM.read(i*TAG_LENGTH+j+1);
      Serial.print(buffer, BYTE);
    }
    Serial.println();
  }
}

void RFIDDB::readTags()
{
  char inByte;
  int done = 0;
  numTags = -1;
  int currTag = 0;
  int idx = 0;
  while(!done){
    if(Serial.available() > 0){
      inByte = Serial.read();
      // get the total number of records, if we don't already have it
      if(numTags == -1){
        numTags = (int) inByte;
        Serial.print("Expecting ");
        Serial.print(numTags, DEC);
        Serial.println(" tags.");
        EEPROM.write(0, numTags);
        if(numTags == 0) { done = 1; } // no tags to write
      } else {
        // otherwise, put this byte wherever it goes
        EEPROM.write(currTag * TAG_LENGTH + idx + 1, inByte); // EEPROM
        // then, increment counters and/or set that we're done
        idx++;
        if(idx >= TAG_LENGTH){
          Serial.print("Wrote ID #: ");
          Serial.println(currTag + 1);
          idx = 0;
          currTag++;
          if(currTag >= numTags){
            done = 1;
          }
        }
      }
    }
  }
  Serial.println("Finished!");
}

bool RFIDDB::validTag(char* tag){
  bool ok = true;
  for(int i = 0; i < numTags; i++){
#if 0
    Serial.print("reading tag_no=");
    Serial.print(i);
    Serial.print(" id=");
#endif
    ok = true;
    for(int j = 0; j < TAG_LENGTH; j++){
      char buffer;
      buffer = (char) EEPROM.read(i*TAG_LENGTH+j+1);
#if 0
      Serial.print(buffer);
#endif
      if(tag[j] != buffer) {
        ok = false;
      }
    }
#if 0
    Serial.print("\n");
#endif
    if(ok) { return true; }
  }
  return false;
}
