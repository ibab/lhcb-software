#include "weights/NNmuon.h"
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
         return neuron0x878ac20();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x851e278() {
   return input0;
}

double NNmuon::neuron0x851e3c0() {
   return input1;
}

double NNmuon::neuron0x851e5b8() {
   return input2;
}

double NNmuon::neuron0x851e7b0() {
   return input3;
}

double NNmuon::neuron0x851e9a8() {
   return input4;
}

double NNmuon::neuron0x851eba0() {
   return input5;
}

double NNmuon::neuron0x851ed98() {
   return input6;
}

double NNmuon::input0x8789df0() {
   double input = 0.614429;
   input += synapse0x84ef628();
   input += synapse0x8789f80();
   input += synapse0x8789fa8();
   input += synapse0x8789fd0();
   input += synapse0x8789ff8();
   input += synapse0x878a020();
   input += synapse0x878a048();
   return input;
}

double NNmuon::neuron0x8789df0() {
   double input = input0x8789df0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878a070() {
   double input = -0.0370679;
   input += synapse0x878a248();
   input += synapse0x878a270();
   input += synapse0x878a298();
   input += synapse0x878a2c0();
   input += synapse0x878a2e8();
   input += synapse0x878a310();
   input += synapse0x878a338();
   return input;
}

double NNmuon::neuron0x878a070() {
   double input = input0x878a070();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878a360() {
   double input = -0.683384;
   input += synapse0x878a538();
   input += synapse0x878a560();
   input += synapse0x878a588();
   input += synapse0x878a638();
   input += synapse0x878a660();
   input += synapse0x878a688();
   input += synapse0x878a6b0();
   return input;
}

double NNmuon::neuron0x878a360() {
   double input = input0x878a360();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878a6d8() {
   double input = -0.160265;
   input += synapse0x878a868();
   input += synapse0x878a890();
   input += synapse0x878a8b8();
   input += synapse0x878a8e0();
   input += synapse0x878a908();
   input += synapse0x878a930();
   input += synapse0x878a958();
   return input;
}

double NNmuon::neuron0x878a6d8() {
   double input = input0x878a6d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878a980() {
   double input = -0.560629;
   input += synapse0x878ab58();
   input += synapse0x878ab80();
   input += synapse0x878aba8();
   input += synapse0x878abd0();
   input += synapse0x878abf8();
   input += synapse0x851ef70();
   input += synapse0x851ef98();
   return input;
}

double NNmuon::neuron0x878a980() {
   double input = input0x878a980();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878ad28() {
   double input = 0.977849;
   input += synapse0x878af00();
   input += synapse0x878af28();
   input += synapse0x878af50();
   input += synapse0x878af78();
   input += synapse0x878afa0();
   input += synapse0x878afc8();
   input += synapse0x878aff0();
   return input;
}

double NNmuon::neuron0x878ad28() {
   double input = input0x878ad28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878b018() {
   double input = -0.616402;
   input += synapse0x878b210();
   input += synapse0x878b238();
   input += synapse0x878b260();
   input += synapse0x878b288();
   input += synapse0x878b2b0();
   input += synapse0x878b2d8();
   input += synapse0x878b300();
   return input;
}

double NNmuon::neuron0x878b018() {
   double input = input0x878b018();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878b328() {
   double input = 1.08281;
   input += synapse0x878b520();
   input += synapse0x878b548();
   input += synapse0x878b570();
   input += synapse0x878b598();
   input += synapse0x878b5c0();
   input += synapse0x878b5e8();
   input += synapse0x878b610();
   return input;
}

double NNmuon::neuron0x878b328() {
   double input = input0x878b328();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878b638() {
   double input = 0.0655689;
   input += synapse0x878b830();
   input += synapse0x878b858();
   input += synapse0x878b880();
   input += synapse0x878b8a8();
   input += synapse0x878b8d0();
   input += synapse0x878b8f8();
   input += synapse0x878b920();
   return input;
}

double NNmuon::neuron0x878b638() {
   double input = input0x878b638();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878b948() {
   double input = -0.902516;
   input += synapse0x878bb40();
   input += synapse0x878bb68();
   input += synapse0x851e158();
   input += synapse0x854cf28();
   input += synapse0x854ceb0();
   input += synapse0x86e6738();
   input += synapse0x854cbe0();
   return input;
}

double NNmuon::neuron0x878b948() {
   double input = input0x878b948();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x878ac20() {
   double input = -0.487846;
   input += synapse0x8789da8();
   input += synapse0x878bd98();
   input += synapse0x878bdc0();
   input += synapse0x878bde8();
   input += synapse0x878be10();
   input += synapse0x878be38();
   input += synapse0x878be60();
   input += synapse0x878be88();
   input += synapse0x878beb0();
   input += synapse0x878bed8();
   return input;
}

double NNmuon::neuron0x878ac20() {
   double input = input0x878ac20();
   return (input * 0.46042)+0.694971;
}

double NNmuon::synapse0x84ef628() {
   return (neuron0x851e278()*-0.906431);
}

double NNmuon::synapse0x8789f80() {
   return (neuron0x851e3c0()*-0.312638);
}

double NNmuon::synapse0x8789fa8() {
   return (neuron0x851e5b8()*2.39213);
}

double NNmuon::synapse0x8789fd0() {
   return (neuron0x851e7b0()*0.0942862);
}

double NNmuon::synapse0x8789ff8() {
   return (neuron0x851e9a8()*-0.726742);
}

double NNmuon::synapse0x878a020() {
   return (neuron0x851eba0()*-0.325113);
}

double NNmuon::synapse0x878a048() {
   return (neuron0x851ed98()*0.103596);
}

double NNmuon::synapse0x878a248() {
   return (neuron0x851e278()*0.717657);
}

double NNmuon::synapse0x878a270() {
   return (neuron0x851e3c0()*-0.494942);
}

double NNmuon::synapse0x878a298() {
   return (neuron0x851e5b8()*-0.382775);
}

double NNmuon::synapse0x878a2c0() {
   return (neuron0x851e7b0()*0.750031);
}

double NNmuon::synapse0x878a2e8() {
   return (neuron0x851e9a8()*0.166326);
}

double NNmuon::synapse0x878a310() {
   return (neuron0x851eba0()*-0.0132721);
}

double NNmuon::synapse0x878a338() {
   return (neuron0x851ed98()*-0.0829581);
}

double NNmuon::synapse0x878a538() {
   return (neuron0x851e278()*-4.52241);
}

double NNmuon::synapse0x878a560() {
   return (neuron0x851e3c0()*0.228085);
}

double NNmuon::synapse0x878a588() {
   return (neuron0x851e5b8()*-1.5218);
}

double NNmuon::synapse0x878a638() {
   return (neuron0x851e7b0()*-3.07852);
}

double NNmuon::synapse0x878a660() {
   return (neuron0x851e9a8()*-0.147867);
}

double NNmuon::synapse0x878a688() {
   return (neuron0x851eba0()*-0.995336);
}

double NNmuon::synapse0x878a6b0() {
   return (neuron0x851ed98()*-2.14597);
}

double NNmuon::synapse0x878a868() {
   return (neuron0x851e278()*0.106059);
}

double NNmuon::synapse0x878a890() {
   return (neuron0x851e3c0()*-0.478191);
}

double NNmuon::synapse0x878a8b8() {
   return (neuron0x851e5b8()*-0.524039);
}

double NNmuon::synapse0x878a8e0() {
   return (neuron0x851e7b0()*0.0484609);
}

double NNmuon::synapse0x878a908() {
   return (neuron0x851e9a8()*-0.0307593);
}

double NNmuon::synapse0x878a930() {
   return (neuron0x851eba0()*-0.414532);
}

double NNmuon::synapse0x878a958() {
   return (neuron0x851ed98()*0.141528);
}

double NNmuon::synapse0x878ab58() {
   return (neuron0x851e278()*-0.500042);
}

double NNmuon::synapse0x878ab80() {
   return (neuron0x851e3c0()*0.669698);
}

double NNmuon::synapse0x878aba8() {
   return (neuron0x851e5b8()*-0.414532);
}

double NNmuon::synapse0x878abd0() {
   return (neuron0x851e7b0()*3.29586);
}

double NNmuon::synapse0x878abf8() {
   return (neuron0x851e9a8()*0.441621);
}

double NNmuon::synapse0x851ef70() {
   return (neuron0x851eba0()*0.934888);
}

double NNmuon::synapse0x851ef98() {
   return (neuron0x851ed98()*0.713193);
}

double NNmuon::synapse0x878af00() {
   return (neuron0x851e278()*0.656165);
}

double NNmuon::synapse0x878af28() {
   return (neuron0x851e3c0()*-3.40581);
}

double NNmuon::synapse0x878af50() {
   return (neuron0x851e5b8()*-0.834557);
}

double NNmuon::synapse0x878af78() {
   return (neuron0x851e7b0()*-4.50368);
}

double NNmuon::synapse0x878afa0() {
   return (neuron0x851e9a8()*-0.687956);
}

double NNmuon::synapse0x878afc8() {
   return (neuron0x851eba0()*0.0114822);
}

double NNmuon::synapse0x878aff0() {
   return (neuron0x851ed98()*-0.292844);
}

double NNmuon::synapse0x878b210() {
   return (neuron0x851e278()*0.576572);
}

double NNmuon::synapse0x878b238() {
   return (neuron0x851e3c0()*-0.852121);
}

double NNmuon::synapse0x878b260() {
   return (neuron0x851e5b8()*-0.0620506);
}

double NNmuon::synapse0x878b288() {
   return (neuron0x851e7b0()*0.159579);
}

double NNmuon::synapse0x878b2b0() {
   return (neuron0x851e9a8()*0.0675665);
}

double NNmuon::synapse0x878b2d8() {
   return (neuron0x851eba0()*0.116412);
}

double NNmuon::synapse0x878b300() {
   return (neuron0x851ed98()*-0.0248692);
}

double NNmuon::synapse0x878b520() {
   return (neuron0x851e278()*0.342088);
}

double NNmuon::synapse0x878b548() {
   return (neuron0x851e3c0()*-0.548668);
}

double NNmuon::synapse0x878b570() {
   return (neuron0x851e5b8()*1.52637);
}

double NNmuon::synapse0x878b598() {
   return (neuron0x851e7b0()*-1.72984);
}

double NNmuon::synapse0x878b5c0() {
   return (neuron0x851e9a8()*0.50132);
}

double NNmuon::synapse0x878b5e8() {
   return (neuron0x851eba0()*-0.107651);
}

double NNmuon::synapse0x878b610() {
   return (neuron0x851ed98()*1.11141);
}

double NNmuon::synapse0x878b830() {
   return (neuron0x851e278()*-0.0705066);
}

double NNmuon::synapse0x878b858() {
   return (neuron0x851e3c0()*-0.205191);
}

double NNmuon::synapse0x878b880() {
   return (neuron0x851e5b8()*0.0431846);
}

double NNmuon::synapse0x878b8a8() {
   return (neuron0x851e7b0()*-0.551643);
}

double NNmuon::synapse0x878b8d0() {
   return (neuron0x851e9a8()*-0.279522);
}

double NNmuon::synapse0x878b8f8() {
   return (neuron0x851eba0()*-0.22825);
}

double NNmuon::synapse0x878b920() {
   return (neuron0x851ed98()*-0.631766);
}

double NNmuon::synapse0x878bb40() {
   return (neuron0x851e278()*0.801265);
}

double NNmuon::synapse0x878bb68() {
   return (neuron0x851e3c0()*-0.0462158);
}

double NNmuon::synapse0x851e158() {
   return (neuron0x851e5b8()*-2.01416);
}

double NNmuon::synapse0x854cf28() {
   return (neuron0x851e7b0()*0.243463);
}

double NNmuon::synapse0x854ceb0() {
   return (neuron0x851e9a8()*-0.113477);
}

double NNmuon::synapse0x86e6738() {
   return (neuron0x851eba0()*-0.0755445);
}

double NNmuon::synapse0x854cbe0() {
   return (neuron0x851ed98()*-0.597662);
}

double NNmuon::synapse0x8789da8() {
   return (neuron0x8789df0()*1.14073);
}

double NNmuon::synapse0x878bd98() {
   return (neuron0x878a070()*0.853893);
}

double NNmuon::synapse0x878bdc0() {
   return (neuron0x878a360()*4.14912);
}

double NNmuon::synapse0x878bde8() {
   return (neuron0x878a6d8()*0.34975);
}

double NNmuon::synapse0x878be10() {
   return (neuron0x878a980()*-1.71256);
}

double NNmuon::synapse0x878be38() {
   return (neuron0x878ad28()*-2.65574);
}

double NNmuon::synapse0x878be60() {
   return (neuron0x878b018()*0.688819);
}

double NNmuon::synapse0x878be88() {
   return (neuron0x878b328()*0.408926);
}

double NNmuon::synapse0x878beb0() {
   return (neuron0x878b638()*-0.101958);
}

double NNmuon::synapse0x878bed8() {
   return (neuron0x878b948()*-1.07468);
}

