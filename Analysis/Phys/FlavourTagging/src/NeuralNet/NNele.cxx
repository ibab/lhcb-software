#include "NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x860fb40();
     default:
         return 0.;
   }
}

double NNele::neuron0x8614940() {
   return input0;
}

double NNele::neuron0x8614ad0() {
   return input1;
}

double NNele::neuron0x8614ca8() {
   return input2;
}

double NNele::neuron0x8614ea0() {
   return input3;
}

double NNele::input0x86151b8() {
   double input = 0.25893;
   input += synapse0x8614908();
   input += synapse0x8615348();
   input += synapse0x8615370();
   input += synapse0x8615398();
   return input;
}

double NNele::neuron0x86151b8() {
   double input = input0x86151b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x86153c0() {
   double input = -0.290994;
   input += synapse0x8615598();
   input += synapse0x86155c0();
   input += synapse0x86155e8();
   input += synapse0x8615610();
   return input;
}

double NNele::neuron0x86153c0() {
   double input = input0x86153c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8615638() {
   double input = -0.146656;
   input += synapse0x8615810();
   input += synapse0x8615838();
   input += synapse0x8615860();
   input += synapse0x8615888();
   return input;
}

double NNele::neuron0x8615638() {
   double input = input0x8615638();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x86158b0() {
   double input = -1.4537;
   input += synapse0x8615a88();
   input += synapse0x8615ab0();
   input += synapse0x8615ad8();
   input += synapse0x8615b00();
   return input;
}

double NNele::neuron0x86158b0() {
   double input = input0x86158b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8615b28() {
   double input = -1.53091;
   input += synapse0x8615d20();
   input += synapse0x8615dd0();
   input += synapse0x8615df8();
   input += synapse0x8615e20();
   return input;
}

double NNele::neuron0x8615b28() {
   double input = input0x8615b28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8615e48() {
   double input = 0.366459;
   input += synapse0x8615f68();
   input += synapse0x860fac8();
   input += synapse0x860faf0();
   input += synapse0x860fb18();
   return input;
}

double NNele::neuron0x8615e48() {
   double input = input0x8615e48();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x860fb40() {
   double input = -0.947684;
   input += synapse0x860fd38();
   input += synapse0x860fd60();
   input += synapse0x860fd88();
   input += synapse0x860fdb0();
   input += synapse0x860fdd8();
   input += synapse0x860fe00();
   return input;
}

double NNele::neuron0x860fb40() {
   double input = input0x860fb40();
   return (input * 0.457969)+0.70066;
}

double NNele::synapse0x8614908() {
   return (neuron0x8614940()*0.685476);
}

double NNele::synapse0x8615348() {
   return (neuron0x8614ad0()*-0.996518);
}

double NNele::synapse0x8615370() {
   return (neuron0x8614ca8()*-1.65747);
}

double NNele::synapse0x8615398() {
   return (neuron0x8614ea0()*-1.5602);
}

double NNele::synapse0x8615598() {
   return (neuron0x8614940()*1.41223);
}

double NNele::synapse0x86155c0() {
   return (neuron0x8614ad0()*0.825597);
}

double NNele::synapse0x86155e8() {
   return (neuron0x8614ca8()*-0.285219);
}

double NNele::synapse0x8615610() {
   return (neuron0x8614ea0()*0.797488);
}

double NNele::synapse0x8615810() {
   return (neuron0x8614940()*0.368444);
}

double NNele::synapse0x8615838() {
   return (neuron0x8614ad0()*0.0308008);
}

double NNele::synapse0x8615860() {
   return (neuron0x8614ca8()*-0.403894);
}

double NNele::synapse0x8615888() {
   return (neuron0x8614ea0()*-0.0970016);
}

double NNele::synapse0x8615a88() {
   return (neuron0x8614940()*1.14647);
}

double NNele::synapse0x8615ab0() {
   return (neuron0x8614ad0()*2.84673);
}

double NNele::synapse0x8615ad8() {
   return (neuron0x8614ca8()*-1.63894);
}

double NNele::synapse0x8615b00() {
   return (neuron0x8614ea0()*3.98964);
}

double NNele::synapse0x8615d20() {
   return (neuron0x8614940()*-3.92155);
}

double NNele::synapse0x8615dd0() {
   return (neuron0x8614ad0()*1.99957);
}

double NNele::synapse0x8615df8() {
   return (neuron0x8614ca8()*4.56351);
}

double NNele::synapse0x8615e20() {
   return (neuron0x8614ea0()*6.56425);
}

double NNele::synapse0x8615f68() {
   return (neuron0x8614940()*-0.445644);
}

double NNele::synapse0x860fac8() {
   return (neuron0x8614ad0()*-0.835114);
}

double NNele::synapse0x860faf0() {
   return (neuron0x8614ca8()*-0.171866);
}

double NNele::synapse0x860fb18() {
   return (neuron0x8614ea0()*0.454836);
}

double NNele::synapse0x860fd38() {
   return (neuron0x86151b8()*1.46836);
}

double NNele::synapse0x860fd60() {
   return (neuron0x86153c0()*0.476761);
}

double NNele::synapse0x860fd88() {
   return (neuron0x8615638()*-0.217148);
}

double NNele::synapse0x860fdb0() {
   return (neuron0x86158b0()*-0.728443);
}

double NNele::synapse0x860fdd8() {
   return (neuron0x8615b28()*1.44852);
}

double NNele::synapse0x860fe00() {
   return (neuron0x8615e48()*-0.500875);
}

