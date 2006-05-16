#include "NNele.h"
#include "math.h"

double NNele::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron0xba2e6b8()*0.469718)+0.671362);
     default:
         return 0.;
   }
}

double NNele::neuron0xba2a9b8() {
   return input0;
}

double NNele::neuron0xba309f8() {
   return input1;
}

double NNele::neuron0xba30b50() {
   return input2;
}

double NNele::neuron0xba2e540() {
   return input3;
}

double NNele::neuron0xba4e600() {
   double input = 0.625753;
   input += synapse0xba4e758();
   input += synapse0xba4e780();
   input += synapse0xba4e7a8();
   input += synapse0xba4e7d0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba4e7f8() {
   double input = 0.212902;
   input += synapse0xba2ab10();
   input += synapse0xba30ca8();
   input += synapse0xba30cd0();
   input += synapse0xba2e7b0();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba35c38() {
   double input = 0.598824;
   input += synapse0xba35d90();
   input += synapse0xba35db8();
   input += synapse0xba35de0();
   input += synapse0xba35e08();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba35e30() {
   double input = 0.746238;
   input += synapse0xba2e820();
   input += synapse0xba35f60();
   input += synapse0xba2dc90();
   input += synapse0xba2dcb8();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba2dce0() {
   double input = 0.129337;
   input += synapse0xba2de38();
   input += synapse0xba2dee8();
   input += synapse0xba2df10();
   input += synapse0xba2df38();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba2df60() {
   double input = -0.358016;
   input += synapse0xba59d18();
   input += synapse0xba59d40();
   input += synapse0xba59d68();
   input += synapse0xba59d90();
   return ((1/(1+exp(-input)))*1)+0;
}

double NNele::neuron0xba2e6b8() {
   double input = -0.830392;
   input += synapse0xba59e48();
   input += synapse0xba59e70();
   input += synapse0xba59e98();
   input += synapse0xba59ec0();
   input += synapse0xba59ee8();
   input += synapse0xba59f10();
   return input;
}

double NNele::synapse0xba4e758() {
   return (neuron0xba2a9b8()*0.909279);
}

double NNele::synapse0xba4e780() {
   return (neuron0xba309f8()*-1.05364);
}

double NNele::synapse0xba4e7a8() {
   return (neuron0xba30b50()*-1.15594);
}

double NNele::synapse0xba4e7d0() {
   return (neuron0xba2e540()*-0.142159);
}

double NNele::synapse0xba2ab10() {
   return (neuron0xba2a9b8()*-0.423483);
}

double NNele::synapse0xba30ca8() {
   return (neuron0xba309f8()*0.0398399);
}

double NNele::synapse0xba30cd0() {
   return (neuron0xba30b50()*0.85487);
}

double NNele::synapse0xba2e7b0() {
   return (neuron0xba2e540()*1.61091);
}

double NNele::synapse0xba35d90() {
   return (neuron0xba2a9b8()*0.481039);
}

double NNele::synapse0xba35db8() {
   return (neuron0xba309f8()*3.21574);
}

double NNele::synapse0xba35de0() {
   return (neuron0xba30b50()*0.194944);
}

double NNele::synapse0xba35e08() {
   return (neuron0xba2e540()*-0.827025);
}

double NNele::synapse0xba2e820() {
   return (neuron0xba2a9b8()*-1.48525);
}

double NNele::synapse0xba35f60() {
   return (neuron0xba309f8()*1.87102);
}

double NNele::synapse0xba2dc90() {
   return (neuron0xba30b50()*0.265475);
}

double NNele::synapse0xba2dcb8() {
   return (neuron0xba2e540()*2.07681);
}

double NNele::synapse0xba2de38() {
   return (neuron0xba2a9b8()*0.252744);
}

double NNele::synapse0xba2dee8() {
   return (neuron0xba309f8()*0.452997);
}

double NNele::synapse0xba2df10() {
   return (neuron0xba30b50()*0.381826);
}

double NNele::synapse0xba2df38() {
   return (neuron0xba2e540()*-0.317201);
}

double NNele::synapse0xba59d18() {
   return (neuron0xba2a9b8()*0.79324);
}

double NNele::synapse0xba59d40() {
   return (neuron0xba309f8()*0.222135);
}

double NNele::synapse0xba59d68() {
   return (neuron0xba30b50()*0.846111);
}

double NNele::synapse0xba59d90() {
   return (neuron0xba2e540()*-1.15858);
}

double NNele::synapse0xba59e48() {
   return (neuron0xba4e600()*-0.504457);
}

double NNele::synapse0xba59e70() {
   return (neuron0xba4e7f8()*-0.779954);
}

double NNele::synapse0xba59e98() {
   return (neuron0xba35c38()*-2.13911);
}

double NNele::synapse0xba59ec0() {
   return (neuron0xba35e30()*2.38564);
}

double NNele::synapse0xba59ee8() {
   return (neuron0xba2dce0()*0.457206);
}

double NNele::synapse0xba59f10() {
   return (neuron0xba2df60()*2.38384);
}

