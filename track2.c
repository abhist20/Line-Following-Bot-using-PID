

#include "mbed.h"
#include "m3pi.h"

                                            // API objects
m3pi robot;
Timer timer;

                                           // Varible Inistiallisation
void init();
AnalogIn pot_S(p20);
BusOut leds(LED4,LED3,LED2,LED1);
DigitalIn button_enter(p24);
const char g_song[] =  "O6 T40 L16 d#<b<f#<d#<f#<bd#f#"
  "T80 c#<b-<f#<c#<f#<b-c#8"
  "T180 d#b<f#d#f#>bd#f#c#b-<f#c#f#>b-c#8 c>c#<c#>c#<b>c#<c#>c#c>c#<c#>c#<b>c#<c#>c#"
  "c>c#<c#>c#<b->c#<c#>c#c>c#<c#>c#<b->c#<c#>c#"
  "c>c#<c#>c#f>c#<c#>c#c>c#<c#>c#f>c#<c#>c#"
  "c>c#<c#>c#f#>c#<c#>c#c>c#<c#>c#f#>c#<c#>c#d#bb-bd#bf#d#c#b-ab-c#b-f#d#";
const char g_song1[] =  "v12 L16 o5 eererce8g8r8<g8r8"
    "c8.<gr8<e8.<a8<b8<b-<a8<g.e.g.a8fgre8cd<b8."
    "c8.<gr8<e8.<a8<b8<b-<a8<g.e.g.a8fgre8cd<b8."
    "r8gf#fd#8er<g#<acr<acd"
    "r8gf#fd#8er>c8>c>c4"
    "r8gf#fd#8er<g#<acr<acd"
    "r8e-8rd8.c4";
float ki = 0.001;
float sum =0;
float def=0.2;
float kd = 1.5;
float dif=0;
float D;
float motor_speed1 = 0;
float motor_speed = 0;
int left=1;
int l;
int r;
int last=0;
int right=1;
float I=0;
float P;
float kp= 0.25;
float line=0;
//////////////

                                    // Main Function

int main()
{   
                               //Auto Calibration and Timer Initiallisation and Music.
    init();
    robot.play_music(g_song1,sizeof(g_song1)); // music
    robot.reset_calibration();        // reset previous calibration values and run auto-calibrate routine
    robot.auto_calibrate();          //auto calibrationn bot must be placed over a black line
    unsigned int values[5]= {0};    // an array to store each of the sensor values
    timer.start();                 //initiallising timer.
    
    
                                 // While loop - this runs forever
    
    while(1) {

                                //Sensor Data acquisition 
        
        robot.get_calibrated_values(values); // Get sensor values.
        float line_position = robot.calc_line_position(values);
        
                                //Timer data acquisition 
        
        float t = timer.read();             //read timer.
        timer.reset();                     //reset timer.
                                
                                //Error Integral calculations.
        
        sum = (sum + (line_position/8))/t; //calculation of the actual error integral.
        I = ki * sum;                      //Multiplying integral gain to calculated integral to obtain integral output. 
        
                               //Limiting integral values.
        
        if(I > 0.4) {
            I = 0;
        }
        if(I < -0.4) {
            I = 0;
        }
                            //Error Differential calculations.
        
        dif = ((line_position/8)-dif);  //calculation of the actual error differential.
        D = kd*dif;                     //Multiplying differential gain to calculated differential to obtain differential output.
                            //Error Proportional calculation.
        
        P = line_position * kp; // claculation if the actual error and multiplying it with a gain. 
                              
                               //Motor Control Code.   
        
        float motor_speed_right = def - P + I + D ;   //Using PID calculate speed for Right motor.
        float motor_speed_left = def +  P - I - D ;   //Using PID calculate speed for Left motor.
        robot.motors(motor_speed_left,motor_speed_right);
        
                            //COUNTING LINES ON LEFT.
                             
        if(values[0] > 500 and values[4] < 100 and last == 1 ) {
            l=left++;
            last=0;
        }
                            //COUNTING LINES ON RIGHT.
                                   
        if(values[4] > 500 and values[0] < 100  and last == 0) {
            r=right++;
            line = 1;
            last=1;
        }
        
                            //STOPING AND PRINTING LINES FOUND..
                            
        if (values[0] == 1000 and values[4] == 1000 and values[2] == 1000 and line == 1) {
                    
                    //Reducing speed and writing it onto motors
            
            motor_speed_right = 0;
            motor_speed_left  = 0;
            robot.motors(motor_speed_left,motor_speed_right);
                   //Printing lines on screen.
            robot.lcd_clear();
            char bufferl[6];
            sprintf(bufferl,"%i",l);
            robot.lcd_goto_xy(0,0);
            robot.lcd_print("L",1);
            robot.lcd_print(bufferl,2);
            char bufferR[6];
            sprintf(bufferR,"%i",r);
            robot.lcd_goto_xy(3,0);
            robot.lcd_print("R",1);
            robot.lcd_print(bufferR,2);
            robot.lcd_goto_xy(0,1);
            robot.lcd_print("track2",6);
            robot.play_music(g_song,sizeof(g_song));
            break;          //breaking the loop so that bot will not start moving again.
        }
        

    }

}

// Functions
void init()
{
    robot.init();
    button_enter.mode(PullUp);
}
