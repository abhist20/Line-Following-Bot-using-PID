

#include "mbed.h"
#include "m3pi.h"

// API objects
m3pi robot;
Timer timer;

// Varible Inistiallisation
///////////////////////
void init();
AnalogIn pot_S(p20);
BusOut leds(LED4,LED3,LED2,LED1);
const char g_song[] = "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4"; // notes for song have to be initiallised here.
float ki = 0.008;
float sum =0;
float def=0.545;
float kd = 0.1;//
float dif=0;
float D;
float P;
float I=0;
float kp = 0.2;

//////////////////////
                                      // Main Function
int main()
{
                                     //CALIBRATION AND TIMER INITIALLISATION CODE    
    init();
    robot.play_music(g_song,sizeof(g_song));  // Play music 
    robot.reset_calibration();               // reset previous calibration values and run auto-calibrate routine
    robot.auto_calibrate();                 // robot must be placed over a black line ,AUTO CALIBRATION. 
    unsigned int values[5]= {0};           // an array to store each of the sensor values
    timer.reset();                      //Reset the timer.
    timer.start();                        //initiallising timer.
                // while loop - this runs forever
    while(1) {
                                     
                                     //SENSOR DATA ACCUQUISATION.
         robot.lcd_clear();                                          // clear the LCD so we can draw new information on every loop
         robot.get_calibrated_values(values);                       // read the sensor values and store in array, // these values are 0 to 1000 (0 is white/1000 is black)
         robot.display_sensor_values(values,1);                    //display the sensor values 
         float line_position = robot.calc_line_position(values);  // get line positions form raw sensor data.
         // in range -1 to 1, no error when line is at 0.0
        // number is negative when the line is to the left
        // number is positive when the line is to the right

                                      //DISPLAY CODE.
                                                
        char buffer[6];                         // empty buffer to store values
        sprintf(buffer,"%+.3f",line_position); // create message and store in array .3 means three decimal places
        robot.lcd_goto_xy(1,0);               // print line position on LCD
        robot.lcd_print(buffer,6);
        
                                      //TIMER READ.
                                     
        float DeltaT = timer.read();              //Read and store the timer values in variable called 't' ehich will be used for calculating PID outputs. 
        timer.reset();                      //Reset the timer.
                                     //INTEGRAL OUTPUT CALCULATION. 
        sum = (sum + (line_position * DeltaT)); //calculating integral error.
        I = ki * sum;                     //Multiplying integral gain with integral error. 
                                    //LIMITING THE VLAUE OF I OUTPUT.
        if(I > 0.4) {
            I = 0;
        }
        if(I < -0.4) {
            I = 0;
        }
                                  //DIFFERRNTIAL OUTPUT CALCULATION.
        
        dif = ((line_position)-dif);      //calculating differential.
        D = kd*dif*DeltaT;               //multiplying differential with differential gain to calculate output. 
                                 //PROPORTIONAL OUTPUT CALCULATION.
        
        P =  line_position*kp;         //Multiplying errror with P gain.
        
                                //MOTOR CONTROL CODE.                               
        float motor_speed_right = def - (P + I + D) ;    //Using PID calculate speed for Right motor.
        float motor_speed_left = def + (P + I + D) ;
            //Using PID calculate speed for Left motor.
        robot.motors(motor_speed_left,motor_speed_right); //writing the speeds to motors.
        

    }
}

// Functions
void init()
{
    robot.init();
}
