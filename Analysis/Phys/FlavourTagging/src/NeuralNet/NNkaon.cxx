#include "NNkaon.h"
#include <cmath>

double NNkaon::value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x1e32a480();
     default:
         return 0.;
   }
}

double NNkaon::neuron0x1e3261f0() {
   return input0;
}

double NNkaon::neuron0x1e326500() {
   return input1;
}

double NNkaon::neuron0x1e326810() {
   return input2;
}

double NNkaon::neuron0x1e326b20() {
   return input3;
}

double NNkaon::neuron0x1e326e60() {
   return input4;
}

double NNkaon::neuron0x1e3271a0() {
   return input5;
}

double NNkaon::input0x1e327620() {
   double input = 1.82519;
   input += synapse0x1e14e840();
   input += synapse0x1e51c330();
   input += synapse0x1e32ce50();
   input += synapse0x1e14e8d0();
   input += synapse0x1e3278a0();
   input += synapse0x1e3278e0();
   return input;
}

double NNkaon::neuron0x1e327620() {
   double input = input0x1e327620();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e327920() {
   double input = -0.655938;
   input += synapse0x1e327c30();
   input += synapse0x1e327c70();
   input += synapse0x1e327cb0();
   input += synapse0x1e327cf0();
   input += synapse0x1e327d30();
   input += synapse0x1e327d70();
   return input;
}

double NNkaon::neuron0x1e327920() {
   double input = input0x1e327920();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e327db0() {
   double input = -0.157043;
   input += synapse0x1e3280c0();
   input += synapse0x1e328100();
   input += synapse0x1e328140();
   input += synapse0x1e328180();
   input += synapse0x1e3281c0();
   input += synapse0x1e155e70();
   return input;
}

double NNkaon::neuron0x1e327db0() {
   double input = input0x1e327db0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e328310() {
   double input = 0.747192;
   input += synapse0x1e328620();
   input += synapse0x1e328660();
   input += synapse0x1e3286a0();
   input += synapse0x1e3286e0();
   input += synapse0x1e328720();
   input += synapse0x1e328760();
   return input;
}

double NNkaon::neuron0x1e328310() {
   double input = input0x1e328310();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e3287a0() {
   double input = 2.60315;
   input += synapse0x1e328ab0();
   input += synapse0x1e328af0();
   input += synapse0x1e328b30();
   input += synapse0x1e328b70();
   input += synapse0x1e328bb0();
   input += synapse0x1e328bf0();
   return input;
}

double NNkaon::neuron0x1e3287a0() {
   double input = input0x1e3287a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e328c30() {
   double input = -0.19235;
   input += synapse0x1e328f40();
   input += synapse0x1e328f80();
   input += synapse0x1e328fc0();
   input += synapse0x1e155eb0();
   input += synapse0x1e32ce00();
   input += synapse0x1e14e880();
   return input;
}

double NNkaon::neuron0x1e328c30() {
   double input = input0x1e328c30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e329210() {
   double input = 0.197898;
   input += synapse0x1e329520();
   input += synapse0x1e329560();
   input += synapse0x1e3295a0();
   input += synapse0x1e3295e0();
   input += synapse0x1e329620();
   input += synapse0x1e329660();
   return input;
}

double NNkaon::neuron0x1e329210() {
   double input = input0x1e329210();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e3296a0() {
   double input = 0.202597;
   input += synapse0x1e3299b0();
   input += synapse0x1e3299f0();
   input += synapse0x1e329a30();
   input += synapse0x1e329a70();
   input += synapse0x1e329ab0();
   input += synapse0x1e329af0();
   return input;
}

double NNkaon::neuron0x1e3296a0() {
   double input = input0x1e3296a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e329b30() {
   double input = -0.678346;
   input += synapse0x1e329e40();
   input += synapse0x1e329e80();
   input += synapse0x1e329ec0();
   input += synapse0x1e329f00();
   input += synapse0x1e329f40();
   input += synapse0x1e329f80();
   return input;
}

double NNkaon::neuron0x1e329b30() {
   double input = input0x1e329b30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e329fc0() {
   double input = -0.560421;
   input += synapse0x1e32a300();
   input += synapse0x1e32a340();
   input += synapse0x1e32a380();
   input += synapse0x1e32a3c0();
   input += synapse0x1e32a400();
   input += synapse0x1e32a440();
   return input;
}

double NNkaon::neuron0x1e329fc0() {
   double input = input0x1e329fc0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0x1e32a480() {
   double input = -1.03424;
   input += synapse0x1e32a7c0();
   input += synapse0x1e32a800();
   input += synapse0x1e32a840();
   input += synapse0x1e32a880();
   input += synapse0x1e32a8c0();
   input += synapse0x1e502f80();
   input += synapse0x1e14ea30();
   input += synapse0x1e3260c0();
   input += synapse0x1e326100();
   input += synapse0x1e328200();
   return input;
}

double NNkaon::neuron0x1e32a480() {
   double input = input0x1e32a480();
   return (input * 0.479985)+0.640052;
}

double NNkaon::synapse0x1e14e840() {
   return (neuron0x1e3261f0()*-0.225396);
}

double NNkaon::synapse0x1e51c330() {
   return (neuron0x1e326500()*1.99379);
}

double NNkaon::synapse0x1e32ce50() {
   return (neuron0x1e326810()*5.60668);
}

double NNkaon::synapse0x1e14e8d0() {
   return (neuron0x1e326b20()*1.78052);
}

double NNkaon::synapse0x1e3278a0() {
   return (neuron0x1e326e60()*-0.243213);
}

double NNkaon::synapse0x1e3278e0() {
   return (neuron0x1e3271a0()*0.208367);
}

double NNkaon::synapse0x1e327c30() {
   return (neuron0x1e3261f0()*-0.440354);
}

double NNkaon::synapse0x1e327c70() {
   return (neuron0x1e326500()*1.1271);
}

double NNkaon::synapse0x1e327cb0() {
   return (neuron0x1e326810()*0.858771);
}

double NNkaon::synapse0x1e327cf0() {
   return (neuron0x1e326b20()*-0.625828);
}

double NNkaon::synapse0x1e327d30() {
   return (neuron0x1e326e60()*-0.0867354);
}

double NNkaon::synapse0x1e327d70() {
   return (neuron0x1e3271a0()*0.255108);
}

double NNkaon::synapse0x1e3280c0() {
   return (neuron0x1e3261f0()*-0.298409);
}

double NNkaon::synapse0x1e328100() {
   return (neuron0x1e326500()*0.929382);
}

double NNkaon::synapse0x1e328140() {
   return (neuron0x1e326810()*0.0778933);
}

double NNkaon::synapse0x1e328180() {
   return (neuron0x1e326b20()*-0.101309);
}

double NNkaon::synapse0x1e3281c0() {
   return (neuron0x1e326e60()*0.433722);
}

double NNkaon::synapse0x1e155e70() {
   return (neuron0x1e3271a0()*1.02202);
}

double NNkaon::synapse0x1e328620() {
   return (neuron0x1e3261f0()*2.21031);
}

double NNkaon::synapse0x1e328660() {
   return (neuron0x1e326500()*-0.846371);
}

double NNkaon::synapse0x1e3286a0() {
   return (neuron0x1e326810()*0.495635);
}

double NNkaon::synapse0x1e3286e0() {
   return (neuron0x1e326b20()*-0.092394);
}

double NNkaon::synapse0x1e328720() {
   return (neuron0x1e326e60()*-0.466076);
}

double NNkaon::synapse0x1e328760() {
   return (neuron0x1e3271a0()*-1.46863);
}

double NNkaon::synapse0x1e328ab0() {
   return (neuron0x1e3261f0()*-0.000873089);
}

double NNkaon::synapse0x1e328af0() {
   return (neuron0x1e326500()*0.675226);
}

double NNkaon::synapse0x1e328b30() {
   return (neuron0x1e326810()*4.90323);
}

double NNkaon::synapse0x1e328b70() {
   return (neuron0x1e326b20()*-0.310893);
}

double NNkaon::synapse0x1e328bb0() {
   return (neuron0x1e326e60()*0.241856);
}

double NNkaon::synapse0x1e328bf0() {
   return (neuron0x1e3271a0()*0.896616);
}

double NNkaon::synapse0x1e328f40() {
   return (neuron0x1e3261f0()*0.182928);
}

double NNkaon::synapse0x1e328f80() {
   return (neuron0x1e326500()*-1.13403);
}

double NNkaon::synapse0x1e328fc0() {
   return (neuron0x1e326810()*-2.39144);
}

double NNkaon::synapse0x1e155eb0() {
   return (neuron0x1e326b20()*0.348638);
}

double NNkaon::synapse0x1e32ce00() {
   return (neuron0x1e326e60()*0.570762);
}

double NNkaon::synapse0x1e14e880() {
   return (neuron0x1e3271a0()*0.0891261);
}

double NNkaon::synapse0x1e329520() {
   return (neuron0x1e3261f0()*1.06399);
}

double NNkaon::synapse0x1e329560() {
   return (neuron0x1e326500()*-0.399258);
}

double NNkaon::synapse0x1e3295a0() {
   return (neuron0x1e326810()*-0.872589);
}

double NNkaon::synapse0x1e3295e0() {
   return (neuron0x1e326b20()*-0.0720948);
}

double NNkaon::synapse0x1e329620() {
   return (neuron0x1e326e60()*0.331009);
}

double NNkaon::synapse0x1e329660() {
   return (neuron0x1e3271a0()*-0.0532707);
}

double NNkaon::synapse0x1e3299b0() {
   return (neuron0x1e3261f0()*0.743857);
}

double NNkaon::synapse0x1e3299f0() {
   return (neuron0x1e326500()*0.542699);
}

double NNkaon::synapse0x1e329a30() {
   return (neuron0x1e326810()*0.397304);
}

double NNkaon::synapse0x1e329a70() {
   return (neuron0x1e326b20()*0.0270061);
}

double NNkaon::synapse0x1e329ab0() {
   return (neuron0x1e326e60()*-0.613127);
}

double NNkaon::synapse0x1e329af0() {
   return (neuron0x1e3271a0()*1.11719);
}

double NNkaon::synapse0x1e329e40() {
   return (neuron0x1e3261f0()*-0.56494);
}

double NNkaon::synapse0x1e329e80() {
   return (neuron0x1e326500()*-0.686625);
}

double NNkaon::synapse0x1e329ec0() {
   return (neuron0x1e326810()*-1.61698);
}

double NNkaon::synapse0x1e329f00() {
   return (neuron0x1e326b20()*0.860048);
}

double NNkaon::synapse0x1e329f40() {
   return (neuron0x1e326e60()*-0.0467377);
}

double NNkaon::synapse0x1e329f80() {
   return (neuron0x1e3271a0()*-0.106539);
}

double NNkaon::synapse0x1e32a300() {
   return (neuron0x1e3261f0()*-0.831592);
}

double NNkaon::synapse0x1e32a340() {
   return (neuron0x1e326500()*0.0217942);
}

double NNkaon::synapse0x1e32a380() {
   return (neuron0x1e326810()*0.451275);
}

double NNkaon::synapse0x1e32a3c0() {
   return (neuron0x1e326b20()*-0.201085);
}

double NNkaon::synapse0x1e32a400() {
   return (neuron0x1e326e60()*-0.672884);
}

double NNkaon::synapse0x1e32a440() {
   return (neuron0x1e3271a0()*-0.30332);
}

double NNkaon::synapse0x1e32a7c0() {
   return (neuron0x1e327620()*2.13739);
}

double NNkaon::synapse0x1e32a800() {
   return (neuron0x1e327920()*-1.31067);
}

double NNkaon::synapse0x1e32a840() {
   return (neuron0x1e327db0()*-1.15915);
}

double NNkaon::synapse0x1e32a880() {
   return (neuron0x1e328310()*-1.47059);
}

double NNkaon::synapse0x1e32a8c0() {
   return (neuron0x1e3287a0()*2.32199);
}

double NNkaon::synapse0x1e502f80() {
   return (neuron0x1e328c30()*-2.90414);
}

double NNkaon::synapse0x1e14ea30() {
   return (neuron0x1e329210()*0.549597);
}

double NNkaon::synapse0x1e3260c0() {
   return (neuron0x1e3296a0()*-0.746186);
}

double NNkaon::synapse0x1e326100() {
   return (neuron0x1e329b30()*-0.55463);
}

double NNkaon::synapse0x1e328200() {
   return (neuron0x1e329fc0()*-0.223674);
}

