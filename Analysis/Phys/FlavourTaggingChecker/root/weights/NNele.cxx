#include "weights/NNele.h"
#include <cmath>

double NNele::Value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x9fae5a8();
     default:
         return 0.;
   }
}

double NNele::Value(int index, double* input) {
   input0 = (input[0] - 0)/1;
   input1 = (input[1] - 0)/1;
   input2 = (input[2] - 0)/1;
   input3 = (input[3] - 0)/1;
   input4 = (input[4] - 0)/1;
   input5 = (input[5] - 0)/1;
   switch(index) {
     case 0:
         return neuron0x9fae5a8();
     default:
         return 0.;
   }
}

double NNele::neuron0x9fada80() {
   return input0;
}

double NNele::neuron0x9fadbc8() {
   return input1;
}

double NNele::neuron0x9faddc0() {
   return input2;
}

double NNele::neuron0x9fadfb8() {
   return input3;
}

double NNele::neuron0x9fae1b0() {
   return input4;
}

double NNele::neuron0x9fae3a8() {
   return input5;
}

double NNele::input0x9fae6b0() {
   double input = 0.388264;
   input += synapse0x9fada08();
   input += synapse0x9fae840();
   input += synapse0x9fae868();
   input += synapse0x9fae890();
   input += synapse0x9fae8b8();
   input += synapse0x9fae8e0();
   return input;
}

