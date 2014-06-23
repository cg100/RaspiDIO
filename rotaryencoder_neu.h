////rotaryencoder.h
////17 pins / 2 pins per encoder = 8 maximum encoders
//#define max_encoders 8
//
//typedef enum _direction
//{
//	DIR_LEFT = -1,
//	DIR_NONE = 0,
//	DIR_RIGHT = 1
//} direction;
//
//struct encoder
//{
//    int pin_a;
//    int pin_b;
//	int  pin_button;
//	volatile int button_pressed;
//    volatile int lastEncoded;
//	volatile direction direction;
//};
//
////Pre-allocate encoder object on the stack so we don't have to
////worry about freeing it
//struct encoder default_encoder;
//
///*
//  Should be run for every rotary encoder you want to control
//  Returns a pointer to the new rotary encoder structer
//  The pointer will be NULL is the function failed for any reason
//*/
//struct encoder *setupencoder(int pin_a, int pin_b, int pin_button);
//
//
//
//direction getDirection();
//int getButtonPressed();
