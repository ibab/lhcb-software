#include "NNkaonS.h"
#include <cmath>

double NNkaonS::value(int index,double in0,double in1,double in2,double in3,
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
         return neuron0x1ab0f920();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x1aac5920() {
   return input0;
}

double NNkaonS::neuron0x1aac5c30() {
   return input1;
}

double NNkaonS::neuron0x1aac5f40() {
   return input2;
}

double NNkaonS::neuron0x1aac6250() {
   return input3;
}

double NNkaonS::neuron0x1aac6560() {
   return input4;
}

double NNkaonS::neuron0x1aac68a0() {
   return input5;
}

double NNkaonS::neuron0x1aac6be0() {
   return input6;
}

double NNkaonS::neuron0x1aac6f20() {
   return input7;
}

double NNkaonS::neuron0x1aac7260() {
   return input8;
}

double NNkaonS::input0x1aac77f0() {
   double input = -0.564186;
   input += synapse0x1ad2bb50();
   input += synapse0x1aac7a70();
   input += synapse0x1aac7ab0();
   input += synapse0x1aac7af0();
   input += synapse0x1aac7b30();
   input += synapse0x1aac7b70();
   input += synapse0x1aac7bb0();
   input += synapse0x1aac7bf0();
   input += synapse0x1aac7c30();
   return input;
}

double NNkaonS::neuron0x1aac77f0() {
   double input = input0x1aac77f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1aac7c70() {
   double input = 0.206353;
   input += synapse0x1aac7f80();
   input += synapse0x1aac7fc0();
   input += synapse0x1aac8000();
   input += synapse0x1aac8040();
   input += synapse0x1aac8080();
   input += synapse0x1aac80c0();
   input += synapse0x1aac8100();
   input += synapse0x1aac8140();
   input += synapse0x1ad2bbb0();
   return input;
}

double NNkaonS::neuron0x1aac7c70() {
   double input = input0x1aac7c70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1aac8290() {
   double input = 0.342163;
   input += synapse0x1aac8510();
   input += synapse0x1aac8550();
   input += synapse0x1aac8590();
   input += synapse0x1aac85d0();
   input += synapse0x1aac8610();
   input += synapse0x1aac8650();
   input += synapse0x1aac8690();
   input += synapse0x1aac86d0();
   input += synapse0x1aac8710();
   return input;
}

double NNkaonS::neuron0x1aac8290() {
   double input = input0x1aac8290();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1aac8750() {
   double input = -0.903464;
   input += synapse0x1aac8a60();
   input += synapse0x1aac8aa0();
   input += synapse0x1aac8ae0();
   input += synapse0x1aac8b20();
   input += synapse0x1aac8b60();
   input += synapse0x1aac8ba0();
   input += synapse0x1abc7a00();
   input += synapse0x1aaf6ed0();
   input += synapse0x1ad2b990();
   return input;
}

double NNkaonS::neuron0x1aac8750() {
   double input = input0x1aac8750();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1ad2bce0() {
   double input = 0.51773;
   input += synapse0x1aac8e80();
   input += synapse0x1aac8ec0();
   input += synapse0x1aac8f00();
   input += synapse0x1aac8f40();
   input += synapse0x1aac8f80();
   input += synapse0x1aac8fc0();
   input += synapse0x1aac9000();
   input += synapse0x1aac9040();
   input += synapse0x1aac9080();
   return input;
}

double NNkaonS::neuron0x1ad2bce0() {
   double input = input0x1ad2bce0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1aac90c0() {
   double input = -0.196174;
   input += synapse0x1aac93d0();
   input += synapse0x1aac9410();
   input += synapse0x1aac9450();
   input += synapse0x1aac9490();
   input += synapse0x1aac94d0();
   input += synapse0x1aac9510();
   input += synapse0x1aac9550();
   input += synapse0x1aac9590();
   input += synapse0x1aac95d0();
   return input;
}

double NNkaonS::neuron0x1aac90c0() {
   double input = input0x1aac90c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1aac9610() {
   double input = -0.349063;
   input += synapse0x1ab0e3b0();
   input += synapse0x1ab0e3f0();
   input += synapse0x1ab0e430();
   input += synapse0x1ab0e470();
   input += synapse0x1ab0e4b0();
   input += synapse0x1ab0e4f0();
   input += synapse0x1ab0e530();
   input += synapse0x1ab0e570();
   input += synapse0x1ab0e5b0();
   return input;
}

double NNkaonS::neuron0x1aac9610() {
   double input = input0x1aac9610();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1ab0e5f0() {
   double input = 0.932815;
   input += synapse0x1ad2be40();
   input += synapse0x1ad2be80();
   input += synapse0x1aac8210();
   input += synapse0x1aac8250();
   input += synapse0x1abd6400();
   input += synapse0x1abd6440();
   input += synapse0x1aac8be0();
   input += synapse0x1aac8c20();
   input += synapse0x1aac8c60();
   return input;
}

double NNkaonS::neuron0x1ab0e5f0() {
   double input = input0x1ab0e5f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1ab0ee20() {
   double input = -0.554407;
   input += synapse0x1ab0f160();
   input += synapse0x1ab0f1a0();
   input += synapse0x1ab0f1e0();
   input += synapse0x1ab0f220();
   input += synapse0x1ab0f260();
   input += synapse0x1ab0f2a0();
   input += synapse0x1ab0f2e0();
   input += synapse0x1ab0f320();
   input += synapse0x1ab0f360();
   return input;
}

double NNkaonS::neuron0x1ab0ee20() {
   double input = input0x1ab0ee20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1ab0f3a0() {
   double input = -0.796708;
   input += synapse0x1ab0f6e0();
   input += synapse0x1ab0f720();
   input += synapse0x1ab0f760();
   input += synapse0x1ab0f7a0();
   input += synapse0x1ab0f7e0();
   input += synapse0x1ab0f820();
   input += synapse0x1ab0f860();
   input += synapse0x1ab0f8a0();
   input += synapse0x1ab0f8e0();
   return input;
}

double NNkaonS::neuron0x1ab0f3a0() {
   double input = input0x1ab0f3a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1ab0f920() {
   double input = 1.26187;
   input += synapse0x1ab0fc60();
   input += synapse0x1ab0fca0();
   input += synapse0x1ab0fce0();
   input += synapse0x1ab0fd20();
   input += synapse0x1ab0fd60();
   input += synapse0x1ab0fda0();
   input += synapse0x1ab0fde0();
   input += synapse0x1ab0fe20();
   input += synapse0x1ab0fe60();
   input += synapse0x1ab0fea0();
   return input;
}

double NNkaonS::neuron0x1ab0f920() {
   double input = input0x1ab0f920();
   return (input * 0.476481)+0.651545;
}

double NNkaonS::synapse0x1ad2bb50() {
   return (neuron0x1aac5920()*-1.22453);
}

double NNkaonS::synapse0x1aac7a70() {
   return (neuron0x1aac5c30()*0.23253);
}

double NNkaonS::synapse0x1aac7ab0() {
   return (neuron0x1aac5f40()*1.82404);
}

double NNkaonS::synapse0x1aac7af0() {
   return (neuron0x1aac6250()*-0.123057);
}

double NNkaonS::synapse0x1aac7b30() {
   return (neuron0x1aac6560()*0.696452);
}

double NNkaonS::synapse0x1aac7b70() {
   return (neuron0x1aac68a0()*-1.233);
}

double NNkaonS::synapse0x1aac7bb0() {
   return (neuron0x1aac6be0()*-2.1493);
}

double NNkaonS::synapse0x1aac7bf0() {
   return (neuron0x1aac6f20()*-0.536096);
}

double NNkaonS::synapse0x1aac7c30() {
   return (neuron0x1aac7260()*1.81709);
}

double NNkaonS::synapse0x1aac7f80() {
   return (neuron0x1aac5920()*0.0100079);
}

double NNkaonS::synapse0x1aac7fc0() {
   return (neuron0x1aac5c30()*-0.463846);
}

double NNkaonS::synapse0x1aac8000() {
   return (neuron0x1aac5f40()*-0.103596);
}

double NNkaonS::synapse0x1aac8040() {
   return (neuron0x1aac6250()*0.0463288);
}

double NNkaonS::synapse0x1aac8080() {
   return (neuron0x1aac6560()*-0.688898);
}

double NNkaonS::synapse0x1aac80c0() {
   return (neuron0x1aac68a0()*-0.732981);
}

double NNkaonS::synapse0x1aac8100() {
   return (neuron0x1aac6be0()*-1.66028);
}

double NNkaonS::synapse0x1aac8140() {
   return (neuron0x1aac6f20()*-0.248812);
}

double NNkaonS::synapse0x1ad2bbb0() {
   return (neuron0x1aac7260()*-0.815584);
}

double NNkaonS::synapse0x1aac8510() {
   return (neuron0x1aac5920()*4.52444);
}

double NNkaonS::synapse0x1aac8550() {
   return (neuron0x1aac5c30()*0.871808);
}

double NNkaonS::synapse0x1aac8590() {
   return (neuron0x1aac5f40()*1.06991);
}

double NNkaonS::synapse0x1aac85d0() {
   return (neuron0x1aac6250()*0.439947);
}

double NNkaonS::synapse0x1aac8610() {
   return (neuron0x1aac6560()*1.07329);
}

double NNkaonS::synapse0x1aac8650() {
   return (neuron0x1aac68a0()*-0.585206);
}

double NNkaonS::synapse0x1aac8690() {
   return (neuron0x1aac6be0()*3.03067);
}

double NNkaonS::synapse0x1aac86d0() {
   return (neuron0x1aac6f20()*1.82392);
}

double NNkaonS::synapse0x1aac8710() {
   return (neuron0x1aac7260()*0.560435);
}

double NNkaonS::synapse0x1aac8a60() {
   return (neuron0x1aac5920()*-0.885661);
}

double NNkaonS::synapse0x1aac8aa0() {
   return (neuron0x1aac5c30()*-1.446);
}

double NNkaonS::synapse0x1aac8ae0() {
   return (neuron0x1aac5f40()*-1.09213);
}

double NNkaonS::synapse0x1aac8b20() {
   return (neuron0x1aac6250()*-0.904833);
}

double NNkaonS::synapse0x1aac8b60() {
   return (neuron0x1aac6560()*-0.497838);
}

double NNkaonS::synapse0x1aac8ba0() {
   return (neuron0x1aac68a0()*0.37464);
}

double NNkaonS::synapse0x1abc7a00() {
   return (neuron0x1aac6be0()*1.77178);
}

double NNkaonS::synapse0x1aaf6ed0() {
   return (neuron0x1aac6f20()*-0.388382);
}

double NNkaonS::synapse0x1ad2b990() {
   return (neuron0x1aac7260()*0.633393);
}

double NNkaonS::synapse0x1aac8e80() {
   return (neuron0x1aac5920()*-0.0983595);
}

double NNkaonS::synapse0x1aac8ec0() {
   return (neuron0x1aac5c30()*-0.345881);
}

double NNkaonS::synapse0x1aac8f00() {
   return (neuron0x1aac5f40()*-0.156762);
}

double NNkaonS::synapse0x1aac8f40() {
   return (neuron0x1aac6250()*-0.229957);
}

double NNkaonS::synapse0x1aac8f80() {
   return (neuron0x1aac6560()*-0.757335);
}

double NNkaonS::synapse0x1aac8fc0() {
   return (neuron0x1aac68a0()*-0.366837);
}

double NNkaonS::synapse0x1aac9000() {
   return (neuron0x1aac6be0()*-1.53906);
}

double NNkaonS::synapse0x1aac9040() {
   return (neuron0x1aac6f20()*-0.216169);
}

double NNkaonS::synapse0x1aac9080() {
   return (neuron0x1aac7260()*-1.05875);
}

double NNkaonS::synapse0x1aac93d0() {
   return (neuron0x1aac5920()*0.159883);
}

double NNkaonS::synapse0x1aac9410() {
   return (neuron0x1aac5c30()*-0.269282);
}

double NNkaonS::synapse0x1aac9450() {
   return (neuron0x1aac5f40()*-0.334548);
}

double NNkaonS::synapse0x1aac9490() {
   return (neuron0x1aac6250()*0.174256);
}

double NNkaonS::synapse0x1aac94d0() {
   return (neuron0x1aac6560()*0.110566);
}

double NNkaonS::synapse0x1aac9510() {
   return (neuron0x1aac68a0()*0.0402022);
}

double NNkaonS::synapse0x1aac9550() {
   return (neuron0x1aac6be0()*-0.30129);
}

double NNkaonS::synapse0x1aac9590() {
   return (neuron0x1aac6f20()*0.405113);
}

double NNkaonS::synapse0x1aac95d0() {
   return (neuron0x1aac7260()*-0.30214);
}

double NNkaonS::synapse0x1ab0e3b0() {
   return (neuron0x1aac5920()*-0.526613);
}

double NNkaonS::synapse0x1ab0e3f0() {
   return (neuron0x1aac5c30()*0.271191);
}

double NNkaonS::synapse0x1ab0e430() {
   return (neuron0x1aac5f40()*0.165309);
}

double NNkaonS::synapse0x1ab0e470() {
   return (neuron0x1aac6250()*-0.246144);
}

double NNkaonS::synapse0x1ab0e4b0() {
   return (neuron0x1aac6560()*-0.31947);
}

double NNkaonS::synapse0x1ab0e4f0() {
   return (neuron0x1aac68a0()*-0.113593);
}

double NNkaonS::synapse0x1ab0e530() {
   return (neuron0x1aac6be0()*1.49555);
}

double NNkaonS::synapse0x1ab0e570() {
   return (neuron0x1aac6f20()*-0.0936275);
}

double NNkaonS::synapse0x1ab0e5b0() {
   return (neuron0x1aac7260()*0.363374);
}

double NNkaonS::synapse0x1ad2be40() {
   return (neuron0x1aac5920()*-0.728734);
}

double NNkaonS::synapse0x1ad2be80() {
   return (neuron0x1aac5c30()*-1.51552);
}

double NNkaonS::synapse0x1aac8210() {
   return (neuron0x1aac5f40()*-2.31039);
}

double NNkaonS::synapse0x1aac8250() {
   return (neuron0x1aac6250()*0.816243);
}

double NNkaonS::synapse0x1abd6400() {
   return (neuron0x1aac6560()*-0.782941);
}

double NNkaonS::synapse0x1abd6440() {
   return (neuron0x1aac68a0()*-1.96656);
}

double NNkaonS::synapse0x1aac8be0() {
   return (neuron0x1aac6be0()*2.22194);
}

double NNkaonS::synapse0x1aac8c20() {
   return (neuron0x1aac6f20()*-1.22056);
}

double NNkaonS::synapse0x1aac8c60() {
   return (neuron0x1aac7260()*-1.49691);
}

double NNkaonS::synapse0x1ab0f160() {
   return (neuron0x1aac5920()*-0.249514);
}

double NNkaonS::synapse0x1ab0f1a0() {
   return (neuron0x1aac5c30()*-0.071963);
}

double NNkaonS::synapse0x1ab0f1e0() {
   return (neuron0x1aac5f40()*-0.0526603);
}

double NNkaonS::synapse0x1ab0f220() {
   return (neuron0x1aac6250()*-0.115992);
}

double NNkaonS::synapse0x1ab0f260() {
   return (neuron0x1aac6560()*-0.432906);
}

double NNkaonS::synapse0x1ab0f2a0() {
   return (neuron0x1aac68a0()*-0.403913);
}

double NNkaonS::synapse0x1ab0f2e0() {
   return (neuron0x1aac6be0()*-0.614536);
}

double NNkaonS::synapse0x1ab0f320() {
   return (neuron0x1aac6f20()*0.0776126);
}

double NNkaonS::synapse0x1ab0f360() {
   return (neuron0x1aac7260()*0.0743012);
}

double NNkaonS::synapse0x1ab0f6e0() {
   return (neuron0x1aac5920()*-0.19254);
}

double NNkaonS::synapse0x1ab0f720() {
   return (neuron0x1aac5c30()*-1.10687);
}

double NNkaonS::synapse0x1ab0f760() {
   return (neuron0x1aac5f40()*-0.779297);
}

double NNkaonS::synapse0x1ab0f7a0() {
   return (neuron0x1aac6250()*0.390051);
}

double NNkaonS::synapse0x1ab0f7e0() {
   return (neuron0x1aac6560()*-0.076467);
}

double NNkaonS::synapse0x1ab0f820() {
   return (neuron0x1aac68a0()*-0.87174);
}

double NNkaonS::synapse0x1ab0f860() {
   return (neuron0x1aac6be0()*0.769319);
}

double NNkaonS::synapse0x1ab0f8a0() {
   return (neuron0x1aac6f20()*-0.348607);
}

double NNkaonS::synapse0x1ab0f8e0() {
   return (neuron0x1aac7260()*-0.419198);
}

double NNkaonS::synapse0x1ab0fc60() {
   return (neuron0x1aac77f0()*1.81417);
}

double NNkaonS::synapse0x1ab0fca0() {
   return (neuron0x1aac7c70()*1.458);
}

double NNkaonS::synapse0x1ab0fce0() {
   return (neuron0x1aac8290()*-2.87009);
}

double NNkaonS::synapse0x1ab0fd20() {
   return (neuron0x1aac8750()*-1.49239);
}

double NNkaonS::synapse0x1ab0fd60() {
   return (neuron0x1ad2bce0()*1.50849);
}

double NNkaonS::synapse0x1ab0fda0() {
   return (neuron0x1aac90c0()*0.750315);
}

double NNkaonS::synapse0x1ab0fde0() {
   return (neuron0x1aac9610()*-0.932511);
}

double NNkaonS::synapse0x1ab0fe20() {
   return (neuron0x1ab0e5f0()*-2.07048);
}

double NNkaonS::synapse0x1ab0fe60() {
   return (neuron0x1ab0ee20()*0.996694);
}

double NNkaonS::synapse0x1ab0fea0() {
   return (neuron0x1ab0f3a0()*-0.977807);
}

