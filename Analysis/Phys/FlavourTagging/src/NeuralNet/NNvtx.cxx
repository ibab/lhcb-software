#include "NNvtx.h"
#include <cmath>

double NNvtx::value(int index,double in0,double in1,double in2,double in3,
                    double in4,double in5,double in6,double in7,double in8,double in9) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   input7 = (in7 - 0)/1;
   input8 = (in8 - 0)/1;
   input9 = (in9 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x17ccb4c0();
     default:
         return 0.;
   }
}

double NNvtx::neuron0x17c01840() {
   return input0;
}

double NNvtx::neuron0x17c01b50() {
   return input1;
}

double NNvtx::neuron0x17cc60b0() {
   return input2;
}

double NNvtx::neuron0x17cc63c0() {
   return input3;
}

double NNvtx::neuron0x17cc66d0() {
   return input4;
}

double NNvtx::neuron0x17cc6a10() {
   return input5;
}

double NNvtx::neuron0x17cc6d50() {
   return input6;
}

double NNvtx::neuron0x17cc7090() {
   return input7;
}

double NNvtx::neuron0x17cc73d0() {
   return input8;
}

double NNvtx::neuron0x17cc7710() {
   return input9;
}

double NNvtx::input0x17cc7b90() {
   double input = -0.055723;
   input += synapse0x179fcee0();
   input += synapse0x17c964d0();
   input += synapse0x17cc7ea0();
   input += synapse0x17cc7ee0();
   input += synapse0x17cc7f20();
   input += synapse0x17cc7f60();
   input += synapse0x17cc7fa0();
   input += synapse0x17cc7fe0();
   input += synapse0x17cc8020();
   input += synapse0x17cc8060();
   return input;
}

