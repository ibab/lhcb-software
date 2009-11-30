#include "DC06_NNkaonS.h"
#include <cmath>

double DC06_NNkaonS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8bf2010();
     default:
         return 0.;
   }
}

double DC06_NNkaonS::neuron0x8a01138() {
   return input0;
}

double DC06_NNkaonS::neuron0x8a01280() {
   return input1;
}

double DC06_NNkaonS::neuron0x8be51c0() {
   return input2;
}

double DC06_NNkaonS::neuron0x8be53b8() {
   return input3;
}

double DC06_NNkaonS::neuron0x8be55b0() {
   return input4;
}

double DC06_NNkaonS::neuron0x8bf4230() {
   return input5;
}

double DC06_NNkaonS::neuron0x8bf4428() {
   return input6;
}

double DC06_NNkaonS::input0x8bf4740() {
   double input = -0.40731;
   input += synapse0x8adb650();
   input += synapse0x8bf48d0();
   input += synapse0x8bf48f8();
   input += synapse0x8bf4920();
   input += synapse0x8bf4948();
   input += synapse0x8bf4970();
   input += synapse0x8bf4998();
   return input;
}

double DC06_NNkaonS::neuron0x8bf4740() {
   double input = input0x8bf4740();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf49c0() {
   double input = -0.629775;
   input += synapse0x8bf4b98();
   input += synapse0x8bf4bc0();
   input += synapse0x8bf4be8();
   input += synapse0x8bf4c10();
   input += synapse0x8bf4c38();
   input += synapse0x8bf4c60();
   input += synapse0x8bf4c88();
   return input;
}

double DC06_NNkaonS::neuron0x8bf49c0() {
   double input = input0x8bf49c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf4cb0() {
   double input = 0.36739;
   input += synapse0x8bf4e88();
   input += synapse0x8bf4eb0();
   input += synapse0x8bf4ed8();
   input += synapse0x8be5760();
   input += synapse0x8a014a0();
   input += synapse0x8a014c8();
   input += synapse0x8a19640();
   return input;
}

double DC06_NNkaonS::neuron0x8bf4cb0() {
   double input = input0x8bf4cb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8a19668() {
   double input = 0.840721;
   input += synapse0x8a197f8();
   input += synapse0x8a19820();
   input += synapse0x8a19848();
   input += synapse0x8a19870();
   input += synapse0x8a19898();
   input += synapse0x8a198c0();
   input += synapse0x8a198e8();
   return input;
}

double DC06_NNkaonS::neuron0x8a19668() {
   double input = input0x8a19668();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8a19910() {
   double input = 0.489365;
   input += synapse0x8a19ae8();
   input += synapse0x8a19b10();
   input += synapse0x8a19b38();
   input += synapse0x8a19b60();
   input += synapse0x8a19b88();
   input += synapse0x8bf4f00();
   input += synapse0x8bf4f28();
   return input;
}

double DC06_NNkaonS::neuron0x8a19910() {
   double input = input0x8a19910();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8a19cb8() {
   double input = -0.138569;
   input += synapse0x8a19e90();
   input += synapse0x8a19eb8();
   input += synapse0x8a19ee0();
   input += synapse0x8a19f08();
   input += synapse0x8a19f30();
   input += synapse0x8a19f58();
   input += synapse0x8a19f80();
   return input;
}

double DC06_NNkaonS::neuron0x8a19cb8() {
   double input = input0x8a19cb8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8a19fa8() {
   double input = 0.192886;
   input += synapse0x8a1a180();
   input += synapse0x8a1a1a8();
   input += synapse0x8a1a1d0();
   input += synapse0x8a1a1f8();
   input += synapse0x8a1a220();
   input += synapse0x8a1a248();
   input += synapse0x8a1a270();
   return input;
}

double DC06_NNkaonS::neuron0x8a19fa8() {
   double input = input0x8a19fa8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8a1a298() {
   double input = 0.442085;
   input += synapse0x8a1a448();
   input += synapse0x8ad95f8();
   input += synapse0x8ad9620();
   input += synapse0x8ad9648();
   input += synapse0x8ad9670();
   input += synapse0x8ad9698();
   input += synapse0x8ad96c0();
   return input;
}

double DC06_NNkaonS::neuron0x8a1a298() {
   double input = input0x8a1a298();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8ad96e8() {
   double input = 0.232969;
   input += synapse0x8ad98e0();
   input += synapse0x8ad9908();
   input += synapse0x8ad9930();
   input += synapse0x8ad9958();
   input += synapse0x8ad9980();
   input += synapse0x8ad99a8();
   input += synapse0x8ad99d0();
   return input;
}

double DC06_NNkaonS::neuron0x8ad96e8() {
   double input = input0x8ad96e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8ad99f8() {
   double input = -1.06493;
   input += synapse0x8ad9c78();
   input += synapse0x8ad9ca0();
   input += synapse0x8a01108();
   input += synapse0x8adb678();
   input += synapse0x8adb998();
   input += synapse0x8adb920();
   input += synapse0x8a19bb0();
   return input;
}

double DC06_NNkaonS::neuron0x8ad99f8() {
   double input = input0x8ad99f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8ad9ed0() {
   double input = -0.254391;
   input += synapse0x8ada0d0();
   input += synapse0x8ada0f8();
   input += synapse0x8ada120();
   input += synapse0x8ada148();
   input += synapse0x8ada170();
   input += synapse0x8ada198();
   input += synapse0x8ada1c0();
   return input;
}

double DC06_NNkaonS::neuron0x8ad9ed0() {
   double input = input0x8ad9ed0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8ada1e8() {
   double input = 0.462516;
   input += synapse0x8ada3e8();
   input += synapse0x8ada410();
   input += synapse0x8ada438();
   input += synapse0x8a19bd8();
   input += synapse0x8a19c00();
   input += synapse0x8a19c28();
   input += synapse0x8a19c50();
   return input;
}

double DC06_NNkaonS::neuron0x8ada1e8() {
   double input = input0x8ada1e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf0398() {
   double input = -0.172809;
   input += synapse0x8bf0598();
   input += synapse0x8bf05c0();
   input += synapse0x8bf05e8();
   input += synapse0x8bf0610();
   input += synapse0x8bf0638();
   input += synapse0x8bf0660();
   input += synapse0x8bf0688();
   return input;
}

double DC06_NNkaonS::neuron0x8bf0398() {
   double input = input0x8bf0398();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf06b0() {
   double input = 0.156469;
   input += synapse0x8bf08b0();
   input += synapse0x8bf08d8();
   input += synapse0x8bf0900();
   input += synapse0x8bf0928();
   input += synapse0x8bf0950();
   input += synapse0x8bf0978();
   input += synapse0x8bf09a0();
   return input;
}

double DC06_NNkaonS::neuron0x8bf06b0() {
   double input = input0x8bf06b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf09c8() {
   double input = 0.224392;
   input += synapse0x8bf0bc8();
   input += synapse0x8bf0bf0();
   input += synapse0x8bf0c18();
   input += synapse0x8bf0c40();
   input += synapse0x8bf0c68();
   input += synapse0x8bf0c90();
   input += synapse0x8bf0cb8();
   return input;
}

double DC06_NNkaonS::neuron0x8bf09c8() {
   double input = input0x8bf09c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf0ce0() {
   double input = 0.33677;
   input += synapse0x8bf0ee0();
   input += synapse0x8bf0f90();
   input += synapse0x8bf1040();
   input += synapse0x8bf10f0();
   input += synapse0x8bf11a0();
   input += synapse0x8bf1250();
   input += synapse0x8bf1300();
   return input;
}

double DC06_NNkaonS::neuron0x8bf0ce0() {
   double input = input0x8bf0ce0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf13b0() {
   double input = -0.361923;
   input += synapse0x8bf14d8();
   input += synapse0x8bf1500();
   input += synapse0x8bf1528();
   input += synapse0x8bf1550();
   input += synapse0x8bf1578();
   input += synapse0x8bf15a0();
   input += synapse0x8bf15c8();
   return input;
}

double DC06_NNkaonS::neuron0x8bf13b0() {
   double input = input0x8bf13b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf15f0() {
   double input = 0.226794;
   input += synapse0x8bf1718();
   input += synapse0x8bf1740();
   input += synapse0x8bf1768();
   input += synapse0x8bf1790();
   input += synapse0x8bf17b8();
   input += synapse0x8bf17e0();
   input += synapse0x8bf1808();
   return input;
}

double DC06_NNkaonS::neuron0x8bf15f0() {
   double input = input0x8bf15f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf1830() {
   double input = -0.0687821;
   input += synapse0x8bf19e8();
   input += synapse0x8bf1a10();
   input += synapse0x8bf1a38();
   input += synapse0x8a19c78();
   input += synapse0x8ad9cc8();
   input += synapse0x8ad9cf0();
   input += synapse0x8ad9d18();
   return input;
}

double DC06_NNkaonS::neuron0x8bf1830() {
   double input = input0x8bf1830();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8ad9d40() {
   double input = -0.439243;
   input += synapse0x8bf1ef8();
   input += synapse0x8bf1f20();
   input += synapse0x8bf1f48();
   input += synapse0x8bf1f70();
   input += synapse0x8bf1f98();
   input += synapse0x8bf1fc0();
   input += synapse0x8bf1fe8();
   return input;
}

double DC06_NNkaonS::neuron0x8ad9d40() {
   double input = input0x8ad9d40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaonS::input0x8bf2010() {
   double input = 0.0163465;
   input += synapse0x8bf21e8();
   input += synapse0x8bf2210();
   input += synapse0x8bf2238();
   input += synapse0x8bf2260();
   input += synapse0x8bf2288();
   input += synapse0x8bf22b0();
   input += synapse0x8bf4648();
   input += synapse0x8bf4670();
   input += synapse0x8bf4698();
   input += synapse0x8bf46c0();
   input += synapse0x8bf46e8();
   input += synapse0x8bf4710();
   input += synapse0x8abb128();
   input += synapse0x8abb150();
   input += synapse0x8abb178();
   input += synapse0x8abb1a0();
   input += synapse0x8abb250();
   input += synapse0x8abb278();
   input += synapse0x8abb2a0();
   input += synapse0x8abb2c8();
   return input;
}

double DC06_NNkaonS::neuron0x8bf2010() {
   double input = input0x8bf2010();
   return (input * 0.478732)+0.644275;
}

double DC06_NNkaonS::synapse0x8adb650() {
   return (neuron0x8a01138()*0.540625);
}

double DC06_NNkaonS::synapse0x8bf48d0() {
   return (neuron0x8a01280()*0.826768);
}

double DC06_NNkaonS::synapse0x8bf48f8() {
   return (neuron0x8be51c0()*-3.28904);
}

double DC06_NNkaonS::synapse0x8bf4920() {
   return (neuron0x8be53b8()*1.03967);
}

double DC06_NNkaonS::synapse0x8bf4948() {
   return (neuron0x8be55b0()*1.35965);
}

double DC06_NNkaonS::synapse0x8bf4970() {
   return (neuron0x8bf4230()*-1.30872);
}

double DC06_NNkaonS::synapse0x8bf4998() {
   return (neuron0x8bf4428()*1.54305);
}

double DC06_NNkaonS::synapse0x8bf4b98() {
   return (neuron0x8a01138()*0.426602);
}

double DC06_NNkaonS::synapse0x8bf4bc0() {
   return (neuron0x8a01280()*0.232434);
}

double DC06_NNkaonS::synapse0x8bf4be8() {
   return (neuron0x8be51c0()*-1.44748);
}

double DC06_NNkaonS::synapse0x8bf4c10() {
   return (neuron0x8be53b8()*0.593195);
}

double DC06_NNkaonS::synapse0x8bf4c38() {
   return (neuron0x8be55b0()*0.588618);
}

double DC06_NNkaonS::synapse0x8bf4c60() {
   return (neuron0x8bf4230()*-0.0145991);
}

double DC06_NNkaonS::synapse0x8bf4c88() {
   return (neuron0x8bf4428()*0.797935);
}

double DC06_NNkaonS::synapse0x8bf4e88() {
   return (neuron0x8a01138()*1.00102);
}

double DC06_NNkaonS::synapse0x8bf4eb0() {
   return (neuron0x8a01280()*-0.545269);
}

double DC06_NNkaonS::synapse0x8bf4ed8() {
   return (neuron0x8be51c0()*-0.473794);
}

double DC06_NNkaonS::synapse0x8be5760() {
   return (neuron0x8be53b8()*-0.125837);
}

double DC06_NNkaonS::synapse0x8a014a0() {
   return (neuron0x8be55b0()*-0.389245);
}

double DC06_NNkaonS::synapse0x8a014c8() {
   return (neuron0x8bf4230()*0.820046);
}

double DC06_NNkaonS::synapse0x8a19640() {
   return (neuron0x8bf4428()*1.86211);
}

double DC06_NNkaonS::synapse0x8a197f8() {
   return (neuron0x8a01138()*0.466942);
}

double DC06_NNkaonS::synapse0x8a19820() {
   return (neuron0x8a01280()*-0.269353);
}

double DC06_NNkaonS::synapse0x8a19848() {
   return (neuron0x8be51c0()*-1.33649);
}

double DC06_NNkaonS::synapse0x8a19870() {
   return (neuron0x8be53b8()*-0.303813);
}

double DC06_NNkaonS::synapse0x8a19898() {
   return (neuron0x8be55b0()*0.516536);
}

double DC06_NNkaonS::synapse0x8a198c0() {
   return (neuron0x8bf4230()*-0.810246);
}

double DC06_NNkaonS::synapse0x8a198e8() {
   return (neuron0x8bf4428()*-0.866565);
}

double DC06_NNkaonS::synapse0x8a19ae8() {
   return (neuron0x8a01138()*-0.0928323);
}

double DC06_NNkaonS::synapse0x8a19b10() {
   return (neuron0x8a01280()*0.523594);
}

double DC06_NNkaonS::synapse0x8a19b38() {
   return (neuron0x8be51c0()*3.93812);
}

double DC06_NNkaonS::synapse0x8a19b60() {
   return (neuron0x8be53b8()*-0.301488);
}

double DC06_NNkaonS::synapse0x8a19b88() {
   return (neuron0x8be55b0()*-0.246003);
}

double DC06_NNkaonS::synapse0x8bf4f00() {
   return (neuron0x8bf4230()*1.76649);
}

double DC06_NNkaonS::synapse0x8bf4f28() {
   return (neuron0x8bf4428()*-0.467642);
}

double DC06_NNkaonS::synapse0x8a19e90() {
   return (neuron0x8a01138()*-2.29966);
}

double DC06_NNkaonS::synapse0x8a19eb8() {
   return (neuron0x8a01280()*0.551249);
}

double DC06_NNkaonS::synapse0x8a19ee0() {
   return (neuron0x8be51c0()*1.25742);
}

double DC06_NNkaonS::synapse0x8a19f08() {
   return (neuron0x8be53b8()*0.293245);
}

double DC06_NNkaonS::synapse0x8a19f30() {
   return (neuron0x8be55b0()*-1.05496);
}

double DC06_NNkaonS::synapse0x8a19f58() {
   return (neuron0x8bf4230()*-2.06977);
}

double DC06_NNkaonS::synapse0x8a19f80() {
   return (neuron0x8bf4428()*-2.16476);
}

double DC06_NNkaonS::synapse0x8a1a180() {
   return (neuron0x8a01138()*1.00818);
}

double DC06_NNkaonS::synapse0x8a1a1a8() {
   return (neuron0x8a01280()*-0.0911264);
}

double DC06_NNkaonS::synapse0x8a1a1d0() {
   return (neuron0x8be51c0()*0.0758578);
}

double DC06_NNkaonS::synapse0x8a1a1f8() {
   return (neuron0x8be53b8()*-0.475458);
}

double DC06_NNkaonS::synapse0x8a1a220() {
   return (neuron0x8be55b0()*0.155445);
}

double DC06_NNkaonS::synapse0x8a1a248() {
   return (neuron0x8bf4230()*0.153093);
}

double DC06_NNkaonS::synapse0x8a1a270() {
   return (neuron0x8bf4428()*0.606349);
}

double DC06_NNkaonS::synapse0x8a1a448() {
   return (neuron0x8a01138()*-0.293754);
}

double DC06_NNkaonS::synapse0x8ad95f8() {
   return (neuron0x8a01280()*-0.220012);
}

double DC06_NNkaonS::synapse0x8ad9620() {
   return (neuron0x8be51c0()*-0.255127);
}

double DC06_NNkaonS::synapse0x8ad9648() {
   return (neuron0x8be53b8()*-0.0282623);
}

double DC06_NNkaonS::synapse0x8ad9670() {
   return (neuron0x8be55b0()*-0.00343769);
}

double DC06_NNkaonS::synapse0x8ad9698() {
   return (neuron0x8bf4230()*0.11108);
}

double DC06_NNkaonS::synapse0x8ad96c0() {
   return (neuron0x8bf4428()*0.445292);
}

double DC06_NNkaonS::synapse0x8ad98e0() {
   return (neuron0x8a01138()*0.187379);
}

double DC06_NNkaonS::synapse0x8ad9908() {
   return (neuron0x8a01280()*0.304831);
}

double DC06_NNkaonS::synapse0x8ad9930() {
   return (neuron0x8be51c0()*-0.659829);
}

double DC06_NNkaonS::synapse0x8ad9958() {
   return (neuron0x8be53b8()*-0.422081);
}

double DC06_NNkaonS::synapse0x8ad9980() {
   return (neuron0x8be55b0()*0.609692);
}

double DC06_NNkaonS::synapse0x8ad99a8() {
   return (neuron0x8bf4230()*-0.413501);
}

double DC06_NNkaonS::synapse0x8ad99d0() {
   return (neuron0x8bf4428()*-0.788944);
}

double DC06_NNkaonS::synapse0x8ad9c78() {
   return (neuron0x8a01138()*-0.450326);
}

double DC06_NNkaonS::synapse0x8ad9ca0() {
   return (neuron0x8a01280()*-0.0356851);
}

double DC06_NNkaonS::synapse0x8a01108() {
   return (neuron0x8be51c0()*-2.67231);
}

double DC06_NNkaonS::synapse0x8adb678() {
   return (neuron0x8be53b8()*0.256988);
}

double DC06_NNkaonS::synapse0x8adb998() {
   return (neuron0x8be55b0()*1.3412);
}

double DC06_NNkaonS::synapse0x8adb920() {
   return (neuron0x8bf4230()*-1.39139);
}

double DC06_NNkaonS::synapse0x8a19bb0() {
   return (neuron0x8bf4428()*0.773062);
}

double DC06_NNkaonS::synapse0x8ada0d0() {
   return (neuron0x8a01138()*0.101247);
}

double DC06_NNkaonS::synapse0x8ada0f8() {
   return (neuron0x8a01280()*-0.400797);
}

double DC06_NNkaonS::synapse0x8ada120() {
   return (neuron0x8be51c0()*-0.747361);
}

double DC06_NNkaonS::synapse0x8ada148() {
   return (neuron0x8be53b8()*-0.282187);
}

double DC06_NNkaonS::synapse0x8ada170() {
   return (neuron0x8be55b0()*0.487668);
}

double DC06_NNkaonS::synapse0x8ada198() {
   return (neuron0x8bf4230()*0.228287);
}

double DC06_NNkaonS::synapse0x8ada1c0() {
   return (neuron0x8bf4428()*0.501997);
}

double DC06_NNkaonS::synapse0x8ada3e8() {
   return (neuron0x8a01138()*0.543322);
}

double DC06_NNkaonS::synapse0x8ada410() {
   return (neuron0x8a01280()*-0.422605);
}

double DC06_NNkaonS::synapse0x8ada438() {
   return (neuron0x8be51c0()*-0.987625);
}

double DC06_NNkaonS::synapse0x8a19bd8() {
   return (neuron0x8be53b8()*0.100786);
}

double DC06_NNkaonS::synapse0x8a19c00() {
   return (neuron0x8be55b0()*0.596652);
}

double DC06_NNkaonS::synapse0x8a19c28() {
   return (neuron0x8bf4230()*-0.505282);
}

double DC06_NNkaonS::synapse0x8a19c50() {
   return (neuron0x8bf4428()*-0.638848);
}

double DC06_NNkaonS::synapse0x8bf0598() {
   return (neuron0x8a01138()*-0.572757);
}

double DC06_NNkaonS::synapse0x8bf05c0() {
   return (neuron0x8a01280()*-0.0421478);
}

double DC06_NNkaonS::synapse0x8bf05e8() {
   return (neuron0x8be51c0()*0.63223);
}

double DC06_NNkaonS::synapse0x8bf0610() {
   return (neuron0x8be53b8()*0.165568);
}

double DC06_NNkaonS::synapse0x8bf0638() {
   return (neuron0x8be55b0()*0.0800713);
}

double DC06_NNkaonS::synapse0x8bf0660() {
   return (neuron0x8bf4230()*-0.174011);
}

double DC06_NNkaonS::synapse0x8bf0688() {
   return (neuron0x8bf4428()*0.0892915);
}

double DC06_NNkaonS::synapse0x8bf08b0() {
   return (neuron0x8a01138()*-0.695041);
}

double DC06_NNkaonS::synapse0x8bf08d8() {
   return (neuron0x8a01280()*-0.00457067);
}

double DC06_NNkaonS::synapse0x8bf0900() {
   return (neuron0x8be51c0()*0.937175);
}

double DC06_NNkaonS::synapse0x8bf0928() {
   return (neuron0x8be53b8()*-0.212205);
}

double DC06_NNkaonS::synapse0x8bf0950() {
   return (neuron0x8be55b0()*-0.372794);
}

double DC06_NNkaonS::synapse0x8bf0978() {
   return (neuron0x8bf4230()*0.0382248);
}

double DC06_NNkaonS::synapse0x8bf09a0() {
   return (neuron0x8bf4428()*0.107124);
}

double DC06_NNkaonS::synapse0x8bf0bc8() {
   return (neuron0x8a01138()*-0.266363);
}

double DC06_NNkaonS::synapse0x8bf0bf0() {
   return (neuron0x8a01280()*0.51617);
}

double DC06_NNkaonS::synapse0x8bf0c18() {
   return (neuron0x8be51c0()*0.0281067);
}

double DC06_NNkaonS::synapse0x8bf0c40() {
   return (neuron0x8be53b8()*-0.182994);
}

double DC06_NNkaonS::synapse0x8bf0c68() {
   return (neuron0x8be55b0()*-0.375757);
}

double DC06_NNkaonS::synapse0x8bf0c90() {
   return (neuron0x8bf4230()*-0.287868);
}

double DC06_NNkaonS::synapse0x8bf0cb8() {
   return (neuron0x8bf4428()*-0.192751);
}

double DC06_NNkaonS::synapse0x8bf0ee0() {
   return (neuron0x8a01138()*-0.75156);
}

double DC06_NNkaonS::synapse0x8bf0f90() {
   return (neuron0x8a01280()*-0.0827165);
}

double DC06_NNkaonS::synapse0x8bf1040() {
   return (neuron0x8be51c0()*0.75821);
}

double DC06_NNkaonS::synapse0x8bf10f0() {
   return (neuron0x8be53b8()*-0.586956);
}

double DC06_NNkaonS::synapse0x8bf11a0() {
   return (neuron0x8be55b0()*-0.480438);
}

double DC06_NNkaonS::synapse0x8bf1250() {
   return (neuron0x8bf4230()*-0.222434);
}

double DC06_NNkaonS::synapse0x8bf1300() {
   return (neuron0x8bf4428()*0.173642);
}

double DC06_NNkaonS::synapse0x8bf14d8() {
   return (neuron0x8a01138()*0.681024);
}

double DC06_NNkaonS::synapse0x8bf1500() {
   return (neuron0x8a01280()*0.166942);
}

double DC06_NNkaonS::synapse0x8bf1528() {
   return (neuron0x8be51c0()*-0.124014);
}

double DC06_NNkaonS::synapse0x8bf1550() {
   return (neuron0x8be53b8()*0.00920244);
}

double DC06_NNkaonS::synapse0x8bf1578() {
   return (neuron0x8be55b0()*-0.713443);
}

double DC06_NNkaonS::synapse0x8bf15a0() {
   return (neuron0x8bf4230()*0.169124);
}

double DC06_NNkaonS::synapse0x8bf15c8() {
   return (neuron0x8bf4428()*0.776157);
}

double DC06_NNkaonS::synapse0x8bf1718() {
   return (neuron0x8a01138()*-0.767609);
}

double DC06_NNkaonS::synapse0x8bf1740() {
   return (neuron0x8a01280()*0.0859313);
}

double DC06_NNkaonS::synapse0x8bf1768() {
   return (neuron0x8be51c0()*0.290558);
}

double DC06_NNkaonS::synapse0x8bf1790() {
   return (neuron0x8be53b8()*-0.357412);
}

double DC06_NNkaonS::synapse0x8bf17b8() {
   return (neuron0x8be55b0()*0.291152);
}

double DC06_NNkaonS::synapse0x8bf17e0() {
   return (neuron0x8bf4230()*-0.375479);
}

double DC06_NNkaonS::synapse0x8bf1808() {
   return (neuron0x8bf4428()*-0.661151);
}

double DC06_NNkaonS::synapse0x8bf19e8() {
   return (neuron0x8a01138()*-0.0137304);
}

double DC06_NNkaonS::synapse0x8bf1a10() {
   return (neuron0x8a01280()*0.332496);
}

double DC06_NNkaonS::synapse0x8bf1a38() {
   return (neuron0x8be51c0()*0.719392);
}

double DC06_NNkaonS::synapse0x8a19c78() {
   return (neuron0x8be53b8()*-0.00367813);
}

double DC06_NNkaonS::synapse0x8ad9cc8() {
   return (neuron0x8be55b0()*0.613121);
}

double DC06_NNkaonS::synapse0x8ad9cf0() {
   return (neuron0x8bf4230()*-0.0987248);
}

double DC06_NNkaonS::synapse0x8ad9d18() {
   return (neuron0x8bf4428()*-0.751227);
}

double DC06_NNkaonS::synapse0x8bf1ef8() {
   return (neuron0x8a01138()*0.409118);
}

double DC06_NNkaonS::synapse0x8bf1f20() {
   return (neuron0x8a01280()*-0.0372333);
}

double DC06_NNkaonS::synapse0x8bf1f48() {
   return (neuron0x8be51c0()*-0.988166);
}

double DC06_NNkaonS::synapse0x8bf1f70() {
   return (neuron0x8be53b8()*0.24001);
}

double DC06_NNkaonS::synapse0x8bf1f98() {
   return (neuron0x8be55b0()*0.184905);
}

double DC06_NNkaonS::synapse0x8bf1fc0() {
   return (neuron0x8bf4230()*0.121772);
}

double DC06_NNkaonS::synapse0x8bf1fe8() {
   return (neuron0x8bf4428()*0.47956);
}

double DC06_NNkaonS::synapse0x8bf21e8() {
   return (neuron0x8bf4740()*-1.85126);
}

double DC06_NNkaonS::synapse0x8bf2210() {
   return (neuron0x8bf49c0()*-0.357852);
}

double DC06_NNkaonS::synapse0x8bf2238() {
   return (neuron0x8bf4cb0()*-1.73736);
}

double DC06_NNkaonS::synapse0x8bf2260() {
   return (neuron0x8a19668()*1.822);
}

double DC06_NNkaonS::synapse0x8bf2288() {
   return (neuron0x8a19910()*1.9572);
}

double DC06_NNkaonS::synapse0x8bf22b0() {
   return (neuron0x8a19cb8()*1.65758);
}

double DC06_NNkaonS::synapse0x8bf4648() {
   return (neuron0x8a19fa8()*-0.64477);
}

double DC06_NNkaonS::synapse0x8bf4670() {
   return (neuron0x8a1a298()*-0.374159);
}

double DC06_NNkaonS::synapse0x8bf4698() {
   return (neuron0x8ad96e8()*1.26805);
}

double DC06_NNkaonS::synapse0x8bf46c0() {
   return (neuron0x8ad99f8()*-1.17623);
}

double DC06_NNkaonS::synapse0x8bf46e8() {
   return (neuron0x8ad9ed0()*0.0893492);
}

double DC06_NNkaonS::synapse0x8bf4710() {
   return (neuron0x8ada1e8()*1.34779);
}

double DC06_NNkaonS::synapse0x8abb128() {
   return (neuron0x8bf0398()*-0.958265);
}

double DC06_NNkaonS::synapse0x8abb150() {
   return (neuron0x8bf06b0()*-1.32368);
}

double DC06_NNkaonS::synapse0x8abb178() {
   return (neuron0x8bf09c8()*-0.158685);
}

double DC06_NNkaonS::synapse0x8abb1a0() {
   return (neuron0x8bf0ce0()*-1.22531);
}

double DC06_NNkaonS::synapse0x8abb250() {
   return (neuron0x8bf13b0()*-0.675115);
}

double DC06_NNkaonS::synapse0x8abb278() {
   return (neuron0x8bf15f0()*0.112861);
}

double DC06_NNkaonS::synapse0x8abb2a0() {
   return (neuron0x8bf1830()*0.301284);
}

double DC06_NNkaonS::synapse0x8abb2c8() {
   return (neuron0x8ad9d40()*0.0851769);
}

