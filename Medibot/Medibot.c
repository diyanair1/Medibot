
void configureSensors(tSensors ultrasonicPort, tSensors colourPort, tSensors
gyroPort, tSensors soundPort)
{
	// Configures The Sensors
	SensorType[ultrasonicPort] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[colourPort] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[colourPort] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[gyroPort] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[gyroPort] = modeEV3Gyro_Calibration;
	wait1Msec(50);
	SensorMode[gyroPort] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[soundPort] = sensorSoundDB;
	wait1Msec(50);
}

void drive(int motor_power)
{
	motor[motorA] = motor[motorD] = motor_power;
}

void red_light(int motorPower)
{
	// Record Time
	clearTimer(T1);
	// Stop For 3 Seconds
	motor[motorA] = motor[motorD] = 0;
	while (time1[T1] < 3000.0)
	{}
	// Continue Driving
	drive(motorPower);
}


void yellow_light(int motorPower)
{
		// Record Time
		clearTimer(T1);
		// Slow Down For 3 Seconds
		drive(25);
		while (time1[T1] < 3000.0)
		{}
		// Return To Regular Speed
		drive(motorPower);
}


void check_colour(int motorPower)
{
	if (SensorValue[S3] == (int)colorRed)
	{
		red_light(motorPower);
	}
	else if (SensorValue[S3] == (int)colorYellow)
	{
		yellow_light(motorPower);
	}
}

void rotate(int angle)
{
	nMotorEncoder[motorA] = 0;
	motor[motorA] = 50;
	motor[motorD] = -50;
	while(abs(nMotorEncoder[motorA]) < angle)
	{}
	motor[motorA] = motor[motorD] = 0;
}


void navigate(int motorPower, tSensors ultrasonicPort)
{
	// Rotate Until There Is At Least 50 cm Of Space
	while(SensorValue[ultrasonicPort] < 50)
	{}
	motor[motorA] = motorPower;
	motor[motorD] = -1 * motorPower;
}


int find_open_path(tSensors ultrasonicPort)
{
	nMotorEncoder[motorB] = 0;
	motor[motorB] = 25;
	while(SensorValue[ultrasonicPort] > 20)
	{}
	motor[motorB] = 0;
	return abs(nMotorEncoder[motorB]);
}


void align_body(int turn_angle)
{
}
	

bool check_for_emergency(tSensors soundPort)
{
	if(SensorValue[soundPort] > 65)
	{
		return true;
	}
		return false;
}


void check_for_wall(int motorPower, tSensors ultrasonicPort)
{
	if (SensorValue[ultrasonicPort] < 20)
	{
		drive(0);
		navigate(motorPower-20, ultrasonicPort);
		drive(motorPower);
	}
}


void lift_arm(int angle)
{
	motor[motorC] = 50;
	while (abs(nMotorEncoder[motorC]) < angle)
	{}
	motor[motorC] = 0;
}

	

void lower_arm(int angle)
{
	motor[motorC] = -50;
	while(abs(nMotorEncoder[motorC]) < angle)
	{}
	motor[motorC] = 0;
}


void pick_up_first_aid()
{
	rotate(180);
	lower_arm(30);
	wait1Msec(500);
	drive(50);
	wait1Msec(500);
	drive(0);
	lift_arm(30);
	wait1Msec(500);
	rotate(180);
}

task main()
{
	tSensors ultrasonicPort = S1, colourPort = S2, touchPort = S3, soundPort = S4;
	configureSensors(ultrasonicPort, colourPort, touchPort, soundPort);
	// Initializes Emergency Mode To False
	bool emergency = false;
	while(SensorValue[colourPort] != (int)colorGreen)
	{
		displayBigStringAt(50, 60, "MediBot!");
		// Wait For Enter Button Press And Release
		while(!getButtonPress(buttonEnter))
		{}
		while(getButtonPress(buttonAny))
		{}
		int motorPower = 50;
		drive(motorPower);
		check_for_wall(motorPower, ultrasonicPort);
		emergency = check_for_emergency(soundPort);
		// Checks For Traffic Lights When Not An Emergency
		if (!emergency)
		{
			check_colour(motorPower);
		}
	}
	// Stop The Robot
	drive(0);
	pick_up_first_aid();
}