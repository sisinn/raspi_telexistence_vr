#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <PacketSerial.h>

#define DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

const int buttonYellow = 2;
const int buttonRed    = 4;
const int buttonRight  = 7;
const int buttonDown   = 8;
const int buttonLeft   = 12;
const int buttonUp     = 13;

int reset_flag = 0;
int chatter_cnt = 0;

SLIPPacketSerial myPacketSerial;

void setup(void)
{
    uint8_t tempBuffer[128];

    pinMode(buttonRed,      INPUT_PULLUP);
    pinMode(buttonYellow,   INPUT_PULLUP);
    pinMode(buttonLeft,     INPUT_PULLUP);
    pinMode(buttonDown,     INPUT_PULLUP);
    pinMode(buttonUp,       INPUT_PULLUP);
    pinMode(buttonRight,    INPUT_PULLUP);

    myPacketSerial.begin(115200);

    sprintf(tempBuffer, "\n3Dof Parameters\n");
    myPacketSerial.send(tempBuffer, strlen(tempBuffer));

    if( !bno.begin(bno.OPERATION_MODE_IMUPLUS) ) {
        sprintf(tempBuffer, "No BNO055 detected");
        myPacketSerial.send(tempBuffer, strlen(tempBuffer));
        while(1);
    }

    delay(1000);
    reset_flag = 1;
    chatter_cnt = 0;
}

bool readLen(uint8_t reg, byte * buffer, uint8_t len)
{
  Wire.beginTransmission(BNO055_ADDRESS_A);
  #if ARDUINO >= 100
    Wire.write((uint8_t)reg);
  #else
    Wire.send(reg);
  #endif
  Wire.endTransmission();
  Wire.requestFrom(BNO055_ADDRESS_A, (byte)len);

  for (uint8_t i = 0; i < len; i++)
  {
    #if ARDUINO >= 100
      buffer[i] = Wire.read();
    #else
      buffer[i] = Wire.receive();
    #endif
  }

  /* ToDo: Check for errors! */
  return true;
}

void loop(void)
{
    uint8_t tempBuffer[128];
    int data_size = 0;
    uint8_t reg_buffer[8];

    //imu::Quaternion quat = bno.getQuat();
    memset (reg_buffer, 0, 8);
    /* Read quat data (8 bytes) */
    readLen(0x20, reg_buffer, 8);   //0x20=BNO055_QUATERNION_DATA_W_LSB_ADDR

    // Botton push = Lov Voltage
    if(digitalRead(buttonRed) == LOW){
        sprintf(tempBuffer, "BNO Reset\n");
        myPacketSerial.send(tempBuffer, strlen(tempBuffer));

        bno.begin(bno.OPERATION_MODE_IMUPLUS);

        delay(1000);
        reset_flag = 1;
        chatter_cnt = 0;
        return;
    }

    if(chatter_cnt != 0){
        chatter_cnt += DELAY_MS;
        if(chatter_cnt >= 300){
            chatter_cnt = 0;
        }
    }

    sprintf(tempBuffer, "quat=");
    data_size = 5;
    memcpy(&(tempBuffer[data_size]),reg_buffer,8);
    data_size += 8;

    tempBuffer[data_size] = reset_flag;
    data_size += 1;
    if(reset_flag == 1){
        reset_flag = 0;
    }

    // Botton push = Lov Voltage
    if(digitalRead(buttonYellow) == LOW){
        if(chatter_cnt == 0){
            tempBuffer[data_size] = 1;
            data_size += 1;
            chatter_cnt += DELAY_MS;
        }
        else {
            tempBuffer[data_size] = 0;
            data_size += 1;
        }
    }
    else {
        tempBuffer[data_size] = 0;
        data_size += 1;
    }

    // Botton push = Lov Voltage
    if(digitalRead(buttonLeft) == LOW){
        if(chatter_cnt == 0){
            tempBuffer[data_size] = 1;
            data_size += 1;
            chatter_cnt += DELAY_MS;
        }
        else {
            tempBuffer[data_size] = 0;
            data_size += 1;
        }
    } 
    else {
        tempBuffer[data_size] = 0;
        data_size += 1;
    }
    if(digitalRead(buttonDown) == LOW){
        if(chatter_cnt == 0){
            tempBuffer[data_size] = 1;
            data_size += 1;
            chatter_cnt += DELAY_MS;
        }
        else {
            tempBuffer[data_size] = 0;
            data_size += 1;
        }
    }
    else {
        tempBuffer[data_size] = 0;
        data_size += 1;
    }
    if(digitalRead(buttonUp) == LOW){
        if(chatter_cnt == 0){
            tempBuffer[data_size] = 1;
            data_size += 1;
            chatter_cnt += DELAY_MS;
        }
        else {
            tempBuffer[data_size] = 0;
            data_size += 1;
        }
    }
    else {
        tempBuffer[data_size] = 0;
        data_size += 1;
    }
    if(digitalRead(buttonRight) == LOW){
        if(chatter_cnt == 0){
            tempBuffer[data_size] = 1;
            data_size += 1;
            chatter_cnt += DELAY_MS;
        }
        else {
            tempBuffer[data_size] = 0;
            data_size += 1;
        }
    } 
    else {
        tempBuffer[data_size] = 0;
        data_size += 1;
    }

    tempBuffer[data_size] = 0xFF;
    data_size += 1;
    tempBuffer[data_size] = '\n';
    data_size += 1;
    myPacketSerial.send(tempBuffer, data_size);

    delay(DELAY_MS);
}
