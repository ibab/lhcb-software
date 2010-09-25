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
         return neuron0x4f5ef60();
     default:
         return 0.;
   }
}

double NNcomb::Value(int index, double* input) {
   input0 = (input[0] - 0)/1;
   input1 = (input[1] - 0)/1;
   input2 = (input[2] - 0)/1;
   input3 = (input[3] - 0)/1;
   input4 = (input[4] - 0)/1;
   switch(index) {
     case 0:
         return neuron0x4f5ef60();
     default:
         return 0.;
   }
}

double NNcomb::neuron0x4ce8ef0() {
   return input0;
}

double NNcomb::neuron0x4ce9200() {
   return input1;
}

double NNcomb::neuron0x4ce9510() {
   return input2;
}

double NNcomb::neuron0x4b26920() {
   return input3;
}

double NNcomb::neuron0x4b26c60() {
   return input4;
}

double NNcomb::input0x4b26fa0() {
   double input = -2.4753;
   input += synapse0x4fe7490();
   input += synapse0x4c2d100();
   input += synapse0x4d22710();
   input += synapse0x4b9fb00();
   input += synapse0x4bf82a0();
   return input;
}

double NNcomb::neuron0x4b26fa0() {
   double input = input0x4b26fa0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x4b272b0() {
   double input = 0.563781;
   input += synapse0x4b275c0();
   input += synapse0x4b27600();
   input += synapse0x4b27640();
   input += synapse0x4b27680();
   input += synapse0x4b276c0();
   return input;
}

double NNcomb::neuron0x4b272b0() {
   double input = input0x4b272b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x4b27700() {
   double input = -0.296715;
   input += synapse0x4b27a10();
   input += synapse0x4b27a50();
   input += synapse0x4b27a90();
   input += synapse0x4b27ad0();
   input += synapse0x4b27b10();
   return input;
}

double NNcomb::neuron0x4b27700() {
   double input = input0x4b27700();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x4b27b50() {
   double input = 3.56;
   input += synapse0x4f5e980();
   input += synapse0x4f5e9c0();
   input += synapse0x4ce98e0();
   input += synapse0x4b1d3c0();
   input += synapse0x4b1d400();
   return input;
}

double NNcomb::neuron0x4b27b50() {
   double input = input0x4b27b50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x4f5eb10() {
   double input = 5.1383;
   input += synapse0x4f5ee20();
   input += synapse0x4f5ee60();
   input += synapse0x4f5eea0();
   input += synapse0x4f5eee0();
   input += synapse0x4f5ef20();
   return input;
}

double NNcomb::neuron0x4f5eb10() {
   double input = input0x4f5eb10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNcomb::input0x4f5ef60() {
   double input = 2.11273;
   input += synapse0x4f5f270();
   input += synapse0x4f5f2b0();
   input += synapse0x4f5f2f0();
   input += synapse0x4f5f330();
   input += synapse0x4f5f370();
   return input;
}

double NNcomb::neuron0x4f5ef60() {
   double input = input0x4f5ef60();
   return (input * 0.492275)+0.412448;
}

double NNcomb::synapse0x4fe7490() {
   return (neuron0x4ce8ef0()*0.778612);
}

double NNcomb::synapse0x4c2d100() {
   return (neuron0x4ce9200()*0.879772);
}

double NNcomb::synapse0x4d22710() {
   return (neuron0x4ce9510()*0.220986);
}

double NNcomb::synapse0x4b9fb00() {
   return (neuron0x4b26920()*0.355196);
}

double NNcomb::synapse0x4bf82a0() {
   return (neuron0x4b26c60()*0.44666);
}

double NNcomb::synapse0x4b275c0() {
   return (neuron0x4ce8ef0()*-3.31285);
}

double NNcomb::synapse0x4b27600() {
   return (neuron0x4ce9200()*-2.3739);
}

double NNcomb::synapse0x4b27640() {
   return (neuron0x4ce9510()*-3.79595);
}

double NNcomb::synapse0x4b27680() {
   return (neuron0x4b26920()*-2.83479);
}

double NNcomb::synapse0x4b276c0() {
   return (neuron0x4b26c60()*-0.902309);
}

double NNcomb::synapse0x4b27a10() {
   return (neuron0x4ce8ef0()*-0.711521);
}

double NNcomb::synapse0x4b27a50() {
   return (neuron0x4ce9200()*-0.918274);
}

double NNcomb::synapse0x4b27a90() {
   return (neuron0x4ce9510()*-0.206268);
}

double NNcomb::synapse0x4b27ad0() {
   return (neuron0x4b26920()*-1.34337);
}

double NNcomb::synapse0x4b27b10() {
   return (neuron0x4b26c60()*-0.785968);
}

double NNcomb::synapse0x4f5e980() {
   return (neuron0x4ce8ef0()*1.90856);
}

double NNcomb::synapse0x4f5e9c0() {
   return (neuron0x4ce9200()*1.58952);
}

double NNcomb::synapse0x4ce98e0() {
   return (neuron0x4ce9510()*1.85569);
}

double NNcomb::synapse0x4b1d3c0() {
   return (neuron0x4b26920()*1.97183);
}

double NNcomb::synapse0x4b1d400() {
   return (neuron0x4b26c60()*0.997909);
}

double NNcomb::synapse0x4f5ee20() {
   return (neuron0x4ce8ef0()*-1.32799);
}

double NNcomb::synapse0x4f5ee60() {
   return (neuron0x4ce9200()*-1.49977);
}

double NNcomb::synapse0x4f5eea0() {
   return (neuron0x4ce9510()*-2.50078);
}

double NNcomb::synapse0x4f5eee0() {
   return (neuron0x4b26920()*-1.1817);
}

double NNcomb::synapse0x4f5ef20() {
   return (neuron0x4b26c60()*-1.68935);
}

double NNcomb::synapse0x4f5f270() {
   return (neuron0x4b26fa0()*4.48385);
}

double NNcomb::synapse0x4f5f2b0() {
   return (neuron0x4b272b0()*0.0625624);
}

double NNcomb::synapse0x4f5f2f0() {
   return (neuron0x4b27700()*-2.15015);
}

double NNcomb::synapse0x4f5f330() {
   return (neuron0x4b27b50()*1.52926);
}

double NNcomb::synapse0x4f5f370() {
   return (neuron0x4f5eb10()*-5.35327);
}

