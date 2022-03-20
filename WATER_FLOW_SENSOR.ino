/*  Water flow sensor
 *  PWM To Analog
 *  YTU Racing
 *  
 *  Programmed for YF-G1 by LOUCHEN ZM
 */
#define DEBUG true //TEST MODE TO OUT PRINTS

#define OUTPUT_MODE 0 //0 for L/min, 1 for L/hr.


//######### SENSOR SECTION
int SENS_MIN_MAX[] = {0, 100}; //Minimum and maximum can read litre/minute. 

//Sensor pins must be interrupt-supported and pwm.
int sensorPINS[] = {2, 3}; //Sensor Input PIN --- LEFT OF CAR, RIGHT OF CAR

//Output pin should support analog output.
int outputPINS[] = {4, 5}; //Sensor Output PIN --- LEFT OF CAR, RIGHT OF CAR
//#########



//######### CALCULATION SECTION
#if OUTPUT_MODE
  #define pulse_freQ 1 //Pulse frequency.
  unsigned int l_hour[] = {0, 0}; //Calculated litre/hour.
#endif
volatile int flow_frequency[] = {0, 0}; //Flow frequency from reading.
unsigned long curTime; //Current tick.
unsigned long cloopTime; //Old tick.
//#########


//######### FILTER SECTION
#define FILTER_OPEN true
#define FILTERHASSASIYET 16
//#########














void setup()
{
   for(int i = 0; i <= 1; i++){
      pinMode(sensorPINS[i], INPUT); 
      pinMode(outputPINS[i], OUTPUT);
      digitalWrite(sensorPINS[i], HIGH);
      if(i = 0)   attachInterrupt(digitalPinToInterrupt(sensorPINS[i]), increaseFlowLeft, RISING);
      else    attachInterrupt(digitalPinToInterrupt(sensorPINS[i]), increaseFlowRight, RISING);
   }
   #if DEBUG
    Serial.begin(9600);
   #endif
   interrupts(); // Interrupt ignition
   curTime = millis();
   cloopTime = curTime;
}
void loop ()
{
   curTime = millis();
   if(curTime >= (cloopTime + 1))
   {
      cloopTime = curTime;
      for(int i = 0; i <= 1; i++){
        #if OUTPUT_MODE
          l_hour[i] = sensorTOLPERHRS(flow_frequency[i]);
          unsigned int OUT = map(l_hour, 0, sensorTOLPERHRS(SENS_MIN_MAX[1]), 0, 255);
        #else
          unsigned int OUT = map(flow_frequency[i], SENS_MIN_MAX[0], SENS_MIN_MAX[1], 0, 1023);
        #endif
        #if FILTER_OPEN
        float _buffer;
        for(int i = 0; i <= FILTERHASSASIYET; i++){
          _buffer += OUT;
        }
        _buffer = _buffer/FILTERHASSASIYET;
        analogWrite(outputPINS[i], _buffer);
        _buffer = 0;
        #else
        analogWrite(outputPINS[i], OUT);
        #endif
        #if DEBUG
          #if OUTPUT_MODE
            if(i == 0)  Serial.print("Left: ");
            else  Serial.print("Right: ");
            Serial.print(l_hour[i], DEC);
            Serial.println(" L/Hr");
          #else
            if(i == 0)  Serial.print("Left: ");
            else Serial.print("Right: ");
            Serial.print(flow_frequency[i], DEC);
            Serial.println(" L/min");
          #endif
        #endif
        flow_frequency[i] = 0;
      }
   }
}
void increaseFlowLeft(){flow_frequency[0]++;}
void increaseFlowRight(){flow_frequency[1]++;}
#if OUTPUT_MODE
unsigned int sensorTOLPERHRS(unsigned int DATA){return (DATA * 60 / pulse_freQ);}
#endif
