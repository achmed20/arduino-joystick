ThumbState_t thumbSt;

const bool DEBUG = false;  // set to true to debug the raw values

int xPin = A2;
int yPin = A3;
double xZero, yZero;
float xValue, yValue;
float xValueSq, yValueSq;
double xMax = 100;
double yMax = 100;
double xMin = -100;
double yMin = -100;
int xDeadzone = 55;  // smaller values will be set to 0
int yDeadzone = 55;  // smaller values will be set to 0
int xMaxRange = 350;  // bigger values will be set to 1
int yMaxRange = 350;  // bigger values will be set to 1

#define epsilon 0.0000001

inline float sgn(float input)
{
    float output = 1.0f;
    if (input < 0.0) {
        output = -1.0f;
    }
    return output;
}

// Simple Stretching map
// mapping a circular disc to a square region
// input: (u,v) coordinates in the circle
// output: (x,y) coordinates in the square
void stretchDiscToSquare(float u, float v, float& x, float& y)
{
    if ( (fabs(u) < epsilon) || (fabs(v) < epsilon))  {
        x = u;
        y = v;
        return;
    } 

    float u2 = u * u;
    float v2 = v * v;
    float r = sqrt(u2 + v2);

    // a trick based on Dave Cline's idea
    // link Peter Shirley's blog
    if (u2 >= v2) {
        float sgnu = sgn(u);
        x = sgnu * r;
        y = sgnu * r * v / u;
    } else {
        float sgnv = sgn(v);
        x = sgnv * r * u / v;
        y = sgnv * r;
    }
    
}

void setup(){
 	pinMode(xPin, INPUT);
 	pinMode(yPin, INPUT);

 	if(DEBUG) {
 		Serial.begin(9600);
 	}

	// calculate neutral position
	xZero = analogRead(xPin);
	yZero = analogRead(yPin);

	thumbSt.xAxis = 0;
	thumbSt.yAxis = 0;
}

void loop(){
	xValue = analogRead(xPin) - xZero;
	yValue = analogRead(yPin) - yZero;

	if(abs(xValue) < xDeadzone) {
		xValue = 0;
	}
	if(abs(yValue) < yDeadzone) {
		yValue = 0;
	}


        
        //auto calibration (circle the stick)
/*        if(xValue > xMax) {
          xMax = xValue;
        }
        if(yValue > yMax) {
          yMax = yValue;
        }
        if(xValue < xMin) {
          xMin = xValue;
        }
        if(yValue < yMin) {
          yMin = yValue;
        }
*/       

        yValue = pow(yValue / yMaxRange, 1)*0.99;
        xValue = pow(xValue / xMaxRange, 1)*0.99;

        if(xValue<-0.99) {
          xValue = -0.99;
        }
        if(yValue<-0.99) {
          yValue = -0.99;
        }

        if(xValue>0.99) {
          xValue = 0.99;
        }
        if(yValue>0.99) {
          yValue = 0.99;
        }

//        xValue = xValue/(1+exp(-0.5));
//        yValue = yValue/(1+exp(-0.5));

        stretchDiscToSquare(xValue, yValue, xValueSq, yValueSq);

        //numbers are sometimes bigger, correct that
        if(xValueSq<-0.99) {
          xValueSq = -0.99;
        }
        if(yValueSq<-0.99) {
          yValueSq = -0.99;
        }

        if(xValueSq>0.99) {
          xValueSq = 0.99;
        }
        if(yValueSq>0.99) {
          yValueSq = 0.99;
        }

	thumbSt.xAxis = (xValueSq) * 32768*-1;
	thumbSt.yAxis = (yValueSq) * 32768*-1;

//	thumbSt.xAxis = map(xValueSq*100, xMax, xMin, -32768, 32768);  // here the axis is inverted
//	thumbSt.yAxis = map(yValueSq*100, yMax, yMin, -32768, 32768);

 	if(DEBUG) {
   
   
 		Serial.print("X: ");
		Serial.println(xValue);
                Serial.print("Y: ");
		Serial.println(yValue);
 		Serial.print("Xq: ");
		Serial.println(xValueSq);
                Serial.print("Yq: ");
		Serial.println(yValueSq);
   
	}

	// Send to USB
	Thumbstick.setState(&thumbSt);
}
