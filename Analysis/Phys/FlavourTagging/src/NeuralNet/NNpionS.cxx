#include "NNpionS.h"
#include <cmath>

double NNpionS::value(int index,double in0,double in1,double in2,double in3,
                      double in4,double in5,double in6,double in7,double in8) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   input7 = (in7 - 0)/1;
   input8 = (in8 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x9508af8();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x94f9730() {
   return input0;
}

double NNpionS::neuron0x94f98e0() {
   return input1;
}

double NNpionS::neuron0x94f9ad8() {
   return input2;
}

double NNpionS::neuron0x94f9cd0() {
   return input3;
}

double NNpionS::neuron0x9508158() {
   return input4;
}

double NNpionS::neuron0x9508330() {
   return input5;
}

double NNpionS::neuron0x9508508() {
   return input6;
}

double NNpionS::neuron0x9508700() {
   return input7;
}

double NNpionS::neuron0x95088f8() {
   return input8;
}

double NNpionS::input0x9508c00() {
   double input = -0.0339997;
   input += synapse0x946d3e0();
   input += synapse0x9508d90();
   input += synapse0x9508db8();
   input += synapse0x9508de0();
   input += synapse0x9508e08();
   input += synapse0x9508e30();
   input += synapse0x9508e58();
   input += synapse0x9508e80();
   input += synapse0x9508ea8();
   return input;
}

double NNpionS::neuron0x9508c00() {
   double input = input0x9508c00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9508ed0() {
   double input = 0.143455;
   input += synapse0x95090a8();
   input += synapse0x95090d0();
   input += synapse0x95090f8();
   input += synapse0x9509120();
   input += synapse0x9509148();
   input += synapse0x9509170();
   input += synapse0x9509198();
   input += synapse0x95091c0();
   input += synapse0x9509270();
   return input;
}

double NNpionS::neuron0x9508ed0() {
   double input = input0x9508ed0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9509298() {
   double input = 0.127889;
   input += synapse0x9509428();
   input += synapse0x9509450();
   input += synapse0x9509478();
   input += synapse0x95094a0();
   input += synapse0x95094c8();
   input += synapse0x95094f0();
   input += synapse0x9509518();
   input += synapse0x9509540();
   input += synapse0x9509568();
   return input;
}

double NNpionS::neuron0x9509298() {
   double input = input0x9509298();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9509590() {
   double input = -0.387437;
   input += synapse0x9509768();
   input += synapse0x9509790();
   input += synapse0x95097b8();
   input += synapse0x95097e0();
   input += synapse0x9509808();
   input += synapse0x9509830();
   input += synapse0x95091e8();
   input += synapse0x9509210();
   input += synapse0x9509238();
   return input;
}

double NNpionS::neuron0x9509590() {
   double input = input0x9509590();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9509960() {
   double input = 0.361002;
   input += synapse0x9509b38();
   input += synapse0x9509b60();
   input += synapse0x9509b88();
   input += synapse0x9509bb0();
   input += synapse0x9509bd8();
   input += synapse0x9509c00();
   input += synapse0x9509c28();
   input += synapse0x9509c50();
   input += synapse0x9509c78();
   return input;
}

double NNpionS::neuron0x9509960() {
   double input = input0x9509960();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9509ca0() {
   double input = -0.0499333;
   input += synapse0x9509e78();
   input += synapse0x9509ea0();
   input += synapse0x9509ec8();
   input += synapse0x9509ef0();
   input += synapse0x9509f18();
   input += synapse0x9509f40();
   input += synapse0x9509f68();
   input += synapse0x9509f90();
   input += synapse0x9509fb8();
   return input;
}

double NNpionS::neuron0x9509ca0() {
   double input = input0x9509ca0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9509fe0() {
   double input = 0.6125;
   input += synapse0x950a1b8();
   input += synapse0x950a1e0();
   input += synapse0x950a208();
   input += synapse0x950a230();
   input += synapse0x950a258();
   input += synapse0x950a280();
   input += synapse0x950a2a8();
   input += synapse0x950a2d0();
   input += synapse0x950a2f8();
   return input;
}

double NNpionS::neuron0x9509fe0() {
   double input = input0x9509fe0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x950a320() {
   double input = -0.323666;
   input += synapse0x950a580();
   input += synapse0x950a5a8();
   input += synapse0x946d408();
   input += synapse0x946d560();
   input += synapse0x946d4e8();
   input += synapse0x946d1c8();
   input += synapse0x944d940();
   input += synapse0x9509858();
   input += synapse0x9509880();
   return input;
}

double NNpionS::neuron0x950a320() {
   double input = input0x950a320();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x950a7d8() {
   double input = -1.66218;
   input += synapse0x9509938();
   input += synapse0x950a920();
   input += synapse0x950a948();
   input += synapse0x950a970();
   input += synapse0x950a998();
   input += synapse0x950a9c0();
   input += synapse0x950a9e8();
   input += synapse0x950aa10();
   input += synapse0x950aa38();
   return input;
}

double NNpionS::neuron0x950a7d8() {
   double input = input0x950a7d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x950aa60() {
   double input = -0.392374;
   input += synapse0x950ac58();
   input += synapse0x950ac80();
   input += synapse0x950aca8();
   input += synapse0x950acd0();
   input += synapse0x950acf8();
   input += synapse0x950ad20();
   input += synapse0x950ad48();
   input += synapse0x950ad70();
   input += synapse0x950ad98();
   return input;
}

double NNpionS::neuron0x950aa60() {
   double input = input0x950aa60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x9508af8() {
   double input = 0.440699;
   input += synapse0x950aeb8();
   input += synapse0x950aee0();
   input += synapse0x950af08();
   input += synapse0x950af30();
   input += synapse0x950af58();
   input += synapse0x950af80();
   input += synapse0x950afa8();
   input += synapse0x950afd0();
   input += synapse0x950aff8();
   input += synapse0x950b020();
   return input;
}

double NNpionS::neuron0x9508af8() {
   double input = input0x9508af8();
   return (input * 0.496667)+0.557632;
}

double NNpionS::synapse0x946d3e0() {
   return (neuron0x94f9730()*1.12427);
}

double NNpionS::synapse0x9508d90() {
   return (neuron0x94f98e0()*-0.0150343);
}

double NNpionS::synapse0x9508db8() {
   return (neuron0x94f9ad8()*-0.561812);
}

double NNpionS::synapse0x9508de0() {
   return (neuron0x94f9cd0()*0.703869);
}

double NNpionS::synapse0x9508e08() {
   return (neuron0x9508158()*0.491558);
}

double NNpionS::synapse0x9508e30() {
   return (neuron0x9508330()*-2.65145);
}

double NNpionS::synapse0x9508e58() {
   return (neuron0x9508508()*1.64321);
}

double NNpionS::synapse0x9508e80() {
   return (neuron0x9508700()*2.04755);
}

double NNpionS::synapse0x9508ea8() {
   return (neuron0x95088f8()*1.1044);
}

double NNpionS::synapse0x95090a8() {
   return (neuron0x94f9730()*-0.0600369);
}

double NNpionS::synapse0x95090d0() {
   return (neuron0x94f98e0()*-0.342364);
}

double NNpionS::synapse0x95090f8() {
   return (neuron0x94f9ad8()*1.32259);
}

double NNpionS::synapse0x9509120() {
   return (neuron0x94f9cd0()*0.158666);
}

double NNpionS::synapse0x9509148() {
   return (neuron0x9508158()*0.445964);
}

double NNpionS::synapse0x9509170() {
   return (neuron0x9508330()*-0.130562);
}

double NNpionS::synapse0x9509198() {
   return (neuron0x9508508()*-1.15204);
}

double NNpionS::synapse0x95091c0() {
   return (neuron0x9508700()*0.798389);
}

double NNpionS::synapse0x9509270() {
   return (neuron0x95088f8()*1.85957);
}

double NNpionS::synapse0x9509428() {
   return (neuron0x94f9730()*0.312594);
}

double NNpionS::synapse0x9509450() {
   return (neuron0x94f98e0()*0.0481175);
}

double NNpionS::synapse0x9509478() {
   return (neuron0x94f9ad8()*-0.897606);
}

double NNpionS::synapse0x95094a0() {
   return (neuron0x94f9cd0()*0.287554);
}

double NNpionS::synapse0x95094c8() {
   return (neuron0x9508158()*-0.0333617);
}

double NNpionS::synapse0x95094f0() {
   return (neuron0x9508330()*0.410508);
}

double NNpionS::synapse0x9509518() {
   return (neuron0x9508508()*1.89891);
}

double NNpionS::synapse0x9509540() {
   return (neuron0x9508700()*-0.366474);
}

double NNpionS::synapse0x9509568() {
   return (neuron0x95088f8()*-0.949567);
}

double NNpionS::synapse0x9509768() {
   return (neuron0x94f9730()*0.873614);
}

double NNpionS::synapse0x9509790() {
   return (neuron0x94f98e0()*0.189392);
}

double NNpionS::synapse0x95097b8() {
   return (neuron0x94f9ad8()*-1.00208);
}

double NNpionS::synapse0x95097e0() {
   return (neuron0x94f9cd0()*0.77029);
}

double NNpionS::synapse0x9509808() {
   return (neuron0x9508158()*-0.0514544);
}

double NNpionS::synapse0x9509830() {
   return (neuron0x9508330()*-0.056425);
}

double NNpionS::synapse0x95091e8() {
   return (neuron0x9508508()*0.256673);
}

double NNpionS::synapse0x9509210() {
   return (neuron0x9508700()*0.471533);
}

double NNpionS::synapse0x9509238() {
   return (neuron0x95088f8()*0.0365766);
}

double NNpionS::synapse0x9509b38() {
   return (neuron0x94f9730()*-0.843659);
}

double NNpionS::synapse0x9509b60() {
   return (neuron0x94f98e0()*-0.0308735);
}

double NNpionS::synapse0x9509b88() {
   return (neuron0x94f9ad8()*0.530075);
}

double NNpionS::synapse0x9509bb0() {
   return (neuron0x94f9cd0()*0.144692);
}

double NNpionS::synapse0x9509bd8() {
   return (neuron0x9508158()*-0.337438);
}

double NNpionS::synapse0x9509c00() {
   return (neuron0x9508330()*0.904853);
}

double NNpionS::synapse0x9509c28() {
   return (neuron0x9508508()*1.44138);
}

double NNpionS::synapse0x9509c50() {
   return (neuron0x9508700()*-0.411312);
}

double NNpionS::synapse0x9509c78() {
   return (neuron0x95088f8()*0.101301);
}

double NNpionS::synapse0x9509e78() {
   return (neuron0x94f9730()*0.0717742);
}

double NNpionS::synapse0x9509ea0() {
   return (neuron0x94f98e0()*0.474426);
}

double NNpionS::synapse0x9509ec8() {
   return (neuron0x94f9ad8()*0.982044);
}

double NNpionS::synapse0x9509ef0() {
   return (neuron0x94f9cd0()*-0.536286);
}

double NNpionS::synapse0x9509f18() {
   return (neuron0x9508158()*-0.975085);
}

double NNpionS::synapse0x9509f40() {
   return (neuron0x9508330()*0.0494528);
}

double NNpionS::synapse0x9509f68() {
   return (neuron0x9508508()*-2.24625);
}

double NNpionS::synapse0x9509f90() {
   return (neuron0x9508700()*0.247538);
}

double NNpionS::synapse0x9509fb8() {
   return (neuron0x95088f8()*-0.324759);
}

double NNpionS::synapse0x950a1b8() {
   return (neuron0x94f9730()*0.406084);
}

double NNpionS::synapse0x950a1e0() {
   return (neuron0x94f98e0()*0.195376);
}

double NNpionS::synapse0x950a208() {
   return (neuron0x94f9ad8()*-0.234984);
}

double NNpionS::synapse0x950a230() {
   return (neuron0x94f9cd0()*-0.231545);
}

double NNpionS::synapse0x950a258() {
   return (neuron0x9508158()*0.0053725);
}

double NNpionS::synapse0x950a280() {
   return (neuron0x9508330()*0.03576);
}

double NNpionS::synapse0x950a2a8() {
   return (neuron0x9508508()*0.676447);
}

double NNpionS::synapse0x950a2d0() {
   return (neuron0x9508700()*0.598767);
}

double NNpionS::synapse0x950a2f8() {
   return (neuron0x95088f8()*0.302322);
}

double NNpionS::synapse0x950a580() {
   return (neuron0x94f9730()*-0.00298527);
}

double NNpionS::synapse0x950a5a8() {
   return (neuron0x94f98e0()*0.211275);
}

double NNpionS::synapse0x946d408() {
   return (neuron0x94f9ad8()*-1.2);
}

double NNpionS::synapse0x946d560() {
   return (neuron0x94f9cd0()*0.0872059);
}

double NNpionS::synapse0x946d4e8() {
   return (neuron0x9508158()*-0.393885);
}

double NNpionS::synapse0x946d1c8() {
   return (neuron0x9508330()*0.540737);
}

double NNpionS::synapse0x944d940() {
   return (neuron0x9508508()*1.27221);
}

double NNpionS::synapse0x9509858() {
   return (neuron0x9508700()*-0.328634);
}

double NNpionS::synapse0x9509880() {
   return (neuron0x95088f8()*-1.33754);
}

double NNpionS::synapse0x9509938() {
   return (neuron0x94f9730()*-1.13147);
}

double NNpionS::synapse0x950a920() {
   return (neuron0x94f98e0()*0.691049);
}

double NNpionS::synapse0x950a948() {
   return (neuron0x94f9ad8()*1.08367);
}

double NNpionS::synapse0x950a970() {
   return (neuron0x94f9cd0()*-1.26108);
}

double NNpionS::synapse0x950a998() {
   return (neuron0x9508158()*0.228924);
}

double NNpionS::synapse0x950a9c0() {
   return (neuron0x9508330()*-2.37064);
}

double NNpionS::synapse0x950a9e8() {
   return (neuron0x9508508()*-3.39192);
}

double NNpionS::synapse0x950aa10() {
   return (neuron0x9508700()*0.316141);
}

double NNpionS::synapse0x950aa38() {
   return (neuron0x95088f8()*-0.000360084);
}

double NNpionS::synapse0x950ac58() {
   return (neuron0x94f9730()*-0.0764819);
}

double NNpionS::synapse0x950ac80() {
   return (neuron0x94f98e0()*-0.471129);
}

double NNpionS::synapse0x950aca8() {
   return (neuron0x94f9ad8()*-0.426756);
}

double NNpionS::synapse0x950acd0() {
   return (neuron0x94f9cd0()*-0.293069);
}

double NNpionS::synapse0x950acf8() {
   return (neuron0x9508158()*-0.601599);
}

double NNpionS::synapse0x950ad20() {
   return (neuron0x9508330()*0.293255);
}

double NNpionS::synapse0x950ad48() {
   return (neuron0x9508508()*0.330468);
}

double NNpionS::synapse0x950ad70() {
   return (neuron0x9508700()*0.431786);
}

double NNpionS::synapse0x950ad98() {
   return (neuron0x95088f8()*0.0248647);
}

double NNpionS::synapse0x950aeb8() {
   return (neuron0x9508c00()*-1.60532);
}

double NNpionS::synapse0x950aee0() {
   return (neuron0x9508ed0()*1.27822);
}

double NNpionS::synapse0x950af08() {
   return (neuron0x9509298()*-0.589719);
}

double NNpionS::synapse0x950af30() {
   return (neuron0x9509590()*0.75592);
}

double NNpionS::synapse0x950af58() {
   return (neuron0x9509960()*-1.58228);
}

double NNpionS::synapse0x950af80() {
   return (neuron0x9509ca0()*0.699282);
}

double NNpionS::synapse0x950afa8() {
   return (neuron0x9509fe0()*-0.240607);
}

double NNpionS::synapse0x950afd0() {
   return (neuron0x950a320()*-0.620944);
}

double NNpionS::synapse0x950aff8() {
   return (neuron0x950a7d8()*2.50363);
}

double NNpionS::synapse0x950b020() {
   return (neuron0x950aa60()*0.446261);
}

