#include "NNkaonS.h"
#include "math.h"

double NNkaonS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron0xa3f9b18()*0.472795)+0.662681);
     default:
         return 0.;
   }
}

double NNkaonS::neuron0xa41f2d8() {
   return input0;
}

double NNkaonS::neuron0xa41f3e8() {
   return input1;
}

double NNkaonS::neuron0xa412320() {
   return input2;
}

double NNkaonS::neuron0xa412478() {
   return input3;
}

double NNkaonS::neuron0xa3ea558() {
   return input4;
}

double NNkaonS::neuron0xa3ea6c8() {
   return input5;
}

double NNkaonS::neuron0xa3f99a8() {
   return input6;
}

double NNkaonS::neuron0xa3f9c30() {
   double input = -0.672325;
   input += synapse0xa3ea880();
   input += synapse0xa41f540();
   input += synapse0xa41f568();
   input += synapse0xa41f590();
   input += synapse0xa4125d0();
   input += synapse0xa4125f8();
   input += synapse0xa412620();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa40e4a0() {
   double input = -0.103228;
   input += synapse0xa40e618();
   input += synapse0xa40e640();
   input += synapse0xa40e668();
   input += synapse0xa40e690();
   input += synapse0xa40e6b8();
   input += synapse0xa40e6e0();
   input += synapse0xa40e708();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa40e730() {
   double input = -0.430108;
   input += synapse0xa514710();
   input += synapse0xa514738();
   input += synapse0xa514760();
   input += synapse0xa514810();
   input += synapse0xa514838();
   input += synapse0xa514860();
   input += synapse0xa514888();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa5148b0() {
   double input = 0.337842;
   input += synapse0xa411aa0();
   input += synapse0xa411ac8();
   input += synapse0xa411af0();
   input += synapse0xa411b18();
   input += synapse0xa411b40();
   input += synapse0xa411b68();
   input += synapse0xa411b90();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa411bb8() {
   double input = 0.0329652;
   input += synapse0xa411d10();
   input += synapse0xa411d38();
   input += synapse0xa411d60();
   input += synapse0xa411d88();
   input += synapse0xa411db0();
   input += synapse0xa411dd8();
   input += synapse0xa411e00();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa3f2400() {
   double input = 0.211548;
   input += synapse0xa3f2578();
   input += synapse0xa3f25a0();
   input += synapse0xa3f25c8();
   input += synapse0xa3f25f0();
   input += synapse0xa3f2618();
   input += synapse0xa3f2640();
   input += synapse0xa3f2668();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa4055b0() {
   double input = -0.48307;
   input += synapse0xa405728();
   input += synapse0xa405750();
   input += synapse0xa405778();
   input += synapse0xa4057a0();
   input += synapse0xa4057c8();
   input += synapse0xa4057f0();
   input += synapse0xa405818();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaonS::neuron0xa3f9b18() {
   double input = 0.42071;
   input += synapse0xa3f26d8();
   input += synapse0xa3f9c00();
   input += synapse0xa3f2700();
   input += synapse0xa405840();
   input += synapse0xa405868();
   input += synapse0xa405890();
   input += synapse0xa4058b8();
   return input;
}

double NNkaonS::synapse0xa3ea880() {
   return (neuron0xa41f2d8()*0.33712);
}

double NNkaonS::synapse0xa41f540() {
   return (neuron0xa41f3e8()*-0.124061);
}

double NNkaonS::synapse0xa41f568() {
   return (neuron0xa412320()*-0.74457);
}

double NNkaonS::synapse0xa41f590() {
   return (neuron0xa412478()*0.489421);
}

double NNkaonS::synapse0xa4125d0() {
   return (neuron0xa3ea558()*-0.699029);
}

double NNkaonS::synapse0xa4125f8() {
   return (neuron0xa3ea6c8()*-0.106009);
}

double NNkaonS::synapse0xa412620() {
   return (neuron0xa3f99a8()*0.640412);
}

double NNkaonS::synapse0xa40e618() {
   return (neuron0xa41f2d8()*0.889517);
}

double NNkaonS::synapse0xa40e640() {
   return (neuron0xa41f3e8()*-0.657106);
}

double NNkaonS::synapse0xa40e668() {
   return (neuron0xa412320()*-0.934779);
}

double NNkaonS::synapse0xa40e690() {
   return (neuron0xa412478()*-0.139971);
}

double NNkaonS::synapse0xa40e6b8() {
   return (neuron0xa3ea558()*0.425414);
}

double NNkaonS::synapse0xa40e6e0() {
   return (neuron0xa3ea6c8()*0.754989);
}

double NNkaonS::synapse0xa40e708() {
   return (neuron0xa3f99a8()*-1.15724);
}

double NNkaonS::synapse0xa514710() {
   return (neuron0xa41f2d8()*0.487437);
}

double NNkaonS::synapse0xa514738() {
   return (neuron0xa41f3e8()*-0.0450455);
}

double NNkaonS::synapse0xa514760() {
   return (neuron0xa412320()*-0.0648351);
}

double NNkaonS::synapse0xa514810() {
   return (neuron0xa412478()*0.410791);
}

double NNkaonS::synapse0xa514838() {
   return (neuron0xa3ea558()*0.288546);
}

double NNkaonS::synapse0xa514860() {
   return (neuron0xa3ea6c8()*0.295216);
}

double NNkaonS::synapse0xa514888() {
   return (neuron0xa3f99a8()*-0.389859);
}

double NNkaonS::synapse0xa411aa0() {
   return (neuron0xa41f2d8()*0.39759);
}

double NNkaonS::synapse0xa411ac8() {
   return (neuron0xa41f3e8()*0.113277);
}

double NNkaonS::synapse0xa411af0() {
   return (neuron0xa412320()*-0.773988);
}

double NNkaonS::synapse0xa411b18() {
   return (neuron0xa412478()*0.214856);
}

double NNkaonS::synapse0xa411b40() {
   return (neuron0xa3ea558()*0.129933);
}

double NNkaonS::synapse0xa411b68() {
   return (neuron0xa3ea6c8()*0.587651);
}

double NNkaonS::synapse0xa411b90() {
   return (neuron0xa3f99a8()*0.925531);
}

double NNkaonS::synapse0xa411d10() {
   return (neuron0xa41f2d8()*-1.05516);
}

double NNkaonS::synapse0xa411d38() {
   return (neuron0xa41f3e8()*1.58881);
}

double NNkaonS::synapse0xa411d60() {
   return (neuron0xa412320()*2.11899);
}

double NNkaonS::synapse0xa411d88() {
   return (neuron0xa412478()*-0.61153);
}

double NNkaonS::synapse0xa411db0() {
   return (neuron0xa3ea558()*-0.031167);
}

double NNkaonS::synapse0xa411dd8() {
   return (neuron0xa3ea6c8()*-1.33742);
}

double NNkaonS::synapse0xa411e00() {
   return (neuron0xa3f99a8()*-2.23074);
}

double NNkaonS::synapse0xa3f2578() {
   return (neuron0xa41f2d8()*-0.424179);
}

double NNkaonS::synapse0xa3f25a0() {
   return (neuron0xa41f3e8()*0.502813);
}

double NNkaonS::synapse0xa3f25c8() {
   return (neuron0xa412320()*0.301032);
}

double NNkaonS::synapse0xa3f25f0() {
   return (neuron0xa412478()*0.468652);
}

double NNkaonS::synapse0xa3f2618() {
   return (neuron0xa3ea558()*-0.249506);
}

double NNkaonS::synapse0xa3f2640() {
   return (neuron0xa3ea6c8()*0.00915284);
}

double NNkaonS::synapse0xa3f2668() {
   return (neuron0xa3f99a8()*0.770811);
}

double NNkaonS::synapse0xa405728() {
   return (neuron0xa41f2d8()*0.281451);
}

double NNkaonS::synapse0xa405750() {
   return (neuron0xa41f3e8()*0.633376);
}

double NNkaonS::synapse0xa405778() {
   return (neuron0xa412320()*-0.318552);
}

double NNkaonS::synapse0xa4057a0() {
   return (neuron0xa412478()*0.395635);
}

double NNkaonS::synapse0xa4057c8() {
   return (neuron0xa3ea558()*0.495159);
}

double NNkaonS::synapse0xa4057f0() {
   return (neuron0xa3ea6c8()*0.34604);
}

double NNkaonS::synapse0xa405818() {
   return (neuron0xa3f99a8()*2.44112);
}

double NNkaonS::synapse0xa3f26d8() {
   return (neuron0xa3f9c30()*-0.751969);
}

double NNkaonS::synapse0xa3f9c00() {
   return (neuron0xa40e4a0()*1.58327);
}

double NNkaonS::synapse0xa3f2700() {
   return (neuron0xa40e730()*0.298424);
}

double NNkaonS::synapse0xa405840() {
   return (neuron0xa5148b0()*-0.633145);
}

double NNkaonS::synapse0xa405868() {
   return (neuron0xa411bb8()*2.25024);
}

double NNkaonS::synapse0xa405890() {
   return (neuron0xa3f2400()*-0.956055);
}

double NNkaonS::synapse0xa4058b8() {
   return (neuron0xa4055b0()*-2.65907);
}

