#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include "IMU.c"


#define DT 0.02         // [s/loop] loop period. 20ms
#define AA 0.97         // complementary filter constant

#define A_GAIN 0.0573    // [deg/LSB]
#define G_GAIN 0.070     // [deg/s/LSB]
#define RAD_TO_DEG 57.29578
#define M_PI 3.14159265358979323846
 




void  INThandler(int sig)// Used to do a nice clean exit when Ctrl-C is pressed
{
	signal(sig, SIG_IGN);
	exit(0);
}

int mymillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    return (diff<0);
}

int main(int argc, char *argv[])
{


	float rate_gyr_y = 0.0;   // [deg/s]
	float rate_gyr_x = 0.0;   // [deg/s]
	float rate_gyr_z = 0.0;   // [deg/s]
	
	int value_accelerometer_X, value_accelerometer_Y, value_accelerometer_Z;

	int  accRaw[3];
	int  magRaw[3];
	int  gyrRaw[3];



	float gyroXangle = 0.0;
	float gyroYangle = 0.0;
	float gyroZangle = 0.0;

	int startInt  = mymillis();
	struct  timeval tvBegin, tvEnd,tvDiff;


	signal(SIGINT, INThandler);

	detectIMU();
	enableIMU();

	gettimeofday(&tvBegin, NULL);


	while(1)
	{
		startInt = mymillis();


		//read ACC and GYR data
		readACC(accRaw);
		readGYR(gyrRaw);

		//Convert Gyro raw to degrees per second
		rate_gyr_x = (float) gyrRaw[0] * G_GAIN;
		rate_gyr_y = (float) gyrRaw[1]  * G_GAIN;
		rate_gyr_z = (float) gyrRaw[2]  * G_GAIN;



		//Calculate the angles from the gyro
		gyroXangle+=rate_gyr_x*DT;
		gyroYangle+=rate_gyr_y*DT;
		gyroZangle+=rate_gyr_z*DT;



		//Get raw values of accelerometer
		
		value_accelerometer_X=accRaw[0];
		value_accelerometer_Y=accRaw[1];
		value_accelerometer_Z=accRaw[2];


		printf ("rate_gyr_x: %f,rate_gyr_y: %f ,rate_gyr_z: %f ,value_accelerometer_X: %d ,value_accelerometer_Y: %d ,value_accelerometer_Z: %d ",rate_gyr_x,rate_gyr_y,rate_gyr_z,value_accelerometer_X,value_accelerometer_Y,value_accelerometer_Z);

		//Each loop should be at least 20ms.
		while(mymillis() - startInt < (DT*1000)){
				usleep(100);
		}

		printf("Loop Time %d\t", mymillis()- startInt);
    }
}
