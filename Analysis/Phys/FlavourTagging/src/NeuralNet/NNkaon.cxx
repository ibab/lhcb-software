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
         return neuron0x8763268();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x851c738() {
   return input0;
}

double NNkaon::neuron0x8761768() {
   return input1;
}

double NNkaon::neuron0x8761960() {
   return input2;
}

double NNkaon::neuron0x8761b58() {
   return input3;
}

double NNkaon::neuron0x8761d50() {
   return input4;
}

double NNkaon::neuron0x8761f48() {
   return input5;
}

double NNkaon::neuron0x8762140() {
   return input6;
}

double NNkaon::input0x8762438() {
   double input = 0.778235;
   input += synapse0x855bd90();
   input += synapse0x87625c8();
   input += synapse0x87625f0();
   input += synapse0x8762618();
   input += synapse0x8762640();
   input += synapse0x8762668();
   input += synapse0x8762690();
   return input;
}

double NNkaon::neuron0x8762438() {
   double input = input0x8762438();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x87626b8() {
   double input = 0.601815;
   input += synapse0x8762890();
   input += synapse0x87628b8();
   input += synapse0x87628e0();
   input += synapse0x8762908();
   input += synapse0x8762930();
   input += synapse0x8762958();
   input += synapse0x8762980();
   return input;
}

double NNkaon::neuron0x87626b8() {
   double input = input0x87626b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x87629a8() {
   double input = 0.3317;
   input += synapse0x8762b80();
   input += synapse0x8762ba8();
   input += synapse0x8762bd0();
   input += synapse0x8762c80();
   input += synapse0x8762ca8();
   input += synapse0x8762cd0();
   input += synapse0x8762cf8();
   return input;
}

double NNkaon::neuron0x87629a8() {
   double input = input0x87629a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8762d20() {
   double input = -0.783279;
   input += synapse0x8762eb0();
   input += synapse0x8762ed8();
   input += synapse0x8762f00();
   input += synapse0x8762f28();
   input += synapse0x8762f50();
   input += synapse0x8762f78();
   input += synapse0x8762fa0();
   return input;
}

double NNkaon::neuron0x8762d20() {
   double input = input0x8762d20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8762fc8() {
   double input = 0.188026;
   input += synapse0x87631a0();
   input += synapse0x87631c8();
   input += synapse0x87631f0();
   input += synapse0x8763218();
   input += synapse0x8763240();
   input += synapse0x8762bf8();
   input += synapse0x8762c20();
   return input;
}

double NNkaon::neuron0x8762fc8() {
   double input = input0x8762fc8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763370() {
   double input = -0.317004;
   input += synapse0x8763548();
   input += synapse0x8763570();
   input += synapse0x8763598();
   input += synapse0x87635c0();
   input += synapse0x87635e8();
   input += synapse0x8763610();
   input += synapse0x8763638();
   return input;
}

double NNkaon::neuron0x8763370() {
   double input = input0x8763370();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763660() {
   double input = 0.0304555;
   input += synapse0x8763858();
   input += synapse0x8763880();
   input += synapse0x87638a8();
   input += synapse0x87638d0();
   input += synapse0x87638f8();
   input += synapse0x8763920();
   input += synapse0x8763948();
   return input;
}

double NNkaon::neuron0x8763660() {
   double input = input0x8763660();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763970() {
   double input = -0.345356;
   input += synapse0x8763b68();
   input += synapse0x8763b90();
   input += synapse0x8763bb8();
   input += synapse0x8763be0();
   input += synapse0x8763c08();
   input += synapse0x8763c30();
   input += synapse0x8763c58();
   return input;
}

double NNkaon::neuron0x8763970() {
   double input = input0x8763970();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763c80() {
   double input = 0.286783;
   input += synapse0x8763e78();
   input += synapse0x8763ea0();
   input += synapse0x8763ec8();
   input += synapse0x8763ef0();
   input += synapse0x8763f18();
   input += synapse0x8763f40();
   input += synapse0x8763f68();
   return input;
}

double NNkaon::neuron0x8763c80() {
   double input = input0x8763c80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763f90() {
   double input = -0.271228;
   input += synapse0x8764210();
   input += synapse0x8764238();
   input += synapse0x851c628();
   input += synapse0x855bdb8();
   input += synapse0x855c0d8();
   input += synapse0x855c060();
   input += synapse0x8762c48();
   return input;
}

double NNkaon::neuron0x8763f90() {
   double input = input0x8763f90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8763268() {
   double input = -0.572471;
   input += synapse0x8762360();
   input += synapse0x8762388();
   input += synapse0x87623b0();
   input += synapse0x87623d8();
   input += synapse0x8762400();
   input += synapse0x8764468();
   input += synapse0x8764490();
   input += synapse0x87644b8();
   input += synapse0x87644e0();
   input += synapse0x8764508();
   return input;
}

double NNkaon::neuron0x8763268() {
   double input = input0x8763268();
   return (input * 0.476952)+0.650057;
}

double NNkaon::synapse0x855bd90() {
   return (neuron0x851c738()*-0.302871);
}

double NNkaon::synapse0x87625c8() {
   return (neuron0x8761768()*-0.547726);
}

double NNkaon::synapse0x87625f0() {
   return (neuron0x8761960()*0.547413);
}

double NNkaon::synapse0x8762618() {
   return (neuron0x8761b58()*-1.11772);
}

double NNkaon::synapse0x8762640() {
   return (neuron0x8761d50()*0.196553);
}

double NNkaon::synapse0x8762668() {
   return (neuron0x8761f48()*0.606072);
}

double NNkaon::synapse0x8762690() {
   return (neuron0x8762140()*-0.1287);
}

double NNkaon::synapse0x8762890() {
   return (neuron0x851c738()*1.40929);
}

double NNkaon::synapse0x87628b8() {
   return (neuron0x8761768()*-0.0291593);
}

double NNkaon::synapse0x87628e0() {
   return (neuron0x8761960()*-0.103167);
}

double NNkaon::synapse0x8762908() {
   return (neuron0x8761b58()*-0.224189);
}

double NNkaon::synapse0x8762930() {
   return (neuron0x8761d50()*-0.0913619);
}

double NNkaon::synapse0x8762958() {
   return (neuron0x8761f48()*0.165582);
}

double NNkaon::synapse0x8762980() {
   return (neuron0x8762140()*0.259115);
}

double NNkaon::synapse0x8762b80() {
   return (neuron0x851c738()*0.477592);
}

double NNkaon::synapse0x8762ba8() {
   return (neuron0x8761768()*0.372755);
}

double NNkaon::synapse0x8762bd0() {
   return (neuron0x8761960()*0.113981);
}

double NNkaon::synapse0x8762c80() {
   return (neuron0x8761b58()*-0.027816);
}

double NNkaon::synapse0x8762ca8() {
   return (neuron0x8761d50()*-0.0440599);
}

double NNkaon::synapse0x8762cd0() {
   return (neuron0x8761f48()*1.28278);
}

double NNkaon::synapse0x8762cf8() {
   return (neuron0x8762140()*1.4123);
}

double NNkaon::synapse0x8762eb0() {
   return (neuron0x851c738()*-0.955101);
}

double NNkaon::synapse0x8762ed8() {
   return (neuron0x8761768()*0.180562);
}

double NNkaon::synapse0x8762f00() {
   return (neuron0x8761960()*-0.474554);
}

double NNkaon::synapse0x8762f28() {
   return (neuron0x8761b58()*1.34505);
}

double NNkaon::synapse0x8762f50() {
   return (neuron0x8761d50()*-0.0186073);
}

double NNkaon::synapse0x8762f78() {
   return (neuron0x8761f48()*-0.0184381);
}

double NNkaon::synapse0x8762fa0() {
   return (neuron0x8762140()*-0.240419);
}

double NNkaon::synapse0x87631a0() {
   return (neuron0x851c738()*1.14078);
}

double NNkaon::synapse0x87631c8() {
   return (neuron0x8761768()*-0.108515);
}

double NNkaon::synapse0x87631f0() {
   return (neuron0x8761960()*-1.42851);
}

double NNkaon::synapse0x8763218() {
   return (neuron0x8761b58()*-2.49045);
}

double NNkaon::synapse0x8763240() {
   return (neuron0x8761d50()*0.643779);
}

double NNkaon::synapse0x8762bf8() {
   return (neuron0x8761f48()*-0.1588);
}

double NNkaon::synapse0x8762c20() {
   return (neuron0x8762140()*0.677853);
}

double NNkaon::synapse0x8763548() {
   return (neuron0x851c738()*0.528021);
}

double NNkaon::synapse0x8763570() {
   return (neuron0x8761768()*-0.924372);
}

double NNkaon::synapse0x8763598() {
   return (neuron0x8761960()*-1.38014);
}

double NNkaon::synapse0x87635c0() {
   return (neuron0x8761b58()*-0.343361);
}

double NNkaon::synapse0x87635e8() {
   return (neuron0x8761d50()*1.37643);
}

double NNkaon::synapse0x8763610() {
   return (neuron0x8761f48()*1.84574);
}

double NNkaon::synapse0x8763638() {
   return (neuron0x8762140()*1.00592);
}

double NNkaon::synapse0x8763858() {
   return (neuron0x851c738()*1.6198);
}

double NNkaon::synapse0x8763880() {
   return (neuron0x8761768()*0.24457);
}

double NNkaon::synapse0x87638a8() {
   return (neuron0x8761960()*-0.835152);
}

double NNkaon::synapse0x87638d0() {
   return (neuron0x8761b58()*0.0273714);
}

double NNkaon::synapse0x87638f8() {
   return (neuron0x8761d50()*0.812029);
}

double NNkaon::synapse0x8763920() {
   return (neuron0x8761f48()*2.42174);
}

double NNkaon::synapse0x8763948() {
   return (neuron0x8762140()*-1.02302);
}

double NNkaon::synapse0x8763b68() {
   return (neuron0x851c738()*0.558528);
}

double NNkaon::synapse0x8763b90() {
   return (neuron0x8761768()*4.81037);
}

double NNkaon::synapse0x8763bb8() {
   return (neuron0x8761960()*2.59398);
}

double NNkaon::synapse0x8763be0() {
   return (neuron0x8761b58()*0.471977);
}

double NNkaon::synapse0x8763c08() {
   return (neuron0x8761d50()*-0.39938);
}

double NNkaon::synapse0x8763c30() {
   return (neuron0x8761f48()*0.286096);
}

double NNkaon::synapse0x8763c58() {
   return (neuron0x8762140()*1.37201);
}

double NNkaon::synapse0x8763e78() {
   return (neuron0x851c738()*-0.00204017);
}

double NNkaon::synapse0x8763ea0() {
   return (neuron0x8761768()*-0.423451);
}

double NNkaon::synapse0x8763ec8() {
   return (neuron0x8761960()*-0.852022);
}

double NNkaon::synapse0x8763ef0() {
   return (neuron0x8761b58()*-0.0807841);
}

double NNkaon::synapse0x8763f18() {
   return (neuron0x8761d50()*0.137084);
}

double NNkaon::synapse0x8763f40() {
   return (neuron0x8761f48()*0.243579);
}

double NNkaon::synapse0x8763f68() {
   return (neuron0x8762140()*0.190434);
}

double NNkaon::synapse0x8764210() {
   return (neuron0x851c738()*-1.37401);
}

double NNkaon::synapse0x8764238() {
   return (neuron0x8761768()*0.0764337);
}

double NNkaon::synapse0x851c628() {
   return (neuron0x8761960()*0.405177);
}

double NNkaon::synapse0x855bdb8() {
   return (neuron0x8761b58()*0.17706);
}

double NNkaon::synapse0x855c0d8() {
   return (neuron0x8761d50()*0.343787);
}

double NNkaon::synapse0x855c060() {
   return (neuron0x8761f48()*-0.506748);
}

double NNkaon::synapse0x8762c48() {
   return (neuron0x8762140()*0.210611);
}

double NNkaon::synapse0x8762360() {
   return (neuron0x8762438()*1.1287);
}

double NNkaon::synapse0x8762388() {
   return (neuron0x87626b8()*0.824443);
}

double NNkaon::synapse0x87623b0() {
   return (neuron0x87629a8()*-2.90188);
}

double NNkaon::synapse0x87623d8() {
   return (neuron0x8762d20()*-1.21498);
}

double NNkaon::synapse0x8762400() {
   return (neuron0x8762fc8()*-1.4197);
}

double NNkaon::synapse0x8764468() {
   return (neuron0x8763370()*1.41634);
}

double NNkaon::synapse0x8764490() {
   return (neuron0x8763660()*-1.2913);
}

double NNkaon::synapse0x87644b8() {
   return (neuron0x8763970()*2.81211);
}

double NNkaon::synapse0x87644e0() {
   return (neuron0x8763c80()*0.043959);
}

double NNkaon::synapse0x8764508() {
   return (neuron0x8763f90()*-0.314515);
}

