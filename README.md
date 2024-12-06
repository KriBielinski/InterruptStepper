# InterruptStepper

An Arduino stepper library for the Arduino Due board that can run stepper motors using timer interrupts.

This is a modification of the popular AccelStepper library that runs the motors using timer interrupts instead of polling them in the main loop. This allows for more precise step timings and running the motors at significantly higher speeds (even up to 30 000 steps/s).

Running stepper motors using interrupts also means that they can run independently of the main program loop. Even if there is a computationally expensive operaration being performed in the main loop, it will have no negative impact on the currently running steppers.

## Dependencies

The InterruptStepper library uses the following libraries:

- [AccelStepper](https://github.com/waspinator/AccelStepper)
- [PrecDueTimer](https://github.com/KriBielinski/PrecDueTimer)

The AccelStepper library is used for step timing calculations as well as providing most of the API for this library. InterruptStepper class in fact inherits from the AccelStepper class. The PrecDueTimer library on the other hand provides the functionality necessary for the timer interrupts.

> [!NOTE]
> PrecDueTimer library can also be used in your project for user defined timer interrupts that are independent of the stepper motors. For more information see the [PrecDueTimer library documentation](https://github.com/KriBielinski/PrecDueTimer).

## Installation

InterruptStepper library can be downloaded from the Arduino Library Manager. If installed this way, then the above dependencies should be installed automatically.

Alternatively you can download this library manually by following the below tutorial, but then you would need to make sure that the above dependencies are already installed:

https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/#importing-a-zip-library

## Using the library

### Simple example

First include the library in your project:

```c++
#include <InterruptStepper.h>
```

Then create the InterruptStepper object that will run the stepper motor  (for the explanation of the constructor arguments see the [Library Reference](#library-reference)):

```c++
void updateFunc() {}

InterruptStepper stepper(Timer1, updateFunc, InterruptStepper::DRIVER, 2, 3);
```

In the setup function you need to put this special line of code that will initialize the stepper:

```c++
void setup() {
  stepper.attachInterrupt([](){ stepper.stepInterrupt(); });
}
```

Then in the main loop place the code to move the motor. Below is a simple example that will move the stepper between positions 0 and 3000 (for the full example code see [Example](examples/Simple/Simple.ino)):

```c++
void loop() {
  // Move the stepper constantly between the positions 0 and 3000
  if(stepper.distanceToGo()==0) {
    if(stepper.currentPosition() >= 3000) {
      stepper.moveTo(0);
      Serial.println("Move to 0");
    }
    else {
      stepper.moveTo(3000);
      Serial.println("Move to 3000");
    }
  }
}
```

> [!WARNING]
> Remember that unlike in the original AccelStepper library you should **NOT** call the `run()` or `runSpeed()` methods in the main loop (or anywhere in your program).

You can see the list of all the available methods for moving the motor in the [Library Reference](#library-reference) 

## Library Reference

Most of this library's API is taken directly from the AccelStepper library. You can see more information about the available methods in the original [AccelStepper Documentation](http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html).

Here are the available constructors for the InterruptStepper class and its' parameters:

- ```c++
  InterruptStepper( PrecDueTimer& timer, 
                    void (&update_func)(), 
                    uint8_t interface = InterruptStepper::FULL4WIRE, 
                    uint8_t pin1 = 2, 
                    uint8_t pin2 = 3, 
                    uint8_t pin3 = 4, 
                    uint8_t pin4 = 5, 
                    bool enable = true) 
  ```

  It takes the following arguments:

  - `PrecDueTimer& timer` - A timer for the interrupt timing purposes. There are 9 timers defined globally: `Timer0`, `Timer1`, ... , `Timer8` . Note that each stepper motor you wish to run needs to be assigned it's own unique timer.
  - `void (&update_func)()` - A user defined update function that takes no parameters and returns no value. This function will be run each time the stepper motor performs a step.
  - `uint8_t interface` - Number of pins to interface to. Available options     are: `DRIVER`, `FULL2WIRE`, `FULL3WIRE`, `FULL4WIRE`, `HALF3WIRE` and `HALF4WIRE`. See [MotorInterfaceType](http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a73bdecf1273d98d8c5fbcb764cabeea5) for more details.
  - `uint8_t pin1, pin2, pin3, pin4` - Arduino digital pin numbers that will run the stepper motor. For more information see [AccelStepper()](http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a3bc75bd6571b98a6177838ca81ac39ab).
  - `bool enable` - If this is true (the default), then the output pins will be enabled at construction time.

<br/>

- ```c++
  InterruptStepper( PrecDueTimer& timer,
                    void (&update_func)(),
                    void (*forward)(), 
                    void (*backward)())
  ```
  It takes the following arguments:

  - `PrecDueTimer& timer` - See above.
  - `void (&update_func)()` - See above.
  - `void (*forward)()` - A custom function which performs a forward step.
  - `void (*backward)()` - A custom function which performs a backward step.

<br/>

Below is a list of all the methods available in the InterruptStepper library and a shortened description of what each one does:

  - `void moveTo(long absolute)` - Move to a target position.
  - `void move(long relative)` - Move to a position relative from the current position.
  - `void setMaxSpeed(float speed)` - Sets the maximum permitted speed.
  - `float maxSpeed()` - Returns the maximum speed configured for this stepper.
  - `void setAcceleration(float acceleration)` - Sets the acceleration/deceleration rate for this stepper.
  - `float acceleration()` - Returns the acceleration/deceleration rate configured for this stepper.
  - `float speed()` - Returns the current speed of the motor.
  - `long distanceToGo()` - Returns the distance from the current position to the target position.
  - `long targetPosition()` - Returns the most recently set target position.
  - `long currentPosition()` - Returns the current position of the motor.
  - `void setCurrentPosition(long position)` - Sets the current position of the motor to the provided value. Should only be called when the motor is     stationary to set an initial position, for example, after an initial hardware positioning move.
  - `void stop()` - Sets a new target position that causes the stepper to stop as quickly as possible, using the current speed and acceleration parameters.
  - `bool isRunning()` - Checks to see if the motor is currently running to a target.
  - `bool direction()` - Returnes the direction the motor is currently spinning in. Value of 1 means clockwise. If the motor is stationary, then
  the output of this method is undefined.

## Miscellaneous information

- The InterruptStepper library provides a protected virtual method `uint32_t getNextInterval()` that is used internally to time steps.

  This method gets called every step and returns the time (in microseconds) until the next step should occur. A return value of 0 indicates that the motor should stop.

  By default this method uses the AccelStepper library to calculate those timings but you can override this method to provide your own step timing implementation.