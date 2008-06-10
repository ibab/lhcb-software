#include "NNmuon.h"
#include <cmath>

double NNmuon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8606710();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x8605f48() {
   return input0;
}

double NNmuon::neuron0x8606120() {
   return input1;
}

double NNmuon::neuron0x86062f8() {
   return input2;
}

double NNmuon::neuron0x86064f0() {
   return input3;
}

double NNmuon::input0x862a500() {
   double input = 0.485525;
   input += synapse0x85df950();
   input += synapse0x862a690();
   input += synapse0x862a6b8();
   input += synapse0x862a6e0();
   return input;
}

double NNmuon::neuron0x862a500() {
   double input = input0x862a500();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x862a708() {
   double input = -0.255234;
   input += synapse0x862a8e0();
   input += synapse0x862a908();
   input += synapse0x862a930();
   input += synapse0x862a958();
   return input;
}

double NNmuon::neuron0x862a708() {
   double input = input0x862a708();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x862a980() {
   double input = 0.492671;
   input += synapse0x862ab58();
   input += synapse0x862ab80();
   input += synapse0x862aba8();
   input += synapse0x862abd0();
   return input;
}

double NNmuon::neuron0x862a980() {
   double input = input0x862a980();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x862abf8() {
   double input = 0.850329;
   input += synapse0x862add0();
   input += synapse0x862adf8();
   input += synapse0x862ae20();
   input += synapse0x862ae48();
   return input;
}

double NNmuon::neuron0x862abf8() {
   double input = input0x862abf8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x862ae70() {
   double input = -0.303988;
   input += synapse0x862b068();
   input += synapse0x862b118();
   input += synapse0x862b140();
   input += synapse0x862b168();
   return input;
}

double NNmuon::neuron0x862ae70() {
   double input = input0x862ae70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x862b190() {
   double input = -0.90962;
   input += synapse0x862b340();
   input += synapse0x862b368();
   input += synapse0x862b390();
   input += synapse0x862b3b8();
   return input;
}

double NNmuon::neuron0x862b190() {
   double input = input0x862b190();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x8606710() {
   double input = 0.839732;
   input += synapse0x862b428();
   input += synapse0x862b450();
   input += synapse0x862b478();
   input += synapse0x862b4a0();
   input += synapse0x862b4c8();
   input += synapse0x862b4f0();
   return input;
}

double NNmuon::neuron0x8606710() {
   double input = input0x8606710();
   return (input * 0.46246)+0.690082;
}

double NNmuon::synapse0x85df950() {
   return (neuron0x8605f48()*1.40997);
}

double NNmuon::synapse0x862a690() {
   return (neuron0x8606120()*-0.400436);
}

double NNmuon::synapse0x862a6b8() {
   return (neuron0x86062f8()*-3.74766);
}

double NNmuon::synapse0x862a6e0() {
   return (neuron0x86064f0()*-0.417901);
}

double NNmuon::synapse0x862a8e0() {
   return (neuron0x8605f48()*1.0701);
}

double NNmuon::synapse0x862a908() {
   return (neuron0x8606120()*-1.3274);
}

double NNmuon::synapse0x862a930() {
   return (neuron0x86062f8()*-2.88738);
}

double NNmuon::synapse0x862a958() {
   return (neuron0x86064f0()*-1.48778);
}

double NNmuon::synapse0x862ab58() {
   return (neuron0x8605f48()*2.40266);
}

double NNmuon::synapse0x862ab80() {
   return (neuron0x8606120()*-1.12499);
}

double NNmuon::synapse0x862aba8() {
   return (neuron0x86062f8()*-2.64145);
}

double NNmuon::synapse0x862abd0() {
   return (neuron0x86064f0()*-1.1791);
}

double NNmuon::synapse0x862add0() {
   return (neuron0x8605f48()*0.985237);
}

double NNmuon::synapse0x862adf8() {
   return (neuron0x8606120()*0.199191);
}

double NNmuon::synapse0x862ae20() {
   return (neuron0x86062f8()*1.504);
}

double NNmuon::synapse0x862ae48() {
   return (neuron0x86064f0()*1.40007);
}

double NNmuon::synapse0x862b068() {
   return (neuron0x8605f48()*0.367569);
}

double NNmuon::synapse0x862b118() {
   return (neuron0x8606120()*0.0511259);
}

double NNmuon::synapse0x862b140() {
   return (neuron0x86062f8()*1.23334);
}

double NNmuon::synapse0x862b168() {
   return (neuron0x86064f0()*1.25481);
}

double NNmuon::synapse0x862b340() {
   return (neuron0x8605f48()*-1.85493);
}

double NNmuon::synapse0x862b368() {
   return (neuron0x8606120()*1.87316);
}

double NNmuon::synapse0x862b390() {
   return (neuron0x86062f8()*2.94055);
}

double NNmuon::synapse0x862b3b8() {
   return (neuron0x86064f0()*3.92743);
}

double NNmuon::synapse0x862b428() {
   return (neuron0x862a500()*-2.26194);
}

double NNmuon::synapse0x862b450() {
   return (neuron0x862a708()*-0.932995);
}

double NNmuon::synapse0x862b478() {
   return (neuron0x862a980()*2.43631);
}

double NNmuon::synapse0x862b4a0() {
   return (neuron0x862abf8()*-1.45719);
}

double NNmuon::synapse0x862b4c8() {
   return (neuron0x862ae70()*-1.18331);
}

double NNmuon::synapse0x862b4f0() {
   return (neuron0x862b190()*1.59807);
}

