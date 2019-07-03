//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "hwlib.hpp"
#include "mcbelib.hpp"

int main( void ) {

   //Without it hwlib::cout wouldn't work
   hwlib::wait_ms(1);

   namespace target = hwlib::target;

   //gyroscoop
   auto scl      = target::pin_oc( target::pins::d5 );
   auto sda      = target::pin_oc( target::pins::d6 );
   auto gyro_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
   auto chip     = mcbelib::gy521( gyro_bus, GYRO_ADDRESS, 1000 );

   //communication
   auto slavePin_in  = target::pin_in( target::pins::d2);
   auto slavePin_out = target::pin_out( target::pins::d3);
   auto slaveState   = target::pin_in( target::pins::d4);
   auto start        = target::pin_in( target::pins::d7);
   auto reset        = target::pin_in( target::pins::d8);
   auto slave        = mcbelib::hc05( slavePin_in, slavePin_out, slaveState, start, reset );

   //activate gyroscope
   chip.activate();
   //activate connection
   slave.connectionCheck();
   //send the data continuously
   slave.sendData( chip );
}
