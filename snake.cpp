#include "snake.hpp"

//screen arrays
bool screenPixel[DISPLAY_X][DISPLAY_Y] = {};
bool playfield[DISPLAY_X / PLAYFIELD_FACTOR][DISPLAY_Y / PLAYFIELD_FACTOR] = {};
//snake pixel array
hwlib::xy snakeBody[SNAKE_LENGHT] = {hwlib::xy(15,8), hwlib::xy(16,8), hwlib::xy(17,8)};

namespace mcbelib {

    int random() {
        return hwlib::now_us();
    }

    //==================================PRIVATE==================================
    // Field Functions
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

    //======================================Snake functions===========================================
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

    int checkPixelVertical( const int &direction ) {
        if ( direction > DISPLAY_MIN_MOVE ) {
            //hwlib::cout << "accept!\n" << snakeBody[0].y << '<' << DISPLAY_Y/PLAYFIELD_FACTOR-1 << '\n';
            if ( snakeBody[0].y < DISPLAY_Y/PLAYFIELD_FACTOR-1 ) {
                //hwlib::cout << "real!\n";
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

    void snake::moveFilter( const int &directionX, const int &directionY ) {
        int moveX = checkPixelHorizontal( directionX );
        int moveY = checkPixelVertical( directionY );

        // hwlib::cout << moveX << ' ' << moveY << hwlib::endl;

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

    void snake::checkTail() {
        for (int i=1; i<snakeSize; i++) {
            if (snakeBody[0] == snakeBody[i]) {
                movingTowards = hwlib::xy(2,2);
                return;
            }
        }
    }

    //=========================================================================

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

    //=========================================================================    

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
        hwlib::wait_ms(1000);
    }

    void snake::moveSnakeBody() {
        if (enableMove) {
            for (int i=snakeSize-1; i>0; i--) {
                snakeBody[i] = hwlib::xy(snakeBody[i-1].x, snakeBody[i-1].y);
            }
        }
    }

    void snake::killSnakeBody() {
        for (int i=0; i<SNAKE_LENGHT; i++) {
            snakeBody[i] = hwlib::xy(-1,-1);
        }
        makeSnakeBody();
    }

    void snake::changePosition( const int &directionX, const int &directionY ) {
        moveFilter( directionX, directionY );
        checkTail();
        int moveX = movingTowards.x;
        int moveY = movingTowards.y;

        if (!enableMove) {
            if (moveX != 0 || moveY != 0) {
                enableMove = true;
            }
        }

        //hwlib::cout << moveX << moveY << hwlib::endl;

        if ( moveX == 2 || moveY == 2 ) {
            //Dood
            killSnakeBody();
            enableMove = false;
            candyPosition = hwlib::xy(-1, 0);
        
        } else {
            //Verder
            candyLogic();
            moveSnakeBody();
            snakeBody[0] = hwlib::xy(snakeBody[0].x + moveX, snakeBody[0].y + moveY);
            
            //hwlib::cout << snakeSize << hwlib::endl;
        }
    }
    
    // ============================================================================

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

    //==================================PUBLIC==================================

    void snake::movePlayer( const int &directionX, const int &directionY ) {
        changePosition( directionX, directionY);
        syncSnake();
    }

    void snake::setup() {
        killSnakeBody();
        syncSnake();
    }

    void snake::homeScreen() {
        
    }

    void snake::testScreen() {
        for (int x=0; x<DISPLAY_X/PLAYFIELD_FACTOR-1; x++) {
            for (int y=0; y<DISPLAY_Y/PLAYFIELD_FACTOR-1; y++) {
                playfield[x][y] = 1;
                writePlayfield();
                playfield[x][y] = 0;
            }
        }
    }

    
};
