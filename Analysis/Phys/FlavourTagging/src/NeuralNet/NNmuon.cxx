#include "NNmuon.h"
#include <cmath>

double NNmuon::value(int index,double in0,double in1,double in2,double in3,double in4) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x1dcedff0();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x19941410() {
   return input0;
}

double NNmuon::neuron0x199415a0() {
   return input1;
}

double NNmuon::neuron0x1dcedb40() {
   return input2;
}

double NNmuon::neuron0x1dcedcd0() {
   return input3;
}

double NNmuon::neuron0x1dcede60() {
   return input4;
}

double NNmuon::input0x196480b0() {
   double input = -0.802117;
   input += synapse0x199edc10();
   input += synapse0x1ad7ce20();
   input += synapse0x1c3f4610();
   input += synapse0x198e7a00();
   input += synapse0x199e6760();
   return input;
}

double NNmuon::neuron0x196480b0() {
   double input = input0x196480b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x19648240() {
   double input = -0.0672562;
   input += synapse0x19e3ba40();
   input += synapse0x19809660();
   input += synapse0x1c4192b0();
   input += synapse0x1ac24810();
   input += synapse0x1a314ad0();
   return input;
}

double NNmuon::neuron0x19648240() {
   double input = input0x19648240();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x196483d0() {
   double input = 0.183162;
   input += synapse0x1c77fa40();
   input += synapse0x1ddbf600();
   input += synapse0x19884200();
   input += synapse0x1bfed830();
   input += synapse0x1abffa40();
   return input;
}

double NNmuon::neuron0x196483d0() {
   double input = input0x196483d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x19648560() {
   double input = -0.104712;
   input += synapse0x19767330();
   input += synapse0x1c77df60();
   input += synapse0x1ddd9cd0();
   input += synapse0x19e228f0();
   input += synapse0x198a8100();
   return input;
}

double NNmuon::neuron0x19648560() {
   double input = input0x19648560();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x196486f0() {
   double input = -0.22136;
   input += synapse0x195fa040();
   input += synapse0x198ebc40();
   input += synapse0x1d29a170();
   input += synapse0x197d86d0();
   input += synapse0x19780b30();
   return input;
}

double NNmuon::neuron0x196486f0() {
   double input = input0x196486f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x19648880() {
   double input = -0.095573;
   input += synapse0x1cd523d0();
   input += synapse0x1bc5fac0();
   input += synapse0x19450740();
   input += synapse0x1afd37f0();
   input += synapse0x1ac2cc30();
   return input;
}

double NNmuon::neuron0x19648880() {
   double input = input0x19648880();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x191436e0() {
   double input = 0.516664;
   input += synapse0x1ae57160();
   input += synapse0x1ad94650();
   input += synapse0x1ac2d0a0();
   input += synapse0x196f9e40();
   input += synapse0x19e41ea0();
   return input;
}

double NNmuon::neuron0x191436e0() {
   double input = input0x191436e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x19143a80() {
   double input = -0.658823;
   input += synapse0x1ad77380();
   input += synapse0x19780c20();
   input += synapse0x1c77b0e0();
   input += synapse0x1ac1cb00();
   input += synapse0x19c79670();
   return input;
}

double NNmuon::neuron0x19143a80() {
   double input = input0x19143a80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x1dcedff0() {
   double input = -1.24181;
   input += synapse0x1da50e70();
   input += synapse0x1bc61560();
   input += synapse0x1c3df4f0();
   input += synapse0x1b410ad0();
   input += synapse0x19810cf0();
   input += synapse0x1942d700();
   input += synapse0x1c17a770();
   input += synapse0x199e3380();
   return input;
}

double NNmuon::neuron0x1dcedff0() {
   double input = input0x1dcedff0();
   return (input * 0.468162)+0.675568;
}

double NNmuon::synapse0x199edc10() {
   return (neuron0x19941410()*1.76231);
}

double NNmuon::synapse0x1ad7ce20() {
   return (neuron0x199415a0()*1.42998);
}

double NNmuon::synapse0x1c3f4610() {
   return (neuron0x1dcedb40()*-0.616422);
}

double NNmuon::synapse0x198e7a00() {
   return (neuron0x1dcedcd0()*-1.15652);
}

double NNmuon::synapse0x199e6760() {
   return (neuron0x1dcede60()*2.6773);
}

double NNmuon::synapse0x19e3ba40() {
   return (neuron0x19941410()*-0.08356);
}

double NNmuon::synapse0x19809660() {
   return (neuron0x199415a0()*1.40181);
}

double NNmuon::synapse0x1c4192b0() {
   return (neuron0x1dcedb40()*1.10196);
}

double NNmuon::synapse0x1ac24810() {
   return (neuron0x1dcedcd0()*-1.18023);
}

double NNmuon::synapse0x1a314ad0() {
   return (neuron0x1dcede60()*-1.88755);
}

double NNmuon::synapse0x1c77fa40() {
   return (neuron0x19941410()*-0.797092);
}

double NNmuon::synapse0x1ddbf600() {
   return (neuron0x199415a0()*-0.588591);
}

double NNmuon::synapse0x19884200() {
   return (neuron0x1dcedb40()*-0.404628);
}

double NNmuon::synapse0x1bfed830() {
   return (neuron0x1dcedcd0()*0.127431);
}

double NNmuon::synapse0x1abffa40() {
   return (neuron0x1dcede60()*0.330745);
}

double NNmuon::synapse0x19767330() {
   return (neuron0x19941410()*-0.719039);
}

double NNmuon::synapse0x1c77df60() {
   return (neuron0x199415a0()*-0.796066);
}

double NNmuon::synapse0x1ddd9cd0() {
   return (neuron0x1dcedb40()*-0.46833);
}

double NNmuon::synapse0x19e228f0() {
   return (neuron0x1dcedcd0()*1.07162);
}

double NNmuon::synapse0x198a8100() {
   return (neuron0x1dcede60()*0.399257);
}

double NNmuon::synapse0x195fa040() {
   return (neuron0x19941410()*0.118088);
}

double NNmuon::synapse0x198ebc40() {
   return (neuron0x199415a0()*-0.0520872);
}

double NNmuon::synapse0x1d29a170() {
   return (neuron0x1dcedb40()*0.19115);
}

double NNmuon::synapse0x197d86d0() {
   return (neuron0x1dcedcd0()*-0.0768647);
}

double NNmuon::synapse0x19780b30() {
   return (neuron0x1dcede60()*1.39176);
}

double NNmuon::synapse0x1cd523d0() {
   return (neuron0x19941410()*1.64457);
}

double NNmuon::synapse0x1bc5fac0() {
   return (neuron0x199415a0()*3.15375);
}

double NNmuon::synapse0x19450740() {
   return (neuron0x1dcedb40()*0.644539);
}

double NNmuon::synapse0x1afd37f0() {
   return (neuron0x1dcedcd0()*-0.182691);
}

double NNmuon::synapse0x1ac2cc30() {
   return (neuron0x1dcede60()*0.326286);
}

double NNmuon::synapse0x1ae57160() {
   return (neuron0x19941410()*0.373434);
}

double NNmuon::synapse0x1ad94650() {
   return (neuron0x199415a0()*1.01184);
}

double NNmuon::synapse0x1ac2d0a0() {
   return (neuron0x1dcedb40()*-0.00867029);
}

double NNmuon::synapse0x196f9e40() {
   return (neuron0x1dcedcd0()*-0.399019);
}

double NNmuon::synapse0x19e41ea0() {
   return (neuron0x1dcede60()*-0.0705991);
}

double NNmuon::synapse0x1ad77380() {
   return (neuron0x19941410()*0.517851);
}

double NNmuon::synapse0x19780c20() {
   return (neuron0x199415a0()*-2.23047);
}

double NNmuon::synapse0x1c77b0e0() {
   return (neuron0x1dcedb40()*-0.862545);
}

double NNmuon::synapse0x1ac1cb00() {
   return (neuron0x1dcedcd0()*-0.662234);
}

double NNmuon::synapse0x19c79670() {
   return (neuron0x1dcede60()*1.23438);
}

double NNmuon::synapse0x1da50e70() {
   return (neuron0x196480b0()*1.29644);
}

double NNmuon::synapse0x1bc61560() {
   return (neuron0x19648240()*0.745227);
}

double NNmuon::synapse0x1c3df4f0() {
   return (neuron0x196483d0()*0.467044);
}

double NNmuon::synapse0x1b410ad0() {
   return (neuron0x19648560()*0.676173);
}

double NNmuon::synapse0x19810cf0() {
   return (neuron0x196486f0()*-0.684499);
}

double NNmuon::synapse0x1942d700() {
   return (neuron0x19648880()*0.881135);
}

double NNmuon::synapse0x1c17a770() {
   return (neuron0x191436e0()*-1.0217);
}

double NNmuon::synapse0x199e3380() {
   return (neuron0x19143a80()*-1.15545);
}

