#include "mbed.h"
#include "uLCD_4DGL.h"
using namespace std::chrono;

InterruptIn bt_up(D12);      // D12
InterruptIn bt_down(D11);    // D11
InterruptIn bt_sl(D10);      // D10
DigitalOut led_down(LED2);
DigitalOut led_sl(LED3);
AnalogOut  aout(D7);
AnalogOut  Aout(D13);
AnalogIn ain(A0);
Timer t;
bool sample_finished = 0;
int freq = 0;
int pre_freq = 1;
int sample = 500;
float ADCdata[500];
// generate wave
Thread thread;
Thread eventThread;
//Thread Wave_Sampling;

EventQueue queue;
void wave_sampling();

uLCD_4DGL uLCD(D1, D0, D2);

void up () {
    if (freq == 1) {    // 1
        pre_freq = 1;
    } else if (freq == 2) { // 1/2
        pre_freq = 1;
    } else if (freq == 3) { // 1/4
        pre_freq = 2;
    } else if (freq == 4) { // 1
        pre_freq = 3;
    } else if (freq == 0) {
        pre_freq = 1;
    }
}
void down () {
    if (freq == 1) {    // 1
        pre_freq = 2;
    } else if (freq == 2) { // 1/2
        pre_freq = 3;
    } else if (freq == 3) { // 1/4
        pre_freq = 4;
    } else if (freq == 4) { // 1
        pre_freq = 4;
    } else if (freq == 0) {
        pre_freq = 4;
    }
}

void sl () {
    freq = pre_freq;
    queue.call(&wave_sampling);
    //pre_freq = 0;
}

void wave()
{   
    float i;
    while (1){
        if (freq == 1 && !sample_finished) {        // 1
            for (i = 0.0f; i < 0.909091; i+=0.00015319f) {
                aout = i;
            }
            wait_us(80000);
            for (i = 0.909091f; i > 0.0f; i -= 0.00012132f) {
                aout = i;
            }
        } else if(freq == 2 && !sample_finished) {  // 1/2 Hz
            for (i = 0.0f; i < 0.909091; i+=0.000306380f) {
                aout = i;
            }
            wait_us(160000);
            for (i = 0.909091f; i > 0.0f; i -= 0.00024264f) {
                aout = i;
            }
        } else if(freq == 3 && !sample_finished) {  // 1/4 Hz
            for (i = 0.0f; i < 0.909091; i+=0.00061276f) {
                aout = i;
            }
            wait_us(200000);
            for (i = 0.909091f; i > 0.0f; i -= 0.00048529f) {
                aout = i;
            }
        } else if(freq == 4 && !sample_finished) {  // 1/8 Hz
            for (i = 0.0f; i < 0.909091; i+=0.00122552f) {
                aout = i;
            }
            wait_us(220000);
            for (i = 0.909091f; i > 0.0f; i -= 0.00097058f) {
                aout = i;
            } 
        } else if (!sample_finished) {    // >> 10*200Hz
            for (i = 0.0f; i < 1; i+=0.27f) {
                aout = i;
            }
            for (i = 1.0f; i > 0.0f; i -= 0.03f) {
                aout = i;
            }
        } else {
            aout = 0.0f;
        }
    }
}

void wave_sampling() {
    int i = 0;
    sample_finished = 0;
    //t.start();
    for (i = 0; i < sample; i++){
        Aout = ain;
        ADCdata[i] = ain;
        ThisThread::sleep_for(1000ms/sample); // sampling rate = 500/s 實際166.666667/s
    }
    sample_finished = 1;
    //t.stop();
    //auto ms = chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count();
    for (i = 0; i < sample; i++) {
        printf("%f\r\n", ADCdata[i]);
    }
    //printf ("Timer time: %llu ms\n", ms);
}


int main()
{
    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));
    // bool sl = 0;
    led_down = 0;
    led_sl = 0;
    // uLCD initialization
    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    uLCD.color(BLUE);
    uLCD.printf("\n108061113\n"); 
    uLCD.text_width(1); //3X size text
    uLCD.text_height(1);
    uLCD.color(GREEN);
    uLCD.locate(1,2);
    uLCD.printf("slew rate:   1");
    uLCD.locate(1,4);
    uLCD.printf("slew rate: 1/2");
    uLCD.locate(1,6);
    uLCD.printf("slew rate: 1/4");
    uLCD.locate(1,8);
    uLCD.printf("slew rate: 1/8");
    thread.start(wave);
    bt_up.rise(&up);
    bt_down.rise(&down);
    bt_sl.rise(&sl);
    //Wave_Sampling.start(wave_sampling);
    //button.rise(&wave_sampling);

    while (1) {
        //led_up = bt_up; // toggle led based on value of button
        led_down = bt_down; // toggle led based on value of button
        led_sl = bt_sl; // toggle led based on value of button
        // printf("led_up = %d, led_down = %d, led_sl = %d\n", bt_up.read(), bt_down.read(), bt_sl.read());
        if (freq == 1) {    // 1
            // show freq_1 on uLCD
            uLCD.locate(1,2);
            uLCD.printf("slew rate:   1");
            uLCD.locate(1,4);
            uLCD.printf("              ");
            uLCD.locate(1,6);
            uLCD.printf("              ");
            uLCD.locate(1,8);
            uLCD.printf("              ");

            if (pre_freq == 0) {
                //wave_sampling();
                pre_freq = 1;
            } 
        } else if (freq == 2) { // 1/2
            // show freq_2 on uLCD
            uLCD.locate(1,2);
            uLCD.printf("              ");
            uLCD.locate(1,4);
            uLCD.printf("slew rate: 1/2");
            uLCD.locate(1,6);
            uLCD.printf("              ");
            uLCD.locate(1,8);
            uLCD.printf("              ");

            if (pre_freq == 0) {
                //wave_sampling();
                pre_freq = 2;
            } 
        } else if (freq == 3) { // 1/4
            // show freq_2 on uLCD
            uLCD.locate(1,2);
            uLCD.printf("              ");
            uLCD.locate(1,4);
            uLCD.printf("              ");
            uLCD.locate(1,6);
            uLCD.printf("slew rate: 1/4");
            uLCD.locate(1,8);
            uLCD.printf("              ");

            if (pre_freq == 0) {
                //wave_sampling();
                pre_freq = 3;
            }
        } else if (freq == 4) { // 1
            // show freq_2 on uLCD
            uLCD.locate(1,2);
            uLCD.printf("              ");
            uLCD.locate(1,4);
            uLCD.printf("              ");
            uLCD.locate(1,6);
            uLCD.printf("              ");
            uLCD.locate(1,8);
            uLCD.printf("slew rate: 1/8");

            if (pre_freq == 0) {
                //wave_sampling();
                pre_freq = 4;
            }
        } else if (freq == 0) {
            // show freq_1 and freq_2 on uLCD
            uLCD.locate(1,2);
            uLCD.printf("slew rate:   1");
            uLCD.locate(1,4);
            uLCD.printf("slew rate: 1/2");
            uLCD.locate(1,6);
            uLCD.printf("slew rate: 1/4");
            uLCD.locate(1,8);
            uLCD.printf("slew rate: 1/8");
        }
    }
}