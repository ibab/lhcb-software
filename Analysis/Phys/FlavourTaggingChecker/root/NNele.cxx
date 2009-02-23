#include "NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x875b148();
     default:
         return 0.;
   }
}

double NNele::neuron0x8771f40() {
   return input0;
}

double NNele::neuron0x87720d0() {
   return input1;
}

double NNele::neuron0x8755e58() {
   return input2;
}

double NNele::neuron0x8756030() {
   return input3;
}

double NNele::input0x8756348() {
   double input = 0.29549;
   input += synapse0x8771ee8();
   input += synapse0x87564d8();
   input += synapse0x8756500();
   input += synapse0x8756528();
   return input;
}

double NNele::neuron0x8756348() {
   double input = input0x8756348();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x87594c8() {
   double input = -0.0207896;
   input += synapse0x87596a0();
   input += synapse0x87596c8();
   input += synapse0x87596f0();
   input += synapse0x8759718();
   return input;
}

double NNele::neuron0x87594c8() {
   double input = input0x87594c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8759740() {
   double input = 1.72209;
   input += synapse0x8759918();
   input += synapse0x8759940();
   input += synapse0x8759968();
   input += synapse0x8759990();
   return input;
}

double NNele::neuron0x8759740() {
   double input = input0x8759740();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x87599b8() {
   double input = -0.462216;
   input += synapse0x8759b90();
   input += synapse0x8759bb8();
   input += synapse0x8759be0();
   input += synapse0x8759c08();
   return input;
}

double NNele::neuron0x87599b8() {
   double input = input0x87599b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8759c30() {
   double input = -0.447031;
   input += synapse0x8759e28();
   input += synapse0x8759e50();
   input += synapse0x8759e78();
   input += synapse0x8759ea0();
   return input;
}

double NNele::neuron0x8759c30() {
   double input = input0x8759c30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8759ec8() {
   double input = 0.449039;
   input += synapse0x875a078();
   input += synapse0x875a0a0();
   input += synapse0x875a0c8();
   input += synapse0x875a0f0();
   return input;
}

double NNele::neuron0x8759ec8() {
   double input = input0x8759ec8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875a118() {
   double input = -0.0997036;
   input += synapse0x875a310();
   input += synapse0x875a338();
   input += synapse0x875a360();
   input += synapse0x875a388();
   return input;
}

double NNele::neuron0x875a118() {
   double input = input0x875a118();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875a3b0() {
   double input = 0.274489;
   input += synapse0x875a5a8();
   input += synapse0x875a5d0();
   input += synapse0x875a5f8();
   input += synapse0x875a620();
   return input;
}

double NNele::neuron0x875a3b0() {
   double input = input0x875a3b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875a648() {
   double input = 0.286476;
   input += synapse0x875a840();
   input += synapse0x8756550();
   input += synapse0x8756578();
   input += synapse0x87565a0();
   return input;
}

double NNele::neuron0x875a648() {
   double input = input0x875a648();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875a970() {
   double input = 0.318669;
   input += synapse0x875ab68();
   input += synapse0x875ab90();
   input += synapse0x875abb8();
   input += synapse0x875abe0();
   return input;
}

double NNele::neuron0x875a970() {
   double input = input0x875a970();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875ac08() {
   double input = 0.052352;
   input += synapse0x875ae08();
   input += synapse0x875ae30();
   input += synapse0x875ae58();
   input += synapse0x875ae80();
   return input;
}

double NNele::neuron0x875ac08() {
   double input = input0x875ac08();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875aea8() {
   double input = 0.736864;
   input += synapse0x875b0a8();
   input += synapse0x875b0d0();
   input += synapse0x875b0f8();
   input += synapse0x875b120();
   return input;
}

double NNele::neuron0x875aea8() {
   double input = input0x875aea8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875b148() {
   double input = -0.0707479;
   input += synapse0x875b340();
   input += synapse0x875b368();
   input += synapse0x875b390();
   input += synapse0x875b3b8();
   input += synapse0x875b3e0();
   input += synapse0x875b408();
   input += synapse0x875b430();
   input += synapse0x875b458();
   input += synapse0x875b480();
   input += synapse0x875b4a8();
   input += synapse0x875b4d0();
   input += synapse0x875b4f8();
   return input;
}

double NNele::neuron0x875b148() {
   double input = input0x875b148();
   return (input * 0.462572)+0.689809;
}

double NNele::synapse0x8771ee8() {
   return (neuron0x8771f40()*1.0721);
}

double NNele::synapse0x87564d8() {
   return (neuron0x87720d0()*0.193075);
}

double NNele::synapse0x8756500() {
   return (neuron0x8755e58()*0.636611);
}

double NNele::synapse0x8756528() {
   return (neuron0x8756030()*5.77138);
}

double NNele::synapse0x87596a0() {
   return (neuron0x8771f40()*-6.2984);
}

double NNele::synapse0x87596c8() {
   return (neuron0x87720d0()*0.496876);
}

double NNele::synapse0x87596f0() {
   return (neuron0x8755e58()*-4.48476);
}

double NNele::synapse0x8759718() {
   return (neuron0x8756030()*-0.299137);
}

double NNele::synapse0x8759918() {
   return (neuron0x8771f40()*-1.64701);
}

double NNele::synapse0x8759940() {
   return (neuron0x87720d0()*4.96987);
}

double NNele::synapse0x8759968() {
   return (neuron0x8755e58()*3.14714);
}

double NNele::synapse0x8759990() {
   return (neuron0x8756030()*0.203614);
}

double NNele::synapse0x8759b90() {
   return (neuron0x8771f40()*0.998401);
}

double NNele::synapse0x8759bb8() {
   return (neuron0x87720d0()*0.471158);
}

double NNele::synapse0x8759be0() {
   return (neuron0x8755e58()*-0.120256);
}

double NNele::synapse0x8759c08() {
   return (neuron0x8756030()*0.162431);
}

double NNele::synapse0x8759e28() {
   return (neuron0x8771f40()*0.961133);
}

double NNele::synapse0x8759e50() {
   return (neuron0x87720d0()*1.54344);
}

double NNele::synapse0x8759e78() {
   return (neuron0x8755e58()*1.52374);
}

double NNele::synapse0x8759ea0() {
   return (neuron0x8756030()*3.33977);
}

double NNele::synapse0x875a078() {
   return (neuron0x8771f40()*2.00082);
}

double NNele::synapse0x875a0a0() {
   return (neuron0x87720d0()*0.392362);
}

double NNele::synapse0x875a0c8() {
   return (neuron0x8755e58()*0.138007);
}

double NNele::synapse0x875a0f0() {
   return (neuron0x8756030()*-1.94131);
}

double NNele::synapse0x875a310() {
   return (neuron0x8771f40()*0.713758);
}

double NNele::synapse0x875a338() {
   return (neuron0x87720d0()*0.581123);
}

double NNele::synapse0x875a360() {
   return (neuron0x8755e58()*0.219154);
}

double NNele::synapse0x875a388() {
   return (neuron0x8756030()*0.107658);
}

double NNele::synapse0x875a5a8() {
   return (neuron0x8771f40()*0.629105);
}

double NNele::synapse0x875a5d0() {
   return (neuron0x87720d0()*0.35599);
}

double NNele::synapse0x875a5f8() {
   return (neuron0x8755e58()*-0.0795395);
}

double NNele::synapse0x875a620() {
   return (neuron0x8756030()*-0.236051);
}

double NNele::synapse0x875a840() {
   return (neuron0x8771f40()*-0.811996);
}

double NNele::synapse0x8756550() {
   return (neuron0x87720d0()*-0.596231);
}

double NNele::synapse0x8756578() {
   return (neuron0x8755e58()*-7.61238);
}

double NNele::synapse0x87565a0() {
   return (neuron0x8756030()*0.0341318);
}

double NNele::synapse0x875ab68() {
   return (neuron0x8771f40()*0.0706547);
}

double NNele::synapse0x875ab90() {
   return (neuron0x87720d0()*0.166089);
}

double NNele::synapse0x875abb8() {
   return (neuron0x8755e58()*0.170666);
}

double NNele::synapse0x875abe0() {
   return (neuron0x8756030()*-0.66735);
}

double NNele::synapse0x875ae08() {
   return (neuron0x8771f40()*-1.15246);
}

double NNele::synapse0x875ae30() {
   return (neuron0x87720d0()*-0.758704);
}

double NNele::synapse0x875ae58() {
   return (neuron0x8755e58()*2.41266);
}

double NNele::synapse0x875ae80() {
   return (neuron0x8756030()*7.65676);
}

double NNele::synapse0x875b0a8() {
   return (neuron0x8771f40()*1.88887);
}

double NNele::synapse0x875b0d0() {
   return (neuron0x87720d0()*2.20151);
}

double NNele::synapse0x875b0f8() {
   return (neuron0x8755e58()*1.3646);
}

double NNele::synapse0x875b120() {
   return (neuron0x8756030()*1.60512);
}

double NNele::synapse0x875b340() {
   return (neuron0x8756348()*-1.46855);
}

double NNele::synapse0x875b368() {
   return (neuron0x87594c8()*3.72952);
}

double NNele::synapse0x875b390() {
   return (neuron0x8759740()*-2.81546);
}

double NNele::synapse0x875b3b8() {
   return (neuron0x87599b8()*-0.911149);
}

double NNele::synapse0x875b3e0() {
   return (neuron0x8759c30()*-0.0618437);
}

double NNele::synapse0x875b408() {
   return (neuron0x8759ec8()*0.900647);
}

double NNele::synapse0x875b430() {
   return (neuron0x875a118()*-0.487455);
}

double NNele::synapse0x875b458() {
   return (neuron0x875a3b0()*-0.435085);
}

double NNele::synapse0x875b480() {
   return (neuron0x875a648()*-3.75751);
}

double NNele::synapse0x875b4a8() {
   return (neuron0x875a970()*-0.148525);
}

double NNele::synapse0x875b4d0() {
   return (neuron0x875ac08()*1.36254);
}

double NNele::synapse0x875b4f8() {
   return (neuron0x875aea8()*3.8457);
}

