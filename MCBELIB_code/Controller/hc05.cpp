//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "mcbelib.hpp"

namespace mcbelib {

    ///Write a char over bluetooth
    /**Write a char over the bus via the Serial (UART) protocol*/
    void hc05::write( char c ){

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

    ///Read a char over bluetooth
    /**Read a char over the bus via the Serial (UART) protocol*/
    char hc05::read(){
                    
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

    ///Check the Bluetooth connection
    /**Check the state pin and set bool active_connection on to give acces
     * to all the commands, this function has to be called before other
     * functions will be available*/
    void hc05::connectionCheck() {
        while(true) {
            pin_state.refresh();
            if (pin_state.read() == 1) {
                active_connection = true;
                break;
            }
            //hwlib::cout << "check";
            hwlib::wait_ms(200); 
        }
    }

    ///Write a string over bluetooth
    /**Write a string over the bus by giving it an array with chars and
     * size of the array*/
    void hc05::writeString( char* characters, int size ) {
        if ( active_connection ) {
            for ( int i=0; i<size; i++) {
                write(characters[i]);
                hwlib::wait_ms(50);
            }
        }
    }

    ///Read a string over bluetooth
    /**Read a string over the bus by giving it an (empty) array and
     * the size of the string*/
    void hc05::readString( char* characters, int size ) {
        if ( active_connection ) {
            for (int i=0; i<size; ++i) {
                characters[i] = read();
            }
        }
    }

    ///Write a char over bluetooth for controller
    /**Write a char over the bus by getting the data from the
     * gyroscope and sending them constantly over the bluetooth
     * connection*/
    void hc05::sendData( gy521 &chip ) { 
        if (active_connection) {
            for (;;) {
                start.refresh();
                reset.refresh();
                char data = chip.getWayPointing( true );
                data |= start.read() << 3;
                data |= reset.read() << 2;
                write( data );
            }
        }
    }
} // namespace mcbelib
