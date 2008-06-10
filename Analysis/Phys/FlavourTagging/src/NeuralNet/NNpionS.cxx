#include "NNpionS.h"
#include <cmath>

double NNpionS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8854d50();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x8b2dde8() {
   return input0;
}

double NNpionS::neuron0x87e7638() {
   return input1;
}

double NNpionS::neuron0x87e7810() {
   return input2;
}

double NNpionS::neuron0x8a42938() {
   return input3;
}

double NNpionS::neuron0x8a42b30() {
   return input4;
}

double NNpionS::neuron0x898fac0() {
   return input5;
}

double NNpionS::neuron0x898fcb8() {
   return input6;
}

double NNpionS::input0x898feb0() {
   double input = -0.131424;
   input += synapse0x8a20030();
   input += synapse0x8854f00();
   input += synapse0x8854f28();
   input += synapse0x8854f50();
   input += synapse0x8854f78();
   input += synapse0x8854fa0();
   input += synapse0x8854fc8();
   return input;
}

double NNpionS::neuron0x898feb0() {
   double input = input0x898feb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8854ff0() {
   double input = -0.444871;
   input += synapse0x88551c8();
   input += synapse0x88551f0();
   input += synapse0x8855218();
   input += synapse0x8b2df30();
   input += synapse0x8855240();
   input += synapse0x87e7a08();
   input += synapse0x87e7a30();
   return input;
}

double NNpionS::neuron0x8854ff0() {
   double input = input0x8854ff0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8c33188() {
   double input = 0.0656774;
   input += synapse0x8c33360();
   input += synapse0x8c33388();
   input += synapse0x8c333b0();
   input += synapse0x8c33460();
   input += synapse0x8c33488();
   input += synapse0x8c334b0();
   input += synapse0x8c334d8();
   return input;
}

double NNpionS::neuron0x8c33188() {
   double input = input0x8c33188();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8c33500() {
   double input = 0.228625;
   input += synapse0x8c33690();
   input += synapse0x8c336b8();
   input += synapse0x8c336e0();
   input += synapse0x8c33708();
   input += synapse0x8c33730();
   input += synapse0x8c33758();
   input += synapse0x8c33780();
   return input;
}

double NNpionS::neuron0x8c33500() {
   double input = input0x8c33500();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8c337a8() {
   double input = -0.189701;
   input += synapse0x8a93868();
   input += synapse0x8a93890();
   input += synapse0x8a938b8();
   input += synapse0x8a938e0();
   input += synapse0x8a93908();
   input += synapse0x8c333d8();
   input += synapse0x8c33400();
   return input;
}

double NNpionS::neuron0x8c337a8() {
   double input = input0x8c337a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8a93a38() {
   double input = 1.65333;
   input += synapse0x8a93c10();
   input += synapse0x8a93c38();
   input += synapse0x8a93c60();
   input += synapse0x8a93c88();
   input += synapse0x8a93cb0();
   input += synapse0x8a93cd8();
   input += synapse0x8a93d00();
   return input;
}

double NNpionS::neuron0x8a93a38() {
   double input = input0x8a93a38();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8a93d28() {
   double input = 0.768651;
   input += synapse0x8a93f00();
   input += synapse0x8a93f28();
   input += synapse0x8a93f50();
   input += synapse0x8a93f78();
   input += synapse0x8a93fa0();
   input += synapse0x8a93fc8();
   input += synapse0x8a93ff0();
   return input;
}

double NNpionS::neuron0x8a93d28() {
   double input = input0x8a93d28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8854d50() {
   double input = 0.467319;
   input += synapse0x8a940f0();
   input += synapse0x8a94118();
   input += synapse0x8a94140();
   input += synapse0x8a94168();
   input += synapse0x8a94190();
   input += synapse0x8a941b8();
   input += synapse0x8a941e0();
   return input;
}

double NNpionS::neuron0x8854d50() {
   double input = input0x8854d50();
   return (input * 0.495509)+0.566863;
}

double NNpionS::synapse0x8a20030() {
   return (neuron0x8b2dde8()*-0.580204);
}

double NNpionS::synapse0x8854f00() {
   return (neuron0x87e7638()*0.616598);
}

double NNpionS::synapse0x8854f28() {
   return (neuron0x87e7810()*1.07604);
}

double NNpionS::synapse0x8854f50() {
   return (neuron0x8a42938()*-0.330206);
}

double NNpionS::synapse0x8854f78() {
   return (neuron0x8a42b30()*-0.0651621);
}

double NNpionS::synapse0x8854fa0() {
   return (neuron0x898fac0()*-1.26405);
}

double NNpionS::synapse0x8854fc8() {
   return (neuron0x898fcb8()*0.679687);
}

double NNpionS::synapse0x88551c8() {
   return (neuron0x8b2dde8()*-0.0180483);
}

double NNpionS::synapse0x88551f0() {
   return (neuron0x87e7638()*0.0261731);
}

double NNpionS::synapse0x8855218() {
   return (neuron0x87e7810()*-0.205576);
}

double NNpionS::synapse0x8b2df30() {
   return (neuron0x8a42938()*-0.401831);
}

double NNpionS::synapse0x8855240() {
   return (neuron0x8a42b30()*0.772838);
}

double NNpionS::synapse0x87e7a08() {
   return (neuron0x898fac0()*0.0176721);
}

double NNpionS::synapse0x87e7a30() {
   return (neuron0x898fcb8()*1.11872);
}

double NNpionS::synapse0x8c33360() {
   return (neuron0x8b2dde8()*-0.525016);
}

double NNpionS::synapse0x8c33388() {
   return (neuron0x87e7638()*0.388868);
}

double NNpionS::synapse0x8c333b0() {
   return (neuron0x87e7810()*0.663749);
}

double NNpionS::synapse0x8c33460() {
   return (neuron0x8a42938()*-0.505905);
}

double NNpionS::synapse0x8c33488() {
   return (neuron0x8a42b30()*0.843306);
}

double NNpionS::synapse0x8c334b0() {
   return (neuron0x898fac0()*-1.04582);
}

double NNpionS::synapse0x8c334d8() {
   return (neuron0x898fcb8()*-0.163047);
}

double NNpionS::synapse0x8c33690() {
   return (neuron0x8b2dde8()*-0.787191);
}

double NNpionS::synapse0x8c336b8() {
   return (neuron0x87e7638()*0.0511211);
}

double NNpionS::synapse0x8c336e0() {
   return (neuron0x87e7810()*-0.551477);
}

double NNpionS::synapse0x8c33708() {
   return (neuron0x8a42938()*-0.222702);
}

double NNpionS::synapse0x8c33730() {
   return (neuron0x8a42b30()*-0.454459);
}

double NNpionS::synapse0x8c33758() {
   return (neuron0x898fac0()*-0.156887);
}

double NNpionS::synapse0x8c33780() {
   return (neuron0x898fcb8()*-0.28726);
}

double NNpionS::synapse0x8a93868() {
   return (neuron0x8b2dde8()*-0.37785);
}

double NNpionS::synapse0x8a93890() {
   return (neuron0x87e7638()*-0.180723);
}

double NNpionS::synapse0x8a938b8() {
   return (neuron0x87e7810()*0.510083);
}

double NNpionS::synapse0x8a938e0() {
   return (neuron0x8a42938()*-0.447811);
}

double NNpionS::synapse0x8a93908() {
   return (neuron0x8a42b30()*0.384017);
}

double NNpionS::synapse0x8c333d8() {
   return (neuron0x898fac0()*-1.05968);
}

double NNpionS::synapse0x8c33400() {
   return (neuron0x898fcb8()*-1.91609);
}

double NNpionS::synapse0x8a93c10() {
   return (neuron0x8b2dde8()*-0.0553942);
}

double NNpionS::synapse0x8a93c38() {
   return (neuron0x87e7638()*-0.433753);
}

double NNpionS::synapse0x8a93c60() {
   return (neuron0x87e7810()*-1.60756);
}

double NNpionS::synapse0x8a93c88() {
   return (neuron0x8a42938()*-0.0273833);
}

double NNpionS::synapse0x8a93cb0() {
   return (neuron0x8a42b30()*-1.3809);
}

double NNpionS::synapse0x8a93cd8() {
   return (neuron0x898fac0()*1.31746);
}

double NNpionS::synapse0x8a93d00() {
   return (neuron0x898fcb8()*-1.33227);
}

double NNpionS::synapse0x8a93f00() {
   return (neuron0x8b2dde8()*0.393142);
}

double NNpionS::synapse0x8a93f28() {
   return (neuron0x87e7638()*-0.827028);
}

double NNpionS::synapse0x8a93f50() {
   return (neuron0x87e7810()*-2.92734);
}

double NNpionS::synapse0x8a93f78() {
   return (neuron0x8a42938()*0.0717651);
}

double NNpionS::synapse0x8a93fa0() {
   return (neuron0x8a42b30()*-0.96493);
}

double NNpionS::synapse0x8a93fc8() {
   return (neuron0x898fac0()*1.21369);
}

double NNpionS::synapse0x8a93ff0() {
   return (neuron0x898fcb8()*2.93202);
}

double NNpionS::synapse0x8a940f0() {
   return (neuron0x898feb0()*-2.34634);
}

double NNpionS::synapse0x8a94118() {
   return (neuron0x8854ff0()*-1.15056);
}

double NNpionS::synapse0x8a94140() {
   return (neuron0x8c33188()*0.185101);
}

double NNpionS::synapse0x8a94168() {
   return (neuron0x8c33500()*0.895339);
}

double NNpionS::synapse0x8a94190() {
   return (neuron0x8c337a8()*1.10898);
}

double NNpionS::synapse0x8a941b8() {
   return (neuron0x8a93a38()*2.63594);
}

double NNpionS::synapse0x8a941e0() {
   return (neuron0x8a93d28()*-3.12255);
}

