#include "NNmuon.h"
#include "math.h"

double NNmuon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron0xb9c05c8()*0.459246)+0.69772);
     default:
         return 0.;
   }
}

double NNmuon::neuron0xb9bc8c8() {
   return input0;
}

double NNmuon::neuron0xb9c2908() {
   return input1;
}

double NNmuon::neuron0xb9c2a60() {
   return input2;
}

double NNmuon::neuron0xb9c0450() {
   return input3;
}

double NNmuon::neuron0xb9e0510() {
   double input = 0.350788;
   input += synapse0xb9e0668();
   input += synapse0xb9e0690();
   input += synapse0xb9e06b8();
   input += synapse0xb9e06e0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9e0708() {
   double input = -0.573666;
   input += synapse0xb9bca20();
   input += synapse0xb9c2bb8();
   input += synapse0xb9c2be0();
   input += synapse0xb9c06c0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9c7b48() {
   double input = -1.59232;
   input += synapse0xb9c7ca0();
   input += synapse0xb9c7cc8();
   input += synapse0xb9c7cf0();
   input += synapse0xb9c7d18();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9c7d40() {
   double input = -0.400469;
   input += synapse0xb9c0730();
   input += synapse0xb9c7e70();
   input += synapse0xb9bfba0();
   input += synapse0xb9bfbc8();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9bfbf0() {
   double input = -0.424459;
   input += synapse0xb9bfd48();
   input += synapse0xb9bfdf8();
   input += synapse0xb9bfe20();
   input += synapse0xb9bfe48();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9bfe70() {
   double input = 0.344951;
   input += synapse0xb9ebc28();
   input += synapse0xb9ebc50();
   input += synapse0xb9ebc78();
   input += synapse0xb9ebca0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNmuon::neuron0xb9c05c8() {
   double input = -0.261479;
   input += synapse0xb9ebd58();
   input += synapse0xb9ebd80();
   input += synapse0xb9ebda8();
   input += synapse0xb9ebdd0();
   input += synapse0xb9ebdf8();
   input += synapse0xb9ebe20();
   return input;
}

double NNmuon::synapse0xb9e0668() {
   return (neuron0xb9bc8c8()*-0.522318);
}

double NNmuon::synapse0xb9e0690() {
   return (neuron0xb9c2908()*0.102178);
}

double NNmuon::synapse0xb9e06b8() {
   return (neuron0xb9c2a60()*-0.078235);
}

double NNmuon::synapse0xb9e06e0() {
   return (neuron0xb9c0450()*-0.805722);
}

double NNmuon::synapse0xb9bca20() {
   return (neuron0xb9bc8c8()*-0.112092);
}

double NNmuon::synapse0xb9c2bb8() {
   return (neuron0xb9c2908()*0.703686);
}

double NNmuon::synapse0xb9c2be0() {
   return (neuron0xb9c2a60()*0.303727);
}

double NNmuon::synapse0xb9c06c0() {
   return (neuron0xb9c0450()*0.631018);
}

double NNmuon::synapse0xb9c7ca0() {
   return (neuron0xb9bc8c8()*-0.300864);
}

double NNmuon::synapse0xb9c7cc8() {
   return (neuron0xb9c2908()*-0.839097);
}

double NNmuon::synapse0xb9c7cf0() {
   return (neuron0xb9c2a60()*-0.381678);
}

double NNmuon::synapse0xb9c7d18() {
   return (neuron0xb9c0450()*1.98987);
}

double NNmuon::synapse0xb9c0730() {
   return (neuron0xb9bc8c8()*-0.361818);
}

double NNmuon::synapse0xb9c7e70() {
   return (neuron0xb9c2908()*-0.404265);
}

double NNmuon::synapse0xb9bfba0() {
   return (neuron0xb9c2a60()*-0.10066);
}

double NNmuon::synapse0xb9bfbc8() {
   return (neuron0xb9c0450()*-1.02543);
}

double NNmuon::synapse0xb9bfd48() {
   return (neuron0xb9bc8c8()*0.142338);
}

double NNmuon::synapse0xb9bfdf8() {
   return (neuron0xb9c2908()*0.973204);
}

double NNmuon::synapse0xb9bfe20() {
   return (neuron0xb9c2a60()*1.10313);
}

double NNmuon::synapse0xb9bfe48() {
   return (neuron0xb9c0450()*1.90637);
}

double NNmuon::synapse0xb9ebc28() {
   return (neuron0xb9bc8c8()*0.740708);
}

double NNmuon::synapse0xb9ebc50() {
   return (neuron0xb9c2908()*-0.465834);
}

double NNmuon::synapse0xb9ebc78() {
   return (neuron0xb9c2a60()*-0.650894);
}

double NNmuon::synapse0xb9ebca0() {
   return (neuron0xb9c0450()*-2.52622);
}

double NNmuon::synapse0xb9ebd58() {
   return (neuron0xb9e0510()*0.614115);
}

double NNmuon::synapse0xb9ebd80() {
   return (neuron0xb9e0708()*-1.20942);
}

double NNmuon::synapse0xb9ebda8() {
   return (neuron0xb9c7b48()*-1.1163);
}

double NNmuon::synapse0xb9ebdd0() {
   return (neuron0xb9c7d40()*0.992096);
}

double NNmuon::synapse0xb9ebdf8() {
   return (neuron0xb9bfbf0()*1.23372);
}

double NNmuon::synapse0xb9ebe20() {
   return (neuron0xb9bfe70()*-1.0627);
}

