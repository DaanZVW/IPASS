// =========================================================
// = mcbelib (Motion Controlled Bluetooth Enabled LIBrary) =
// = game: Snake                                           =
// = author: Daan Zimmerman van Woesik                     =
// = institue: HU, Class: V1a                              =
// =========================================================


#ifndef GYRO_HPP
#define GYRO_HPP
//=======================
#include "snake.hpp"
//=======================
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
// ======================




#include "hwlib.hpp"

//https://www.mschoeffler.de/2017/10/05/tutorial-how-to-use-the-gy-521-module-mpu-6050-breakout-board-with-the-arduino-uno/

namespace mcbelib {

    class gy521 {

    private:

        hwlib::i2c_bus & bus;
        uint8_t address; 
        int calibrateAccel;
        int calibrateGyro;

        uint8_t registerRead( const uint8_t &regAddress ) {
            uint8_t new_data;
            bus.i2c_bus::write( address ).write( regAddress );
            bus.i2c_bus::read( address ).read( new_data );

            return new_data;
        }

        void registerWrite( const uint8_t* package ) {
            bus.i2c_bus::write( address ).write( package, 2 );
        }

        int16_t readBuffer( const uint8_t* package ) {  
            return registerRead( package[0] ) << 8 | registerRead( package[1] );
        } 

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

        void activate() {
            uint8_t data[2] = { PWR_MGMT_1, 0 };
            registerWrite( data );
        }

        //accelerometer
        int16_t getAccelX() {
            uint8_t data[2] = { ACCEL_XOUT_H, ACCEL_XOUT_L };
            return readBuffer( data ) / calibrateAccel;
        }

        int16_t getAccelY() {
            uint8_t data[2] = { ACCEL_YOUT_H, ACCEL_YOUT_L };
            return readBuffer( data ) / calibrateAccel;
        }
       
        int16_t getAccelZ() {
            uint8_t data[2] = { ACCEL_ZOUT_H, ACCEL_ZOUT_L };
            return readBuffer( data ) / calibrateAccel;
        }

        //gryoscoop
        int16_t getGyroX() {
            uint8_t data[2] = { GYRO_XOUT_H, GYRO_XOUT_L };
            return readBuffer( data ) / calibrateGyro;
        }

        int16_t getGyroY() {
            uint8_t data[2] = { GYRO_YOUT_H, GYRO_YOUT_L };
            return readBuffer( data ) / calibrateGyro;
        }

        int16_t getGyroZ() {
            uint8_t data[2] = { GYRO_ZOUT_H, GYRO_ZOUT_L };
            return readBuffer( data ) / calibrateGyro;
        }

        //temperatuur
        int16_t getTemperature() {
            uint8_t data[2] = { TEMP_OUT_H, TEMP_OUT_L };
            return readBuffer( data ) / 340 + 36.53;
        }

    }; // class gy521
        
}; // namespace mcbelib
 

#endif /* end */


















class hc05 {
    private:

        hwlib::pin_in &pin_in;
        hwlib::pin_out &pin_out;
        hwlib::pin_in &pin_state;
        bool active_connection;
        int &baudrate;

        char beginMessage[2] = {'m', 'B'};

        //Beide functies (write en read) heb ik beide gebruikt van hwlib
        //echter heb ik deze een beetje aangepast zodat ze iets meer aanpasbaar zijn vanuit mcbelib

        void write( char c ){

            const auto bit_cel = ( ( 1000L * 1000L ) / baudrate );
  
            pin_out.write( 1 );
            // use busy waiting, otherwise logging from within the RTOS
            // will cause serious problems
            hwlib::wait_us_busy( bit_cel );
            
            // start bit
            pin_out.write( 0 );
            hwlib::wait_us_busy( bit_cel );
            
            // 8 data bits
            for( uint_fast8_t i = 0; i < 8; ++i ){
                pin_out.write( ( c & 0x01 ) != 0x00 );
                c = c >> 1;
                hwlib::wait_us_busy( bit_cel );
            }   
            
            // 2 stop bits
            pin_out.write( 1 );
            hwlib::wait_us_busy( 2 * bit_cel ); 
        } 

       char read(){
                
            char c = 0;  
            
            const auto bit_cel = ( ( 1000L * 1000L ) / baudrate );
                
            // wait for start of startbit
            while( pin_in.read() ){ hwlib::wait_ns( 1 ); }
                
            // wait until halfway the first data bit
            auto t = hwlib::now_us();
            t += bit_cel + ( bit_cel / 2 );
            while( hwlib::now_us() < t ){};
                
            // 8 data bits
            for( uint_fast8_t i = 0; i < 8; ++i ){
                c = c >> 1;            
                if( pin_in.read() ){
                    c = c | 0x80;                
                }
                    
                t+= bit_cel;
                while( hwlib::now_us() < t ){};
            }   
            
            return c;
        } 

    public:

        // class autoRead {
        // public:
        //     char* chars;
        //     int size;

        //     autoRead( 
        //         char* chars,
        //         int size
        //     ):
        //         chars( chars ),
        //         size( size )
        //     {}
        // }; //autoRead

        hc05(
            hwlib::pin_in &pin_in,
            hwlib::pin_out &pin_out,
            hwlib::pin_in &pin_state,
            bool active_connection = false,
            int baudrate = 2400
        ):
            pin_in ( pin_in ),
            pin_out ( pin_out ),
            pin_state ( pin_state ),
            active_connection ( active_connection),
            baudrate ( baudrate )
        {}

        void connectionCheck() {
            while(true) {
                pin_state.refresh();
                if (pin_state.read() == 1) {
                    active_connection = true;
                    break;
                }
                hwlib::wait_ms(200); 
            }
        }

        void writeString( char* characters, int size = 1 ) {
            if ( active_connection ) {
                for ( int i=0; i<size; i++) {
                    write(characters[i]);
                }
            }
        }

        void readString( char* characters, int size = 1 ) {
            if ( active_connection ) {
                for (int i=0; i<size; ++i) {
                    characters[i] = read();
                    hwlib::wait_ms(40);
                }
            }
        }
        
        void auto_writeString( char* characters, int size = 1) {
            if ( active_connection ) {
                write(size);
                writeString( characters, size );
                write('\n');
            }
        }

        int auto_readString( char* characters ) {
            if ( active_connection ) {
                int size = 5;
                readString( characters, size );
                return size;
            }
            return 0;
        }

               
    }; //class hc05
