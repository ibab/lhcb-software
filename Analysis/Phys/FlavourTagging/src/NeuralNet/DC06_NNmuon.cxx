#include "DC06_NNmuon.h"
#include <cmath>

double DC06_NNmuon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8611dc8();
     default:
         return 0.;
   }
}

double DC06_NNmuon::neuron0x8615b98() {
   return input0;
}

double DC06_NNmuon::neuron0x8615d28() {
   return input1;
}

double DC06_NNmuon::neuron0x8615f00() {
   return input2;
}

double DC06_NNmuon::neuron0x86160f8() {
   return input3;
}

double DC06_NNmuon::input0x8616410() {
   double input = -0.141489;
   input += synapse0x8615b60();
   input += synapse0x86165a0();
   input += synapse0x86165c8();
   input += synapse0x86165f0();
   return input;
}

double DC06_NNmuon::neuron0x8616410() {
   double input = input0x8616410();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8616618() {
   double input = -0.548749;
   input += synapse0x86167f0();
   input += synapse0x8616818();
   input += synapse0x8616840();
   input += synapse0x8616868();
   return input;
}

double DC06_NNmuon::neuron0x8616618() {
   double input = input0x8616618();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8616890() {
   double input = 0.388659;
   input += synapse0x8616a68();
   input += synapse0x8616a90();
   input += synapse0x8616ab8();
   input += synapse0x8616ae0();
   return input;
}

double DC06_NNmuon::neuron0x8616890() {
   double input = input0x8616890();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8616b08() {
   double input = -0.59709;
   input += synapse0x8616ce0();
   input += synapse0x8616d08();
   input += synapse0x8616d30();
   input += synapse0x8616d58();
   return input;
}

double DC06_NNmuon::neuron0x8616b08() {
   double input = input0x8616b08();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8616d80() {
   double input = 2.02651;
   input += synapse0x8616f78();
   input += synapse0x8617028();
   input += synapse0x8617050();
   input += synapse0x8617078();
   return input;
}

double DC06_NNmuon::neuron0x8616d80() {
   double input = input0x8616d80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x86170a0() {
   double input = -0.118954;
   input += synapse0x86171c0();
   input += synapse0x8610d20();
   input += synapse0x8610d48();
   input += synapse0x8610d70();
   return input;
}

double DC06_NNmuon::neuron0x86170a0() {
   double input = input0x86170a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8610d98() {
   double input = -1.92058;
   input += synapse0x8610f90();
   input += synapse0x8610fb8();
   input += synapse0x8610fe0();
   input += synapse0x8611008();
   return input;
}

double DC06_NNmuon::neuron0x8610d98() {
   double input = input0x8610d98();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8611030() {
   double input = 1.54304;
   input += synapse0x8611228();
   input += synapse0x8611250();
   input += synapse0x8611278();
   input += synapse0x86112a0();
   return input;
}

double DC06_NNmuon::neuron0x8611030() {
   double input = input0x8611030();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x86112c8() {
   double input = 1.08936;
   input += synapse0x86114c0();
   input += synapse0x8616fa0();
   input += synapse0x8616fc8();
   input += synapse0x8616ff0();
   return input;
}

double DC06_NNmuon::neuron0x86112c8() {
   double input = input0x86112c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x86115f0() {
   double input = 0.86095;
   input += synapse0x86117e8();
   input += synapse0x8611810();
   input += synapse0x8611838();
   input += synapse0x8611860();
   return input;
}

double DC06_NNmuon::neuron0x86115f0() {
   double input = input0x86115f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8611888() {
   double input = -0.207476;
   input += synapse0x8611a88();
   input += synapse0x8611ab0();
   input += synapse0x8611ad8();
   input += synapse0x8611b00();
   return input;
}

double DC06_NNmuon::neuron0x8611888() {
   double input = input0x8611888();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8611b28() {
   double input = 2.59911;
   input += synapse0x8611d28();
   input += synapse0x8611d50();
   input += synapse0x8611d78();
   input += synapse0x8611da0();
   return input;
}

double DC06_NNmuon::neuron0x8611b28() {
   double input = input0x8611b28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNmuon::input0x8611dc8() {
   double input = -0.467005;
   input += synapse0x8611fc0();
   input += synapse0x8611fe8();
   input += synapse0x8612010();
   input += synapse0x8612038();
   input += synapse0x8612060();
   input += synapse0x8612088();
   input += synapse0x86120b0();
   input += synapse0x86120d8();
   input += synapse0x8612100();
   input += synapse0x8612128();
   input += synapse0x8612150();
   input += synapse0x8612178();
   return input;
}

double DC06_NNmuon::neuron0x8611dc8() {
   double input = input0x8611dc8();
   return (input * 0.468603)+0.674388;
}

double DC06_NNmuon::synapse0x8615b60() {
   return (neuron0x8615b98()*-0.224397);
}

double DC06_NNmuon::synapse0x86165a0() {
   return (neuron0x8615d28()*-1.72181);
}

double DC06_NNmuon::synapse0x86165c8() {
   return (neuron0x8615f00()*-1.47099);
}

double DC06_NNmuon::synapse0x86165f0() {
   return (neuron0x86160f8()*1.82496);
}

double DC06_NNmuon::synapse0x86167f0() {
   return (neuron0x8615b98()*-3.13912);
}

double DC06_NNmuon::synapse0x8616818() {
   return (neuron0x8615d28()*5.80444);
}

double DC06_NNmuon::synapse0x8616840() {
   return (neuron0x8615f00()*1.03738);
}

double DC06_NNmuon::synapse0x8616868() {
   return (neuron0x86160f8()*-0.919038);
}

double DC06_NNmuon::synapse0x8616a68() {
   return (neuron0x8615b98()*1.32132);
}

double DC06_NNmuon::synapse0x8616a90() {
   return (neuron0x8615d28()*-2.39222);
}

double DC06_NNmuon::synapse0x8616ab8() {
   return (neuron0x8615f00()*-2.19738);
}

double DC06_NNmuon::synapse0x8616ae0() {
   return (neuron0x86160f8()*-3.70133);
}

double DC06_NNmuon::synapse0x8616ce0() {
   return (neuron0x8615b98()*-1.00876);
}

double DC06_NNmuon::synapse0x8616d08() {
   return (neuron0x8615d28()*-4.068);
}

double DC06_NNmuon::synapse0x8616d30() {
   return (neuron0x8615f00()*0.928497);
}

double DC06_NNmuon::synapse0x8616d58() {
   return (neuron0x86160f8()*-0.227098);
}

double DC06_NNmuon::synapse0x8616f78() {
   return (neuron0x8615b98()*-2.33602);
}

double DC06_NNmuon::synapse0x8617028() {
   return (neuron0x8615d28()*1.99477);
}

double DC06_NNmuon::synapse0x8617050() {
   return (neuron0x8615f00()*0.252964);
}

double DC06_NNmuon::synapse0x8617078() {
   return (neuron0x86160f8()*-2.71625);
}

double DC06_NNmuon::synapse0x86171c0() {
   return (neuron0x8615b98()*-0.0232829);
}

double DC06_NNmuon::synapse0x8610d20() {
   return (neuron0x8615d28()*-0.481088);
}

double DC06_NNmuon::synapse0x8610d48() {
   return (neuron0x8615f00()*0.599994);
}

double DC06_NNmuon::synapse0x8610d70() {
   return (neuron0x86160f8()*0.401776);
}

double DC06_NNmuon::synapse0x8610f90() {
   return (neuron0x8615b98()*-0.676819);
}

double DC06_NNmuon::synapse0x8610fb8() {
   return (neuron0x8615d28()*7.23245);
}

double DC06_NNmuon::synapse0x8610fe0() {
   return (neuron0x8615f00()*-0.580657);
}

double DC06_NNmuon::synapse0x8611008() {
   return (neuron0x86160f8()*0.840211);
}

double DC06_NNmuon::synapse0x8611228() {
   return (neuron0x8615b98()*-0.247531);
}

double DC06_NNmuon::synapse0x8611250() {
   return (neuron0x8615d28()*2.30582);
}

double DC06_NNmuon::synapse0x8611278() {
   return (neuron0x8615f00()*0.699131);
}

double DC06_NNmuon::synapse0x86112a0() {
   return (neuron0x86160f8()*-4.90221);
}

double DC06_NNmuon::synapse0x86114c0() {
   return (neuron0x8615b98()*0.309209);
}

double DC06_NNmuon::synapse0x8616fa0() {
   return (neuron0x8615d28()*-1.52498);
}

double DC06_NNmuon::synapse0x8616fc8() {
   return (neuron0x8615f00()*-0.748209);
}

double DC06_NNmuon::synapse0x8616ff0() {
   return (neuron0x86160f8()*-1.02724);
}

double DC06_NNmuon::synapse0x86117e8() {
   return (neuron0x8615b98()*-0.0936927);
}

double DC06_NNmuon::synapse0x8611810() {
   return (neuron0x8615d28()*-0.937894);
}

double DC06_NNmuon::synapse0x8611838() {
   return (neuron0x8615f00()*0.0460369);
}

double DC06_NNmuon::synapse0x8611860() {
   return (neuron0x86160f8()*0.138288);
}

double DC06_NNmuon::synapse0x8611a88() {
   return (neuron0x8615b98()*0.184834);
}

double DC06_NNmuon::synapse0x8611ab0() {
   return (neuron0x8615d28()*-0.43831);
}

double DC06_NNmuon::synapse0x8611ad8() {
   return (neuron0x8615f00()*-0.607144);
}

double DC06_NNmuon::synapse0x8611b00() {
   return (neuron0x86160f8()*0.00277847);
}

double DC06_NNmuon::synapse0x8611d28() {
   return (neuron0x8615b98()*-0.663391);
}

double DC06_NNmuon::synapse0x8611d50() {
   return (neuron0x8615d28()*-1.53962);
}

double DC06_NNmuon::synapse0x8611d78() {
   return (neuron0x8615f00()*3.58468);
}

double DC06_NNmuon::synapse0x8611da0() {
   return (neuron0x86160f8()*-2.0104);
}

double DC06_NNmuon::synapse0x8611fc0() {
   return (neuron0x8616410()*2.00504);
}

double DC06_NNmuon::synapse0x8611fe8() {
   return (neuron0x8616618()*2.1855);
}

double DC06_NNmuon::synapse0x8612010() {
   return (neuron0x8616890()*-0.590639);
}

double DC06_NNmuon::synapse0x8612038() {
   return (neuron0x8616b08()*-3.05163);
}

double DC06_NNmuon::synapse0x8612060() {
   return (neuron0x8616d80()*-2.02078);
}

double DC06_NNmuon::synapse0x8612088() {
   return (neuron0x86170a0()*-0.422454);
}

double DC06_NNmuon::synapse0x86120b0() {
   return (neuron0x8610d98()*-2.34146);
}

double DC06_NNmuon::synapse0x86120d8() {
   return (neuron0x8611030()*1.65201);
}

double DC06_NNmuon::synapse0x8612100() {
   return (neuron0x86112c8()*-1.22827);
}

double DC06_NNmuon::synapse0x8612128() {
   return (neuron0x86115f0()*-0.624251);
}

double DC06_NNmuon::synapse0x8612150() {
   return (neuron0x8611888()*-0.330291);
}

double DC06_NNmuon::synapse0x8612178() {
   return (neuron0x8611b28()*2.23908);
}

