#ifndef _RGBLED_H_
#define _RGBLED_H_

//#include <math.h>

typedef int RgbColor[3];

const RgbColor LED_COLOR_WHITE = {200,200,200};
const RgbColor LED_COLOR_RED = {200,0,0};
const RgbColor LED_COLOR_YELLOW = {200,200,0};
const RgbColor LED_COLOR_GREEN = {0,200,0};
const RgbColor LED_COLOR_CYAN = {0,200,200};
const RgbColor LED_COLOR_BLUE = {0,0,200};
const RgbColor LED_COLOR_MAGENTA = {200,0,200};

RgbColor ALL_COLORS[7] = {{200,200,200},{200,0,0},{200,200,0},{0,200,0},{0,200,200},{0,0,200},{200,0,200}};

class RgbLed {
  private:
    unsigned long timer = 0;
    const int interval = 100; //100 milli sec
    int current_color = 0;
    int rgb_pin[3];
    int rgb[3]; //current color
    int dest_rgb[3]; //destination color, for fading to
    bool flag_rotate_color = 0;
    bool flag_blink = 0;
    bool flag_persist_color = 0; //will show previous color after blink finishes
    int status = 0; //on or off
    int blink_count = 0;
    const int blink_total = 6; // # of times/2 to blink
    void interpolateColor(); // by increment
    void interpolateColorDiff(); // by diff/10
    void writeOutput();
    bool finishInterpolateColor();
    void blinkOn();
    void blinkOff();
    void setDestColor(int r, int g, int b);

  public:
    RgbLed(int rp, int gp, int bp);
    void setOutputPin(const int *p);
    void setColor(const RgbColor c, int persist);
    void setColor(const RgbColor c);
    void setColor(int r, int g, int b);
    void fadeColor(const RgbColor c);
    
    void rotateColor();
    void blinkColor(const RgbColor c);
    void blinkColor(int r, int g, int b);
    void off();
    void execute();
    void getRandomColor(int *c);
};

