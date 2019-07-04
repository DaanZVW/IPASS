//     Copyright Daan Zimmerman van Woesik 2019 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "mcbelib.hpp"

//screen arrays
bool screenPixel[DISPLAY_X][DISPLAY_Y] = {};
bool playfield[DISPLAY_X / PLAYFIELD_FACTOR][DISPLAY_Y / PLAYFIELD_FACTOR] = {};
//snake pixel array
hwlib::xy snakeBody[SNAKE_LENGHT] = {hwlib::xy(15,8), hwlib::xy(16,8), hwlib::xy(17,8)};
//homescreen array
hwlib::xy snakeLogo[LOGO_LENGHT] = {
    hwlib::xy(3,0), hwlib::xy(2,0), hwlib::xy(1,0), hwlib::xy(0,0), hwlib::xy(0,1),
    hwlib::xy(0,2), hwlib::xy(1,2), hwlib::xy(2,2), hwlib::xy(3,2), hwlib::xy(3,3),
    hwlib::xy(3,4), hwlib::xy(2,4), hwlib::xy(1,4), hwlib::xy(0,4), // S

    hwlib::xy(5,4), hwlib::xy(5,3), hwlib::xy(5,2), hwlib::xy(5,1), hwlib::xy(5,0),
    hwlib::xy(6,1), hwlib::xy(7,2), hwlib::xy(8,3), hwlib::xy(9,4), hwlib::xy(9,3),
    hwlib::xy(9,2), hwlib::xy(9,1), hwlib::xy(9,0), //N

    hwlib::xy(11,1), hwlib::xy(11,2), hwlib::xy(11,3), hwlib::xy(11,4), hwlib::xy(12,0), 
    hwlib::xy(13,0), hwlib::xy(12,2), hwlib::xy(13,2), hwlib::xy(14,1), hwlib::xy(14,2),
    hwlib::xy(14,3), hwlib::xy(14,4), //A

    hwlib::xy(16,0), hwlib::xy(16,1), hwlib::xy(16,2), hwlib::xy(16,3), hwlib::xy(16,4),
    hwlib::xy(17,2), hwlib::xy(18,1), hwlib::xy(18,3), hwlib::xy(19,0), hwlib::xy(19,4), //K
    
    hwlib::xy(21,0), hwlib::xy(21,1), hwlib::xy(21,2), hwlib::xy(21,3), hwlib::xy(21,4), 
    hwlib::xy(22,0), hwlib::xy(23,0), hwlib::xy(22,2), hwlib::xy(22,4), hwlib::xy(23,4) //E
};

namespace mcbelib {

    // =========================================================================
    // ================================PRIVATE==================================
    // =========================================================================

    ///Return random number
    /**Return the time in Nano Seconds
    */
    int random() {
        return hwlib::now_us();
    }

    // =================================PRIVATE==================================
    // =============================Field Functions==============================
    // =================================PRIVATE==================================

    ///Convert playfield into pixels into screen
    /**Pixels in playfield array are upscaled and put in the Screen pixels array
    */
    void snake::convertPlayfield() {
        bool writePixel;

        for ( uint16_t xp=0; xp<DISPLAY_X/PLAYFIELD_FACTOR; xp++ ) {
            for ( uint16_t yp=0; yp<DISPLAY_Y/PLAYFIELD_FACTOR; yp++ ) {
                if ( (xp == 0 || yp == 0) || (xp == DISPLAY_X/PLAYFIELD_FACTOR-1 || yp == DISPLAY_Y/PLAYFIELD_FACTOR-1) ) {
                    writePixel = 1;
                } else {
                    writePixel = playfield[xp][yp];
                }
                for ( uint16_t x=xp*PLAYFIELD_FACTOR; x<xp*PLAYFIELD_FACTOR+PLAYFIELD_FACTOR; x++ ) {
                    for ( uint16_t y=yp*PLAYFIELD_FACTOR; y<yp*PLAYFIELD_FACTOR+PLAYFIELD_FACTOR; y++ ) {
                        screenPixel[x][y] = writePixel;
                    }
                }
            }
        }

    }

    ///Write screenPixels on screen
    /**Write the array for the pixels onto the screen
     */
    void snake::writePlayfield() {
        convertPlayfield();

        display.clear();
        for ( uint16_t x=0; x<DISPLAY_X; x++ ) {
            for ( uint16_t y=0; y<DISPLAY_Y; y++ ) {
                if ( screenPixel[x][y] ){
                    display.write( hwlib::xy( x, y ) );
                }
            }
        }
        display.flush();
    }

