#include <AccelStepper.h>
#include <Servo.h>
#include <math.h>

// Globals
#define BUFFER_SIZE 10

char buffer[BUFFER_SIZE];
char command;

size_t curr_idx = 0;

// Switchs
#define limitSwitch1 11
#define limitSwitch2 10
#define limitSwitch3 9
#define limitSwitch4 A3

// Steppers
AccelStepper stepper1(1, 2, 5);
AccelStepper stepper2(1, 3, 6);
AccelStepper stepper3(1, 4, 7);
AccelStepper stepper4(1, 12, 13);

// Gripper
Servo gripper;
int gripper_open = 100, gripper_close = 0; // Angles

void setup()
{
	Serial.begin(115200);

	gripper.attach(A0);

	pinMode(limitSwitch1, INPUT_PULLUP);
	pinMode(limitSwitch2, INPUT_PULLUP);
	pinMode(limitSwitch3, INPUT_PULLUP);
	pinMode(limitSwitch4, INPUT_PULLUP);

	// Stepper motors max speed
	stepper1.setMaxSpeed(4000);
	stepper1.setAcceleration(2000);
	stepper2.setMaxSpeed(4000);
	stepper2.setAcceleration(2000);
	stepper3.setMaxSpeed(4000);
	stepper3.setAcceleration(2000);
	stepper4.setMaxSpeed(4000);
	stepper4.setAcceleration(2000);

	calibration();
}

void loop()
{
	if (Serial.available() > 0)
	{
		command = Serial.read();

		if (command != 'N')
		{
			buffer[curr_idx++] = command;

			if (curr_idx == BUFFER_SIZE)
			{
				for (int i = 0; i < BUFFER_SIZE; ++i)
				{
					switch (buffer[i])
					{
					case 'O':
						move_gripper(gripper_open);
						break;
					case 'C':
						move_gripper(gripper_close);
						break;
					case 'U':
						move_stepper(&stepper4, 15000);
						break;
					case 'D':
						move_stepper(&stepper4, 5000);
						break;
					case 'L':
						move_stepper(&stepper1, -3500);
						break;
					case 'R':
						move_stepper(&stepper1, 0);
						break;
					}
				}
				curr_idx = 0;
			}
		}
		else
		{
			for (int i = 0; i < BUFFER_SIZE; ++i)
			{
				switch (buffer[i])
				{
				case 'O':
					move_gripper(gripper_open);
					break;
				case 'C':
					move_gripper(gripper_close);
					break;
				case 'U':
					move_stepper(&stepper4, 15000);
					break;
				case 'D':
					move_stepper(&stepper4, 5000);
					break;
				case 'L':
					move_stepper(&stepper1, -3500);
					break;
				case 'R':
					move_stepper(&stepper1, 0);
					break;
				}
			}
			curr_idx = 0;
		}
	}
}

void calibration()
{
	while (digitalRead(limitSwitch4) != 1)
	{
		stepper4.setSpeed(1500);
		stepper4.runSpeed();
		stepper4.setCurrentPosition(17000);
	}

	delay(20);

	stepper4.moveTo(10000);
	while (stepper4.currentPosition() != 10000)
		stepper4.run();

	delay(20);

	while (digitalRead(limitSwitch1) != 1)
	{
		stepper1.setSpeed(-1200);
		stepper1.runSpeed();
		stepper1.setCurrentPosition(-4000);
	}

	delay(20);

	stepper1.moveTo(-2000);
	while (stepper1.currentPosition() != -2000)
		stepper1.run();
}

void move_gripper(int gripper_angle)
{
	gripper.write(gripper_angle);
}

void move_stepper(AccelStepper *stepper, int steps)
{
	stepper->moveTo(steps);

	while (stepper->currentPosition() != steps)
		stepper->run();
}