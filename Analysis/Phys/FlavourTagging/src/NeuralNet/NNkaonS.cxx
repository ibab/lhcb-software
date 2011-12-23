#include "NNkaonS.h"
#include <cmath>

double NNkaonS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0xf0bd1d0();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x10145310() {
   return input0;
}

double NNkaonS::neuron0xf57e730() {
   return input1;
}

double NNkaonS::neuron0xf57e890() {
   return input2;
}

double NNkaonS::neuron0xf57e9f0() {
   return input3;
}

double NNkaonS::neuron0xf57eb50() {
   return input4;
}

double NNkaonS::neuron0xfcb3270() {
   return input5;
}

double NNkaonS::neuron0xfcb3400() {
   return input6;
}

double NNkaonS::input0xfcb36d0() {
   double input = 0.277074;
   input += synapse0x1041fb80();
   input += synapse0xf2ed110();
   input += synapse0xf2ed240();
   input += synapse0xf2ed340();
   input += synapse0xf2ed6e0();
   input += synapse0xf2ed7e0();
   input += synapse0xf2ed930();
   return input;
}

double NNkaonS::neuron0xfcb36d0() {
   double input = input0xfcb36d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xfc34c70() {
   double input = -0.159111;
   input += synapse0xfc34dd0();
   input += synapse0xfc34e10();
   input += synapse0xfc34e50();
   input += synapse0xfc34e90();
   input += synapse0xfc34ed0();
   input += synapse0xfc34f10();
   input += synapse0xfc34f50();
   return input;
}

double NNkaonS::neuron0xfc34c70() {
   double input = input0xfc34c70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xfc34f90() {
   double input = 1.79278;
   input += synapse0xfc350f0();
   input += synapse0xfc35130();
   input += synapse0xfc35170();
   input += synapse0xf2eda30();
   input += synapse0xf2edb30();
   input += synapse0xf2eddd0();
   input += synapse0xf2eded0();
   return input;
}

double NNkaonS::neuron0xfc34f90() {
   double input = input0xfc34f90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x10593130() {
   double input = 0.183573;
   input += synapse0x10593290();
   input += synapse0x105932d0();
   input += synapse0x10593310();
   input += synapse0x10593350();
   input += synapse0x10593390();
   input += synapse0x105933d0();
   input += synapse0x10593410();
   return input;
}

double NNkaonS::neuron0x10593130() {
   double input = input0x10593130();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x10593450() {
   double input = 0.0501519;
   input += synapse0x105935b0();
   input += synapse0x105935f0();
   input += synapse0x10593630();
   input += synapse0x10593670();
   input += synapse0x105936b0();
   input += synapse0xf2ee120();
   input += synapse0xf2ee230();
   return input;
}

double NNkaonS::neuron0x10593450() {
   double input = input0x10593450();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1004ac10() {
   double input = -0.580411;
   input += synapse0x1004ad70();
   input += synapse0x1004adb0();
   input += synapse0x1004adf0();
   input += synapse0x1004ae30();
   input += synapse0x1004ae70();
   input += synapse0x1004aeb0();
   input += synapse0x1004aef0();
   return input;
}

double NNkaonS::neuron0x1004ac10() {
   double input = input0x1004ac10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1004af30() {
   double input = -0.0756966;
   input += synapse0x1004b090();
   input += synapse0x1004b0d0();
   input += synapse0x1004b110();
   input += synapse0xf2ee330();
   input += synapse0xf2ee430();
   input += synapse0xf2ee530();
   input += synapse0xf2ee640();
   return input;
}

double NNkaonS::neuron0x1004af30() {
   double input = input0x1004af30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xf0bc620() {
   double input = -0.7571;
   input += synapse0xf0bc780();
   input += synapse0xf0bc7c0();
   input += synapse0xf0bc800();
   input += synapse0xf0bc840();
   input += synapse0xf0bc880();
   input += synapse0xf0bc8c0();
   input += synapse0xf0bc900();
   return input;
}

double NNkaonS::neuron0xf0bc620() {
   double input = input0xf0bc620();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xf0bc940() {
   double input = 0.658238;
   input += synapse0xf0bcaa0();
   input += synapse0xf0bcae0();
   input += synapse0xf0bcb20();
   input += synapse0xf0bcb60();
   input += synapse0xf0bcba0();
   input += synapse0xf0bcbe0();
   input += synapse0xf0bcc20();
   return input;
}

double NNkaonS::neuron0xf0bc940() {
   double input = input0xf0bc940();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xf0bcc60() {
   double input = -0.467523;
   input += synapse0xf2ee8e0();
   input += synapse0xf2eea30();
   input += synapse0xf2eeb80();
   input += synapse0xf2eecd0();
   input += synapse0xf2eedd0();
   input += synapse0xf2eeed0();
   input += synapse0xf2eefd0();
   return input;
}

double NNkaonS::neuron0xf0bcc60() {
   double input = input0xf0bcc60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x1004aa00() {
   double input = -0.0147563;
   input += synapse0x1004ab60();
   input += synapse0x1004aba0();
   input += synapse0xf2ef620();
   input += synapse0xf2efc10();
   input += synapse0xf2efe80();
   input += synapse0xf2effd0();
   input += synapse0xf2f00e0();
   return input;
}

double NNkaonS::neuron0x1004aa00() {
   double input = input0x1004aa00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0xf0bd1d0() {
   double input = -0.577358;
   input += synapse0xf0bd330();
   input += synapse0xf0bd370();
   input += synapse0xf0bd3b0();
   input += synapse0xf0bd3f0();
   input += synapse0xf0bd430();
   input += synapse0xf0bd470();
   input += synapse0xf0bd4b0();
   input += synapse0xf0bd4f0();
   input += synapse0xf0bd530();
   input += synapse0xf0bd570();
   input += synapse0xf0bd5b0();
   return input;
}

double NNkaonS::neuron0xf0bd1d0() {
   double input = input0xf0bd1d0();
   return (input * 0.463721)+0.686983;
}

double NNkaonS::synapse0x1041fb80() {
   return (neuron0x10145310()*0.0124388);
}

double NNkaonS::synapse0xf2ed110() {
   return (neuron0xf57e730()*0.985613);
}

double NNkaonS::synapse0xf2ed240() {
   return (neuron0xf57e890()*0.244027);
}

double NNkaonS::synapse0xf2ed340() {
   return (neuron0xf57e9f0()*0.274275);
}

double NNkaonS::synapse0xf2ed6e0() {
   return (neuron0xf57eb50()*-0.391943);
}

double NNkaonS::synapse0xf2ed7e0() {
   return (neuron0xfcb3270()*0.859832);
}

double NNkaonS::synapse0xf2ed930() {
   return (neuron0xfcb3400()*-1.66377);
}

double NNkaonS::synapse0xfc34dd0() {
   return (neuron0x10145310()*-0.457046);
}

double NNkaonS::synapse0xfc34e10() {
   return (neuron0xf57e730()*0.601204);
}

double NNkaonS::synapse0xfc34e50() {
   return (neuron0xf57e890()*0.313173);
}

double NNkaonS::synapse0xfc34e90() {
   return (neuron0xf57e9f0()*0.315787);
}

double NNkaonS::synapse0xfc34ed0() {
   return (neuron0xf57eb50()*-0.98873);
}

double NNkaonS::synapse0xfc34f10() {
   return (neuron0xfcb3270()*1.79456);
}

double NNkaonS::synapse0xfc34f50() {
   return (neuron0xfcb3400()*-1.18368);
}

double NNkaonS::synapse0xfc350f0() {
   return (neuron0x10145310()*1.10743);
}

double NNkaonS::synapse0xfc35130() {
   return (neuron0xf57e730()*-0.370916);
}

double NNkaonS::synapse0xfc35170() {
   return (neuron0xf57e890()*-0.331279);
}

double NNkaonS::synapse0xf2eda30() {
   return (neuron0xf57e9f0()*0.159571);
}

double NNkaonS::synapse0xf2edb30() {
   return (neuron0xf57eb50()*1.04427);
}

double NNkaonS::synapse0xf2eddd0() {
   return (neuron0xfcb3270()*-0.453378);
}

double NNkaonS::synapse0xf2eded0() {
   return (neuron0xfcb3400()*3.74195);
}

double NNkaonS::synapse0x10593290() {
   return (neuron0x10145310()*0.841833);
}

double NNkaonS::synapse0x105932d0() {
   return (neuron0xf57e730()*-0.25517);
}

double NNkaonS::synapse0x10593310() {
   return (neuron0xf57e890()*-0.334634);
}

double NNkaonS::synapse0x10593350() {
   return (neuron0xf57e9f0()*-0.00243266);
}

double NNkaonS::synapse0x10593390() {
   return (neuron0xf57eb50()*0.529943);
}

double NNkaonS::synapse0x105933d0() {
   return (neuron0xfcb3270()*-0.317081);
}

double NNkaonS::synapse0x10593410() {
   return (neuron0xfcb3400()*-0.289002);
}

double NNkaonS::synapse0x105935b0() {
   return (neuron0x10145310()*0.3311);
}

double NNkaonS::synapse0x105935f0() {
   return (neuron0xf57e730()*0.683904);
}

double NNkaonS::synapse0x10593630() {
   return (neuron0xf57e890()*0.0236215);
}

double NNkaonS::synapse0x10593670() {
   return (neuron0xf57e9f0()*0.0481938);
}

double NNkaonS::synapse0x105936b0() {
   return (neuron0xf57eb50()*-0.284219);
}

double NNkaonS::synapse0xf2ee120() {
   return (neuron0xfcb3270()*0.097452);
}

double NNkaonS::synapse0xf2ee230() {
   return (neuron0xfcb3400()*-2.18582);
}

double NNkaonS::synapse0x1004ad70() {
   return (neuron0x10145310()*-1.04923);
}

double NNkaonS::synapse0x1004adb0() {
   return (neuron0xf57e730()*0.610906);
}

double NNkaonS::synapse0x1004adf0() {
   return (neuron0xf57e890()*-0.0645154);
}

double NNkaonS::synapse0x1004ae30() {
   return (neuron0xf57e9f0()*-0.249972);
}

double NNkaonS::synapse0x1004ae70() {
   return (neuron0xf57eb50()*-0.607583);
}

double NNkaonS::synapse0x1004aeb0() {
   return (neuron0xfcb3270()*0.131681);
}

double NNkaonS::synapse0x1004aef0() {
   return (neuron0xfcb3400()*-2.2865);
}

double NNkaonS::synapse0x1004b090() {
   return (neuron0x10145310()*0.618348);
}

double NNkaonS::synapse0x1004b0d0() {
   return (neuron0xf57e730()*-0.13669);
}

double NNkaonS::synapse0x1004b110() {
   return (neuron0xf57e890()*0.0279487);
}

double NNkaonS::synapse0xf2ee330() {
   return (neuron0xf57e9f0()*0.310473);
}

double NNkaonS::synapse0xf2ee430() {
   return (neuron0xf57eb50()*1.03699);
}

double NNkaonS::synapse0xf2ee530() {
   return (neuron0xfcb3270()*-0.339658);
}

double NNkaonS::synapse0xf2ee640() {
   return (neuron0xfcb3400()*1.42422);
}

double NNkaonS::synapse0xf0bc780() {
   return (neuron0x10145310()*-0.128733);
}

double NNkaonS::synapse0xf0bc7c0() {
   return (neuron0xf57e730()*0.209903);
}

double NNkaonS::synapse0xf0bc800() {
   return (neuron0xf57e890()*1.08024);
}

double NNkaonS::synapse0xf0bc840() {
   return (neuron0xf57e9f0()*-0.327653);
}

double NNkaonS::synapse0xf0bc880() {
   return (neuron0xf57eb50()*0.253427);
}

double NNkaonS::synapse0xf0bc8c0() {
   return (neuron0xfcb3270()*-0.737827);
}

double NNkaonS::synapse0xf0bc900() {
   return (neuron0xfcb3400()*-1.83734);
}

double NNkaonS::synapse0xf0bcaa0() {
   return (neuron0x10145310()*-0.0922683);
}

double NNkaonS::synapse0xf0bcae0() {
   return (neuron0xf57e730()*-0.740499);
}

double NNkaonS::synapse0xf0bcb20() {
   return (neuron0xf57e890()*0.252976);
}

double NNkaonS::synapse0xf0bcb60() {
   return (neuron0xf57e9f0()*-0.383462);
}

double NNkaonS::synapse0xf0bcba0() {
   return (neuron0xf57eb50()*0.124421);
}

double NNkaonS::synapse0xf0bcbe0() {
   return (neuron0xfcb3270()*-0.441509);
}

double NNkaonS::synapse0xf0bcc20() {
   return (neuron0xfcb3400()*3.08449);
}

double NNkaonS::synapse0xf2ee8e0() {
   return (neuron0x10145310()*-1.22181);
}

double NNkaonS::synapse0xf2eea30() {
   return (neuron0xf57e730()*1.04588);
}

double NNkaonS::synapse0xf2eeb80() {
   return (neuron0xf57e890()*-0.742718);
}

double NNkaonS::synapse0xf2eecd0() {
   return (neuron0xf57e9f0()*-0.0986137);
}

double NNkaonS::synapse0xf2eedd0() {
   return (neuron0xf57eb50()*-0.172168);
}

double NNkaonS::synapse0xf2eeed0() {
   return (neuron0xfcb3270()*0.720838);
}

double NNkaonS::synapse0xf2eefd0() {
   return (neuron0xfcb3400()*-2.76803);
}

double NNkaonS::synapse0x1004ab60() {
   return (neuron0x10145310()*-0.244185);
}

double NNkaonS::synapse0x1004aba0() {
   return (neuron0xf57e730()*-0.405756);
}

double NNkaonS::synapse0xf2ef620() {
   return (neuron0xf57e890()*0.0354638);
}

double NNkaonS::synapse0xf2efc10() {
   return (neuron0xf57e9f0()*-0.110475);
}

double NNkaonS::synapse0xf2efe80() {
   return (neuron0xf57eb50()*0.293461);
}

double NNkaonS::synapse0xf2effd0() {
   return (neuron0xfcb3270()*-0.431071);
}

double NNkaonS::synapse0xf2f00e0() {
   return (neuron0xfcb3400()*1.66595);
}

double NNkaonS::synapse0xf0bd330() {
   return (neuron0xfcb36d0()*0.372919);
}

double NNkaonS::synapse0xf0bd370() {
   return (neuron0xfc34c70()*-0.091833);
}

double NNkaonS::synapse0xf0bd3b0() {
   return (neuron0xfc34f90()*-2.30291);
}

double NNkaonS::synapse0xf0bd3f0() {
   return (neuron0x10593130()*1.20463);
}

double NNkaonS::synapse0xf0bd430() {
   return (neuron0x10593450()*1.87129);
}

double NNkaonS::synapse0xf0bd470() {
   return (neuron0x1004ac10()*1.19877);
}

double NNkaonS::synapse0xf0bd4b0() {
   return (neuron0x1004af30()*1.25548);
}

double NNkaonS::synapse0xf0bd4f0() {
   return (neuron0xf0bc620()*2.16668);
}

double NNkaonS::synapse0xf0bd530() {
   return (neuron0xf0bc940()*-2.25004);
}

double NNkaonS::synapse0xf0bd570() {
   return (neuron0xf0bcc60()*1.88754);
}

double NNkaonS::synapse0xf0bd5b0() {
   return (neuron0x1004aa00()*-0.928687);
}

