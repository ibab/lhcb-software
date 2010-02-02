#include "weights/NNkaon.h"
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
         return neuron0x851bcc0();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x851b010() {
   return input0;
}

double NNkaon::neuron0x851b110() {
   return input1;
}

double NNkaon::neuron0x851b308() {
   return input2;
}

double NNkaon::neuron0x851b500() {
   return input3;
}

double NNkaon::neuron0x851b6f8() {
   return input4;
}

double NNkaon::neuron0x851b8f0() {
   return input5;
}

double NNkaon::neuron0x851bae8() {
   return input6;
}

double NNkaon::input0x851bde0() {
   double input = -0.122995;
   input += synapse0x84b4c40();
   input += synapse0x851bf70();
   input += synapse0x851bf98();
   input += synapse0x851bfc0();
   input += synapse0x851bfe8();
   input += synapse0x851c010();
   input += synapse0x851c038();
   return input;
}

double NNkaon::neuron0x851bde0() {
   double input = input0x851bde0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x851c060() {
   double input = -0.0087787;
   input += synapse0x851c238();
   input += synapse0x851c260();
   input += synapse0x851c288();
   input += synapse0x851c2b0();
   input += synapse0x851c2d8();
   input += synapse0x851c300();
   input += synapse0x851c328();
   return input;
}

double NNkaon::neuron0x851c060() {
   double input = input0x851c060();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x851c350() {
   double input = -0.138534;
   input += synapse0x851c528();
   input += synapse0x851c550();
   input += synapse0x851c578();
   input += synapse0x851c628();
   input += synapse0x851c650();
   input += synapse0x851c678();
   input += synapse0x851c6a0();
   return input;
}

double NNkaon::neuron0x851c350() {
   double input = input0x851c350();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x851c6c8() {
   double input = 0.129741;
   input += synapse0x851c858();
   input += synapse0x851c880();
   input += synapse0x851c8a8();
   input += synapse0x851c8d0();
   input += synapse0x851c8f8();
   input += synapse0x851c920();
   input += synapse0x851c948();
   return input;
}

double NNkaon::neuron0x851c6c8() {
   double input = input0x851c6c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x851c970() {
   double input = -0.731651;
   input += synapse0x851cb48();
   input += synapse0x851cb70();
   input += synapse0x851cb98();
   input += synapse0x851cbc0();
   input += synapse0x851cbe8();
   input += synapse0x851c5a0();
   input += synapse0x851c5c8();
   return input;
}

double NNkaon::neuron0x851c970() {
   double input = input0x851c970();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8516090() {
   double input = 0.30804;
   input += synapse0x8516268();
   input += synapse0x8516290();
   input += synapse0x85162b8();
   input += synapse0x85162e0();
   input += synapse0x8516308();
   input += synapse0x8516330();
   input += synapse0x8516358();
   return input;
}

double NNkaon::neuron0x8516090() {
   double input = input0x8516090();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8516380() {
   double input = 0.306475;
   input += synapse0x8516578();
   input += synapse0x85165a0();
   input += synapse0x85165c8();
   input += synapse0x85165f0();
   input += synapse0x8516618();
   input += synapse0x8516640();
   input += synapse0x8516668();
   return input;
}

double NNkaon::neuron0x8516380() {
   double input = input0x8516380();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8516690() {
   double input = 0.823726;
   input += synapse0x8516888();
   input += synapse0x85168b0();
   input += synapse0x85168d8();
   input += synapse0x8516900();
   input += synapse0x8516928();
   input += synapse0x8516950();
   input += synapse0x8516978();
   return input;
}

double NNkaon::neuron0x8516690() {
   double input = input0x8516690();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x85169a0() {
   double input = 0.203251;
   input += synapse0x8516b98();
   input += synapse0x8516bc0();
   input += synapse0x8516be8();
   input += synapse0x8516c10();
   input += synapse0x8516c38();
   input += synapse0x8516c60();
   input += synapse0x8516c88();
   return input;
}

double NNkaon::neuron0x85169a0() {
   double input = input0x85169a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x8516cb0() {
   double input = 0.0865406;
   input += synapse0x8516f30();
   input += synapse0x8516f58();
   input += synapse0x851af10();
   input += synapse0x8521118();
   input += synapse0x85210a0();
   input += synapse0x851c5f0();
   input += synapse0x84f7fb0();
   return input;
}

double NNkaon::neuron0x8516cb0() {
   double input = input0x8516cb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x851bcc0() {
   double input = -0.38296;
   input += synapse0x851cce8();
   input += synapse0x851cd10();
   input += synapse0x851cd38();
   input += synapse0x851cd60();
   input += synapse0x851cd88();
   input += synapse0x851cdb0();
   input += synapse0x8517188();
   input += synapse0x85171b0();
   input += synapse0x85171d8();
   input += synapse0x8517200();
   return input;
}

double NNkaon::neuron0x851bcc0() {
   double input = input0x851bcc0();
   return (input * 0.47868)+0.644449;
}

double NNkaon::synapse0x84b4c40() {
   return (neuron0x851b010()*-0.810234);
}

double NNkaon::synapse0x851bf70() {
   return (neuron0x851b110()*-0.215266);
}

double NNkaon::synapse0x851bf98() {
   return (neuron0x851b308()*0.329197);
}

double NNkaon::synapse0x851bfc0() {
   return (neuron0x851b500()*-0.181059);
}

double NNkaon::synapse0x851bfe8() {
   return (neuron0x851b6f8()*0.083685);
}

double NNkaon::synapse0x851c010() {
   return (neuron0x851b8f0()*-0.792847);
}

double NNkaon::synapse0x851c038() {
   return (neuron0x851bae8()*-0.340305);
}

double NNkaon::synapse0x851c238() {
   return (neuron0x851b010()*0.190838);
}

double NNkaon::synapse0x851c260() {
   return (neuron0x851b110()*-0.552797);
}

double NNkaon::synapse0x851c288() {
   return (neuron0x851b308()*-0.48777);
}

double NNkaon::synapse0x851c2b0() {
   return (neuron0x851b500()*-0.461066);
}

double NNkaon::synapse0x851c2d8() {
   return (neuron0x851b6f8()*0.175181);
}

double NNkaon::synapse0x851c300() {
   return (neuron0x851b8f0()*0.941191);
}

double NNkaon::synapse0x851c328() {
   return (neuron0x851bae8()*0.595794);
}

double NNkaon::synapse0x851c528() {
   return (neuron0x851b010()*-0.857306);
}

double NNkaon::synapse0x851c550() {
   return (neuron0x851b110()*0.755486);
}

double NNkaon::synapse0x851c578() {
   return (neuron0x851b308()*-0.034547);
}

double NNkaon::synapse0x851c628() {
   return (neuron0x851b500()*-0.199637);
}

double NNkaon::synapse0x851c650() {
   return (neuron0x851b6f8()*-0.390167);
}

double NNkaon::synapse0x851c678() {
   return (neuron0x851b8f0()*-1.30269);
}

double NNkaon::synapse0x851c6a0() {
   return (neuron0x851bae8()*-0.925889);
}

double NNkaon::synapse0x851c858() {
   return (neuron0x851b010()*-0.632243);
}

double NNkaon::synapse0x851c880() {
   return (neuron0x851b110()*1.53442);
}

double NNkaon::synapse0x851c8a8() {
   return (neuron0x851b308()*-1.56552);
}

double NNkaon::synapse0x851c8d0() {
   return (neuron0x851b500()*0.0557026);
}

double NNkaon::synapse0x851c8f8() {
   return (neuron0x851b6f8()*0.708615);
}

double NNkaon::synapse0x851c920() {
   return (neuron0x851b8f0()*2.06034);
}

double NNkaon::synapse0x851c948() {
   return (neuron0x851bae8()*1.60539);
}

double NNkaon::synapse0x851cb48() {
   return (neuron0x851b010()*0.770833);
}

double NNkaon::synapse0x851cb70() {
   return (neuron0x851b110()*0.988413);
}

double NNkaon::synapse0x851cb98() {
   return (neuron0x851b308()*-0.920777);
}

double NNkaon::synapse0x851cbc0() {
   return (neuron0x851b500()*0.181976);
}

double NNkaon::synapse0x851cbe8() {
   return (neuron0x851b6f8()*1.19804);
}

double NNkaon::synapse0x851c5a0() {
   return (neuron0x851b8f0()*0.128531);
}

double NNkaon::synapse0x851c5c8() {
   return (neuron0x851bae8()*-1.72806);
}

double NNkaon::synapse0x8516268() {
   return (neuron0x851b010()*0.052051);
}

double NNkaon::synapse0x8516290() {
   return (neuron0x851b110()*-0.105142);
}

double NNkaon::synapse0x85162b8() {
   return (neuron0x851b308()*0.415602);
}

double NNkaon::synapse0x85162e0() {
   return (neuron0x851b500()*-0.523353);
}

double NNkaon::synapse0x8516308() {
   return (neuron0x851b6f8()*0.106349);
}

double NNkaon::synapse0x8516330() {
   return (neuron0x851b8f0()*0.477195);
}

double NNkaon::synapse0x8516358() {
   return (neuron0x851bae8()*0.331983);
}

double NNkaon::synapse0x8516578() {
   return (neuron0x851b010()*-1.06858);
}

double NNkaon::synapse0x85165a0() {
   return (neuron0x851b110()*5.20232);
}

double NNkaon::synapse0x85165c8() {
   return (neuron0x851b308()*-0.942931);
}

double NNkaon::synapse0x85165f0() {
   return (neuron0x851b500()*2.09562);
}

double NNkaon::synapse0x8516618() {
   return (neuron0x851b6f8()*0.801511);
}

double NNkaon::synapse0x8516640() {
   return (neuron0x851b8f0()*0.0654823);
}

double NNkaon::synapse0x8516668() {
   return (neuron0x851bae8()*-0.023755);
}

double NNkaon::synapse0x8516888() {
   return (neuron0x851b010()*1.05386);
}

double NNkaon::synapse0x85168b0() {
   return (neuron0x851b110()*2.04043);
}

double NNkaon::synapse0x85168d8() {
   return (neuron0x851b308()*-0.505239);
}

double NNkaon::synapse0x8516900() {
   return (neuron0x851b500()*0.731035);
}

double NNkaon::synapse0x8516928() {
   return (neuron0x851b6f8()*0.327824);
}

double NNkaon::synapse0x8516950() {
   return (neuron0x851b8f0()*0.866337);
}

double NNkaon::synapse0x8516978() {
   return (neuron0x851bae8()*1.31525);
}

double NNkaon::synapse0x8516b98() {
   return (neuron0x851b010()*0.717816);
}

double NNkaon::synapse0x8516bc0() {
   return (neuron0x851b110()*0.997094);
}

double NNkaon::synapse0x8516be8() {
   return (neuron0x851b308()*0.249383);
}

double NNkaon::synapse0x8516c10() {
   return (neuron0x851b500()*0.719246);
}

double NNkaon::synapse0x8516c38() {
   return (neuron0x851b6f8()*-0.476617);
}

double NNkaon::synapse0x8516c60() {
   return (neuron0x851b8f0()*-1.06807);
}

double NNkaon::synapse0x8516c88() {
   return (neuron0x851bae8()*-0.604151);
}

double NNkaon::synapse0x8516f30() {
   return (neuron0x851b010()*-0.0879671);
}

double NNkaon::synapse0x8516f58() {
   return (neuron0x851b110()*-0.540778);
}

double NNkaon::synapse0x851af10() {
   return (neuron0x851b308()*0.607811);
}

double NNkaon::synapse0x8521118() {
   return (neuron0x851b500()*-0.496658);
}

double NNkaon::synapse0x85210a0() {
   return (neuron0x851b6f8()*-0.187279);
}

double NNkaon::synapse0x851c5f0() {
   return (neuron0x851b8f0()*-0.328679);
}

double NNkaon::synapse0x84f7fb0() {
   return (neuron0x851bae8()*-0.189382);
}

double NNkaon::synapse0x851cce8() {
   return (neuron0x851bde0()*0.116451);
}

double NNkaon::synapse0x851cd10() {
   return (neuron0x851c060()*0.34149);
}

double NNkaon::synapse0x851cd38() {
   return (neuron0x851c350()*-0.516344);
}

double NNkaon::synapse0x851cd60() {
   return (neuron0x851c6c8()*-1.64159);
}

double NNkaon::synapse0x851cd88() {
   return (neuron0x851c970()*-0.617674);
}

double NNkaon::synapse0x851cdb0() {
   return (neuron0x8516090()*1.0225);
}

double NNkaon::synapse0x8517188() {
   return (neuron0x8516380()*2.29837);
}

double NNkaon::synapse0x85171b0() {
   return (neuron0x8516690()*-0.709738);
}

double NNkaon::synapse0x85171d8() {
   return (neuron0x85169a0()*-0.924437);
}

double NNkaon::synapse0x8517200() {
   return (neuron0x8516cb0()*0.458958);
}

