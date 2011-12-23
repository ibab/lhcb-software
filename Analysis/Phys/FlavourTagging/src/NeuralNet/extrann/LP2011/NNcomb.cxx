#include "NNcomb.h"
#include <cmath>

double NNcomb::Value(int index,double in0,double in1,double in2,double in3,double in4) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x21972500();
     default:
         return 0.;
   }
}


double NNcomb::neuron0x2167d900() {
   return input0;
}

double NNcomb::neuron0x21862790() {
   return input1;
}

double NNcomb::neuron0x218628f0() {
   return input2;
}

double NNcomb::neuron0x21862a50() {
   return input3;
}

double NNcomb::neuron0x217fac10() {
   return input4;
}

double NNcomb::input0x217fad70() {
   double input = 0.799793;
   input += synapse0x20fe6e80();
   input += synapse0x21681c40();
   input += synapse0x215275b0();
   input += synapse0x217faed0();
   input += synapse0x217faf10();
   return input;
}

double NNcomb::neuron0x217fad70() {
   double input = input0x217fad70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x217faf50() {
   double input = 1.92054;
   input += synapse0x217fb0b0();
   input += synapse0x217fb0f0();
   input += synapse0x217fb130();
   input += synapse0x217fb170();
   input += synapse0x217fb1b0();
   return input;
}

double NNcomb::neuron0x217faf50() {
   double input = input0x217faf50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x21677d50() {
   double input = 0.722772;
   input += synapse0x21677eb0();
   input += synapse0x21677ef0();
   input += synapse0x21677f30();
   input += synapse0x21677f70();
   input += synapse0x21677fb0();
   return input;
}

double NNcomb::neuron0x21677d50() {
   double input = input0x21677d50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x21677ff0() {
   double input = 0.533284;
   input += synapse0x21678150();
   input += synapse0x21678190();
   input += synapse0x21673da0();
   input += synapse0x210ea0b0();
   input += synapse0x210ea0f0();
   return input;
}

double NNcomb::neuron0x21677ff0() {
   double input = input0x21677ff0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x21972260() {
   double input = -2.11825;
   input += synapse0x219723c0();
   input += synapse0x21972400();
   input += synapse0x21972440();
   input += synapse0x21972480();
   input += synapse0x219724c0();
   return input;
}

double NNcomb::neuron0x21972260() {
   double input = input0x21972260();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x21972500() {
   double input = -0.985014;
   input += synapse0x21972660();
   input += synapse0x219726a0();
   input += synapse0x219726e0();
   input += synapse0x21972720();
   input += synapse0x21972760();
   return input;
}

double NNcomb::neuron0x21972500() {
   double input = input0x21972500();
   return (input * 0.499976)+0.495056;
}

double NNcomb::synapse0x20fe6e80() {
   return (neuron0x2167d900()*-0.736405);
}

double NNcomb::synapse0x21681c40() {
   return (neuron0x21862790()*-0.290388);
}

double NNcomb::synapse0x215275b0() {
   return (neuron0x218628f0()*-1.04718);
}

double NNcomb::synapse0x217faed0() {
   return (neuron0x21862a50()*-0.564108);
}

double NNcomb::synapse0x217faf10() {
   return (neuron0x217fac10()*-0.170423);
}

double NNcomb::synapse0x217fb0b0() {
   return (neuron0x2167d900()*-0.666618);
}

double NNcomb::synapse0x217fb0f0() {
   return (neuron0x21862790()*-0.860989);
}

double NNcomb::synapse0x217fb130() {
   return (neuron0x218628f0()*-0.478075);
}

double NNcomb::synapse0x217fb170() {
   return (neuron0x21862a50()*-0.655659);
}

double NNcomb::synapse0x217fb1b0() {
   return (neuron0x217fac10()*-1.00066);
}

double NNcomb::synapse0x21677eb0() {
   return (neuron0x2167d900()*0.36657);
}

double NNcomb::synapse0x21677ef0() {
   return (neuron0x21862790()*0.775455);
}

double NNcomb::synapse0x21677f30() {
   return (neuron0x218628f0()*0.7418);
}

double NNcomb::synapse0x21677f70() {
   return (neuron0x21862a50()*0.322896);
}

double NNcomb::synapse0x21677fb0() {
   return (neuron0x217fac10()*0.345244);
}

double NNcomb::synapse0x21678150() {
   return (neuron0x2167d900()*-0.452512);
}

double NNcomb::synapse0x21678190() {
   return (neuron0x21862790()*-1.02218);
}

double NNcomb::synapse0x21673da0() {
   return (neuron0x218628f0()*-0.0943423);
}

double NNcomb::synapse0x210ea0b0() {
   return (neuron0x21862a50()*-0.72695);
}

double NNcomb::synapse0x210ea0f0() {
   return (neuron0x217fac10()*-1.09895);
}

double NNcomb::synapse0x219723c0() {
   return (neuron0x2167d900()*0.467346);
}

double NNcomb::synapse0x21972400() {
   return (neuron0x21862790()*0.537837);
}

double NNcomb::synapse0x21972440() {
   return (neuron0x218628f0()*0.965898);
}

double NNcomb::synapse0x21972480() {
   return (neuron0x21862a50()*0.983399);
}

double NNcomb::synapse0x219724c0() {
   return (neuron0x217fac10()*0.40155);
}

double NNcomb::synapse0x21972660() {
   return (neuron0x217fad70()*1.98212);
}

double NNcomb::synapse0x219726a0() {
   return (neuron0x217faf50()*2.81695);
}

double NNcomb::synapse0x219726e0() {
   return (neuron0x21677d50()*-0.31119);
}

double NNcomb::synapse0x21972720() {
   return (neuron0x21677ff0()*1.81151);
}

double NNcomb::synapse0x21972760() {
   return (neuron0x21972260()*-3.43063);
}

