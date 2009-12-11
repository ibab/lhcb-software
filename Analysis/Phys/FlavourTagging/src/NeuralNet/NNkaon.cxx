#include "NNkaon.h"
#include <cmath>

double NNkaon::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8761ea8();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x86d4890() {
   return input0;
}

double NNkaon::neuron0x86d49d8() {
   return input1;
}

double NNkaon::neuron0x87605a0() {
   return input2;
}

double NNkaon::neuron0x8760798() {
   return input3;
}

double NNkaon::neuron0x8760990() {
   return input4;
}

double NNkaon::neuron0x8760b88() {
   return input5;
}

double NNkaon::neuron0x8760d80() {
   return input6;
}

double NNkaon::input0x8761078() {
   double input = 0.0487978;
   input += synapse0x84fd850();
   input += synapse0x8761208();
   input += synapse0x8761230();
   input += synapse0x8761258();
   input += synapse0x8761280();
   input += synapse0x87612a8();
   input += synapse0x87612d0();
   return input;
}

double NNkaon::neuron0x8761078() {
   double input = input0x8761078();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x87612f8() {
   double input = -0.100819;
   input += synapse0x87614d0();
   input += synapse0x87614f8();
   input += synapse0x8761520();
   input += synapse0x8761548();
   input += synapse0x8761570();
   input += synapse0x8761598();
   input += synapse0x87615c0();
   return input;
}

double NNkaon::neuron0x87612f8() {
   double input = input0x87612f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x87615e8() {
   double input = -0.351837;
   input += synapse0x87617c0();
   input += synapse0x87617e8();
   input += synapse0x8761810();
   input += synapse0x87618c0();
   input += synapse0x87618e8();
   input += synapse0x8761910();
   input += synapse0x8761938();
   return input;
}

double NNkaon::neuron0x87615e8() {
   double input = input0x87615e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8761960() {
   double input = 0.267598;
   input += synapse0x8761af0();
   input += synapse0x8761b18();
   input += synapse0x8761b40();
   input += synapse0x8761b68();
   input += synapse0x8761b90();
   input += synapse0x8761bb8();
   input += synapse0x8761be0();
   return input;
}

double NNkaon::neuron0x8761960() {
   double input = input0x8761960();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8761c08() {
   double input = 0.109265;
   input += synapse0x8761de0();
   input += synapse0x8761e08();
   input += synapse0x8761e30();
   input += synapse0x8761e58();
   input += synapse0x8761e80();
   input += synapse0x86d4bd0();
   input += synapse0x86d4bf8();
   return input;
}

double NNkaon::neuron0x8761c08() {
   double input = input0x8761c08();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8761fb0() {
   double input = -0.123366;
   input += synapse0x8762188();
   input += synapse0x87621b0();
   input += synapse0x87621d8();
   input += synapse0x8762200();
   input += synapse0x8762228();
   input += synapse0x8762250();
   input += synapse0x8762278();
   return input;
}

double NNkaon::neuron0x8761fb0() {
   double input = input0x8761fb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x87622a0() {
   double input = 0.592008;
   input += synapse0x8762498();
   input += synapse0x87624c0();
   input += synapse0x86d4c20();
   input += synapse0x8761838();
   input += synapse0x8761860();
   input += synapse0x8761888();
   input += synapse0x86de2a0();
   return input;
}

double NNkaon::neuron0x87622a0() {
   double input = input0x87622a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x86dc1a8() {
   double input = 0.704382;
   input += synapse0x86dc3a0();
   input += synapse0x86dc3c8();
   input += synapse0x86dc3f0();
   input += synapse0x86dc418();
   input += synapse0x86dc440();
   input += synapse0x86dc468();
   input += synapse0x86dc490();
   return input;
}

double NNkaon::neuron0x86dc1a8() {
   double input = input0x86dc1a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x86dc4b8() {
   double input = -0.494358;
   input += synapse0x86dc6b0();
   input += synapse0x86dc6d8();
   input += synapse0x86dc700();
   input += synapse0x86dc728();
   input += synapse0x86dc750();
   input += synapse0x86dc778();
   input += synapse0x86dc7a0();
   return input;
}

double NNkaon::neuron0x86dc4b8() {
   double input = input0x86dc4b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x86dc7c8() {
   double input = -0.0578372;
   input += synapse0x86dca48();
   input += synapse0x86dca70();
   input += synapse0x86de150();
   input += synapse0x86de270();
   input += synapse0x84fd878();
   input += synapse0x84fdbc0();
   input += synapse0x84fdb48();
   return input;
}

double NNkaon::neuron0x86dc7c8() {
   double input = input0x86dc7c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8761ea8() {
   double input = -1.02448;
   input += synapse0x8760fa0();
   input += synapse0x8760fc8();
   input += synapse0x8760ff0();
   input += synapse0x8761018();
   input += synapse0x8761040();
   input += synapse0x86de358();
   input += synapse0x86dcca0();
   input += synapse0x86dccc8();
   input += synapse0x86dccf0();
   input += synapse0x86dcd18();
   return input;
}

double NNkaon::neuron0x8761ea8() {
   double input = input0x8761ea8();
   return (input * 0.479016)+0.643331;
}

double NNkaon::synapse0x84fd850() {
   return (neuron0x86d4890()*-1.48924);
}

double NNkaon::synapse0x8761208() {
   return (neuron0x86d49d8()*-0.0727993);
}

double NNkaon::synapse0x8761230() {
   return (neuron0x87605a0()*1.31577);
}

double NNkaon::synapse0x8761258() {
   return (neuron0x8760798()*2.11846);
}

double NNkaon::synapse0x8761280() {
   return (neuron0x8760990()*-0.506963);
}

double NNkaon::synapse0x87612a8() {
   return (neuron0x8760b88()*-0.703293);
}

double NNkaon::synapse0x87612d0() {
   return (neuron0x8760d80()*-0.110577);
}

double NNkaon::synapse0x87614d0() {
   return (neuron0x86d4890()*-0.409846);
}

double NNkaon::synapse0x87614f8() {
   return (neuron0x86d49d8()*0.594059);
}

double NNkaon::synapse0x8761520() {
   return (neuron0x87605a0()*0.144789);
}

double NNkaon::synapse0x8761548() {
   return (neuron0x8760798()*0.553826);
}

double NNkaon::synapse0x8761570() {
   return (neuron0x8760990()*-0.066921);
}

double NNkaon::synapse0x8761598() {
   return (neuron0x8760b88()*0.0437467);
}

double NNkaon::synapse0x87615c0() {
   return (neuron0x8760d80()*0.557854);
}

double NNkaon::synapse0x87617c0() {
   return (neuron0x86d4890()*0.503804);
}

double NNkaon::synapse0x87617e8() {
   return (neuron0x86d49d8()*-0.824534);
}

double NNkaon::synapse0x8761810() {
   return (neuron0x87605a0()*-0.774942);
}

double NNkaon::synapse0x87618c0() {
   return (neuron0x8760798()*-0.612986);
}

double NNkaon::synapse0x87618e8() {
   return (neuron0x8760990()*0.376996);
}

double NNkaon::synapse0x8761910() {
   return (neuron0x8760b88()*0.387114);
}

double NNkaon::synapse0x8761938() {
   return (neuron0x8760d80()*0.571424);
}

double NNkaon::synapse0x8761af0() {
   return (neuron0x86d4890()*-0.941266);
}

double NNkaon::synapse0x8761b18() {
   return (neuron0x86d49d8()*0.369652);
}

double NNkaon::synapse0x8761b40() {
   return (neuron0x87605a0()*0.43645);
}

double NNkaon::synapse0x8761b68() {
   return (neuron0x8760798()*0.40963);
}

double NNkaon::synapse0x8761b90() {
   return (neuron0x8760990()*-0.14078);
}

double NNkaon::synapse0x8761bb8() {
   return (neuron0x8760b88()*-0.522592);
}

double NNkaon::synapse0x8761be0() {
   return (neuron0x8760d80()*-0.367974);
}

double NNkaon::synapse0x8761de0() {
   return (neuron0x86d4890()*-0.250075);
}

double NNkaon::synapse0x8761e08() {
   return (neuron0x86d49d8()*-0.0611901);
}

double NNkaon::synapse0x8761e30() {
   return (neuron0x87605a0()*-0.303602);
}

double NNkaon::synapse0x8761e58() {
   return (neuron0x8760798()*-0.0248258);
}

double NNkaon::synapse0x8761e80() {
   return (neuron0x8760990()*-0.341681);
}

double NNkaon::synapse0x86d4bd0() {
   return (neuron0x8760b88()*-0.115994);
}

double NNkaon::synapse0x86d4bf8() {
   return (neuron0x8760d80()*-0.66535);
}

double NNkaon::synapse0x8762188() {
   return (neuron0x86d4890()*-0.670141);
}

double NNkaon::synapse0x87621b0() {
   return (neuron0x86d49d8()*0.526366);
}

double NNkaon::synapse0x87621d8() {
   return (neuron0x87605a0()*0.53707);
}

double NNkaon::synapse0x8762200() {
   return (neuron0x8760798()*0.564049);
}

double NNkaon::synapse0x8762228() {
   return (neuron0x8760990()*-0.424081);
}

double NNkaon::synapse0x8762250() {
   return (neuron0x8760b88()*-0.279471);
}

double NNkaon::synapse0x8762278() {
   return (neuron0x8760d80()*0.397529);
}

double NNkaon::synapse0x8762498() {
   return (neuron0x86d4890()*1.375);
}

double NNkaon::synapse0x87624c0() {
   return (neuron0x86d49d8()*0.214918);
}

double NNkaon::synapse0x86d4c20() {
   return (neuron0x87605a0()*-0.752692);
}

double NNkaon::synapse0x8761838() {
   return (neuron0x8760798()*-0.178891);
}

double NNkaon::synapse0x8761860() {
   return (neuron0x8760990()*1.1192);
}

double NNkaon::synapse0x8761888() {
   return (neuron0x8760b88()*3.26143);
}

double NNkaon::synapse0x86de2a0() {
   return (neuron0x8760d80()*0.22324);
}

double NNkaon::synapse0x86dc3a0() {
   return (neuron0x86d4890()*-0.305154);
}

double NNkaon::synapse0x86dc3c8() {
   return (neuron0x86d49d8()*-0.328819);
}

double NNkaon::synapse0x86dc3f0() {
   return (neuron0x87605a0()*2.07316);
}

double NNkaon::synapse0x86dc418() {
   return (neuron0x8760798()*0.511347);
}

double NNkaon::synapse0x86dc440() {
   return (neuron0x8760990()*-0.623641);
}

double NNkaon::synapse0x86dc468() {
   return (neuron0x8760b88()*0.193206);
}

double NNkaon::synapse0x86dc490() {
   return (neuron0x8760d80()*0.220445);
}

double NNkaon::synapse0x86dc6b0() {
   return (neuron0x86d4890()*-0.693242);
}

double NNkaon::synapse0x86dc6d8() {
   return (neuron0x86d49d8()*3.12937);
}

double NNkaon::synapse0x86dc700() {
   return (neuron0x87605a0()*2.6492);
}

double NNkaon::synapse0x86dc728() {
   return (neuron0x8760798()*0.419127);
}

double NNkaon::synapse0x86dc750() {
   return (neuron0x8760990()*0.400547);
}

double NNkaon::synapse0x86dc778() {
   return (neuron0x8760b88()*-0.21391);
}

double NNkaon::synapse0x86dc7a0() {
   return (neuron0x8760d80()*-0.0811211);
}

double NNkaon::synapse0x86dca48() {
   return (neuron0x86d4890()*0.659827);
}

double NNkaon::synapse0x86dca70() {
   return (neuron0x86d49d8()*-0.983033);
}

double NNkaon::synapse0x86de150() {
   return (neuron0x87605a0()*-1.63069);
}

double NNkaon::synapse0x86de270() {
   return (neuron0x8760798()*-0.684755);
}

double NNkaon::synapse0x84fd878() {
   return (neuron0x8760990()*0.256711);
}

double NNkaon::synapse0x84fdbc0() {
   return (neuron0x8760b88()*0.935616);
}

double NNkaon::synapse0x84fdb48() {
   return (neuron0x8760d80()*0.253297);
}

double NNkaon::synapse0x8760fa0() {
   return (neuron0x8761078()*0.951895);
}

double NNkaon::synapse0x8760fc8() {
   return (neuron0x87612f8()*-0.749608);
}

double NNkaon::synapse0x8760ff0() {
   return (neuron0x87615e8()*1.05751);
}

double NNkaon::synapse0x8761018() {
   return (neuron0x8761960()*-0.707788);
}

double NNkaon::synapse0x8761040() {
   return (neuron0x8761c08()*0.0411069);
}

double NNkaon::synapse0x86de358() {
   return (neuron0x8761fb0()*-0.965745);
}

double NNkaon::synapse0x86dcca0() {
   return (neuron0x87622a0()*-1.19754);
}

double NNkaon::synapse0x86dccc8() {
   return (neuron0x86dc1a8()*0.862303);
}

double NNkaon::synapse0x86dccf0() {
   return (neuron0x86dc4b8()*1.7606);
}

double NNkaon::synapse0x86dcd18() {
   return (neuron0x86dc7c8()*0.710402);
}

