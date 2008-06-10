#include "NNkaon.h"
#include <cmath>

double NNkaon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8bdf210();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x8b73c40() {
   return input0;
}

double NNkaon::neuron0x8b73e18() {
   return input1;
}

double NNkaon::neuron0x8bdee20() {
   return input2;
}

double NNkaon::neuron0x8bdf018() {
   return input3;
}

double NNkaon::input0x8bdf330() {
   double input = -1.2749;
   input += synapse0x85b52a8();
   input += synapse0x8bdf4c0();
   input += synapse0x8bdf4e8();
   input += synapse0x8bdf510();
   return input;
}

double NNkaon::neuron0x8bdf330() {
   double input = input0x8bdf330();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8a6ccb0() {
   double input = 3.40904;
   input += synapse0x8a6ce88();
   input += synapse0x8a6ceb0();
   input += synapse0x8a6ced8();
   input += synapse0x8a6cf00();
   return input;
}

double NNkaon::neuron0x8a6ccb0() {
   double input = input0x8a6ccb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8a6cf28() {
   double input = -2.78199;
   input += synapse0x8a6d100();
   input += synapse0x8a6d128();
   input += synapse0x8a6d150();
   input += synapse0x8a6d178();
   return input;
}

double NNkaon::neuron0x8a6cf28() {
   double input = input0x8a6cf28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8a6d1a0() {
   double input = -0.70988;
   input += synapse0x8a6d378();
   input += synapse0x8a6d3a0();
   input += synapse0x8a6d3c8();
   input += synapse0x8a6d3f0();
   return input;
}

double NNkaon::neuron0x8a6d1a0() {
   double input = input0x8a6d1a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8a6d418() {
   double input = -0.218093;
   input += synapse0x8a6d610();
   input += synapse0x8a6d6c0();
   input += synapse0x8a6d6e8();
   input += synapse0x8a6d710();
   return input;
}

double NNkaon::neuron0x8a6d418() {
   double input = input0x8a6d418();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8a6d738() {
   double input = -1.38985;
   input += synapse0x8a6d8e8();
   input += synapse0x8a6d910();
   input += synapse0x8a6d938();
   input += synapse0x8a6d960();
   return input;
}

double NNkaon::neuron0x8a6d738() {
   double input = input0x8a6d738();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8bdf210() {
   double input = -1.80931;
   input += synapse0x8a6d9d0();
   input += synapse0x8a6d9f8();
   input += synapse0x8a6da20();
   input += synapse0x8a6da48();
   input += synapse0x8a6da70();
   input += synapse0x8a6da98();
   return input;
}

double NNkaon::neuron0x8bdf210() {
   double input = input0x8bdf210();
   return (input * 0.475059)+0.655946;
}

double NNkaon::synapse0x85b52a8() {
   return (neuron0x8b73c40()*1.10611);
}

double NNkaon::synapse0x8bdf4c0() {
   return (neuron0x8b73e18()*-0.496052);
}

double NNkaon::synapse0x8bdf4e8() {
   return (neuron0x8bdee20()*-2.43153);
}

double NNkaon::synapse0x8bdf510() {
   return (neuron0x8bdf018()*0.230996);
}

double NNkaon::synapse0x8a6ce88() {
   return (neuron0x8b73c40()*-1.81044);
}

double NNkaon::synapse0x8a6ceb0() {
   return (neuron0x8b73e18()*0.902393);
}

double NNkaon::synapse0x8a6ced8() {
   return (neuron0x8bdee20()*3.79545);
}

double NNkaon::synapse0x8a6cf00() {
   return (neuron0x8bdf018()*-1.27602);
}

double NNkaon::synapse0x8a6d100() {
   return (neuron0x8b73c40()*-0.392328);
}

double NNkaon::synapse0x8a6d128() {
   return (neuron0x8b73e18()*1.47046);
}

double NNkaon::synapse0x8a6d150() {
   return (neuron0x8bdee20()*9.64528);
}

double NNkaon::synapse0x8a6d178() {
   return (neuron0x8bdf018()*6.86892);
}

double NNkaon::synapse0x8a6d378() {
   return (neuron0x8b73c40()*0.438812);
}

double NNkaon::synapse0x8a6d3a0() {
   return (neuron0x8b73e18()*0.0685656);
}

double NNkaon::synapse0x8a6d3c8() {
   return (neuron0x8bdee20()*-4.19235);
}

double NNkaon::synapse0x8a6d3f0() {
   return (neuron0x8bdf018()*-1.5408);
}

double NNkaon::synapse0x8a6d610() {
   return (neuron0x8b73c40()*1.81269);
}

double NNkaon::synapse0x8a6d6c0() {
   return (neuron0x8b73e18()*-1.50812);
}

double NNkaon::synapse0x8a6d6e8() {
   return (neuron0x8bdee20()*-2.97862);
}

double NNkaon::synapse0x8a6d710() {
   return (neuron0x8bdf018()*-2.16387);
}

double NNkaon::synapse0x8a6d8e8() {
   return (neuron0x8b73c40()*0.785187);
}

double NNkaon::synapse0x8a6d910() {
   return (neuron0x8b73e18()*-0.6785);
}

double NNkaon::synapse0x8a6d938() {
   return (neuron0x8bdee20()*-0.0889208);
}

double NNkaon::synapse0x8a6d960() {
   return (neuron0x8bdf018()*-0.27692);
}

double NNkaon::synapse0x8a6d9d0() {
   return (neuron0x8bdf330()*-0.257856);
}

double NNkaon::synapse0x8a6d9f8() {
   return (neuron0x8a6ccb0()*2.08416);
}

double NNkaon::synapse0x8a6da20() {
   return (neuron0x8a6cf28()*0.538422);
}

double NNkaon::synapse0x8a6da48() {
   return (neuron0x8a6d1a0()*-1.35206);
}

double NNkaon::synapse0x8a6da70() {
   return (neuron0x8a6d418()*1.63683);
}

double NNkaon::synapse0x8a6da98() {
   return (neuron0x8a6d738()*-3.80197);
}