    // =================================PRIVATE==================================
    // =============================Snake functions==============================
    // =================================PRIVATE==================================

    ///Checks for the horizontal axis 
    /**Checks for the horizontal axis if it goes out of boundaries, otherwise it
     * will return which way it goes
     */
    int checkPixelHorizontal( const int &direction ) {
        if ( direction > DISPLAY_MIN_MOVE ) {
            if ( snakeBody[0].x < DISPLAY_X/PLAYFIELD_FACTOR-1 ) {
                return 1;
            }
        } else if ( direction < DISPLAY_MIN_MOVE * -1 ) {
            if ( snakeBody[0].x > 0 ) {
                return -1;
            }
        }
        if ( snakeBody[0].x == DISPLAY_X/PLAYFIELD_FACTOR-1 ) {
            return 2;
        } else if ( snakeBody[0].x == 0) {
            return 2;
        }
        return 0;
    }

    ///Checks for the vertical axis 
    /**Checks for the vertical axis if it goes out of boundaries, otherwise it
     * will return which way it goes
     */
    int checkPixelVertical( const int &direction ) {
        if ( direction > DISPLAY_MIN_MOVE ) {
            if ( snakeBody[0].y < DISPLAY_Y/PLAYFIELD_FACTOR-1 ) {
                return 1;
            } 
        } else if ( direction < DISPLAY_MIN_MOVE * -1 ) {
            if ( snakeBody[0].y > 0 ) {
                return -1;
            }
        }
        if ( snakeBody[0].y == DISPLAY_Y/PLAYFIELD_FACTOR-1 ) {
            return 2;
        } else if ( snakeBody[0].y == 0 ) {
            return 2;
        }
        return 0;
    }

    ///Checks controllers movements
    /**Looks if an movement given by the controller is valiable
     */
    void snake::moveFilter( const int &directionX, const int &directionY ) {
        int moveX, moveY;
        if (accept_bluetooth) {
            moveX = checkPixelHorizontal( directionX *(DISPLAY_MIN_MOVE+1) );
            moveY = checkPixelVertical( directionY *(DISPLAY_MIN_MOVE+1) );
        } else {
            moveX = checkPixelHorizontal( directionX );
            moveY = checkPixelVertical( directionY );
        }

        if ( moveX == 0 && moveY == 0 ) {
            return;
        } else if ( moveX == 2 || moveY == 2) {
            movingTowards = hwlib::xy(2,2);
            return;
        } else if ( movingTowards.x == 1 && moveY == -1 ) {
            movingTowards = hwlib::xy(0, moveY);
        } else if ( movingTowards.x == -1 && moveY == -1 ) {
            movingTowards = hwlib::xy(0, moveY);
        } else if ( movingTowards.x == 1 && moveY == 1 ) {
            movingTowards = hwlib::xy(0, moveY);
        } else if ( movingTowards.x == -1 && moveY == 1 ) {
            movingTowards = hwlib::xy(0, moveY);
        } else if ( movingTowards.y == 1 && moveX == -1 ) {
            movingTowards = hwlib::xy(moveX, 0);
        } else if ( movingTowards.y == -1 && moveX == -1 ) {
            movingTowards = hwlib::xy(moveX, 0);
        } else if ( movingTowards.y == 1 && moveX == 1 ) {
            movingTowards = hwlib::xy(moveX, 0);
        } else if ( movingTowards.y == -1 && moveX == 1 ) {
            movingTowards = hwlib::xy(moveX, 0);
        } else if ( movingTowards.y == 0 && movingTowards.x == 0 ) {
            if (moveX != 0 && moveY != 0) {
                moveY = 0;
            }
            if (snakeBody[0].x + moveX != snakeBody[1].x || snakeBody[0].y + moveY != snakeBody[1].y) {
                movingTowards = hwlib::xy(moveX, moveY);
            }
        }
    } 

    ///Check tail
    /**Check the tail for any collisions with the head
     */
    void snake::checkTail() {
        for (int i=1; i<snakeSize; i++) {
            if (snakeBody[0] == snakeBody[i]) {
                movingTowards = hwlib::xy(2,2);
                return;
            }
        }
    }

    // =================================PRIVATE==================================
    // ==================================Candy===================================
    // =================================PRIVATE==================================


