/**
 * Credits: http://www.lucidarme.me/?p=5057
 * 
 * https://github.com/NelisW/myOpenHab/blob/master/docs/707-MPU-9250-9265%20IMU.md
 * 
 * Wiring:
 * 
 * SCL - SCL
 * SDA - SDA
 * GND - GND
 * VCC - 3.3 v
 * 
 */

#include <Wire.h>

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18



// This function read Nbytes bytes from I2C device at address Address. 
// Put read bytes starting at register Register in the Data array. 
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();
  
  // Read Nbytes
  Wire.requestFrom(Address, Nbytes); 
  uint8_t index=0;
  while (Wire.available())
    Data[index++]=Wire.read();
}


// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}


// Initializations
void setupsensors()
{
  // Arduino initializations
  Wire.begin();
  
  // Configure gyroscope range
  I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_2000_DPS);
  // Configure accelerometers range
  I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_16_G);
  // Set by pass mode for the magnetometers
  I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
  
  // Request first magnetometer single measurement
  I2CwriteByte(MAG_ADDRESS,0x0A,0x02);


}


long int cpt=0;
// Main loop, read and display data
void loopsensors()
{
  
  // _______________
  // ::: Counter :::

  if (cpt>256)
  {
    cpt=0;
  }

  if (debug)
  {
    // Display data counter
    Serial.print (cpt++,DEC);
    Serial.print ("\t");
  }  
 
  
  // ____________________________________
  // :::  accelerometer and gyroscope ::: 

  // Read accelerometer and gyroscope
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
  
  
  // Create 16 bits values from 8 bits data
  
  // Accelerometer
  int16_t ax=-(Buf[0]<<8 | Buf[1]);
  int16_t ay=-(Buf[2]<<8 | Buf[3]);
  int16_t az=Buf[4]<<8 | Buf[5];

  // Gyroscope
  int16_t gx=-(Buf[8]<<8 | Buf[9]);
  int16_t gy=-(Buf[10]<<8 | Buf[11]);
  int16_t gz=Buf[12]<<8 | Buf[13];
  
    // Display values

  if (debug) 
  {
    // Accelerometer
    Serial.print (ax,DEC); 
    Serial.print ("\t");
    Serial.print (ay,DEC);
    Serial.print ("\t");
    Serial.print (az,DEC);  
    Serial.print ("\t");
    
    // Gyroscope
    Serial.print (gx,DEC); 
    Serial.print ("\t");
    Serial.print (gy,DEC);
    Serial.print ("\t");
    Serial.print (gz,DEC);  
    Serial.print ("\t");
  }

  
  // _____________________
  // :::  Magnetometer ::: 

  
  // Read register Status 1 and wait for the DRDY: Data Ready


  // DEmora mucho
  int16_t mx=0;
  int16_t my=0;
  int16_t mz=0; 

  int breakcounter=10;
  
  if (compassenabled)
  {
    uint8_t ST1;
    do
    {
      I2Cread(MAG_ADDRESS,0x02,1,&ST1);
      if ((breakcounter--)<=1)
        {
          break;
        }
    }
    while (!(ST1&0x01));
  
    // Read magnetometer data  
    uint8_t Mag[7];  
    I2Cread(MAG_ADDRESS,0x03,7,Mag);
    
  
    // Create 16 bits values from 8 bits data
    
    // Magnetometer
    mx=-(Mag[3]<<8 | Mag[2]);
    my=-(Mag[1]<<8 | Mag[0]);
    mz=-(Mag[5]<<8 | Mag[4]);
  }

  if (debug)
  {
    // Magnetometer
    Serial.print (mx+200,DEC); 
    Serial.print ("\t");
    Serial.print (my-70,DEC);
    Serial.print ("\t");
    Serial.print (mz-700,DEC);  
    Serial.print ("\t");
  }

  sensor.acx = ax;
  sensor.acy = ay;
  sensor.acz = az;

  sensor.gx = gx;
  sensor.gy = gy;
  sensor.gz = gz;

  sensor.mx = mx+200;
  sensor.my = my-70;
  sensor.mz = mz-70;
  
  if (debug)
  {
    // End of line
    Serial.println("");
  }
  //delay(100);    
}
