#include "weights/NNmuon.h"
#include <cmath>

double NNmuon::Value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x11112670();
     default:
         return 0.;
   }
}

double NNmuon::Value(int index, double* input) {
   input0 = (input[0] - 0)/1;
   input1 = (input[1] - 0)/1;
   input2 = (input[2] - 0)/1;
   input3 = (input[3] - 0)/1;
   input4 = (input[4] - 0)/1;
   input5 = (input[5] - 0)/1;
   switch(index) {
     case 0:
         return neuron0x11112670();
     default:
         return 0.;
   }
}

double NNmuon::neuron0x1110e3e0() {
   return input0;
}

double NNmuon::neuron0x1110e6f0() {
   return input1;
}

double NNmuon::neuron0x1110ea00() {
   return input2;
}

double NNmuon::neuron0x1110ed10() {
   return input3;
}

double NNmuon::neuron0x1110f050() {
   return input4;
}

double NNmuon::neuron0x1110f390() {
   return input5;
}

double NNmuon::input0x1110f810() {
   double input = 0.371028;
   input += synapse0x10fe5ce0();
   input += synapse0xfb59f30();
   input += synapse0x11115050();
   input += synapse0x10fe5d70();
   input += synapse0x1110fa90();
   input += synapse0x1110fad0();
   return input;
}

