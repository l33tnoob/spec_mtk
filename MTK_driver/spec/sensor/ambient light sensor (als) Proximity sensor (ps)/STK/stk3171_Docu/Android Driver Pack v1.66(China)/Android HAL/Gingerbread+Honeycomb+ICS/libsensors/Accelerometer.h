/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>


#include "sensors_poll_impl.h"
#include "SensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/
#define ACCELEROMETER_NAME          "accelerometer"
#define ACC_DEVICE_PATH			 "/dev/stk831x"

// coordinate transformation: direction
#define EVENT_TYPE_ACCEL_X          ABS_X
#define EVENT_TYPE_ACCEL_Y          ABS_Y
#define EVENT_TYPE_ACCEL_Z          ABS_Z


// coordinate transformation: sign
// conversion of acceleration data to SI units (m/s^2)
#ifdef IS_STK8311
	#define CONVERT_A                   (GRAVITY_EARTH / 256.0)	//stk8311
#elif defined IS_STK8312
	#define CONVERT_A                   (GRAVITY_EARTH / 21.34)	//stk8312
#endif	//#ifdef IS_STK8311
#define CONVERT_A_X                 (CONVERT_A)
#define CONVERT_A_Y                 (CONVERT_A)
#define CONVERT_A_Z                 (CONVERT_A)

struct input_event;

class Accelerometer : public SensorBase {
public:
            Accelerometer();
    virtual ~Accelerometer();


    int setInitialState();

private:
    uint32_t mEnabled;
    bool mHasPendingEvent;
    InputEventCircularReader mInputReader;
    sensors_event_t mPendingEvent;
    void processEvent(int code, int value);

public:
    virtual int readEvents(sensors_event_t* data, int count);
    virtual bool hasPendingEvents() const;
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int enable(int32_t handle, int enabled);


};

/*****************************************************************************/

#endif  // ACCELEROMETER_H