double NNele::neuron0x9fae6b0() {
   double input = input0x9fae6b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9fae908() {
   double input = 0.78806;
   input += synapse0x9faeae0();
   input += synapse0x9faeb08();
   input += synapse0x9faeb30();
   input += synapse0x9faeb58();
   input += synapse0x9faeb80();
   input += synapse0x9faeba8();
   return input;
}

double NNele::neuron0x9fae908() {
   double input = input0x9fae908();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9faebd0() {
   double input = 0.625781;
   input += synapse0x9faeda8();
   input += synapse0x9faedd0();
   input += synapse0x9faedf8();
   input += synapse0x9faee20();
   input += synapse0x9faee48();
   input += synapse0x9faeef8();
   return input;
}

double NNele::neuron0x9faebd0() {
   double input = input0x9faebd0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9faef20() {
   double input = -0.310711;
   input += synapse0x9faf0b0();
   input += synapse0x9faf0d8();
   input += synapse0x9faf100();
   input += synapse0x9faf128();
   input += synapse0x9faf150();
   input += synapse0x9faf178();
   return input;
}

double NNele::neuron0x9faef20() {
   double input = input0x9faef20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9faf1a0() {
   double input = -0.250735;
   input += synapse0x9faf378();
   input += synapse0x9faf3a0();
   input += synapse0x9faf3c8();
   input += synapse0x9faf3f0();
   input += synapse0x9faf418();
   input += synapse0x9faf440();
   return input;
}

double NNele::neuron0x9faf1a0() {
   double input = input0x9faf1a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9faf468() {
   double input = 0.712877;
   input += synapse0x9faf660();
   input += synapse0x9faf688();
   input += synapse0x9faf6b0();
   input += synapse0x9faee70();
   input += synapse0x9faee98();
   input += synapse0x9faeec0();
   return input;
}

double NNele::neuron0x9faf468() {
   double input = input0x9faf468();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9faf7e0() {
   double input = 0.0870427;
   input += synapse0x9faf9d8();
   input += synapse0x9fafa00();
   input += synapse0x9fafa28();
   input += synapse0x9fafa50();
   input += synapse0x9fafa78();
   input += synapse0x9fafaa0();
   return input;
}

double NNele::neuron0x9faf7e0() {
   double input = input0x9faf7e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9fafac8() {
   double input = 2.22077;
   input += synapse0x9fafcc0();
   input += synapse0x9fafce8();
   input += synapse0x9fafd10();
   input += synapse0x9fafd38();
   input += synapse0x9fafd60();
   input += synapse0x9fafd88();
   return input;
}

double NNele::neuron0x9fafac8() {
   double input = input0x9fafac8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9fafdb0() {
   double input = -0.386289;
   input += synapse0x9faffa8();
   input += synapse0x9faffd0();
   input += synapse0x9fafff8();
   input += synapse0x9fb0020();
   input += synapse0x9fb0048();
   input += synapse0x9fb0070();
   return input;
}

double NNele::neuron0x9fafdb0() {
   double input = input0x9fafdb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9fb0098() {
   double input = 0.520439;
   input += synapse0x9fb0290();
   input += synapse0x9fb02b8();
   input += synapse0x9fb02e0();
   input += synapse0x9fb0308();
   input += synapse0x9fb0330();
   input += synapse0x9fb0358();
   return input;
}

double NNele::neuron0x9fb0098() {
   double input = input0x9fb0098();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x9fae5a8() {
   double input = 0.803893;
   input += synapse0x9fb0478();
   input += synapse0x9fb04a0();
   input += synapse0x9fb04c8();
   input += synapse0x9fb04f0();
   input += synapse0x9fb0518();
   input += synapse0x9fb2840();
   input += synapse0x9f47570();
   input += synapse0x9f474f8();
   input += synapse0x9f6ea58();
   input += synapse0x9fada30();
   return input;
}

double NNele::neuron0x9fae5a8() {
   double input = input0x9fae5a8();
   return (input * 0.458168)+0.700204;
}

double NNele::synapse0x9fada08() {
   return (neuron0x9fada80()*-0.514431);
}

double NNele::synapse0x9fae840() {
   return (neuron0x9fadbc8()*-0.0889709);
}

double NNele::synapse0x9fae868() {
   return (neuron0x9faddc0()*-0.136541);
}

double NNele::synapse0x9fae890() {
   return (neuron0x9fadfb8()*-0.521373);
}

double NNele::synapse0x9fae8b8() {
   return (neuron0x9fae1b0()*-0.30546);
}

double NNele::synapse0x9fae8e0() {
   return (neuron0x9fae3a8()*0.424198);
}

double NNele::synapse0x9faeae0() {
   return (neuron0x9fada80()*0.506792);
}

double NNele::synapse0x9faeb08() {
   return (neuron0x9fadbc8()*-1.32112);
}

double NNele::synapse0x9faeb30() {
   return (neuron0x9faddc0()*-0.401179);
}

double NNele::synapse0x9faeb58() {
   return (neuron0x9fadfb8()*0.335645);
}

double NNele::synapse0x9faeb80() {
   return (neuron0x9fae1b0()*-0.0615436);
}

double NNele::synapse0x9faeba8() {
   return (neuron0x9fae3a8()*1.17405);
}

double NNele::synapse0x9faeda8() {
   return (neuron0x9fada80()*-1.3133);
}

double NNele::synapse0x9faedd0() {
   return (neuron0x9fadbc8()*0.60016);
}

double NNele::synapse0x9faedf8() {
   return (neuron0x9faddc0()*1.41002);
}

double NNele::synapse0x9faee20() {
   return (neuron0x9fadfb8()*-0.873563);
}

double NNele::synapse0x9faee48() {
   return (neuron0x9fae1b0()*1.46053);
}

double NNele::synapse0x9faeef8() {
   return (neuron0x9fae3a8()*-0.456242);
}

double NNele::synapse0x9faf0b0() {
   return (neuron0x9fada80()*0.0278191);
}

double NNele::synapse0x9faf0d8() {
   return (neuron0x9fadbc8()*-0.383552);
}

double NNele::synapse0x9faf100() {
   return (neuron0x9faddc0()*-0.65674);
}

double NNele::synapse0x9faf128() {
   return (neuron0x9fadfb8()*0.678775);
}

double NNele::synapse0x9faf150() {
   return (neuron0x9fae1b0()*0.440377);
}

double NNele::synapse0x9faf178() {
   return (neuron0x9fae3a8()*-0.252756);
}

double NNele::synapse0x9faf378() {
   return (neuron0x9fada80()*0.940463);
}

double NNele::synapse0x9faf3a0() {
   return (neuron0x9fadbc8()*-0.982427);
}

double NNele::synapse0x9faf3c8() {
   return (neuron0x9faddc0()*0.682563);
}

double NNele::synapse0x9faf3f0() {
   return (neuron0x9fadfb8()*-3.0113);
}

double NNele::synapse0x9faf418() {
   return (neuron0x9fae1b0()*-0.730284);
}

double NNele::synapse0x9faf440() {
   return (neuron0x9fae3a8()*0.70079);
}

double NNele::synapse0x9faf660() {
   return (neuron0x9fada80()*-0.390308);
}

double NNele::synapse0x9faf688() {
   return (neuron0x9fadbc8()*-0.0614691);
}

double NNele::synapse0x9faf6b0() {
   return (neuron0x9faddc0()*0.301282);
}

double NNele::synapse0x9faee70() {
   return (neuron0x9fadfb8()*-0.642056);
}

double NNele::synapse0x9faee98() {
   return (neuron0x9fae1b0()*-0.237186);
}

double NNele::synapse0x9faeec0() {
   return (neuron0x9fae3a8()*-0.11343);
}

double NNele::synapse0x9faf9d8() {
   return (neuron0x9fada80()*-0.0733358);
}

double NNele::synapse0x9fafa00() {
   return (neuron0x9fadbc8()*-0.241295);
}

double NNele::synapse0x9fafa28() {
   return (neuron0x9faddc0()*0.240758);
}

double NNele::synapse0x9fafa50() {
   return (neuron0x9fadfb8()*0.485829);
}

double NNele::synapse0x9fafa78() {
   return (neuron0x9fae1b0()*-0.835082);
}

double NNele::synapse0x9fafaa0() {
   return (neuron0x9fae3a8()*0.251526);
}

double NNele::synapse0x9fafcc0() {
   return (neuron0x9fada80()*3.1026);
}

double NNele::synapse0x9fafce8() {
   return (neuron0x9fadbc8()*-0.800979);
}

double NNele::synapse0x9fafd10() {
   return (neuron0x9faddc0()*-0.553857);
}

double NNele::synapse0x9fafd38() {
   return (neuron0x9fadfb8()*0.471128);
}

double NNele::synapse0x9fafd60() {
   return (neuron0x9fae1b0()*-0.0472295);
}

double NNele::synapse0x9fafd88() {
   return (neuron0x9fae3a8()*0.0310812);
}

double NNele::synapse0x9faffa8() {
   return (neuron0x9fada80()*-0.0842626);
}

double NNele::synapse0x9faffd0() {
   return (neuron0x9fadbc8()*-0.231481);
}

double NNele::synapse0x9fafff8() {
   return (neuron0x9faddc0()*0.309645);
}

double NNele::synapse0x9fb0020() {
   return (neuron0x9fadfb8()*-0.613858);
}

double NNele::synapse0x9fb0048() {
   return (neuron0x9fae1b0()*0.580824);
}

double NNele::synapse0x9fb0070() {
   return (neuron0x9fae3a8()*-0.912212);
}

double NNele::synapse0x9fb0290() {
   return (neuron0x9fada80()*-0.162394);
}

double NNele::synapse0x9fb02b8() {
   return (neuron0x9fadbc8()*-0.753731);
}

double NNele::synapse0x9fb02e0() {
   return (neuron0x9faddc0()*-2.79063);
}

double NNele::synapse0x9fb0308() {
   return (neuron0x9fadfb8()*3.40102);
}

double NNele::synapse0x9fb0330() {
   return (neuron0x9fae1b0()*-1.07433);
}

double NNele::synapse0x9fb0358() {
   return (neuron0x9fae3a8()*1.90255);
}

double NNele::synapse0x9fb0478() {
   return (neuron0x9fae6b0()*0.87548);
}

double NNele::synapse0x9fb04a0() {
   return (neuron0x9fae908()*3.4408);
}

double NNele::synapse0x9fb04c8() {
   return (neuron0x9faebd0()*-1.3252);
}

double NNele::synapse0x9fb04f0() {
   return (neuron0x9faef20()*-0.30573);
}

double NNele::synapse0x9fb0518() {
   return (neuron0x9faf1a0()*-2.52247);
}

double NNele::synapse0x9fb2840() {
   return (neuron0x9faf468()*1.50517);
}

double NNele::synapse0x9f47570() {
   return (neuron0x9faf7e0()*1.66658);
}

double NNele::synapse0x9f474f8() {
   return (neuron0x9fafac8()*-2.48164);
}

double NNele::synapse0x9f6ea58() {
   return (neuron0x9fafdb0()*0.380575);
}

double NNele::synapse0x9fada30() {
   return (neuron0x9fb0098()*-2.59005);
}

