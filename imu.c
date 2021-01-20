#include <stdint.h>
#include "i2c-dev.h"
#include "LSM6DSL.h"


int file;


void  readBlock(uint8_t command, uint8_t size, uint8_t *data)
{
    int result = i2c_smbus_read_i2c_block_data(file, command, size, data);
    if (result != size){
		printf("Failed to read block from I2C.");
		exit(1);
	}
}

void selectDevice(int file, int addr)
{
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		 printf("Failed to select I2C device.");
	}
}


void readACC(int  a[])
{
	uint8_t block[6];
	
	selectDevice(file,LSM6DSL_ADDRESS);
	readBlock(LSM6DSL_OUTX_L_XL, sizeof(block), block); 
	
	// Combine readings for each axis.
	a[0] = (int16_t)(block[0] | block[1] << 8);
	a[1] = (int16_t)(block[2] | block[3] << 8);
	a[2] = (int16_t)(block[4] | block[5] << 8);
}


void readGYR(int g[])
{
	uint8_t block[6];
   
	selectDevice(file,LSM6DSL_ADDRESS);
	readBlock(LSM6DSL_OUTX_L_G, sizeof(block), block);   
	
	// Combine readings for each axis.
	g[0] = (int16_t)(block[0] | block[1] << 8);
	g[1] = (int16_t)(block[2] | block[3] << 8);
	g[2] = (int16_t)(block[4] | block[5] << 8);
}


void writeAccReg(uint8_t reg, uint8_t value)
{
	
	selectDevice(file,LSM6DSL_ADDRESS);

	int result = i2c_smbus_write_byte_data(file, reg, value);
	if (result == -1)
	{
		printf ("Failed to write byte to I2C Acc.");
        exit(1);
    }
}

void writeGyrReg(uint8_t reg, uint8_t value)
{
	selectDevice(file,LSM6DSL_ADDRESS);

	int result = i2c_smbus_write_byte_data(file, reg, value);
	if (result == -1)
	{
		printf("Failed to write byte to I2C Gyr.");
		exit(1);
	}
}



void detectIMU()
{

	__u16 block[I2C_SMBUS_BLOCK_MAX];

	int res, bus,  size;


	char filename[20];
	sprintf(filename, "/dev/i2c-%d", 1);
	file = open(filename, O_RDWR);
	if (file<0) 
	{
		printf("Unable to open I2C bus!");
			exit(1);
	}

	//Detect if LSM6DSL is connected
	selectDevice(file,LSM6DSL_ADDRESS);
	int LSM6DSL_WHO_M_response = i2c_smbus_read_byte_data(file, LSM6DSL_WHO_AM_I);

	selectDevice(file,LIS3MDL_ADDRESS);	
	int LIS3MDL_WHO_XG_response = i2c_smbus_read_byte_data(file, LIS3MDL_WHO_AM_I);

	if ( LSM6DSL_WHO_M_response == 0x6A && LIS3MDL_WHO_XG_response == 0x3D){
		printf ("\n\n\n#####   BerryIMUv3  DETECTED    #####\n\n");
		BerryIMUversion = 3;
	}

	sleep(1);
}

void enableIMU()
{
		//Enable  gyroscope
		writeGyrReg(LSM6DSL_CTRL2_G,0b10011100);        // ODR 3.3 kHz, 2000 dps

		// Enable the accelerometer
		writeAccReg(LSM6DSL_CTRL1_XL,0b10011111);       // ODR 3.33 kHz, +/- 8g , BW = 400hz
		writeAccReg(LSM6DSL_CTRL8_XL,0b11001000);       // Low pass filter enabled, BW9, composite filter
		writeAccReg(LSM6DSL_CTRL3_C,0b01000100);        // Enable Block Data update, increment during multi byte read

		//Enable  magnetometer
		writeMagReg(LIS3MDL_CTRL_REG1, 0b11011100);     // Temp sesnor enabled, High performance, ODR 80 Hz, FAST ODR disabled and Selft test disabled.
		writeMagReg(LIS3MDL_CTRL_REG2, 0b00100000);     // +/- 8 gauss
		writeMagReg(LIS3MDL_CTRL_REG3, 0b00000000);     // Continuous-conversion mode
	}


}