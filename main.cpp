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

   //game
   auto scl_game = target::pin_oc( target::pins::d19 );
   auto sda_game = target::pin_oc( target::pins::d18 );
   auto game_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl_game, sda_game );
   auto setup    = hwlib::glcd_oled( game_bus, DISPLAY_ADDRESS );
   auto display  = mcbelib::snake( setup, 10 );

   chip.activate();
   //display.homeScreen();
   for (;;){
      int accelX = chip.getAccelX();
      int accelY = chip.getAccelY();

      display.movePlayer(accelY, accelX - accelX * 2);
      
   }
   // for ( uint16_t x=0; x<DISPLAY_X; x++ ) {
   //    for ( uint16_t y=0; y<DISPLAY_Y; y++ ) {
   //       hwlib::glcd_oled( game_bus, 0x3c ).write( hwlib::xy( x, y ) );
   //    }
   // }
}

   // //communication
   // auto masterPin_in  = target::pin_in( target::pins::d28);
   // auto masterPin_out = target::pin_out( target::pins::d30);
   // auto masterState   = target::pin_in( target::pins::d32);
   // auto master        = mcbelib::hc05( masterPin_in, masterPin_out, masterState );