    ///Logic for the candys
    /**Looks if the head of the snake equals the x and y cooridinates of the candy,
     * if so the snake will grow and a new candy will be generated
     */
    void snake::candyLogic() {
        //hwlib::cout << "muhahaha";
        if ((snakeBody[0] == candyPosition) || (!enableMove && candyPosition.x == -1)) {
            int candyX, candyY;
            for(;;) {
                bool inSnake = 0;
                candyX = random() % (DISPLAY_X/PLAYFIELD_FACTOR-2) +1;
                candyY = random() % (DISPLAY_Y/PLAYFIELD_FACTOR-2) +1; 
                for (int i=0; i<snakeSize; i++) {
                    if (snakeBody[i] == hwlib::xy(candyX, candyY)) {
                        inSnake = true;
                        break;
                    }
                }
                if (inSnake == false) {
                    break;
                }
            }
            candyPosition = hwlib::xy(candyX, candyY);
            snakeSize++;
            //hwlib::cout << candyPosition.x << ' ' << candyPosition.y << hwlib::endl;
        }
    }

    ///Kill candy
    /**Kill the candy's position
     */
    void snake::killCandy() {
        candyPosition = hwlib::xy(-1,0);
    }

    // =================================PRIVATE==================================
    // ============================MainSnakeFunctions============================
    // =================================PRIVATE==================================  

    ///Make the snakeBody
    /**Make the snake on Random coordinates 
     */
    void snake::makeSnakeBody() {
        int snakeX = random() % (DISPLAY_X/PLAYFIELD_FACTOR-14) +7;
        int snakeY = random() % (DISPLAY_Y/PLAYFIELD_FACTOR-14) +7;
        int direction = random() % 4;
        int directionX = 0, directionY = 0;

        if (direction == 0) {
            directionX = 1;
        } else if (direction == 1) {
            directionX = -1;
        } else if (direction == 2) {
            directionY = 1;
        } else {
            directionY = -1;
        }

        snakeBody[0] = hwlib::xy(snakeX, snakeY);
        snakeBody[1] = hwlib::xy(snakeX+(directionX), snakeY+(directionY));
        snakeBody[2] = hwlib::xy(snakeX+(directionX*2), snakeY+(directionY*2));
        snakeBody[3] = hwlib::xy(snakeX+(directionX*3), snakeY+(directionY*3));
        snakeBody[4] = hwlib::xy(snakeX+(directionX*4), snakeY+(directionY*4));
        snakeBody[5] = hwlib::xy(snakeX+(directionX*5), snakeY+(directionY*5));
        snakeSize = 6;
        movingTowards = hwlib::xy(0,0);
    }

    ///Move the snake body
    /**Let the snakebody follow the head
     */
    void snake::moveSnakeBody() {
        if (enableMove) {
            for (int i=snakeSize-1; i>0; i--) {
                snakeBody[i] = hwlib::xy(snakeBody[i-1].x, snakeBody[i-1].y);
            }
        }
    }

    ///Kill the snakeBody
    /**Kill the snake by putting all the coordinates to -1
     */
    void snake::killSnakeBody() {
        for (int i=0; i<SNAKE_LENGHT; i++) {
            snakeBody[i] = hwlib::xy(-1,-1);
        }
    }

    ///Move the snake with logic
    /**Move the snake while looking if its moves, grows or dies
     */
    void snake::changePosition( const int &directionX, const int &directionY ) {
        moveFilter( directionX, directionY );
        checkTail();
        int moveX = movingTowards.x;
        int moveY = movingTowards.y;
        
        //Makes the body move when standing still
        if (!enableMove) {
            if (moveX != 0 || moveY != 0) {
                enableMove = true;
            }
        }

        if ( moveX == 2 || moveY == 2 ) {
            //Death
            killSnakeBody();
            enableMove = false;
            died = 1;
        
        } else {
            //Go further
            candyLogic();
            moveSnakeBody();
            snakeBody[0] = hwlib::xy(snakeBody[0].x + moveX, snakeBody[0].y + moveY);
        }
    }

    // =================================PRIVATE==================================
    // ================================Bluetooth=================================
    // =================================PRIVATE==================================

    ///Make homescreen when bluetooth is enabled
    /**Make homescreen while extracting the incoming packages looking for the start signal
     */
    void snake::extractPackageHomeScreen() {
        for (int i=0; i<LOGO_LENGHT; i++) {
            playfield[snakeLogo[i].x + LOGO_SPACING_X][snakeLogo[i].y + LOGO_SPACING_Y] = 1;
        }
        writePlayfield();
        for(;;){
            char data = bluetooth.readData();
            bool start_button = (data & (1 << 3)) > 0;
            if(start_button && (data & (1 << 1)) > 0 &&(data & (1 << 0)) > 0) {
                break;
            }
            hwlib::wait_ms(50);
        }
    }