double NNvtx::neuron0x17cc7b90() {
   double input = input0x17cc7b90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc80a0() {
   double input = 0.114417;
   input += synapse0x17cc83b0();
   input += synapse0x17cc83f0();
   input += synapse0x17cc8430();
   input += synapse0x17cc8470();
   input += synapse0x17cc84b0();
   input += synapse0x17cc84f0();
   input += synapse0x17cc8530();
   input += synapse0x17a5fa60();
   input += synapse0x17a5faa0();
   input += synapse0x17c02dc0();
   return input;
}

double NNvtx::neuron0x17cc80a0() {
   double input = input0x17cc80a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc8680() {
   double input = -0.171223;
   input += synapse0x17cc8990();
   input += synapse0x17cc89d0();
   input += synapse0x17cc8a10();
   input += synapse0x17cc8a50();
   input += synapse0x17cc8a90();
   input += synapse0x17cc8ad0();
   input += synapse0x17cc8b10();
   input += synapse0x17cc8b50();
   input += synapse0x17cc8b90();
   input += synapse0x17cc8bd0();
   return input;
}

double NNvtx::neuron0x17cc8680() {
   double input = input0x17cc8680();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc8c10() {
   double input = -0.4011;
   input += synapse0x17cc8f20();
   input += synapse0x17cc8f60();
   input += synapse0x17cc8fa0();
   input += synapse0x17c02d80();
   input += synapse0x17c016f0();
   input += synapse0x179fca30();
   input += synapse0x17c02e00();
   input += synapse0x17cc8570();
   input += synapse0x17cc85b0();
   input += synapse0x17cc85f0();
   return input;
}

double NNvtx::neuron0x17cc8c10() {
   double input = input0x17cc8c10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc8fe0() {
   double input = 0.0240443;
   input += synapse0x17cc92f0();
   input += synapse0x17cc9330();
   input += synapse0x17cc9370();
   input += synapse0x17cc93b0();
   input += synapse0x17cc93f0();
   input += synapse0x17cc9430();
   input += synapse0x17cc9470();
   input += synapse0x17cc94b0();
   input += synapse0x17cc94f0();
   input += synapse0x17cc9530();
   return input;
}

double NNvtx::neuron0x17cc8fe0() {
   double input = input0x17cc8fe0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc9570() {
   double input = 0.228697;
   input += synapse0x17cc9880();
   input += synapse0x17cc98c0();
   input += synapse0x17cc9900();
   input += synapse0x17cc9940();
   input += synapse0x17cc9980();
   input += synapse0x17cc99c0();
   input += synapse0x17cc9a00();
   input += synapse0x17cc9a40();
   input += synapse0x17cc9a80();
   input += synapse0x17cc9ac0();
   return input;
}

double NNvtx::neuron0x17cc9570() {
   double input = input0x17cc9570();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cc9b00() {
   double input = 0.61027;
   input += synapse0x17cc8630();
   input += synapse0x17a5f8b0();
   input += synapse0x17a5f8f0();
   input += synapse0x17cc9f20();
   input += synapse0x17cc9f60();
   input += synapse0x179fd070();
   input += synapse0x179fd0b0();
   input += synapse0x179fd0f0();
   input += synapse0x179fd130();
   input += synapse0x179fd170();
   return input;
}

double NNvtx::neuron0x17cc9b00() {
   double input = input0x17cc9b00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cca3b0() {
   double input = -0.425987;
   input += synapse0x17cca6c0();
   input += synapse0x17cca700();
   input += synapse0x17cca740();
   input += synapse0x17cca780();
   input += synapse0x17cca7c0();
   input += synapse0x17cca800();
   input += synapse0x17cca840();
   input += synapse0x17cca880();
   input += synapse0x17cca8c0();
   input += synapse0x17cca900();
   return input;
}

double NNvtx::neuron0x17cca3b0() {
   double input = input0x17cca3b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17cca940() {
   double input = -0.253343;
   input += synapse0x17ccac80();
   input += synapse0x17ccacc0();
   input += synapse0x17ccad00();
   input += synapse0x17ccad40();
   input += synapse0x17ccad80();
   input += synapse0x17ccadc0();
   input += synapse0x17ccae00();
   input += synapse0x17ccae40();
   input += synapse0x17ccae80();
   input += synapse0x17ccaec0();
   return input;
}

double NNvtx::neuron0x17cca940() {
   double input = input0x17cca940();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17ccaf00() {
   double input = 0.359334;
   input += synapse0x17ccb240();
   input += synapse0x17ccb280();
   input += synapse0x17ccb2c0();
   input += synapse0x17ccb300();
   input += synapse0x17ccb340();
   input += synapse0x17ccb380();
   input += synapse0x17ccb3c0();
   input += synapse0x17ccb400();
   input += synapse0x17ccb440();
   input += synapse0x17ccb480();
   return input;
}

double NNvtx::neuron0x17ccaf00() {
   double input = input0x17ccaf00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x17ccb4c0() {
   double input = -0.255347;
   input += synapse0x17ccb800();
   input += synapse0x17ccb840();
   input += synapse0x17ccb880();
   input += synapse0x17ccb8c0();
   input += synapse0x17ccb900();
   input += synapse0x17ccb940();
   input += synapse0x17ccb980();
   input += synapse0x17ccb9c0();
   input += synapse0x17ccba00();
   input += synapse0x17ccba40();
   return input;
}

double NNvtx::neuron0x17ccb4c0() {
   double input = input0x17ccb4c0();
   return (input * 0.492455)+0.586534;
}

double NNvtx::synapse0x179fcee0() {
   return (neuron0x17c01840()*-0.221345);
}

double NNvtx::synapse0x17c964d0() {
   return (neuron0x17c01b50()*-0.479049);
}

double NNvtx::synapse0x17cc7ea0() {
   return (neuron0x17cc60b0()*-0.389796);
}

double NNvtx::synapse0x17cc7ee0() {
   return (neuron0x17cc63c0()*-0.934429);
}

double NNvtx::synapse0x17cc7f20() {
   return (neuron0x17cc66d0()*-0.515354);
}

double NNvtx::synapse0x17cc7f60() {
   return (neuron0x17cc6a10()*0.218918);
}

double NNvtx::synapse0x17cc7fa0() {
   return (neuron0x17cc6d50()*0.238819);
}

double NNvtx::synapse0x17cc7fe0() {
   return (neuron0x17cc7090()*-0.134991);
}

double NNvtx::synapse0x17cc8020() {
   return (neuron0x17cc73d0()*-0.409285);
}

double NNvtx::synapse0x17cc8060() {
   return (neuron0x17cc7710()*0.35829);
}

double NNvtx::synapse0x17cc83b0() {
   return (neuron0x17c01840()*0.536235);
}

double NNvtx::synapse0x17cc83f0() {
   return (neuron0x17c01b50()*-0.249094);
}

double NNvtx::synapse0x17cc8430() {
   return (neuron0x17cc60b0()*0.601351);
}

double NNvtx::synapse0x17cc8470() {
   return (neuron0x17cc63c0()*-0.136687);
}

double NNvtx::synapse0x17cc84b0() {
   return (neuron0x17cc66d0()*-0.044026);
}

double NNvtx::synapse0x17cc84f0() {
   return (neuron0x17cc6a10()*-0.418807);
}

double NNvtx::synapse0x17cc8530() {
   return (neuron0x17cc6d50()*-0.0499633);
}

double NNvtx::synapse0x17a5fa60() {
   return (neuron0x17cc7090()*0.25369);
}

double NNvtx::synapse0x17a5faa0() {
   return (neuron0x17cc73d0()*-0.30155);
}

double NNvtx::synapse0x17c02dc0() {
   return (neuron0x17cc7710()*-0.597004);
}

double NNvtx::synapse0x17cc8990() {
   return (neuron0x17c01840()*-0.0290857);
}

double NNvtx::synapse0x17cc89d0() {
   return (neuron0x17c01b50()*-0.13477);
}

double NNvtx::synapse0x17cc8a10() {
   return (neuron0x17cc60b0()*0.26419);
}

double NNvtx::synapse0x17cc8a50() {
   return (neuron0x17cc63c0()*0.379774);
}

double NNvtx::synapse0x17cc8a90() {
   return (neuron0x17cc66d0()*-0.178132);
}

double NNvtx::synapse0x17cc8ad0() {
   return (neuron0x17cc6a10()*-0.321493);
}

double NNvtx::synapse0x17cc8b10() {
   return (neuron0x17cc6d50()*0.290446);
}

double NNvtx::synapse0x17cc8b50() {
   return (neuron0x17cc7090()*-0.178154);
}

double NNvtx::synapse0x17cc8b90() {
   return (neuron0x17cc73d0()*0.238979);
}

double NNvtx::synapse0x17cc8bd0() {
   return (neuron0x17cc7710()*0.321944);
}

double NNvtx::synapse0x17cc8f20() {
   return (neuron0x17c01840()*-0.137099);
}

double NNvtx::synapse0x17cc8f60() {
   return (neuron0x17c01b50()*0.129315);
}

double NNvtx::synapse0x17cc8fa0() {
   return (neuron0x17cc60b0()*0.43948);
}

double NNvtx::synapse0x17c02d80() {
   return (neuron0x17cc63c0()*0.236806);
}

double NNvtx::synapse0x17c016f0() {
   return (neuron0x17cc66d0()*0.376396);
}

double NNvtx::synapse0x179fca30() {
   return (neuron0x17cc6a10()*0.00459779);
}

double NNvtx::synapse0x17c02e00() {
   return (neuron0x17cc6d50()*-0.335984);
}

double NNvtx::synapse0x17cc8570() {
   return (neuron0x17cc7090()*-0.298429);
}

double NNvtx::synapse0x17cc85b0() {
   return (neuron0x17cc73d0()*-0.369345);
}

double NNvtx::synapse0x17cc85f0() {
   return (neuron0x17cc7710()*-0.519158);
}

double NNvtx::synapse0x17cc92f0() {
   return (neuron0x17c01840()*0.124263);
}

double NNvtx::synapse0x17cc9330() {
   return (neuron0x17c01b50()*0.379186);
}

double NNvtx::synapse0x17cc9370() {
   return (neuron0x17cc60b0()*0.460658);
}

double NNvtx::synapse0x17cc93b0() {
   return (neuron0x17cc63c0()*-0.124661);
}

double NNvtx::synapse0x17cc93f0() {
   return (neuron0x17cc66d0()*0.1649);
}

double NNvtx::synapse0x17cc9430() {
   return (neuron0x17cc6a10()*-0.0791785);
}

double NNvtx::synapse0x17cc9470() {
   return (neuron0x17cc6d50()*-0.920411);
}

double NNvtx::synapse0x17cc94b0() {
   return (neuron0x17cc7090()*0.164344);
}

double NNvtx::synapse0x17cc94f0() {
   return (neuron0x17cc73d0()*0.189167);
}

double NNvtx::synapse0x17cc9530() {
   return (neuron0x17cc7710()*0.566277);
}

double NNvtx::synapse0x17cc9880() {
   return (neuron0x17c01840()*-0.468646);
}

double NNvtx::synapse0x17cc98c0() {
   return (neuron0x17c01b50()*0.22661);
}

double NNvtx::synapse0x17cc9900() {
   return (neuron0x17cc60b0()*-0.298416);
}

double NNvtx::synapse0x17cc9940() {
   return (neuron0x17cc63c0()*-0.201424);
}

double NNvtx::synapse0x17cc9980() {
   return (neuron0x17cc66d0()*0.57541);
}

double NNvtx::synapse0x17cc99c0() {
   return (neuron0x17cc6a10()*0.024372);
}

double NNvtx::synapse0x17cc9a00() {
   return (neuron0x17cc6d50()*-0.150739);
}

double NNvtx::synapse0x17cc9a40() {
   return (neuron0x17cc7090()*-0.212101);
}

double NNvtx::synapse0x17cc9a80() {
   return (neuron0x17cc73d0()*-0.284783);
}

double NNvtx::synapse0x17cc9ac0() {
   return (neuron0x17cc7710()*0.214395);
}

double NNvtx::synapse0x17cc8630() {
   return (neuron0x17c01840()*0.110974);
}

double NNvtx::synapse0x17a5f8b0() {
   return (neuron0x17c01b50()*0.182185);
}

double NNvtx::synapse0x17a5f8f0() {
   return (neuron0x17cc60b0()*0.397422);
}

double NNvtx::synapse0x17cc9f20() {
   return (neuron0x17cc63c0()*0.101115);
}

double NNvtx::synapse0x17cc9f60() {
   return (neuron0x17cc66d0()*-0.101493);
}

double NNvtx::synapse0x179fd070() {
   return (neuron0x17cc6a10()*0.177455);
}

double NNvtx::synapse0x179fd0b0() {
   return (neuron0x17cc6d50()*-0.253801);
}

double NNvtx::synapse0x179fd0f0() {
   return (neuron0x17cc7090()*-0.628361);
}

double NNvtx::synapse0x179fd130() {
   return (neuron0x17cc73d0()*0.236662);
}

double NNvtx::synapse0x179fd170() {
   return (neuron0x17cc7710()*-0.323249);
}

double NNvtx::synapse0x17cca6c0() {
   return (neuron0x17c01840()*0.14067);
}

double NNvtx::synapse0x17cca700() {
   return (neuron0x17c01b50()*-0.179295);
}

double NNvtx::synapse0x17cca740() {
   return (neuron0x17cc60b0()*-0.334131);
}

double NNvtx::synapse0x17cca780() {
   return (neuron0x17cc63c0()*-0.30983);
}

double NNvtx::synapse0x17cca7c0() {
   return (neuron0x17cc66d0()*-0.423702);
}

double NNvtx::synapse0x17cca800() {
   return (neuron0x17cc6a10()*-0.0434287);
}

double NNvtx::synapse0x17cca840() {
   return (neuron0x17cc6d50()*-0.0316741);
}

double NNvtx::synapse0x17cca880() {
   return (neuron0x17cc7090()*0.134762);
}

double NNvtx::synapse0x17cca8c0() {
   return (neuron0x17cc73d0()*-0.372503);
}

double NNvtx::synapse0x17cca900() {
   return (neuron0x17cc7710()*0.0308681);
}

double NNvtx::synapse0x17ccac80() {
   return (neuron0x17c01840()*0.45547);
}

double NNvtx::synapse0x17ccacc0() {
   return (neuron0x17c01b50()*0.0889857);
}

double NNvtx::synapse0x17ccad00() {
   return (neuron0x17cc60b0()*-0.270655);
}

double NNvtx::synapse0x17ccad40() {
   return (neuron0x17cc63c0()*0.238827);
}

double NNvtx::synapse0x17ccad80() {
   return (neuron0x17cc66d0()*-0.360771);
}

double NNvtx::synapse0x17ccadc0() {
   return (neuron0x17cc6a10()*-0.120253);
}

double NNvtx::synapse0x17ccae00() {
   return (neuron0x17cc6d50()*-0.351762);
}

double NNvtx::synapse0x17ccae40() {
   return (neuron0x17cc7090()*0.373353);
}

double NNvtx::synapse0x17ccae80() {
   return (neuron0x17cc73d0()*-0.101593);
}

double NNvtx::synapse0x17ccaec0() {
   return (neuron0x17cc7710()*-0.22031);
}

double NNvtx::synapse0x17ccb240() {
   return (neuron0x17c01840()*-0.479614);
}

double NNvtx::synapse0x17ccb280() {
   return (neuron0x17c01b50()*0.483143);
}

double NNvtx::synapse0x17ccb2c0() {
   return (neuron0x17cc60b0()*0.110346);
}

double NNvtx::synapse0x17ccb300() {
   return (neuron0x17cc63c0()*-0.359172);
}

double NNvtx::synapse0x17ccb340() {
   return (neuron0x17cc66d0()*0.530828);
}

double NNvtx::synapse0x17ccb380() {
   return (neuron0x17cc6a10()*0.593775);
}

double NNvtx::synapse0x17ccb3c0() {
   return (neuron0x17cc6d50()*-0.0307087);
}

double NNvtx::synapse0x17ccb400() {
   return (neuron0x17cc7090()*-0.083581);
}

double NNvtx::synapse0x17ccb440() {
   return (neuron0x17cc73d0()*-0.190205);
}

double NNvtx::synapse0x17ccb480() {
   return (neuron0x17cc7710()*0.456357);
}

double NNvtx::synapse0x17ccb800() {
   return (neuron0x17cc7b90()*-0.747535);
}

double NNvtx::synapse0x17ccb840() {
   return (neuron0x17cc80a0()*-0.458739);
}

double NNvtx::synapse0x17ccb880() {
   return (neuron0x17cc8680()*0.628624);
}

double NNvtx::synapse0x17ccb8c0() {
   return (neuron0x17cc8c10()*-0.536239);
}

double NNvtx::synapse0x17ccb900() {
   return (neuron0x17cc8fe0()*0.376385);
}

double NNvtx::synapse0x17ccb940() {
   return (neuron0x17cc9570()*0.504571);
}

double NNvtx::synapse0x17ccb980() {
   return (neuron0x17cc9b00()*-0.754602);
}

double NNvtx::synapse0x17ccb9c0() {
   return (neuron0x17cca3b0()*-0.414879);
}

double NNvtx::synapse0x17ccba00() {
   return (neuron0x17cca940()*0.193767);
}

double NNvtx::synapse0x17ccba40() {
   return (neuron0x17ccaf00()*0.689098);
}

