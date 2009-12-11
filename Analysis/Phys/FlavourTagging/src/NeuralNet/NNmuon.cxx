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
         return neuron0x875fe50();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x86ca0a8() {
   return input0;
}

double NNmuon::neuron0x85180f0() {
   return input1;
}

double NNmuon::neuron0x85182e8() {
   return input2;
}

double NNmuon::neuron0x85184e0() {
   return input3;
}

double NNmuon::neuron0x85186d8() {
   return input4;
}

double NNmuon::neuron0x875eb30() {
   return input5;
}

double NNmuon::neuron0x875ed28() {
   return input6;
}

double NNmuon::input0x875f020() {
   double input = 0.329033;
   input += synapse0x84fd378();
   input += synapse0x875f1b0();
   input += synapse0x875f1d8();
   input += synapse0x875f200();
   input += synapse0x875f228();
   input += synapse0x875f250();
   input += synapse0x875f278();
   return input;
}

double NNmuon::neuron0x875f020() {
   double input = input0x875f020();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875f2a0() {
   double input = -2.05368;
   input += synapse0x875f478();
   input += synapse0x875f4a0();
   input += synapse0x875f4c8();
   input += synapse0x875f4f0();
   input += synapse0x875f518();
   input += synapse0x875f540();
   input += synapse0x875f568();
   return input;
}

double NNmuon::neuron0x875f2a0() {
   double input = input0x875f2a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875f590() {
   double input = 0.285254;
   input += synapse0x875f768();
   input += synapse0x875f790();
   input += synapse0x875f7b8();
   input += synapse0x875f868();
   input += synapse0x875f890();
   input += synapse0x875f8b8();
   input += synapse0x875f8e0();
   return input;
}

double NNmuon::neuron0x875f590() {
   double input = input0x875f590();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875f908() {
   double input = 0.0532806;
   input += synapse0x875fa98();
   input += synapse0x875fac0();
   input += synapse0x875fae8();
   input += synapse0x875fb10();
   input += synapse0x875fb38();
   input += synapse0x875fb60();
   input += synapse0x875fb88();
   return input;
}

double NNmuon::neuron0x875f908() {
   double input = input0x875f908();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875fbb0() {
   double input = -0.478816;
   input += synapse0x875fd88();
   input += synapse0x875fdb0();
   input += synapse0x875fdd8();
   input += synapse0x875fe00();
   input += synapse0x875fe28();
   input += synapse0x8518888();
   input += synapse0x86ca258();
   return input;
}

double NNmuon::neuron0x875fbb0() {
   double input = input0x875fbb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875ff58() {
   double input = 2.04269;
   input += synapse0x8760130();
   input += synapse0x8760158();
   input += synapse0x8760180();
   input += synapse0x87601a8();
   input += synapse0x87601d0();
   input += synapse0x87601f8();
   input += synapse0x8760220();
   return input;
}

double NNmuon::neuron0x875ff58() {
   double input = input0x875ff58();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8760248() {
   double input = 0.88867;
   input += synapse0x8760440();
   input += synapse0x8760468();
   input += synapse0x8760490();
   input += synapse0x87604b8();
   input += synapse0x87604e0();
   input += synapse0x8760508();
   input += synapse0x8760530();
   return input;
}

double NNmuon::neuron0x8760248() {
   double input = input0x8760248();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8760558() {
   double input = -0.253223;
   input += synapse0x8760750();
   input += synapse0x8760778();
   input += synapse0x87607a0();
   input += synapse0x87607c8();
   input += synapse0x87607f0();
   input += synapse0x8760818();
   input += synapse0x8760840();
   return input;
}

double NNmuon::neuron0x8760558() {
   double input = input0x8760558();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8760868() {
   double input = 0.272386;
   input += synapse0x8760a60();
   input += synapse0x8760a88();
   input += synapse0x8760ab0();
   input += synapse0x8760ad8();
   input += synapse0x8760b00();
   input += synapse0x8760b28();
   input += synapse0x8760b50();
   return input;
}

double NNmuon::neuron0x8760868() {
   double input = input0x8760868();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8760b78() {
   double input = -0.793956;
   input += synapse0x8760d70();
   input += synapse0x8760d98();
   input += synapse0x86c9ba8();
   input += synapse0x86ca078();
   input += synapse0x84fd3a0();
   input += synapse0x84fd6e8();
   input += synapse0x84fd670();
   return input;
}

double NNmuon::neuron0x8760b78() {
   double input = input0x8760b78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x875fe50() {
   double input = 0.922818;
   input += synapse0x875ef90();
   input += synapse0x875efb8();
   input += synapse0x875efe0();
   input += synapse0x8760fc8();
   input += synapse0x8760ff0();
   input += synapse0x8761018();
   input += synapse0x8761040();
   input += synapse0x8761068();
   input += synapse0x8761090();
   input += synapse0x87610b8();
   return input;
}

double NNmuon::neuron0x875fe50() {
   double input = input0x875fe50();
   return (input * 0.463794)+0.686802;
}

double NNmuon::synapse0x84fd378() {
   return (neuron0x86ca0a8()*-0.572056);
}

double NNmuon::synapse0x875f1b0() {
   return (neuron0x85180f0()*0.0857081);
}

double NNmuon::synapse0x875f1d8() {
   return (neuron0x85182e8()*-0.98801);
}

double NNmuon::synapse0x875f200() {
   return (neuron0x85184e0()*-1.49176);
}

double NNmuon::synapse0x875f228() {
   return (neuron0x85186d8()*0.045823);
}

double NNmuon::synapse0x875f250() {
   return (neuron0x875eb30()*-1.04385);
}

double NNmuon::synapse0x875f278() {
   return (neuron0x875ed28()*1.43381);
}

double NNmuon::synapse0x875f478() {
   return (neuron0x86ca0a8()*1.88831);
}

double NNmuon::synapse0x875f4a0() {
   return (neuron0x85180f0()*-1.53876);
}

double NNmuon::synapse0x875f4c8() {
   return (neuron0x85182e8()*-0.924294);
}

double NNmuon::synapse0x875f4f0() {
   return (neuron0x85184e0()*-1.28744);
}

double NNmuon::synapse0x875f518() {
   return (neuron0x85186d8()*0.340732);
}

double NNmuon::synapse0x875f540() {
   return (neuron0x875eb30()*1.66524);
}

double NNmuon::synapse0x875f568() {
   return (neuron0x875ed28()*0.956053);
}

double NNmuon::synapse0x875f768() {
   return (neuron0x86ca0a8()*0.856015);
}

double NNmuon::synapse0x875f790() {
   return (neuron0x85180f0()*0.69814);
}

double NNmuon::synapse0x875f7b8() {
   return (neuron0x85182e8()*0.852744);
}

double NNmuon::synapse0x875f868() {
   return (neuron0x85184e0()*-0.00855212);
}

double NNmuon::synapse0x875f890() {
   return (neuron0x85186d8()*0.397337);
}

double NNmuon::synapse0x875f8b8() {
   return (neuron0x875eb30()*-0.385399);
}

double NNmuon::synapse0x875f8e0() {
   return (neuron0x875ed28()*0.226417);
}

double NNmuon::synapse0x875fa98() {
   return (neuron0x86ca0a8()*1.24751);
}

double NNmuon::synapse0x875fac0() {
   return (neuron0x85180f0()*0.0455854);
}

double NNmuon::synapse0x875fae8() {
   return (neuron0x85182e8()*-3.08311);
}

double NNmuon::synapse0x875fb10() {
   return (neuron0x85184e0()*-0.379395);
}

double NNmuon::synapse0x875fb38() {
   return (neuron0x85186d8()*0.382259);
}

double NNmuon::synapse0x875fb60() {
   return (neuron0x875eb30()*-0.64731);
}

double NNmuon::synapse0x875fb88() {
   return (neuron0x875ed28()*0.516089);
}

double NNmuon::synapse0x875fd88() {
   return (neuron0x86ca0a8()*-2.62196);
}

double NNmuon::synapse0x875fdb0() {
   return (neuron0x85180f0()*0.172701);
}

double NNmuon::synapse0x875fdd8() {
   return (neuron0x85182e8()*-0.322219);
}

double NNmuon::synapse0x875fe00() {
   return (neuron0x85184e0()*-0.0424886);
}

double NNmuon::synapse0x875fe28() {
   return (neuron0x85186d8()*0.0799829);
}

double NNmuon::synapse0x8518888() {
   return (neuron0x875eb30()*-0.788965);
}

double NNmuon::synapse0x86ca258() {
   return (neuron0x875ed28()*-0.079329);
}

double NNmuon::synapse0x8760130() {
   return (neuron0x86ca0a8()*-2.63838);
}

double NNmuon::synapse0x8760158() {
   return (neuron0x85180f0()*0.755977);
}

double NNmuon::synapse0x8760180() {
   return (neuron0x85182e8()*0.739751);
}

double NNmuon::synapse0x87601a8() {
   return (neuron0x85184e0()*0.42931);
}

double NNmuon::synapse0x87601d0() {
   return (neuron0x85186d8()*-0.112317);
}

double NNmuon::synapse0x87601f8() {
   return (neuron0x875eb30()*-0.891196);
}

double NNmuon::synapse0x8760220() {
   return (neuron0x875ed28()*-0.460364);
}

double NNmuon::synapse0x8760440() {
   return (neuron0x86ca0a8()*1.47698);
}

double NNmuon::synapse0x8760468() {
   return (neuron0x85180f0()*-1.06754);
}

double NNmuon::synapse0x8760490() {
   return (neuron0x85182e8()*-2.89492);
}

double NNmuon::synapse0x87604b8() {
   return (neuron0x85184e0()*-2.0314);
}

double NNmuon::synapse0x87604e0() {
   return (neuron0x85186d8()*-0.431806);
}

double NNmuon::synapse0x8760508() {
   return (neuron0x875eb30()*-1.06861);
}

double NNmuon::synapse0x8760530() {
   return (neuron0x875ed28()*-0.108985);
}

double NNmuon::synapse0x8760750() {
   return (neuron0x86ca0a8()*0.652193);
}

double NNmuon::synapse0x8760778() {
   return (neuron0x85180f0()*-0.711499);
}

double NNmuon::synapse0x87607a0() {
   return (neuron0x85182e8()*-1.23813);
}

double NNmuon::synapse0x87607c8() {
   return (neuron0x85184e0()*-1.03401);
}

double NNmuon::synapse0x87607f0() {
   return (neuron0x85186d8()*-0.334165);
}

double NNmuon::synapse0x8760818() {
   return (neuron0x875eb30()*0.170882);
}

double NNmuon::synapse0x8760840() {
   return (neuron0x875ed28()*0.514604);
}

double NNmuon::synapse0x8760a60() {
   return (neuron0x86ca0a8()*-1.51579);
}

double NNmuon::synapse0x8760a88() {
   return (neuron0x85180f0()*2.75709);
}

double NNmuon::synapse0x8760ab0() {
   return (neuron0x85182e8()*-0.203863);
}

double NNmuon::synapse0x8760ad8() {
   return (neuron0x85184e0()*3.42314);
}

double NNmuon::synapse0x8760b00() {
   return (neuron0x85186d8()*0.648669);
}

double NNmuon::synapse0x8760b28() {
   return (neuron0x875eb30()*-1.36844);
}

double NNmuon::synapse0x8760b50() {
   return (neuron0x875ed28()*-1.53417);
}

double NNmuon::synapse0x8760d70() {
   return (neuron0x86ca0a8()*-1.74793);
}

double NNmuon::synapse0x8760d98() {
   return (neuron0x85180f0()*0.714623);
}

double NNmuon::synapse0x86c9ba8() {
   return (neuron0x85182e8()*0.403543);
}

double NNmuon::synapse0x86ca078() {
   return (neuron0x85184e0()*0.990373);
}

double NNmuon::synapse0x84fd3a0() {
   return (neuron0x85186d8()*0.280874);
}

double NNmuon::synapse0x84fd6e8() {
   return (neuron0x875eb30()*1.49015);
}

double NNmuon::synapse0x84fd670() {
   return (neuron0x875ed28()*0.711878);
}

double NNmuon::synapse0x875ef90() {
   return (neuron0x875f020()*2.14851);
}

double NNmuon::synapse0x875efb8() {
   return (neuron0x875f2a0()*2.1571);
}

double NNmuon::synapse0x875efe0() {
   return (neuron0x875f590()*-0.201431);
}

double NNmuon::synapse0x8760fc8() {
   return (neuron0x875f908()*-2.88758);
}

double NNmuon::synapse0x8760ff0() {
   return (neuron0x875fbb0()*1.09146);
}

double NNmuon::synapse0x8761018() {
   return (neuron0x875ff58()*-1.7827);
}

double NNmuon::synapse0x8761040() {
   return (neuron0x8760248()*-1.28615);
}

double NNmuon::synapse0x8761068() {
   return (neuron0x8760558()*0.288667);
}

double NNmuon::synapse0x8761090() {
   return (neuron0x8760868()*1.79213);
}

double NNmuon::synapse0x87610b8() {
   return (neuron0x8760b78()*-2.11902);
}

