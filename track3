

#include "mbed.h"
#include "m3pi.h"

// API objects
m3pi robot;
Timer timer;


// Varible Inistiallisation
void init();
AnalogIn pot_S(p20);
BusOut leds(LED4,LED3,LED2,LED1);
const char g_song[] = "!T240 L4 ag";
float ki = 0.001;
float sum =0;
float def=0.3;
float kd = 1.5;
float dif=0;
float D;
float buf;
const char g_song1[] =  "v12 L16 o5 eererce8g8r8<g8r8"
    "c8.<gr8<e8.<a8<b8<b-<a8<g.e.g.a8fgre8cd<b8."
    "c8.<gr8<e8.<a8<b8<b-<a8<g.e.g.a8fgre8cd<b8."
    "r8gf#fd#8er<g#<acr<acd"
    "r8gf#fd#8er>c8>c>c4"
    "r8gf#fd#8er<g#<acr<acd"
    "r8e-8rd8.c4";
float motor_speed1 = 0;
float motor_speed = 0;
int left=0;
int l;
int r;
int last=0;
int right=0;
int ct=0;
int latch =0;
float ds = 0;
float dr = 0;
float I=0;
// Main Function
int main()
{
    init();
    // reset previous calibration values and run auto-calibrate routine
    robot.reset_calibration();
    // music
    robot.play_music(g_song1,sizeof(g_song1)); 
    // robot must be placed over a black line
    robot.auto_calibrate();
    // an array to store each of the sensor values
    unsigned int values[5]= {0};
    //initiallising timer.
    timer.start();
    // main loop - this runs forever
    while(1) {
        //this loop is used for making the small speeds to each motor zero after inner loop completion.
        if (latch == 1)
        {
            ds = 0;
            dr = 0;
            } 

        // read the sensor values and store in array
        // these values are 0 to 1000 (0 is white/1000 is black)
        robot.get_calibrated_values(values);

        robot.display_sensor_values(values,1);

        // in range -1 to 1, no error when line is at 0.0
        // number is negative when the line is to the left
        // number is positive when the line is to the right
        float line_position = robot.calc_line_position(values);
        //read the timer.  
        float t = timer.read();
        timer.reset();
        //Integral calculations.
        sum = (sum + (line_position/8))/t;
        I = ki * sum;
        //limiting I contoller output.
        if(I > 0.4) {
            I = 0;
        }
        if(I < -0.4) {
            I = 0;
        }
        //this loop is used for entering the inner loop.
        if (ct == 2 and values[4] > 500)
        {
            ds = 0.2;
            latch=1;
            }
        //Differential calculations.    
        dif = ((line_position/8)-dif);
        D = kd*dif;
        //this loop is used to prevent bot from entering the inner loop before first loop.
        if (ct == 3 and values[0] > 500 )
        {
            def = 0.3;
            dr = 0.2;
            latch = 1;
            
            }
         //this loop is used to increase speed of bot in the inner loop.   
        if (ct == 4)
            {
                def = 0.3;}        
        //motor code
        float motor_speed1 = def - (line_position/5) + I + D + dr;
        float motor_speed = def + (line_position/5) - I - D + ds ;
        //assigning the calculated speeds to the bot wheels.
        robot.motors(motor_speed,motor_speed1);
        //loop for temporarly stopping the bot and stopping after the program is over.
        if (values[0] == 1000 and values[4] == 1000 and values[2] == 1000) {
            motor_speed1 = 0;
            motor_speed  = 0;
            robot.motors(motor_speed,motor_speed1);
            robot.stop();
            robot.lcd_clear();
            char bufferR[6];
            sprintf(bufferR,"%i",ct);
            robot.lcd_goto_xy(3,0);
            robot.lcd_print("ct",1);
            robot.lcd_print(bufferR,2);//print the count of the start line on the lcd.
            ct++; //counter increment.
            wait(1);
            //ending loop for stopping the program.
            if ( ct == 5)
            {
                robot.lcd_goto_xy(0,1);
            robot.lcd_print("track2",6);
                break;
                }
        }
        
     



    }

}

// Functions
void init()
{
    robot.init();
}