double NNmuon::neuron0x1110f810() {
   double input = input0x1110f810();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x1110fb10() {
   double input = 0.303458;
   input += synapse0x1110fe20();
   input += synapse0x1110fe60();
   input += synapse0x1110fea0();
   input += synapse0x1110fee0();
   input += synapse0x1110ff20();
   input += synapse0x1110ff60();
   return input;
}

double NNmuon::neuron0x1110fb10() {
   double input = input0x1110fb10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x1110ffa0() {
   double input = -0.463226;
   input += synapse0x111102b0();
   input += synapse0x111102f0();
   input += synapse0x11110330();
   input += synapse0x11110370();
   input += synapse0x111103b0();
   input += synapse0x10fd8dd0();
   return input;
}

double NNmuon::neuron0x1110ffa0() {
   double input = input0x1110ffa0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11110500() {
   double input = 0.44674;
   input += synapse0x11110810();
   input += synapse0x11110850();
   input += synapse0x11110890();
   input += synapse0x111108d0();
   input += synapse0x11110910();
   input += synapse0x11110950();
   return input;
}

double NNmuon::neuron0x11110500() {
   double input = input0x11110500();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11110990() {
   double input = -0.290546;
   input += synapse0x11110ca0();
   input += synapse0x11110ce0();
   input += synapse0x11110d20();
   input += synapse0x11110d60();
   input += synapse0x11110da0();
   input += synapse0x11110de0();
   return input;
}

double NNmuon::neuron0x11110990() {
   double input = input0x11110990();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11110e20() {
   double input = 0.6714;
   input += synapse0x11111130();
   input += synapse0x11111170();
   input += synapse0x111111b0();
   input += synapse0x10fd8e10();
   input += synapse0x11115000();
   input += synapse0x10fe5d20();
   return input;
}

double NNmuon::neuron0x11110e20() {
   double input = input0x11110e20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11111400() {
   double input = -0.978381;
   input += synapse0x11111710();
   input += synapse0x11111750();
   input += synapse0x11111790();
   input += synapse0x111117d0();
   input += synapse0x11111810();
   input += synapse0x11111850();
   return input;
}

double NNmuon::neuron0x11111400() {
   double input = input0x11111400();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11111890() {
   double input = 0.953366;
   input += synapse0x11111ba0();
   input += synapse0x11111be0();
   input += synapse0x11111c20();
   input += synapse0x11111c60();
   input += synapse0x11111ca0();
   input += synapse0x11111ce0();
   return input;
}

double NNmuon::neuron0x11111890() {
   double input = input0x11111890();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11111d20() {
   double input = -0.349311;
   input += synapse0x11112030();
   input += synapse0x11112070();
   input += synapse0x111120b0();
   input += synapse0x111120f0();
   input += synapse0x11112130();
   input += synapse0x11112170();
   return input;
}

double NNmuon::neuron0x11111d20() {
   double input = input0x11111d20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x111121b0() {
   double input = 1.65991;
   input += synapse0x111124f0();
   input += synapse0x11112530();
   input += synapse0x11112570();
   input += synapse0x111125b0();
   input += synapse0x111125f0();
   input += synapse0x11112630();
   return input;
}

double NNmuon::neuron0x111121b0() {
   double input = input0x111121b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0x11112670() {
   double input = -0.474585;
   input += synapse0x111129b0();
   input += synapse0x111129f0();
   input += synapse0x11112a30();
   input += synapse0x11112a70();
   input += synapse0x11112ab0();
   input += synapse0x10fd6b10();
   input += synapse0x10fe5ed0();
   input += synapse0x1110e290();
   input += synapse0x1110e2d0();
   input += synapse0x1110e310();
   return input;
}

double NNmuon::neuron0x11112670() {
   double input = input0x11112670();
   return (input * 0.459495)+0.69714;
}

double NNmuon::synapse0x10fe5ce0() {
   return (neuron0x1110e3e0()*-0.818024);
}

double NNmuon::synapse0xfb59f30() {
   return (neuron0x1110e6f0()*-0.105498);
}

double NNmuon::synapse0x11115050() {
   return (neuron0x1110ea00()*0.631605);
}

double NNmuon::synapse0x10fe5d70() {
   return (neuron0x1110ed10()*0.232551);
}

double NNmuon::synapse0x1110fa90() {
   return (neuron0x1110f050()*-0.381485);
}

double NNmuon::synapse0x1110fad0() {
   return (neuron0x1110f390()*-0.334845);
}

double NNmuon::synapse0x1110fe20() {
   return (neuron0x1110e3e0()*-0.898146);
}

double NNmuon::synapse0x1110fe60() {
   return (neuron0x1110e6f0()*1.73048);
}

double NNmuon::synapse0x1110fea0() {
   return (neuron0x1110ea00()*0.175039);
}

double NNmuon::synapse0x1110fee0() {
   return (neuron0x1110ed10()*3.08814);
}

double NNmuon::synapse0x1110ff20() {
   return (neuron0x1110f050()*1.01281);
}

double NNmuon::synapse0x1110ff60() {
   return (neuron0x1110f390()*-1.95806);
}

double NNmuon::synapse0x111102b0() {
   return (neuron0x1110e3e0()*-0.430126);
}

double NNmuon::synapse0x111102f0() {
   return (neuron0x1110e6f0()*0.640108);
}

double NNmuon::synapse0x11110330() {
   return (neuron0x1110ea00()*0.0632815);
}

double NNmuon::synapse0x11110370() {
   return (neuron0x1110ed10()*-0.899191);
}

double NNmuon::synapse0x111103b0() {
   return (neuron0x1110f050()*0.200353);
}

double NNmuon::synapse0x10fd8dd0() {
   return (neuron0x1110f390()*-0.550221);
}

double NNmuon::synapse0x11110810() {
   return (neuron0x1110e3e0()*-0.679872);
}

double NNmuon::synapse0x11110850() {
   return (neuron0x1110e6f0()*-0.263155);
}

double NNmuon::synapse0x11110890() {
   return (neuron0x1110ea00()*0.273735);
}

double NNmuon::synapse0x111108d0() {
   return (neuron0x1110ed10()*0.410734);
}

double NNmuon::synapse0x11110910() {
   return (neuron0x1110f050()*-0.363235);
}

double NNmuon::synapse0x11110950() {
   return (neuron0x1110f390()*-0.457365);
}

double NNmuon::synapse0x11110ca0() {
   return (neuron0x1110e3e0()*-0.712994);
}

double NNmuon::synapse0x11110ce0() {
   return (neuron0x1110e6f0()*2.20522);
}

double NNmuon::synapse0x11110d20() {
   return (neuron0x1110ea00()*3.15368);
}

double NNmuon::synapse0x11110d60() {
   return (neuron0x1110ed10()*1.43622);
}

double NNmuon::synapse0x11110da0() {
   return (neuron0x1110f050()*-1.19772);
}

double NNmuon::synapse0x11110de0() {
   return (neuron0x1110f390()*1.3598);
}

double NNmuon::synapse0x11111130() {
   return (neuron0x1110e3e0()*1.87071);
}

double NNmuon::synapse0x11111170() {
   return (neuron0x1110e6f0()*-1.18592);
}

double NNmuon::synapse0x111111b0() {
   return (neuron0x1110ea00()*1.00153);
}

double NNmuon::synapse0x10fd8e10() {
   return (neuron0x1110ed10()*-3.09535);
}

double NNmuon::synapse0x11115000() {
   return (neuron0x1110f050()*-0.57193);
}

double NNmuon::synapse0x10fe5d20() {
   return (neuron0x1110f390()*0.0523752);
}

double NNmuon::synapse0x11111710() {
   return (neuron0x1110e3e0()*-0.277565);
}

double NNmuon::synapse0x11111750() {
   return (neuron0x1110e6f0()*0.908514);
}

double NNmuon::synapse0x11111790() {
   return (neuron0x1110ea00()*0.817851);
}

double NNmuon::synapse0x111117d0() {
   return (neuron0x1110ed10()*0.547601);
}

double NNmuon::synapse0x11111810() {
   return (neuron0x1110f050()*-0.303027);
}

double NNmuon::synapse0x11111850() {
   return (neuron0x1110f390()*-1.59822);
}

double NNmuon::synapse0x11111ba0() {
   return (neuron0x1110e3e0()*0.453535);
}

double NNmuon::synapse0x11111be0() {
   return (neuron0x1110e6f0()*1.5143);
}

double NNmuon::synapse0x11111c20() {
   return (neuron0x1110ea00()*0.809455);
}

double NNmuon::synapse0x11111c60() {
   return (neuron0x1110ed10()*-0.509572);
}

double NNmuon::synapse0x11111ca0() {
   return (neuron0x1110f050()*1.26204);
}

double NNmuon::synapse0x11111ce0() {
   return (neuron0x1110f390()*0.0896071);
}

double NNmuon::synapse0x11112030() {
   return (neuron0x1110e3e0()*0.168041);
}

double NNmuon::synapse0x11112070() {
   return (neuron0x1110e6f0()*-0.712596);
}

double NNmuon::synapse0x111120b0() {
   return (neuron0x1110ea00()*-0.954125);
}

double NNmuon::synapse0x111120f0() {
   return (neuron0x1110ed10()*0.0977111);
}

double NNmuon::synapse0x11112130() {
   return (neuron0x1110f050()*0.49898);
}

double NNmuon::synapse0x11112170() {
   return (neuron0x1110f390()*0.155285);
}

double NNmuon::synapse0x111124f0() {
   return (neuron0x1110e3e0()*3.63938);
}

double NNmuon::synapse0x11112530() {
   return (neuron0x1110e6f0()*-0.181763);
}

double NNmuon::synapse0x11112570() {
   return (neuron0x1110ea00()*-1.57741);
}

double NNmuon::synapse0x111125b0() {
   return (neuron0x1110ed10()*-0.0951818);
}

double NNmuon::synapse0x111125f0() {
   return (neuron0x1110f050()*-0.669102);
}

double NNmuon::synapse0x11112630() {
   return (neuron0x1110f390()*0.101291);
}

double NNmuon::synapse0x111129b0() {
   return (neuron0x1110f810()*-0.646383);
}

double NNmuon::synapse0x111129f0() {
   return (neuron0x1110fb10()*1.96503);
}

double NNmuon::synapse0x11112a30() {
   return (neuron0x1110ffa0()*0.28733);
}

double NNmuon::synapse0x11112a70() {
   return (neuron0x11110500()*-0.626466);
}

double NNmuon::synapse0x11112ab0() {
   return (neuron0x11110990()*2.45469);
}

double NNmuon::synapse0x10fd6b10() {
   return (neuron0x11110e20()*1.55213);
}

double NNmuon::synapse0x10fe5ed0() {
   return (neuron0x11111400()*-0.538252);
}

double NNmuon::synapse0x1110e290() {
   return (neuron0x11111890()*-2.18202);
}

double NNmuon::synapse0x1110e2d0() {
   return (neuron0x11111d20()*-0.104759);
}

double NNmuon::synapse0x1110e310() {
   return (neuron0x111121b0()*-1.66865);
}

