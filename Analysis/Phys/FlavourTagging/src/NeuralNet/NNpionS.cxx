#include "NNpionS.h"
#include "math.h"

double NNpionS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron0xa0fc2c8()*0.499091)+0.530132);
     default:
         return 0.;
   }
}

double NNpionS::neuron0xa1015c0() {
   return input0;
}

double NNpionS::neuron0xa1016d0() {
   return input1;
}

double NNpionS::neuron0xa101828() {
   return input2;
}

double NNpionS::neuron0xa0fbd20() {
   return input3;
}

double NNpionS::neuron0xa0fbe78() {
   return input4;
}

double NNpionS::neuron0xa0fbfe8() {
   return input5;
}

double NNpionS::neuron0xa0fc158() {
   return input6;
}

double NNpionS::neuron0xa313c78() {
   double input = 1.0791;
   input += synapse0xa313dd0();
   input += synapse0xa313df8();
   input += synapse0xa313e20();
   input += synapse0xa313e48();
   input += synapse0xa313e70();
   input += synapse0xa313e98();
   input += synapse0xa313ec0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa313ee8() {
   double input = 0.866061;
   input += synapse0xa314040();
   input += synapse0xa314068();
   input += synapse0xa314090();
   input += synapse0xa3140b8();
   input += synapse0xa3140e0();
   input += synapse0xa314108();
   input += synapse0xa314130();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa314158() {
   double input = 0.350689;
   input += synapse0xa3142d0();
   input += synapse0xa3142f8();
   input += synapse0xa314320();
   input += synapse0xa3143d0();
   input += synapse0xa3143f8();
   input += synapse0xa314420();
   input += synapse0xa314448();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa314470() {
   double input = 0.429497;
   input += synapse0xa3145a0();
   input += synapse0xa3145c8();
   input += synapse0xa3145f0();
   input += synapse0xa314618();
   input += synapse0xa314640();
   input += synapse0xa314668();
   input += synapse0xa314690();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa3146b8() {
   double input = -0.664796;
   input += synapse0xa314830();
   input += synapse0xa314858();
   input += synapse0xa314880();
   input += synapse0xa3148a8();
   input += synapse0xa3148d0();
   input += synapse0xa04cb38();
   input += synapse0xa0fc3e0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa314a00() {
   double input = 0.567504;
   input += synapse0xa314b78();
   input += synapse0xa314ba0();
   input += synapse0xa314bc8();
   input += synapse0xa314bf0();
   input += synapse0xa314c18();
   input += synapse0xa314c40();
   input += synapse0xa314c68();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa314c90() {
   double input = 0.436573;
   input += synapse0xa314e08();
   input += synapse0xa314e30();
   input += synapse0xa314e58();
   input += synapse0xa314e80();
   input += synapse0xa314ea8();
   input += synapse0xa314ed0();
   input += synapse0xa314ef8();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNpionS::neuron0xa0fc2c8() {
   double input = -1.16798;
   input += synapse0xa314f68();
   input += synapse0xa314f90();
   input += synapse0xa314fb8();
   input += synapse0xa314fe0();
   input += synapse0xa315008();
   input += synapse0xa315030();
   input += synapse0xa315058();
   return input;
}

double NNpionS::synapse0xa313dd0() {
   return (neuron0xa1015c0()*-1.3756);
}

double NNpionS::synapse0xa313df8() {
   return (neuron0xa1016d0()*1.26093);
}

double NNpionS::synapse0xa313e20() {
   return (neuron0xa101828()*0.522379);
}

double NNpionS::synapse0xa313e48() {
   return (neuron0xa0fbd20()*0.402298);
}

double NNpionS::synapse0xa313e70() {
   return (neuron0xa0fbe78()*0.862616);
}

double NNpionS::synapse0xa313e98() {
   return (neuron0xa0fbfe8()*0.650111);
}

double NNpionS::synapse0xa313ec0() {
   return (neuron0xa0fc158()*-0.519173);
}

double NNpionS::synapse0xa314040() {
   return (neuron0xa1015c0()*1.80648);
}

double NNpionS::synapse0xa314068() {
   return (neuron0xa1016d0()*-0.590974);
}

double NNpionS::synapse0xa314090() {
   return (neuron0xa101828()*-0.0388118);
}

double NNpionS::synapse0xa3140b8() {
   return (neuron0xa0fbd20()*1.0974);
}

double NNpionS::synapse0xa3140e0() {
   return (neuron0xa0fbe78()*-0.956722);
}

double NNpionS::synapse0xa314108() {
   return (neuron0xa0fbfe8()*0.166863);
}

double NNpionS::synapse0xa314130() {
   return (neuron0xa0fc158()*-0.402601);
}

double NNpionS::synapse0xa3142d0() {
   return (neuron0xa1015c0()*0.729053);
}

double NNpionS::synapse0xa3142f8() {
   return (neuron0xa1016d0()*-0.146929);
}

double NNpionS::synapse0xa314320() {
   return (neuron0xa101828()*-0.694958);
}

double NNpionS::synapse0xa3143d0() {
   return (neuron0xa0fbd20()*-0.329053);
}

double NNpionS::synapse0xa3143f8() {
   return (neuron0xa0fbe78()*-0.155275);
}

double NNpionS::synapse0xa314420() {
   return (neuron0xa0fbfe8()*0.291798);
}

double NNpionS::synapse0xa314448() {
   return (neuron0xa0fc158()*0.985709);
}

double NNpionS::synapse0xa3145a0() {
   return (neuron0xa1015c0()*-0.286371);
}

double NNpionS::synapse0xa3145c8() {
   return (neuron0xa1016d0()*-0.240743);
}

double NNpionS::synapse0xa3145f0() {
   return (neuron0xa101828()*-0.210005);
}

double NNpionS::synapse0xa314618() {
   return (neuron0xa0fbd20()*0.192634);
}

double NNpionS::synapse0xa314640() {
   return (neuron0xa0fbe78()*0.330817);
}

double NNpionS::synapse0xa314668() {
   return (neuron0xa0fbfe8()*-0.359596);
}

double NNpionS::synapse0xa314690() {
   return (neuron0xa0fc158()*0.156448);
}

double NNpionS::synapse0xa314830() {
   return (neuron0xa1015c0()*-1.84568);
}

double NNpionS::synapse0xa314858() {
   return (neuron0xa1016d0()*-1.18901);
}

double NNpionS::synapse0xa314880() {
   return (neuron0xa101828()*1.15651);
}

double NNpionS::synapse0xa3148a8() {
   return (neuron0xa0fbd20()*0.92091);
}

double NNpionS::synapse0xa3148d0() {
   return (neuron0xa0fbe78()*-1.4535);
}

double NNpionS::synapse0xa04cb38() {
   return (neuron0xa0fbfe8()*-1.79124);
}

double NNpionS::synapse0xa0fc3e0() {
   return (neuron0xa0fc158()*-3.32857);
}

double NNpionS::synapse0xa314b78() {
   return (neuron0xa1015c0()*0.294732);
}

double NNpionS::synapse0xa314ba0() {
   return (neuron0xa1016d0()*-0.265713);
}

double NNpionS::synapse0xa314bc8() {
   return (neuron0xa101828()*-0.511658);
}

double NNpionS::synapse0xa314bf0() {
   return (neuron0xa0fbd20()*-1.34845);
}

double NNpionS::synapse0xa314c18() {
   return (neuron0xa0fbe78()*-0.271255);
}

double NNpionS::synapse0xa314c40() {
   return (neuron0xa0fbfe8()*1.06886);
}

double NNpionS::synapse0xa314c68() {
   return (neuron0xa0fc158()*1.71356);
}

double NNpionS::synapse0xa314e08() {
   return (neuron0xa1015c0()*-0.49783);
}

double NNpionS::synapse0xa314e30() {
   return (neuron0xa1016d0()*-0.13361);
}

double NNpionS::synapse0xa314e58() {
   return (neuron0xa101828()*0.383398);
}

double NNpionS::synapse0xa314e80() {
   return (neuron0xa0fbd20()*-0.565808);
}

double NNpionS::synapse0xa314ea8() {
   return (neuron0xa0fbe78()*-0.390154);
}

double NNpionS::synapse0xa314ed0() {
   return (neuron0xa0fbfe8()*-0.0854804);
}

double NNpionS::synapse0xa314ef8() {
   return (neuron0xa0fc158()*0.248892);
}

double NNpionS::synapse0xa314f68() {
   return (neuron0xa313c78()*1.23968);
}

double NNpionS::synapse0xa314f90() {
   return (neuron0xa313ee8()*1.2222);
}

double NNpionS::synapse0xa314fb8() {
   return (neuron0xa314158()*0.278762);
}

double NNpionS::synapse0xa314fe0() {
   return (neuron0xa314470()*0.309809);
}

double NNpionS::synapse0xa315008() {
   return (neuron0xa3146b8()*0.860665);
}

double NNpionS::synapse0xa315030() {
   return (neuron0xa314a00()*-0.75915);
}

double NNpionS::synapse0xa315058() {
   return (neuron0xa314c90()*-1.02099);
}

