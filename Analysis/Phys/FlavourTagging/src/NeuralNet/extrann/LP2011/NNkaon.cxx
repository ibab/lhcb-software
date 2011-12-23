#include "NNkaon.h"
#include <cmath>

double NNkaon::value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x1d471250();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x1e6cfec0() {
   return input0;
}

double NNkaon::neuron0x1e6d0050() {
   return input1;
}

double NNkaon::neuron0x1db972c0() {
   return input2;
}

double NNkaon::neuron0x1db97450() {
   return input3;
}

double NNkaon::neuron0x1d470f30() {
   return input4;
}

double NNkaon::neuron0x1d4710c0() {
   return input5;
}

double NNkaon::input0x1db19450() {
   double input = 0.377351;
   input += synapse0x1e1026e0();
   input += synapse0x1ef9d0b0();
   input += synapse0x1e5f57f0();
   input += synapse0x1f4d6420();
   input += synapse0x1e5f0a80();
   input += synapse0x1e6e5540();
   return input;
}

double NNkaon::neuron0x1db19450() {
   double input = input0x1db19450();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1db195e0() {
   double input = -0.70225;
   input += synapse0x1dc98f60();
   input += synapse0x1f75f5f0();
   input += synapse0x21f40300();
   input += synapse0x1e3bf750();
   input += synapse0x1d98e180();
   input += synapse0x221ef7d0();
   return input;
}

double NNkaon::neuron0x1db195e0() {
   double input = input0x1db195e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1db19770() {
   double input = 0.681108;
   input += synapse0x21092ab0();
   input += synapse0x1e00b440();
   input += synapse0x2161cbd0();
   input += synapse0x20365d10();
   input += synapse0x1fc43460();
   input += synapse0x1f34a0c0();
   return input;
}

double NNkaon::neuron0x1db19770() {
   double input = input0x1db19770();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x213d69e0() {
   double input = 0.469359;
   input += synapse0x1e5d92d0();
   input += synapse0x1fd6e6b0();
   input += synapse0x1e60ab30();
   input += synapse0x20a8b2a0();
   input += synapse0x1f3699c0();
   input += synapse0x1da79620();
   return input;
}

double NNkaon::neuron0x213d69e0() {
   double input = input0x213d69e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x213d6b70() {
   double input = 0.333717;
   input += synapse0x1d983eb0();
   input += synapse0x1fd86560();
   input += synapse0x215edb00();
   input += synapse0x1e5e7f30();
   input += synapse0x1db20500();
   input += synapse0x1dfce0e0();
   return input;
}

double NNkaon::neuron0x213d6b70() {
   double input = input0x213d6b70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x213d6d00() {
   double input = -0.0409818;
   input += synapse0x21074480();
   input += synapse0x1ef8fc30();
   input += synapse0x20682920();
   input += synapse0x1dd8a130();
   input += synapse0x1e625a80();
   input += synapse0x1fc028b0();
   return input;
}

double NNkaon::neuron0x213d6d00() {
   double input = input0x213d6d00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1db19900() {
   double input = -0.306897;
   input += synapse0x21076f40();
   input += synapse0x210bcf40();
   input += synapse0x1da65490();
   input += synapse0x1ef913b0();
   input += synapse0x213c2900();
   input += synapse0x1ed16050();
   return input;
}

double NNkaon::neuron0x1db19900() {
   double input = input0x1db19900();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x20c83bc0() {
   double input = -0.0361068;
   input += synapse0x20663100();
   input += synapse0x1fc2b090();
   input += synapse0x1f35ed80();
   input += synapse0x1e61acd0();
   input += synapse0x1ea68a90();
   input += synapse0x1e24dbd0();
   return input;
}

double NNkaon::neuron0x20c83bc0() {
   double input = input0x20c83bc0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1d471250() {
   double input = -0.634252;
   input += synapse0x1e6045e0();
   input += synapse0x1de23b60();
   input += synapse0x1e63c110();
   input += synapse0x1f344b30();
   input += synapse0x1efa1ba0();
   input += synapse0x1df92380();
   input += synapse0x1de20f00();
   input += synapse0x20aa2a90();
   return input;
}

double NNkaon::neuron0x1d471250() {
   double input = input0x1d471250();
   return (input * 0.476017)+0.652996;
}

double NNkaon::synapse0x1e1026e0() {
   return (neuron0x1e6cfec0()*-0.477158);
}

double NNkaon::synapse0x1ef9d0b0() {
   return (neuron0x1e6d0050()*0.29515);
}

double NNkaon::synapse0x1e5f57f0() {
   return (neuron0x1db972c0()*0.231048);
}

double NNkaon::synapse0x1f4d6420() {
   return (neuron0x1db97450()*0.101567);
}

double NNkaon::synapse0x1e5f0a80() {
   return (neuron0x1d470f30()*0.0694219);
}

double NNkaon::synapse0x1e6e5540() {
   return (neuron0x1d4710c0()*0.0190969);
}

double NNkaon::synapse0x1dc98f60() {
   return (neuron0x1e6cfec0()*-1.91783);
}

double NNkaon::synapse0x1f75f5f0() {
   return (neuron0x1e6d0050()*0.363955);
}

double NNkaon::synapse0x21f40300() {
   return (neuron0x1db972c0()*0.342591);
}

double NNkaon::synapse0x1e3bf750() {
   return (neuron0x1db97450()*-1.32611);
}

double NNkaon::synapse0x1d98e180() {
   return (neuron0x1d470f30()*-0.196265);
}

double NNkaon::synapse0x221ef7d0() {
   return (neuron0x1d4710c0()*-0.127198);
}

double NNkaon::synapse0x21092ab0() {
   return (neuron0x1e6cfec0()*-0.406229);
}

double NNkaon::synapse0x1e00b440() {
   return (neuron0x1e6d0050()*1.61114);
}

double NNkaon::synapse0x2161cbd0() {
   return (neuron0x1db972c0()*2.6906);
}

double NNkaon::synapse0x20365d10() {
   return (neuron0x1db97450()*0.821858);
}

double NNkaon::synapse0x1fc43460() {
   return (neuron0x1d470f30()*-0.719766);
}

double NNkaon::synapse0x1f34a0c0() {
   return (neuron0x1d4710c0()*0.355393);
}

double NNkaon::synapse0x1e5d92d0() {
   return (neuron0x1e6cfec0()*-0.068376);
}

double NNkaon::synapse0x1fd6e6b0() {
   return (neuron0x1e6d0050()*0.197258);
}

double NNkaon::synapse0x1e60ab30() {
   return (neuron0x1db972c0()*-0.357561);
}

double NNkaon::synapse0x20a8b2a0() {
   return (neuron0x1db97450()*-0.156158);
}

double NNkaon::synapse0x1f3699c0() {
   return (neuron0x1d470f30()*-0.0968139);
}

double NNkaon::synapse0x1da79620() {
   return (neuron0x1d4710c0()*-0.168975);
}

double NNkaon::synapse0x1d983eb0() {
   return (neuron0x1e6cfec0()*-0.039887);
}

double NNkaon::synapse0x1fd86560() {
   return (neuron0x1e6d0050()*-0.242651);
}

double NNkaon::synapse0x215edb00() {
   return (neuron0x1db972c0()*-0.338649);
}

double NNkaon::synapse0x1e5e7f30() {
   return (neuron0x1db97450()*0.0444028);
}

double NNkaon::synapse0x1db20500() {
   return (neuron0x1d470f30()*-0.527084);
}

double NNkaon::synapse0x1dfce0e0() {
   return (neuron0x1d4710c0()*-0.303838);
}

double NNkaon::synapse0x21074480() {
   return (neuron0x1e6cfec0()*1.03503);
}

double NNkaon::synapse0x1ef8fc30() {
   return (neuron0x1e6d0050()*-2.31265);
}

double NNkaon::synapse0x20682920() {
   return (neuron0x1db972c0()*-2.32381);
}

double NNkaon::synapse0x1dd8a130() {
   return (neuron0x1db97450()*-0.937442);
}

double NNkaon::synapse0x1e625a80() {
   return (neuron0x1d470f30()*-0.0921272);
}

double NNkaon::synapse0x1fc028b0() {
   return (neuron0x1d4710c0()*-1.07325);
}

double NNkaon::synapse0x21076f40() {
   return (neuron0x1e6cfec0()*-0.356357);
}

double NNkaon::synapse0x210bcf40() {
   return (neuron0x1e6d0050()*0.584805);
}

double NNkaon::synapse0x1da65490() {
   return (neuron0x1db972c0()*0.398647);
}

double NNkaon::synapse0x1ef913b0() {
   return (neuron0x1db97450()*0.453276);
}

double NNkaon::synapse0x213c2900() {
   return (neuron0x1d470f30()*0.0971928);
}

double NNkaon::synapse0x1ed16050() {
   return (neuron0x1d4710c0()*0.18182);
}

double NNkaon::synapse0x20663100() {
   return (neuron0x1e6cfec0()*-0.427071);
}

double NNkaon::synapse0x1fc2b090() {
   return (neuron0x1e6d0050()*-0.02175);
}

double NNkaon::synapse0x1f35ed80() {
   return (neuron0x1db972c0()*0.185952);
}

double NNkaon::synapse0x1e61acd0() {
   return (neuron0x1db97450()*0.385528);
}

double NNkaon::synapse0x1ea68a90() {
   return (neuron0x1d470f30()*0.251423);
}

double NNkaon::synapse0x1e24dbd0() {
   return (neuron0x1d4710c0()*0.225259);
}

double NNkaon::synapse0x1e6045e0() {
   return (neuron0x1db19450()*0.136591);
}

double NNkaon::synapse0x1de23b60() {
   return (neuron0x1db195e0()*1.02819);
}

double NNkaon::synapse0x1e63c110() {
   return (neuron0x1db19770()*0.801768);
}

double NNkaon::synapse0x1f344b30() {
   return (neuron0x213d69e0()*0.228239);
}

double NNkaon::synapse0x1efa1ba0() {
   return (neuron0x213d6b70()*-0.455165);
}

double NNkaon::synapse0x1df92380() {
   return (neuron0x213d6d00()*-0.766232);
}

double NNkaon::synapse0x1de20f00() {
   return (neuron0x1db19900()*0.232985);
}

double NNkaon::synapse0x20aa2a90() {
   return (neuron0x20c83bc0()*-0.509038);
}