    ///Start game while bluetooth is enabled
    /**Start game while extracting the incoming data and giving that 
     */
    void snake::extractPackageStart() {
        for (;;) {
            extractPackageHomeScreen();
            setup();
            for (;;) {
                char data = bluetooth.readData();
                if ((data & (1 << 1)) > 0 || (data & (1 << 0)) > 0) {
                    data = STANDARD_RETURN;
                } 

                bool moveX = (data & (1 << 7)) > 0;
                bool nothingX = (data & (1 << 6)) > 0;
                bool moveY = (data & (1 << 5)) > 0;
                bool nothingY = (data & (1 << 4)) > 0;
                bool reset_button = (data & (1 << 2)) > 0;

                int moveX_decoded, moveY_decoded;

                //Perform some logic onto incoming data for X-axis
                if (moveX == 0) {
                    moveX_decoded = -1;
                } else {
                    moveX_decoded = 1;
                }
                if (nothingX == 1) {
                    moveX_decoded = 0;
                }

                //Perform some logic onto incoming data for Y-axis
                if (moveY == 0) {
                    moveY_decoded = -1;
                } else {
                    moveY_decoded = 1;
                }
                if (nothingY == 1) {
                    moveY_decoded = 0;
                }

                 //if reset is pressed or if it died goto homescreen
                if (reset_button || died) {
                    died = 0;
                    killSnakeBody();
                    killCandy();
                    syncSnake();
                    break;
                }                
                movePlayer( moveY_decoded - moveY_decoded * 2, moveX_decoded - moveX_decoded * 2 );
            }
        }       
    }
    // =================================PRIVATE==================================
    // ================================Connector=================================
    // =================================PRIVATE==================================

    ///Sync snake array with playfield
    /**Put the pixels of the snake onto the playfield and write that to the screen
     */
    void snake::syncSnake() {
        for (int x=0; x<DISPLAY_X/PLAYFIELD_FACTOR; x++) {
            for (int y=0; y<DISPLAY_Y/PLAYFIELD_FACTOR; y++) {
                playfield[x][y] = 0;
            }
        }
        for (int i=0; i<snakeSize-1; i++) {
            if (snakeBody[i].x != -1) {
                playfield[snakeBody[i].x][snakeBody[i].y] = 1;
            }
        }
        if (candyPosition.x != -1) {
            playfield[candyPosition.x][candyPosition.y] = 1;
        }
        writePlayfield();
    }

    // =========================================================================
    // =================================PUBLIC==================================
    // =========================================================================


    ///Move the playerhead
    /**This will change the snake and after that will sync the screen
     */
    void snake::movePlayer( const int &directionX, const int &directionY ) {
        changePosition( directionX, directionY);
        syncSnake();
    }

    ///Setup the snake
    /**This function will setup the snake so it will be seen on screen
     */
    void snake::setup() {
        killSnakeBody();
        makeSnakeBody();
        syncSnake();
    }

    ///Show homescreen
    /**The homescreen will be made onto the screen and will wait until start is pressed
     */
    void snake::homeScreen() {
        for (int i=0; i<LOGO_LENGHT; i++) {
            playfield[snakeLogo[i].x + LOGO_SPACING_X][snakeLogo[i].y + LOGO_SPACING_Y] = 1;
        }
        writePlayfield();
        for(;;){
            start_pin.refresh();
            if(start_pin.read()) {
                break;
            }
        }
    }

    ///Main start function
    /**This function will start the bluetooth function if accept_bluetooth is true, if not
     * it will use the internal resources to play the game
     */
    void snake::start() {
        if (accept_bluetooth) {
            bluetooth.connectionCheck();
            extractPackageStart();
        }

        chip.activate();
        for (;;) {
            homeScreen();
            setup();
            for (;;) {
                reset_pin.refresh();
                if (reset_pin.read() || died) {
                    died = 0;
                    killSnakeBody();
                    killCandy();
                    syncSnake();
                    break;
                }
                int accelX = chip.getAccelX();
                int accelY = chip.getAccelY();
                movePlayer( accelY, accelX - accelX * 2 );
            }
        }
    }

    
};
