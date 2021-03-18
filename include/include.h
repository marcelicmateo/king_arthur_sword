#pragma once


#define PIR_SENSOR D1 // human sensor
#define INFRARED_OBSTICLE_SENSOR D2 // sword sensor
#define STEPER_MOTOR D3 // steper control



bool all_pass{false}; //defines user unlock


unsigned char probability_to_pass{20}; // probability to pass
