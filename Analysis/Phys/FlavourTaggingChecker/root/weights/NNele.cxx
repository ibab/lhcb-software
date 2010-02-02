#include "weights/NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x85191d8();
     default:
         return 0.;
   }
}

double NNele::neuron0x8514560() {
   return input0;
}

double NNele::neuron0x85146a8() {
   return input1;
}

double NNele::neuron0x85178f0() {
   return input2;
}

double NNele::neuron0x8517ac8() {
   return input3;
}

double NNele::neuron0x8517cc0() {
   return input4;
}

double NNele::neuron0x8517eb8() {
   return input5;
}

double NNele::neuron0x85180b0() {
   return input6;
}

double NNele::input0x85183a8() {
   double input = 0.624831;
   input += synapse0x86f3e30();
   input += synapse0x8518538();
   input += synapse0x8518560();
   input += synapse0x8518588();
   input += synapse0x85185b0();
   input += synapse0x85185d8();
   input += synapse0x8518600();
   return input;
}

double NNele::neuron0x85183a8() {
   double input = input0x85183a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8518628() {
   double input = -0.277259;
   input += synapse0x8518800();
   input += synapse0x8518828();
   input += synapse0x8518850();
   input += synapse0x8518878();
   input += synapse0x85188a0();
   input += synapse0x85188c8();
   input += synapse0x85188f0();
   return input;
}

double NNele::neuron0x8518628() {
   double input = input0x8518628();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8518918() {
   double input = -1.3558;
   input += synapse0x8518af0();
   input += synapse0x8518b18();
   input += synapse0x8518b40();
   input += synapse0x8518bf0();
   input += synapse0x8518c18();
   input += synapse0x8518c40();
   input += synapse0x8518c68();
   return input;
}

double NNele::neuron0x8518918() {
   double input = input0x8518918();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8518c90() {
   double input = 1.74911;
   input += synapse0x8518e20();
   input += synapse0x8518e48();
   input += synapse0x8518e70();
   input += synapse0x8518e98();
   input += synapse0x8518ec0();
   input += synapse0x8518ee8();
   input += synapse0x8518f10();
   return input;
}

double NNele::neuron0x8518c90() {
   double input = input0x8518c90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8518f38() {
   double input = 0.185381;
   input += synapse0x8519110();
   input += synapse0x8519138();
   input += synapse0x8519160();
   input += synapse0x8519188();
   input += synapse0x85191b0();
   input += synapse0x8518b68();
   input += synapse0x8518b90();
   return input;
}

double NNele::neuron0x8518f38() {
   double input = input0x8518f38();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x85192e0() {
   double input = 0.0951069;
   input += synapse0x85194b8();
   input += synapse0x85194e0();
   input += synapse0x8519508();
   input += synapse0x8519530();
   input += synapse0x8519558();
   input += synapse0x8519580();
   input += synapse0x85195a8();
   return input;
}

double NNele::neuron0x85192e0() {
   double input = input0x85192e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x85195d0() {
   double input = -0.161379;
   input += synapse0x85197c8();
   input += synapse0x85197f0();
   input += synapse0x8519818();
   input += synapse0x8519840();
   input += synapse0x8519868();
   input += synapse0x8519890();
   input += synapse0x85198b8();
   return input;
}

double NNele::neuron0x85195d0() {
   double input = input0x85195d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x85198e0() {
   double input = -0.299441;
   input += synapse0x8519ad8();
   input += synapse0x8519b00();
   input += synapse0x8519b28();
   input += synapse0x8519b50();
   input += synapse0x8519b78();
   input += synapse0x8519ba0();
   input += synapse0x8519bc8();
   return input;
}

double NNele::neuron0x85198e0() {
   double input = input0x85198e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8519bf0() {
   double input = 0.880976;
   input += synapse0x8519de8();
   input += synapse0x8519e10();
   input += synapse0x8519e38();
   input += synapse0x8519e60();
   input += synapse0x8519e88();
   input += synapse0x8519eb0();
   input += synapse0x8519ed8();
   return input;
}

double NNele::neuron0x8519bf0() {
   double input = input0x8519bf0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8519f00() {
   double input = -0.644711;
   input += synapse0x851a180();
   input += synapse0x851a1a8();
   input += synapse0x8514440();
   input += synapse0x854d8c0();
   input += synapse0x854d848();
   input += synapse0x8518bb8();
   input += synapse0x86e03c8();
   return input;
}

double NNele::neuron0x8519f00() {
   double input = input0x8519f00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x85191d8() {
   double input = -0.528284;
   input += synapse0x8518360();
   input += synapse0x854d5c8();
   input += synapse0x851a3d8();
   input += synapse0x851a400();
   input += synapse0x851a428();
   input += synapse0x851a450();
   input += synapse0x851a478();
   input += synapse0x851a4a0();
   input += synapse0x851a4c8();
   input += synapse0x851a4f0();
   return input;
}

double NNele::neuron0x85191d8() {
   double input = input0x85191d8();
   return (input * 0.465818)+0.681696;
}

double NNele::synapse0x86f3e30() {
   return (neuron0x8514560()*0.680716);
}

double NNele::synapse0x8518538() {
   return (neuron0x85146a8()*-0.124618);
}

double NNele::synapse0x8518560() {
   return (neuron0x85178f0()*-1.30705);
}

double NNele::synapse0x8518588() {
   return (neuron0x8517ac8()*-1.02968);
}

double NNele::synapse0x85185b0() {
   return (neuron0x8517cc0()*0.235394);
}

double NNele::synapse0x85185d8() {
   return (neuron0x8517eb8()*-0.307954);
}

double NNele::synapse0x8518600() {
   return (neuron0x85180b0()*-0.524506);
}

double NNele::synapse0x8518800() {
   return (neuron0x8514560()*-0.0404134);
}

double NNele::synapse0x8518828() {
   return (neuron0x85146a8()*0.105494);
}

double NNele::synapse0x8518850() {
   return (neuron0x85178f0()*0.151531);
}

double NNele::synapse0x8518878() {
   return (neuron0x8517ac8()*-0.0661466);
}

double NNele::synapse0x85188a0() {
   return (neuron0x8517cc0()*-0.327589);
}

double NNele::synapse0x85188c8() {
   return (neuron0x8517eb8()*-0.418901);
}

double NNele::synapse0x85188f0() {
   return (neuron0x85180b0()*-0.245236);
}

double NNele::synapse0x8518af0() {
   return (neuron0x8514560()*-2.31611);
}

double NNele::synapse0x8518b18() {
   return (neuron0x85146a8()*-1.24078);
}

double NNele::synapse0x8518b40() {
   return (neuron0x85178f0()*-0.531939);
}

double NNele::synapse0x8518bf0() {
   return (neuron0x8517ac8()*0.99306);
}

double NNele::synapse0x8518c18() {
   return (neuron0x8517cc0()*1.11003);
}

double NNele::synapse0x8518c40() {
   return (neuron0x8517eb8()*0.683656);
}

double NNele::synapse0x8518c68() {
   return (neuron0x85180b0()*1.98656);
}

double NNele::synapse0x8518e20() {
   return (neuron0x8514560()*-1.50222);
}

double NNele::synapse0x8518e48() {
   return (neuron0x85146a8()*-0.669025);
}

double NNele::synapse0x8518e70() {
   return (neuron0x85178f0()*-5.24669);
}

double NNele::synapse0x8518e98() {
   return (neuron0x8517ac8()*-0.763734);
}

double NNele::synapse0x8518ec0() {
   return (neuron0x8517cc0()*0.0232587);
}

double NNele::synapse0x8518ee8() {
   return (neuron0x8517eb8()*0.40971);
}

double NNele::synapse0x8518f10() {
   return (neuron0x85180b0()*-0.854707);
}

double NNele::synapse0x8519110() {
   return (neuron0x8514560()*-0.111961);
}

double NNele::synapse0x8519138() {
   return (neuron0x85146a8()*-0.0509377);
}

double NNele::synapse0x8519160() {
   return (neuron0x85178f0()*0.930369);
}

double NNele::synapse0x8519188() {
   return (neuron0x8517ac8()*0.417453);
}

double NNele::synapse0x85191b0() {
   return (neuron0x8517cc0()*0.194655);
}

double NNele::synapse0x8518b68() {
   return (neuron0x8517eb8()*-0.0451774);
}

double NNele::synapse0x8518b90() {
   return (neuron0x85180b0()*0.174152);
}

double NNele::synapse0x85194b8() {
   return (neuron0x8514560()*2.99965);
}

double NNele::synapse0x85194e0() {
   return (neuron0x85146a8()*-0.397251);
}

double NNele::synapse0x8519508() {
   return (neuron0x85178f0()*-0.226715);
}

double NNele::synapse0x8519530() {
   return (neuron0x8517ac8()*0.597281);
}

double NNele::synapse0x8519558() {
   return (neuron0x8517cc0()*0.58977);
}

double NNele::synapse0x8519580() {
   return (neuron0x8517eb8()*1.93629);
}

double NNele::synapse0x85195a8() {
   return (neuron0x85180b0()*-0.140106);
}

double NNele::synapse0x85197c8() {
   return (neuron0x8514560()*0.0633467);
}

double NNele::synapse0x85197f0() {
   return (neuron0x85146a8()*-0.688023);
}

double NNele::synapse0x8519818() {
   return (neuron0x85178f0()*-0.927926);
}

double NNele::synapse0x8519840() {
   return (neuron0x8517ac8()*-0.498948);
}

double NNele::synapse0x8519868() {
   return (neuron0x8517cc0()*0.21924);
}

double NNele::synapse0x8519890() {
   return (neuron0x8517eb8()*-0.0251014);
}

double NNele::synapse0x85198b8() {
   return (neuron0x85180b0()*-0.359994);
}

double NNele::synapse0x8519ad8() {
   return (neuron0x8514560()*-0.525953);
}

double NNele::synapse0x8519b00() {
   return (neuron0x85146a8()*-0.630635);
}

double NNele::synapse0x8519b28() {
   return (neuron0x85178f0()*1.60218);
}

double NNele::synapse0x8519b50() {
   return (neuron0x8517ac8()*0.0885576);
}

double NNele::synapse0x8519b78() {
   return (neuron0x8517cc0()*0.841201);
}

double NNele::synapse0x8519ba0() {
   return (neuron0x8517eb8()*-0.0473756);
}

double NNele::synapse0x8519bc8() {
   return (neuron0x85180b0()*-0.322841);
}

double NNele::synapse0x8519de8() {
   return (neuron0x8514560()*1.28919);
}

double NNele::synapse0x8519e10() {
   return (neuron0x85146a8()*0.403273);
}

double NNele::synapse0x8519e38() {
   return (neuron0x85178f0()*0.587519);
}

double NNele::synapse0x8519e60() {
   return (neuron0x8517ac8()*-1.39241);
}

double NNele::synapse0x8519e88() {
   return (neuron0x8517cc0()*0.805233);
}

double NNele::synapse0x8519eb0() {
   return (neuron0x8517eb8()*-0.0845308);
}

double NNele::synapse0x8519ed8() {
   return (neuron0x85180b0()*2.05773);
}

double NNele::synapse0x851a180() {
   return (neuron0x8514560()*-1.77172);
}

double NNele::synapse0x851a1a8() {
   return (neuron0x85146a8()*-0.728756);
}

double NNele::synapse0x8514440() {
   return (neuron0x85178f0()*3.18334);
}

double NNele::synapse0x854d8c0() {
   return (neuron0x8517ac8()*2.94919);
}

double NNele::synapse0x854d848() {
   return (neuron0x8517cc0()*1.93443);
}

double NNele::synapse0x8518bb8() {
   return (neuron0x8517eb8()*-0.261813);
}

double NNele::synapse0x86e03c8() {
   return (neuron0x85180b0()*-0.108501);
}

double NNele::synapse0x8518360() {
   return (neuron0x85183a8()*1.2048);
}

double NNele::synapse0x854d5c8() {
   return (neuron0x8518628()*-0.174051);
}

double NNele::synapse0x851a3d8() {
   return (neuron0x8518918()*-1.28283);
}

double NNele::synapse0x851a400() {
   return (neuron0x8518c90()*-3.4355);
}

double NNele::synapse0x851a428() {
   return (neuron0x8518f38()*-0.807277);
}

double NNele::synapse0x851a450() {
   return (neuron0x85192e0()*-1.7025);
}

double NNele::synapse0x851a478() {
   return (neuron0x85195d0()*1.17805);
}

double NNele::synapse0x851a4a0() {
   return (neuron0x85198e0()*-1.30439);
}

double NNele::synapse0x851a4c8() {
   return (neuron0x8519bf0()*1.32813);
}

double NNele::synapse0x851a4f0() {
   return (neuron0x8519f00()*2.16503);
}

