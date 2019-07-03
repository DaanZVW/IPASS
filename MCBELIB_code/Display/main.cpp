//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "hwlib.hpp"
#include "mcbelib.hpp"

int main( void ) {

   hwlib::wait_ms(1);

   namespace target = hwlib::target;

   //gyroscoop
   auto scl      = target::pin_oc( target::pins::d21 );
   auto sda      = target::pin_oc( target::pins::d20 );
   auto gyro_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
   auto chip     = mcbelib::gy521( gyro_bus, GYRO_ADDRESS, 1000 );

   //communication
   auto masterPin_in  = target::pin_in( target::pins::d28);
   auto masterPin_out = target::pin_out( target::pins::d30);
   auto masterState   = target::pin_in( target::pins::d32);
   auto master        = mcbelib::hc05( masterPin_in, masterPin_out, masterState );

   //game
   auto scl_game = target::pin_oc( target::pins::d19 );
   auto sda_game = target::pin_oc( target::pins::d18 );
   auto start    = target::pin_in( target::pins::d22 );
   auto reset    = target::pin_in( target::pins::d24 );
   auto game_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl_game, sda_game );
   auto setup    = hwlib::glcd_oled( game_bus, DISPLAY_ADDRESS );
   auto game     = mcbelib::snake( setup, start, reset, chip, master, true );

   game.start();
}
