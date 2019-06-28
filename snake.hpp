#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "hwlib.hpp"

#define PLAYFIELD_FACTOR 4
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_X 128
#define DISPLAY_Y 64
#define DISPLAY_MIN_MOVE 5
#define SNAKE_LENGHT 50
#define SNAKE_DEFAULT_POS hwlib::xy(16,8)



namespace mcbelib{

    //class for a 128x64 screen,
    //Note: that bigger screens are not supported
    class snake {
    private:

        hwlib::window &display;
        int frameDelay;
        int snakeSize;
        bool enableMove = 0;
        bool addTail = 0;
        hwlib::xy candyPosition = hwlib::xy(-1,0);
        hwlib::xy movingTowards = hwlib::xy(0,0);

        // Field Functions
        void convertPlayfield();
        void writePlayfield();
        void moveFilter( const int &directionX, const int &directionY );
        void checkTail();
        void candyLogic();
        void makeSnakeBody();
        void moveSnakeBody();
        void killSnakeBody();
        void changePosition( const int &directionX, const int &directionY );
        void syncSnake();

    // =====================================

    public:

        snake(
            hwlib::window &display,
            int frameDelay = 0
        ):
            display ( display ),
            frameDelay ( frameDelay )
        {}

        void movePlayer( const int &directionX, const int &directionY );
        void setup();
        void homeScreen();
        void testScreen();

    }; //class snake

}; //namespace mcbelib




#endif