RgbLed::RgbLed(int rp, int gp, int bp) {
    rgb_pin[0] = rp;
    rgb_pin[1] = gp;
    rgb_pin[2] = bp;
    pinMode(rp, OUTPUT);
    pinMode(gp, OUTPUT);
    pinMode(bp, OUTPUT);
    return;
}
void RgbLed::setColor(const RgbColor c, int persist) {
    if(persist) {
        this->flag_persist_color = 1;
        this->setDestColor(int(c[0]),int(c[1]),int(c[2]));
    }
    this->setColor(c);
    return;
}
void RgbLed::setColor(const RgbColor c) {
    this->status = 1;
    rgb[0] = int(c[0]);
    rgb[1] = int(c[1]);
    rgb[2] = int(c[2]);
    this->writeOutput();    
    return;
}
void RgbLed::setColor(int r, int g, int b) {
    this->status = 1;
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    this->writeOutput();
    return;
}
void RgbLed::setDestColor(int r, int g, int b) {
    dest_rgb[0] = r;
    dest_rgb[1] = g;
    dest_rgb[2] = b;
    return;
}
void RgbLed::writeOutput() {
    analogWrite(rgb_pin[0], rgb[0]); 
    analogWrite(rgb_pin[1], rgb[1]); 
    analogWrite(rgb_pin[2], rgb[2]);
    
    //Serial.println(String(rgb_pin[0]) + " " + String(rgb[0]));
    //Serial.println(String(rgb_pin[1]) + " " + String(rgb[1]));
    //Serial.println(String(rgb_pin[2]) + " " + String(rgb[2]));
}
void RgbLed::rotateColor() {
    this->status = 1;
    this->flag_rotate_color = 1;
    this->current_color++;
    this->current_color = this->current_color%7;    
    this->setDestColor(ALL_COLORS[this->current_color][0],ALL_COLORS[this->current_color][1],ALL_COLORS[this->current_color][2]);
    return;
}
void RgbLed::blinkColor(const RgbColor c) {
    this->blinkColor(c[0],c[1],c[2]);
}
void RgbLed::blinkColor(int r, int g, int b) {
    this->blinkOff();
    this->status = 1;
    this->flag_blink = 1;
    this->blink_count = 0;
    this->setColor(r,g,b);
}
void RgbLed::interpolateColor() {
    int n = 20;
    int diff0 = this->dest_rgb[0] - this->rgb[0];
    int diff1 = this->dest_rgb[1] - this->rgb[1];
    int diff2 = this->dest_rgb[2] - this->rgb[2];

    int c0; int c1; int c2;
    if (abs(diff0)>n) {
        diff0 = diff0>0?n:-n;
        c0 = this->rgb[0] + diff0;
    } else {
        c0 = this->dest_rgb[0];
    }
    if (abs(diff1)>n) {
        diff1 = diff1>0?n:-n;
        c1 = this->rgb[1] + diff1;
    } else {
        c1 = this->dest_rgb[1];
    }
    if (abs(diff2)>n) {
        diff2 = diff2>0?n:-n;
        c2 = this->rgb[2] + diff2;
    } else {
        c2 = this->dest_rgb[2];
    }

    this->setColor(c0,c1,c2);
}
void RgbLed::interpolateColorDiff() {
    float diff0 = float(this->dest_rgb[0] - this->rgb[0])/2;
    float diff1 = float(this->dest_rgb[1] - this->rgb[1])/2;
    float diff2 = float(this->dest_rgb[2] - this->rgb[2])/2;

    diff0 = diff0>0?ceil(diff0):floor(diff0);
    diff1 = diff1>0?ceil(diff1):floor(diff1);
    diff2 = diff2>0?ceil(diff2):floor(diff2);

    int c0 = this->rgb[0] + int(diff0);
    int c1 = this->rgb[1] + int(diff1);
    int c2 = this->rgb[2] + int(diff2);

    this->setColor(c0,c1,c2);
}
bool RgbLed::finishInterpolateColor() {    
    if (this->dest_rgb[0] != this->rgb[0] || this->dest_rgb[1] != this->rgb[1] || this->dest_rgb[2] != this->rgb[2]) {
        //Serial.println("current " + String(this->rgb[0]) + " " + String(this->rgb[1]) + " " + String(this->rgb[2])); 
        return false;    
    } else {
        return true;
    }    
}
void RgbLed::off() {
    this->flag_blink = this->flag_rotate_color = this->flag_persist_color = 0;
    rgb[0] = rgb[1] = rgb[2] = 0;
    dest_rgb[0] = dest_rgb[1] = dest_rgb[2] = 0;
    this->blinkOff();
}
void RgbLed::blinkOff() {
    this->status = 0;
    analogWrite(rgb_pin[0], 0); 
    analogWrite(rgb_pin[1], 0); 
    analogWrite(rgb_pin[2], 0);
}
void RgbLed::blinkOn() {
    this->status = 1;
    analogWrite(rgb_pin[0], rgb[0]); 
    analogWrite(rgb_pin[1], rgb[1]); 
    analogWrite(rgb_pin[2], rgb[2]);
}

void RgbLed::execute() {
    if (millis() - this->timer > this->interval) {
        this->timer = millis();

        if (this->flag_blink) {
            if (this->blink_count < blink_total) {
                if (this->status) {
                    this->blinkOff();
                } else {
                    this->blinkOn();
                }
            } else if (this->blink_count < blink_total + 6) {
                this->blinkOff();                
            }  else {
                this->flag_blink = 0;
            }
            this->blink_count++;
        } else if (this->flag_rotate_color) {
            if (!this->finishInterpolateColor()){
                this->interpolateColor();
                //this->interpolateColorDiff(); //faster rotation
            } else {
                this->rotateColor();
            }
        } else if (this->flag_persist_color) {
            if (this->status == 0) {
                this->setColor(dest_rgb[0],dest_rgb[1],dest_rgb[2]);
            }
        }
        
    }
    return;
}

//assign one of the main colors to array that was passed in
void RgbLed::getRandomColor(int *c) {
    int n = random(7);
    c[0] = ALL_COLORS[n][0];
    c[1] = ALL_COLORS[n][1];
    c[2] = ALL_COLORS[n][2];
}


#endif


