#include "NNmuon.h"
#include <cmath>

double NNmuon::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8519f68();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x8509f70() {
   return input0;
}

double NNmuon::neuron0x850a0b8() {
   return input1;
}

double NNmuon::neuron0x850a2b0() {
   return input2;
}

double NNmuon::neuron0x8518878() {
   return input3;
}

double NNmuon::neuron0x8518a50() {
   return input4;
}

double NNmuon::neuron0x8518c48() {
   return input5;
}

double NNmuon::neuron0x8518e40() {
   return input6;
}

double NNmuon::input0x8519138() {
   double input = 0.752419;
   input += synapse0x8422830();
   input += synapse0x85192c8();
   input += synapse0x85192f0();
   input += synapse0x8519318();
   input += synapse0x8519340();
   input += synapse0x8519368();
   input += synapse0x8519390();
   return input;
}

double NNmuon::neuron0x8519138() {
   double input = input0x8519138();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x85193b8() {
   double input = -0.35676;
   input += synapse0x8519590();
   input += synapse0x85195b8();
   input += synapse0x85195e0();
   input += synapse0x8519608();
   input += synapse0x8519630();
   input += synapse0x8519658();
   input += synapse0x8519680();
   return input;
}

double NNmuon::neuron0x85193b8() {
   double input = input0x85193b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x85196a8() {
   double input = -1.00742;
   input += synapse0x8519880();
   input += synapse0x85198a8();
   input += synapse0x85198d0();
   input += synapse0x8519980();
   input += synapse0x85199a8();
   input += synapse0x85199d0();
   input += synapse0x85199f8();
   return input;
}

double NNmuon::neuron0x85196a8() {
   double input = input0x85196a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8519a20() {
   double input = 1.55504;
   input += synapse0x8519bb0();
   input += synapse0x8519bd8();
   input += synapse0x8519c00();
   input += synapse0x8519c28();
   input += synapse0x8519c50();
   input += synapse0x8519c78();
   input += synapse0x8519ca0();
   return input;
}

double NNmuon::neuron0x8519a20() {
   double input = input0x8519a20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8519cc8() {
   double input = 0.256555;
   input += synapse0x8519ea0();
   input += synapse0x8519ec8();
   input += synapse0x8519ef0();
   input += synapse0x8519f18();
   input += synapse0x8519f40();
   input += synapse0x85198f8();
   input += synapse0x8519920();
   return input;
}

double NNmuon::neuron0x8519cc8() {
   double input = input0x8519cc8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x851a070() {
   double input = 0.0867104;
   input += synapse0x851a248();
   input += synapse0x851a270();
   input += synapse0x851a298();
   input += synapse0x851a2c0();
   input += synapse0x851a2e8();
   input += synapse0x851a310();
   input += synapse0x851a338();
   return input;
}

double NNmuon::neuron0x851a070() {
   double input = input0x851a070();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x851a360() {
   double input = -0.149702;
   input += synapse0x851a558();
   input += synapse0x851a580();
   input += synapse0x851a5a8();
   input += synapse0x851a5d0();
   input += synapse0x851a5f8();
   input += synapse0x851a620();
   input += synapse0x851a648();
   return input;
}

double NNmuon::neuron0x851a360() {
   double input = input0x851a360();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x851a670() {
   double input = 0.501168;
   input += synapse0x851a868();
   input += synapse0x851a890();
   input += synapse0x851a8b8();
   input += synapse0x851a8e0();
   input += synapse0x851a908();
   input += synapse0x851a930();
   input += synapse0x851a958();
   return input;
}

double NNmuon::neuron0x851a670() {
   double input = input0x851a670();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x851a980() {
   double input = -0.256597;
   input += synapse0x851ab78();
   input += synapse0x851aba0();
   input += synapse0x851abc8();
   input += synapse0x851abf0();
   input += synapse0x851ac18();
   input += synapse0x851ac40();
   input += synapse0x851ac68();
   return input;
}

double NNmuon::neuron0x851a980() {
   double input = input0x851a980();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x851ac90() {
   double input = -0.0891626;
   input += synapse0x851af10();
   input += synapse0x851af38();
   input += synapse0x851c708();
   input += synapse0x851c808();
   input += synapse0x85586e8();
   input += synapse0x8558670();
   input += synapse0x8519948();
   return input;
}

double NNmuon::neuron0x851ac90() {
   double input = input0x851ac90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8519f68() {
   double input = 0.00512673;
   input += synapse0x84239c0();
   input += synapse0x85190f0();
   input += synapse0x851b168();
   input += synapse0x851b190();
   input += synapse0x851b1b8();
   input += synapse0x851b1e0();
   input += synapse0x851b208();
   input += synapse0x851b230();
   input += synapse0x851b258();
   input += synapse0x851b280();
   return input;
}

double NNmuon::neuron0x8519f68() {
   double input = input0x8519f68();
   return (input * 0.462449)+0.690108;
}

double NNmuon::synapse0x8422830() {
   return (neuron0x8509f70()*1.61889);
}

double NNmuon::synapse0x85192c8() {
   return (neuron0x850a0b8()*-2.02438);
}

double NNmuon::synapse0x85192f0() {
   return (neuron0x850a2b0()*-2.52226);
}

double NNmuon::synapse0x8519318() {
   return (neuron0x8518878()*-1.53933);
}

double NNmuon::synapse0x8519340() {
   return (neuron0x8518a50()*1.30724);
}

double NNmuon::synapse0x8519368() {
   return (neuron0x8518c48()*0.17792);
}

double NNmuon::synapse0x8519390() {
   return (neuron0x8518e40()*-1.59968);
}

double NNmuon::synapse0x8519590() {
   return (neuron0x8509f70()*0.0906071);
}

double NNmuon::synapse0x85195b8() {
   return (neuron0x850a0b8()*0.464525);
}

double NNmuon::synapse0x85195e0() {
   return (neuron0x850a2b0()*0.660659);
}

double NNmuon::synapse0x8519608() {
   return (neuron0x8518878()*-0.224503);
}

double NNmuon::synapse0x8519630() {
   return (neuron0x8518a50()*-0.289365);
}

double NNmuon::synapse0x8519658() {
   return (neuron0x8518c48()*-0.0365858);
}

double NNmuon::synapse0x8519680() {
   return (neuron0x8518e40()*-0.470269);
}

double NNmuon::synapse0x8519880() {
   return (neuron0x8509f70()*-0.647649);
}

double NNmuon::synapse0x85198a8() {
   return (neuron0x850a0b8()*1.33279);
}

double NNmuon::synapse0x85198d0() {
   return (neuron0x850a2b0()*1.16027);
}

double NNmuon::synapse0x8519980() {
   return (neuron0x8518878()*3.08431);
}

double NNmuon::synapse0x85199a8() {
   return (neuron0x8518a50()*0.606612);
}

double NNmuon::synapse0x85199d0() {
   return (neuron0x8518c48()*-0.34585);
}

double NNmuon::synapse0x85199f8() {
   return (neuron0x8518e40()*-0.222775);
}

double NNmuon::synapse0x8519bb0() {
   return (neuron0x8509f70()*0.97307);
}

double NNmuon::synapse0x8519bd8() {
   return (neuron0x850a0b8()*-1.94752);
}

double NNmuon::synapse0x8519c00() {
   return (neuron0x850a2b0()*-1.59013);
}

double NNmuon::synapse0x8519c28() {
   return (neuron0x8518878()*-0.195602);
}

double NNmuon::synapse0x8519c50() {
   return (neuron0x8518a50()*1.12809);
}

double NNmuon::synapse0x8519c78() {
   return (neuron0x8518c48()*0.231187);
}

double NNmuon::synapse0x8519ca0() {
   return (neuron0x8518e40()*0.330243);
}

double NNmuon::synapse0x8519ea0() {
   return (neuron0x8509f70()*0.842382);
}

double NNmuon::synapse0x8519ec8() {
   return (neuron0x850a0b8()*0.10784);
}

double NNmuon::synapse0x8519ef0() {
   return (neuron0x850a2b0()*-1.57544);
}

double NNmuon::synapse0x8519f18() {
   return (neuron0x8518878()*0.123646);
}

double NNmuon::synapse0x8519f40() {
   return (neuron0x8518a50()*-0.201376);
}

double NNmuon::synapse0x85198f8() {
   return (neuron0x8518c48()*0.0488833);
}

double NNmuon::synapse0x8519920() {
   return (neuron0x8518e40()*-0.0275632);
}

double NNmuon::synapse0x851a248() {
   return (neuron0x8509f70()*-0.440295);
}

double NNmuon::synapse0x851a270() {
   return (neuron0x850a0b8()*-0.285227);
}

double NNmuon::synapse0x851a298() {
   return (neuron0x850a2b0()*0.0183601);
}

double NNmuon::synapse0x851a2c0() {
   return (neuron0x8518878()*0.0688563);
}

double NNmuon::synapse0x851a2e8() {
   return (neuron0x8518a50()*0.511981);
}

double NNmuon::synapse0x851a310() {
   return (neuron0x8518c48()*0.395903);
}

double NNmuon::synapse0x851a338() {
   return (neuron0x8518e40()*-0.34659);
}

double NNmuon::synapse0x851a558() {
   return (neuron0x8509f70()*2.7365);
}

double NNmuon::synapse0x851a580() {
   return (neuron0x850a0b8()*-0.41509);
}

double NNmuon::synapse0x851a5a8() {
   return (neuron0x850a2b0()*-2.06594);
}

double NNmuon::synapse0x851a5d0() {
   return (neuron0x8518878()*0.713122);
}

double NNmuon::synapse0x851a5f8() {
   return (neuron0x8518a50()*0.131197);
}

double NNmuon::synapse0x851a620() {
   return (neuron0x8518c48()*0.824043);
}

double NNmuon::synapse0x851a648() {
   return (neuron0x8518e40()*1.35656);
}

double NNmuon::synapse0x851a868() {
   return (neuron0x8509f70()*0.102923);
}

double NNmuon::synapse0x851a890() {
   return (neuron0x850a0b8()*-1.57034);
}

double NNmuon::synapse0x851a8b8() {
   return (neuron0x850a2b0()*-2.8123);
}

double NNmuon::synapse0x851a8e0() {
   return (neuron0x8518878()*0.0424073);
}

double NNmuon::synapse0x851a908() {
   return (neuron0x8518a50()*-1.02625);
}

double NNmuon::synapse0x851a930() {
   return (neuron0x8518c48()*1.05233);
}

double NNmuon::synapse0x851a958() {
   return (neuron0x8518e40()*-0.479793);
}

double NNmuon::synapse0x851ab78() {
   return (neuron0x8509f70()*-0.546249);
}

double NNmuon::synapse0x851aba0() {
   return (neuron0x850a0b8()*0.0105269);
}

double NNmuon::synapse0x851abc8() {
   return (neuron0x850a2b0()*0.143814);
}

double NNmuon::synapse0x851abf0() {
   return (neuron0x8518878()*0.227225);
}

double NNmuon::synapse0x851ac18() {
   return (neuron0x8518a50()*0.467806);
}

double NNmuon::synapse0x851ac40() {
   return (neuron0x8518c48()*0.117095);
}

double NNmuon::synapse0x851ac68() {
   return (neuron0x8518e40()*0.0160732);
}

double NNmuon::synapse0x851af10() {
   return (neuron0x8509f70()*-0.744594);
}

double NNmuon::synapse0x851af38() {
   return (neuron0x850a0b8()*-0.131631);
}

double NNmuon::synapse0x851c708() {
   return (neuron0x850a2b0()*-0.671563);
}

double NNmuon::synapse0x851c808() {
   return (neuron0x8518878()*1.62689);
}

double NNmuon::synapse0x85586e8() {
   return (neuron0x8518a50()*0.461045);
}

double NNmuon::synapse0x8558670() {
   return (neuron0x8518c48()*-0.397277);
}

double NNmuon::synapse0x8519948() {
   return (neuron0x8518e40()*0.620203);
}

double NNmuon::synapse0x84239c0() {
   return (neuron0x8519138()*-1.32403);
}

double NNmuon::synapse0x85190f0() {
   return (neuron0x85193b8()*-0.133211);
}

double NNmuon::synapse0x851b168() {
   return (neuron0x85196a8()*1.43134);
}

double NNmuon::synapse0x851b190() {
   return (neuron0x8519a20()*2.04684);
}

double NNmuon::synapse0x851b1b8() {
   return (neuron0x8519cc8()*1.7867);
}

double NNmuon::synapse0x851b1e0() {
   return (neuron0x851a070()*-0.57513);
}

double NNmuon::synapse0x851b208() {
   return (neuron0x851a360()*-1.36739);
}

double NNmuon::synapse0x851b230() {
   return (neuron0x851a670()*-2.01681);
}

double NNmuon::synapse0x851b258() {
   return (neuron0x851a980()*-0.756392);
}

double NNmuon::synapse0x851b280() {
   return (neuron0x851ac90()*-2.15331);
}

