#include "NNkaon.h"
#include "math.h"

double NNkaon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron0xa7d0e98()*0.471045)+0.667679);
     default:
         return 0.;
   }
}

double NNkaon::neuron0xa7cd198() {
   return input0;
}

double NNkaon::neuron0xa7d31d8() {
   return input1;
}

double NNkaon::neuron0xa7d3330() {
   return input2;
}

double NNkaon::neuron0xa7d0d20() {
   return input3;
}

double NNkaon::neuron0xa7f0de0() {
   double input = 0.246294;
   input += synapse0xa7f0f38();
   input += synapse0xa7f0f60();
   input += synapse0xa7f0f88();
   input += synapse0xa7f0fb0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7f0fd8() {
   double input = -0.162269;
   input += synapse0xa7cd2f0();
   input += synapse0xa7d3488();
   input += synapse0xa7d34b0();
   input += synapse0xa7d0f90();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7d8418() {
   double input = -1.9945;
   input += synapse0xa7d8570();
   input += synapse0xa7d8598();
   input += synapse0xa7d85c0();
   input += synapse0xa7d85e8();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7d8610() {
   double input = -0.148731;
   input += synapse0xa7d1000();
   input += synapse0xa7d8740();
   input += synapse0xa7d0470();
   input += synapse0xa7d0498();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7d04c0() {
   double input = -0.48784;
   input += synapse0xa7d0618();
   input += synapse0xa7d06c8();
   input += synapse0xa7d06f0();
   input += synapse0xa7d0718();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7d0740() {
   double input = 0.274936;
   input += synapse0xa7fc4f8();
   input += synapse0xa7fc520();
   input += synapse0xa7fc548();
   input += synapse0xa7fc570();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNkaon::neuron0xa7d0e98() {
   double input = 0.690609;
   input += synapse0xa7fc628();
   input += synapse0xa7fc650();
   input += synapse0xa7fc678();
   input += synapse0xa7fc6a0();
   input += synapse0xa7fc6c8();
   input += synapse0xa7fc6f0();
   return input;
}

double NNkaon::synapse0xa7f0f38() {
   return (neuron0xa7cd198()*-0.150847);
}

double NNkaon::synapse0xa7f0f60() {
   return (neuron0xa7d31d8()*0.16953);
}

double NNkaon::synapse0xa7f0f88() {
   return (neuron0xa7d3330()*0.460435);
}

double NNkaon::synapse0xa7f0fb0() {
   return (neuron0xa7d0d20()*1.16512);
}

double NNkaon::synapse0xa7cd2f0() {
   return (neuron0xa7cd198()*1.41557);
}

double NNkaon::synapse0xa7d3488() {
   return (neuron0xa7d31d8()*-4.17863);
}

double NNkaon::synapse0xa7d34b0() {
   return (neuron0xa7d3330()*0.293012);
}

double NNkaon::synapse0xa7d0f90() {
   return (neuron0xa7d0d20()*-4.14982);
}

double NNkaon::synapse0xa7d8570() {
   return (neuron0xa7cd198()*-0.610855);
}

double NNkaon::synapse0xa7d8598() {
   return (neuron0xa7d31d8()*2.83623);
}

double NNkaon::synapse0xa7d85c0() {
   return (neuron0xa7d3330()*-5.00842);
}

double NNkaon::synapse0xa7d85e8() {
   return (neuron0xa7d0d20()*0.0196873);
}

double NNkaon::synapse0xa7d1000() {
   return (neuron0xa7cd198()*0.538076);
}

double NNkaon::synapse0xa7d8740() {
   return (neuron0xa7d31d8()*-3.15324);
}

double NNkaon::synapse0xa7d0470() {
   return (neuron0xa7d3330()*-0.0464075);
}

double NNkaon::synapse0xa7d0498() {
   return (neuron0xa7d0d20()*-1.52079);
}

double NNkaon::synapse0xa7d0618() {
   return (neuron0xa7cd198()*-0.938379);
}

double NNkaon::synapse0xa7d06c8() {
   return (neuron0xa7d31d8()*-0.66233);
}

double NNkaon::synapse0xa7d06f0() {
   return (neuron0xa7d3330()*0.251316);
}

double NNkaon::synapse0xa7d0718() {
   return (neuron0xa7d0d20()*-0.940398);
}

double NNkaon::synapse0xa7fc4f8() {
   return (neuron0xa7cd198()*0.265999);
}

double NNkaon::synapse0xa7fc520() {
   return (neuron0xa7d31d8()*-0.521551);
}

double NNkaon::synapse0xa7fc548() {
   return (neuron0xa7d3330()*0.088733);
}

double NNkaon::synapse0xa7fc570() {
   return (neuron0xa7d0d20()*-0.681534);
}

double NNkaon::synapse0xa7fc628() {
   return (neuron0xa7f0de0()*-0.902672);
}

double NNkaon::synapse0xa7fc650() {
   return (neuron0xa7f0fd8()*-0.197192);
}

double NNkaon::synapse0xa7fc678() {
   return (neuron0xa7d8418()*-2.06832);
}

double NNkaon::synapse0xa7fc6a0() {
   return (neuron0xa7d8610()*-1.38169);
}

double NNkaon::synapse0xa7fc6c8() {
   return (neuron0xa7d04c0()*1.24276);
}

double NNkaon::synapse0xa7fc6f0() {
   return (neuron0xa7d0740()*0.301041);
}

