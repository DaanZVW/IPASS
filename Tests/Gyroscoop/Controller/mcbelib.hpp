//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#ifndef GYRO_HPP
#define GYRO_HPP
//===========LIBS===========
#include "hwlib.hpp"
//===========GYRO===========
#define GYRO_ADDRESS 0x68
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75
#define WHO_AM_I_RESPONSE 104




namespace mcbelib {

    ///GY-521 (gyroscoop) class
    /** This class is for the interface of the gyroscope */
    class gy521 {

    private:

        hwlib::i2c_bus & bus;
        uint8_t address; 
        int calibrateAccel;
        int calibrateGyro;

        uint8_t registerRead( const uint8_t &regAddress );
        int16_t readBuffer( const uint8_t* package );
        void registerWrite( const uint8_t* package );

    public:

        gy521( 
            hwlib::i2c_bus & bus,
            uint8_t address = GYRO_ADDRESS,
            int calibrateAccel = 1,
            int calibrateGyro = 1
        ):
            bus( bus ),
            address( address ),
            calibrateAccel( calibrateAccel ),
            calibrateGyro( calibrateGyro )
        {}  

        void activate();

        //accelerometer
        int16_t getAccelX();
        int16_t getAccelY();
        int16_t getAccelZ();

        //gryoscoop
        int16_t getGyroX();
        int16_t getGyroY();
        int16_t getGyroZ();

        //temperatuur
        int16_t getTemperature();

        //bluetooth
        char getWayPointing( bool convertX = false, bool convertY = false );
        
        //test
        bool testGyroscope();

    }; // class gy521


    ///HC-05 (bluetooth) class
    /** This class is for the interface of the bluetooth chip */
    class hc05 {
    private:

        hwlib::pin_in &pin_in;
        hwlib::pin_out &pin_out;
        hwlib::pin_in &pin_state;
        hwlib::pin_in &start;
        hwlib::pin_in &reset;
        bool active_connection;
        int &baudrate;

        //Beide functies (write en read) heb ik beide gebruikt van hwlib
        //echter heb ik deze een beetje aangepast zodat ze iets meer aanpasbaar zijn vanuit mcbelib

        void write( char c ); 
        char read();

    public:

        hc05(
            hwlib::pin_in &pin_in,
            hwlib::pin_out &pin_out,
            hwlib::pin_in &pin_state,
            hwlib::pin_in &start,
            hwlib::pin_in &reset,
            bool active_connection = false,
            int baudrate = 4800
        ):
            pin_in ( pin_in ),
            pin_out ( pin_out ),
            pin_state ( pin_state ),
            start ( start ),
            reset ( reset ),
            active_connection ( active_connection),
            baudrate ( baudrate )
        {}

        void connectionCheck();
        void writeString( char* characters, int size = 1 );
        void readString( char* characters, int size = 1 );
        char readData();
        void sendData( gy521 &chip );

               
    }; //class hc05
        
}; // namespace mcbelib
 

#endif /* end */


