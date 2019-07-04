//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "mcbelib.hpp"

namespace mcbelib {

    ///Read a register
    /**Returns the value of an given register address*/
    uint8_t gy521::registerRead( const uint8_t &regAddress ) {
        uint8_t new_data;
        bus.i2c_bus::write( address ).write( regAddress );
        bus.i2c_bus::read( address ).read( new_data );

        return new_data;
    }

    ///Read a full data piece
    /**Returns the value of two given register addresses in a const uint8_t array,
     * this needs to get the High register before the Low register */
    int16_t gy521::readBuffer( const uint8_t* package ) {  
        return registerRead( package[0] ) << 8 | registerRead( package[1] );
    }

    ///Set a register value
    /**Writes the a value given in an const uint8_t array to a given register address,
     * this needs to get the Register address before the value that it's gonna be set to.*/
    void gy521::registerWrite( const uint8_t* package ) {
        bus.i2c_bus::write( address ).write( package, 2 );
    }

    ///Turn the clock register on
    /**Sets the clock register to 0, this has function has to be called to acces the chip*/
    void gy521::activate() {
        uint8_t data[2] = { PWR_MGMT_1, 0 };
        registerWrite( data );
    }

    ///Read accelerometer value X
    /**Returns the full data of the X-axis*/
    int16_t gy521::getAccelX() {
        uint8_t data[2] = { ACCEL_XOUT_H, ACCEL_XOUT_L };
        return readBuffer( data ) / calibrateAccel;
    }

    ///Read accelerometer value Y
    /**Returns the full data of the Y-axis*/
    int16_t gy521::getAccelY() {
        uint8_t data[2] = { ACCEL_YOUT_H, ACCEL_YOUT_L };
        return readBuffer( data ) / calibrateAccel;
    }

    ///Read accelerometer value Z
    /**Returns the full data of the Z-axis*/
    int16_t gy521::getAccelZ() {
        uint8_t data[2] = { ACCEL_ZOUT_H, ACCEL_ZOUT_L };
        return readBuffer( data ) / calibrateAccel;
    }

    ///Read gyroscope value X
    /**Returns the full data of the X-axis*/
    int16_t gy521::getGyroX() {
        uint8_t data[2] = { GYRO_XOUT_H, GYRO_XOUT_L };
        return readBuffer( data ) / calibrateGyro;
    }

    ///Read gyroscope value Y
    /**Returns the full data of the Y-axis*/
    int16_t gy521::getGyroY() {
        uint8_t data[2] = { GYRO_YOUT_H, GYRO_YOUT_L };
        return readBuffer( data ) / calibrateGyro;
    }

    ///Read gyroscope value Z
    /**Returns the full data of the Z-axis*/
    int16_t gy521::getGyroZ() {
        uint8_t data[2] = { GYRO_ZOUT_H, GYRO_ZOUT_L };
        return readBuffer( data ) / calibrateGyro;
    }

    ///Read temperature
    /**Returns the full data of the temperature sensor*/
    int16_t gy521::getTemperature() {
        uint8_t data[2] = { TEMP_OUT_H, TEMP_OUT_L };
        return readBuffer( data ) / 340 + 36.53;
    }

    ///Read where controller is facing to to
    /**Returns an char with the direction the controller is facing to,
     * only used in the HC-05 Class */ 
    char gy521::getWayPointing( bool convertX, bool convertY ) {
        int accelX = getAccelX();
        int accelY = getAccelY();
        bool nothingX = 0, nothingY = 0;
        
        //change axIs so it's the correct way on screen
        if (convertX) {
            accelX = accelX - (accelX *2);
        }
        if (convertY) {
            accelY = accelY - (accelY *2);
        }

        //simplefy the axis X
        if (accelX > DISPLAY_MIN_MOVE) {
            accelX = 1;
        } else if (accelX < DISPLAY_MIN_MOVE * -1) {
            accelX = 0;
        } else {
            accelX = 0;
            nothingX = 1;
        }

        //simplefy the axis Y
        if (accelY > DISPLAY_MIN_MOVE) {
            accelY = 1;
        } else if (accelY < DISPLAY_MIN_MOVE * -1) {
            accelY = 0;
        } else {
            accelY = 0;
            nothingY = 1;
        }

        //put everything in 1 byte return
        uint8_t data;
        data  = accelX << 7;
        data |= nothingX << 6;
        data |= accelY << 5;
        data |= nothingY << 4;
        return data;
    }

    ///Test the Gyroscope
    /**This will test the Gyroscope if it returns the correct value
     */
    bool gy521::testGyroscope() {
        return (registerRead(WHO_AM_I) == WHO_AM_I_RESPONSE);
    }

    

}// namespace mcbelib