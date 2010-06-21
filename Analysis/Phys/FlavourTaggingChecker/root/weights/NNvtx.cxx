#include "weights/NNvtx.h"
#include <cmath>

double NNvtx::Value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   input7 = (in7 - 0)/1;
   input8 = (in8 - 0)/1;
   input9 = (in9 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x817f2a0();
     default:
         return 0.;
   }
}

double NNvtx::Value(int index, double* input) {
   input0 = (input[0] - 0)/1;
   input1 = (input[1] - 0)/1;
   input2 = (input[2] - 0)/1;
   input3 = (input[3] - 0)/1;
   input4 = (input[4] - 0)/1;
   input5 = (input[5] - 0)/1;
   input6 = (input[6] - 0)/1;
   input7 = (input[7] - 0)/1;
   input8 = (input[8] - 0)/1;
   input9 = (input[9] - 0)/1;
   switch(index) {
     case 0:
         return neuron0x817f2a0();
     default:
         return 0.;
   }
}

double NNvtx::neuron0x8179870() {
   return input0;
}

double NNvtx::neuron0x8179b80() {
   return input1;
}

double NNvtx::neuron0x8179e90() {
   return input2;
}

double NNvtx::neuron0x817a1a0() {
   return input3;
}

double NNvtx::neuron0x817a4b0() {
   return input4;
}

double NNvtx::neuron0x817a7c0() {
   return input5;
}

double NNvtx::neuron0x817aad0() {
   return input6;
}

double NNvtx::neuron0x817ae10() {
   return input7;
}

double NNvtx::neuron0x817b150() {
   return input8;
}

double NNvtx::neuron0x817b490() {
   return input9;
}

double NNvtx::input0x817b910() {
   double input = 0.107389;
   input += synapse0x80b9e50();
   input += synapse0x817bb90();
   input += synapse0x817bbd0();
   input += synapse0x817bc10();
   input += synapse0x817bc50();
   input += synapse0x817bc90();
   input += synapse0x817bcd0();
   input += synapse0x817bd10();
   input += synapse0x817bd50();
   input += synapse0x817bd90();
   return input;
}

double NNvtx::neuron0x817b910() {
   double input = input0x817b910();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817bdd0() {
   double input = -0.264348;
   input += synapse0x817c0e0();
   input += synapse0x817c120();
   input += synapse0x817c160();
   input += synapse0x817c1a0();
   input += synapse0x817c1e0();
   input += synapse0x817c220();
   input += synapse0x817c260();
   input += synapse0x803e2f0();
   input += synapse0x803e330();
   input += synapse0x817c3b0();
   return input;
}

double NNvtx::neuron0x817bdd0() {
   double input = input0x817bdd0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817c3f0() {
   double input = 0.236761;
   input += synapse0x817c700();
   input += synapse0x817c740();
   input += synapse0x817c780();
   input += synapse0x817c7c0();
   input += synapse0x817c800();
   input += synapse0x817c840();
   input += synapse0x817c880();
   input += synapse0x817c8c0();
   input += synapse0x817c900();
   input += synapse0x817c940();
   return input;
}

double NNvtx::neuron0x817c3f0() {
   double input = input0x817c3f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817c980() {
   double input = -0.652667;
   input += synapse0x817cc90();
   input += synapse0x817ccd0();
   input += synapse0x817cd10();
   input += synapse0x8169b30();
   input += synapse0x80b9e90();
   input += synapse0x80b9980();
   input += synapse0x817c2a0();
   input += synapse0x817c2e0();
   input += synapse0x817c320();
   input += synapse0x817c360();
   return input;
}

double NNvtx::neuron0x817c980() {
   double input = input0x817c980();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817cd50() {
   double input = -0.436215;
   input += synapse0x817d060();
   input += synapse0x817d0a0();
   input += synapse0x817d0e0();
   input += synapse0x817d120();
   input += synapse0x817d160();
   input += synapse0x817d1a0();
   input += synapse0x817d1e0();
   input += synapse0x817d220();
   input += synapse0x817d260();
   input += synapse0x817d2a0();
   return input;
}

double NNvtx::neuron0x817cd50() {
   double input = input0x817cd50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817d2e0() {
   double input = -0.204165;
   input += synapse0x817d5f0();
   input += synapse0x817d630();
   input += synapse0x817d670();
   input += synapse0x817d6b0();
   input += synapse0x817d6f0();
   input += synapse0x817d730();
   input += synapse0x817d770();
   input += synapse0x817d7b0();
   input += synapse0x817d7f0();
   input += synapse0x817d830();
   return input;
}

double NNvtx::neuron0x817d2e0() {
   double input = input0x817d2e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817d870() {
   double input = 0.234834;
   input += synapse0x803e140();
   input += synapse0x803e180();
   input += synapse0x817dc90();
   input += synapse0x817dcd0();
   input += synapse0x817dd10();
   input += synapse0x80b9fe0();
   input += synapse0x80ba020();
   input += synapse0x80ba060();
   input += synapse0x80ba0a0();
   input += synapse0x80ba0e0();
   return input;
}

double NNvtx::neuron0x817d870() {
   double input = input0x817d870();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817e160() {
   double input = -0.659547;
   input += synapse0x817e4a0();
   input += synapse0x817e4e0();
   input += synapse0x817e520();
   input += synapse0x817e560();
   input += synapse0x817e5a0();
   input += synapse0x817e5e0();
   input += synapse0x817e620();
   input += synapse0x817e660();
   input += synapse0x817e6a0();
   input += synapse0x817e6e0();
   return input;
}

double NNvtx::neuron0x817e160() {
   double input = input0x817e160();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817e720() {
   double input = 0.128288;
   input += synapse0x817ea60();
   input += synapse0x817eaa0();
   input += synapse0x817eae0();
   input += synapse0x817eb20();
   input += synapse0x817eb60();
   input += synapse0x817eba0();
   input += synapse0x817ebe0();
   input += synapse0x817ec20();
   input += synapse0x817ec60();
   input += synapse0x817eca0();
   return input;
}

double NNvtx::neuron0x817e720() {
   double input = input0x817e720();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817ece0() {
   double input = -0.675987;
   input += synapse0x817f020();
   input += synapse0x817f060();
   input += synapse0x817f0a0();
   input += synapse0x817f0e0();
   input += synapse0x817f120();
   input += synapse0x817f160();
   input += synapse0x817f1a0();
   input += synapse0x817f1e0();
   input += synapse0x817f220();
   input += synapse0x817f260();
   return input;
}

double NNvtx::neuron0x817ece0() {
   double input = input0x817ece0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x817f2a0() {
   double input = 0.309691;
   input += synapse0x80ba1b0();
   input += synapse0x817f550();
   input += synapse0x817b7d0();
   input += synapse0x817b810();
   input += synapse0x817b850();
   input += synapse0x817b890();
   input += synapse0x817b8d0();
   input += synapse0x817f650();
   input += synapse0x817f690();
   input += synapse0x817f6d0();
   return input;
}

double NNvtx::neuron0x817f2a0() {
   double input = input0x817f2a0();
   return (input * 0.488651)+0.605925;
}

double NNvtx::synapse0x80b9e50() {
   return (neuron0x8179870()*0.55421);
}

double NNvtx::synapse0x817bb90() {
   return (neuron0x8179b80()*-0.0654601);
}

double NNvtx::synapse0x817bbd0() {
   return (neuron0x8179e90()*0.219241);
}

double NNvtx::synapse0x817bc10() {
   return (neuron0x817a1a0()*0.358357);
}

double NNvtx::synapse0x817bc50() {
   return (neuron0x817a4b0()*-0.0116919);
}

double NNvtx::synapse0x817bc90() {
   return (neuron0x817a7c0()*-0.391087);
}

double NNvtx::synapse0x817bcd0() {
   return (neuron0x817aad0()*-0.102797);
}

double NNvtx::synapse0x817bd10() {
   return (neuron0x817ae10()*0.180003);
}

double NNvtx::synapse0x817bd50() {
   return (neuron0x817b150()*0.0749522);
}

double NNvtx::synapse0x817bd90() {
   return (neuron0x817b490()*-0.177733);
}

double NNvtx::synapse0x817c0e0() {
   return (neuron0x8179870()*0.377098);
}

double NNvtx::synapse0x817c120() {
   return (neuron0x8179b80()*0.493659);
}

double NNvtx::synapse0x817c160() {
   return (neuron0x8179e90()*0.529211);
}

double NNvtx::synapse0x817c1a0() {
   return (neuron0x817a1a0()*0.104603);
}

double NNvtx::synapse0x817c1e0() {
   return (neuron0x817a4b0()*0.218733);
}

double NNvtx::synapse0x817c220() {
   return (neuron0x817a7c0()*0.779546);
}

double NNvtx::synapse0x817c260() {
   return (neuron0x817aad0()*0.00221871);
}

double NNvtx::synapse0x803e2f0() {
   return (neuron0x817ae10()*0.529299);
}

double NNvtx::synapse0x803e330() {
   return (neuron0x817b150()*0.0251973);
}

double NNvtx::synapse0x817c3b0() {
   return (neuron0x817b490()*-1.31866);
}

double NNvtx::synapse0x817c700() {
   return (neuron0x8179870()*-0.149382);
}

double NNvtx::synapse0x817c740() {
   return (neuron0x8179b80()*-0.321508);
}

double NNvtx::synapse0x817c780() {
   return (neuron0x8179e90()*-0.386764);
}

double NNvtx::synapse0x817c7c0() {
   return (neuron0x817a1a0()*0.824514);
}

double NNvtx::synapse0x817c800() {
   return (neuron0x817a4b0()*-0.550769);
}

double NNvtx::synapse0x817c840() {
   return (neuron0x817a7c0()*0.275448);
}

double NNvtx::synapse0x817c880() {
   return (neuron0x817aad0()*0.599717);
}

double NNvtx::synapse0x817c8c0() {
   return (neuron0x817ae10()*-0.0502442);
}

double NNvtx::synapse0x817c900() {
   return (neuron0x817b150()*0.439101);
}

double NNvtx::synapse0x817c940() {
   return (neuron0x817b490()*0.288232);
}

double NNvtx::synapse0x817cc90() {
   return (neuron0x8179870()*0.2398);
}

double NNvtx::synapse0x817ccd0() {
   return (neuron0x8179b80()*0.176169);
}

double NNvtx::synapse0x817cd10() {
   return (neuron0x8179e90()*-0.106619);
}

double NNvtx::synapse0x8169b30() {
   return (neuron0x817a1a0()*-1.35423);
}

double NNvtx::synapse0x80b9e90() {
   return (neuron0x817a4b0()*-0.836893);
}

double NNvtx::synapse0x80b9980() {
   return (neuron0x817a7c0()*-0.679017);
}

double NNvtx::synapse0x817c2a0() {
   return (neuron0x817aad0()*-0.367106);
}

double NNvtx::synapse0x817c2e0() {
   return (neuron0x817ae10()*-0.0913954);
}

double NNvtx::synapse0x817c320() {
   return (neuron0x817b150()*0.17783);
}

double NNvtx::synapse0x817c360() {
   return (neuron0x817b490()*1.78159);
}

double NNvtx::synapse0x817d060() {
   return (neuron0x8179870()*-0.670666);
}

double NNvtx::synapse0x817d0a0() {
   return (neuron0x8179b80()*0.0873911);
}

double NNvtx::synapse0x817d0e0() {
   return (neuron0x8179e90()*-0.166468);
}

double NNvtx::synapse0x817d120() {
   return (neuron0x817a1a0()*-0.49835);
}

double NNvtx::synapse0x817d160() {
   return (neuron0x817a4b0()*1.42549);
}

double NNvtx::synapse0x817d1a0() {
   return (neuron0x817a7c0()*-0.467205);
}

double NNvtx::synapse0x817d1e0() {
   return (neuron0x817aad0()*-0.453736);
}

double NNvtx::synapse0x817d220() {
   return (neuron0x817ae10()*0.0811978);
}

double NNvtx::synapse0x817d260() {
   return (neuron0x817b150()*-0.160316);
}

double NNvtx::synapse0x817d2a0() {
   return (neuron0x817b490()*0.0931201);
}

double NNvtx::synapse0x817d5f0() {
   return (neuron0x8179870()*0.0233369);
}

double NNvtx::synapse0x817d630() {
   return (neuron0x8179b80()*-0.363835);
}

double NNvtx::synapse0x817d670() {
   return (neuron0x8179e90()*0.408122);
}

double NNvtx::synapse0x817d6b0() {
   return (neuron0x817a1a0()*-0.409748);
}

double NNvtx::synapse0x817d6f0() {
   return (neuron0x817a4b0()*1.31936);
}

double NNvtx::synapse0x817d730() {
   return (neuron0x817a7c0()*0.905136);
}

double NNvtx::synapse0x817d770() {
   return (neuron0x817aad0()*-1.27045);
}

double NNvtx::synapse0x817d7b0() {
   return (neuron0x817ae10()*-0.609872);
}

double NNvtx::synapse0x817d7f0() {
   return (neuron0x817b150()*-0.279812);
}

double NNvtx::synapse0x817d830() {
   return (neuron0x817b490()*-0.399591);
}

double NNvtx::synapse0x803e140() {
   return (neuron0x8179870()*0.651675);
}

double NNvtx::synapse0x803e180() {
   return (neuron0x8179b80()*0.316679);
}

double NNvtx::synapse0x817dc90() {
   return (neuron0x8179e90()*0.464066);
}

double NNvtx::synapse0x817dcd0() {
   return (neuron0x817a1a0()*-0.263413);
}

double NNvtx::synapse0x817dd10() {
   return (neuron0x817a4b0()*0.0301543);
}

double NNvtx::synapse0x80b9fe0() {
   return (neuron0x817a7c0()*-0.334459);
}

double NNvtx::synapse0x80ba020() {
   return (neuron0x817aad0()*0.176659);
}

double NNvtx::synapse0x80ba060() {
   return (neuron0x817ae10()*-0.386588);
}

double NNvtx::synapse0x80ba0a0() {
   return (neuron0x817b150()*0.218307);
}

double NNvtx::synapse0x80ba0e0() {
   return (neuron0x817b490()*-1.17116);
}

double NNvtx::synapse0x817e4a0() {
   return (neuron0x8179870()*0.420631);
}

double NNvtx::synapse0x817e4e0() {
   return (neuron0x8179b80()*-0.167787);
}

double NNvtx::synapse0x817e520() {
   return (neuron0x8179e90()*0.0708671);
}

double NNvtx::synapse0x817e560() {
   return (neuron0x817a1a0()*0.14406);
}

double NNvtx::synapse0x817e5a0() {
   return (neuron0x817a4b0()*-2.07501);
}

double NNvtx::synapse0x817e5e0() {
   return (neuron0x817a7c0()*0.238548);
}

double NNvtx::synapse0x817e620() {
   return (neuron0x817aad0()*0.37246);
}

double NNvtx::synapse0x817e660() {
   return (neuron0x817ae10()*-1.10008);
}

double NNvtx::synapse0x817e6a0() {
   return (neuron0x817b150()*-0.518884);
}

double NNvtx::synapse0x817e6e0() {
   return (neuron0x817b490()*1.00238);
}

double NNvtx::synapse0x817ea60() {
   return (neuron0x8179870()*0.333478);
}

double NNvtx::synapse0x817eaa0() {
   return (neuron0x8179b80()*0.197591);
}

double NNvtx::synapse0x817eae0() {
   return (neuron0x8179e90()*-0.0910974);
}

double NNvtx::synapse0x817eb20() {
   return (neuron0x817a1a0()*-0.0112961);
}

double NNvtx::synapse0x817eb60() {
   return (neuron0x817a4b0()*-0.0459641);
}

double NNvtx::synapse0x817eba0() {
   return (neuron0x817a7c0()*-0.641015);
}

double NNvtx::synapse0x817ebe0() {
   return (neuron0x817aad0()*-0.278334);
}

double NNvtx::synapse0x817ec20() {
   return (neuron0x817ae10()*0.0612023);
}

double NNvtx::synapse0x817ec60() {
   return (neuron0x817b150()*-0.119719);
}

double NNvtx::synapse0x817eca0() {
   return (neuron0x817b490()*0.338553);
}

double NNvtx::synapse0x817f020() {
   return (neuron0x8179870()*0.210295);
}

double NNvtx::synapse0x817f060() {
   return (neuron0x8179b80()*-0.157573);
}

double NNvtx::synapse0x817f0a0() {
   return (neuron0x8179e90()*-0.437485);
}

double NNvtx::synapse0x817f0e0() {
   return (neuron0x817a1a0()*-0.0244171);
}

double NNvtx::synapse0x817f120() {
   return (neuron0x817a4b0()*0.841423);
}

double NNvtx::synapse0x817f160() {
   return (neuron0x817a7c0()*-0.434562);
}

double NNvtx::synapse0x817f1a0() {
   return (neuron0x817aad0()*0.228022);
}

double NNvtx::synapse0x817f1e0() {
   return (neuron0x817ae10()*-0.157276);
}

double NNvtx::synapse0x817f220() {
   return (neuron0x817b150()*-0.20118);
}

double NNvtx::synapse0x817f260() {
   return (neuron0x817b490()*-1.80765);
}

double NNvtx::synapse0x80ba1b0() {
   return (neuron0x817b910()*-0.0689025);
}

double NNvtx::synapse0x817f550() {
   return (neuron0x817bdd0()*-0.849255);
}

double NNvtx::synapse0x817b7d0() {
   return (neuron0x817c3f0()*0.572624);
}

double NNvtx::synapse0x817b810() {
   return (neuron0x817c980()*-1.33222);
}

double NNvtx::synapse0x817b850() {
   return (neuron0x817cd50()*0.442143);
}

double NNvtx::synapse0x817b890() {
   return (neuron0x817d2e0()*0.424168);
}

double NNvtx::synapse0x817b8d0() {
   return (neuron0x817d870()*-0.678765);
}

double NNvtx::synapse0x817f650() {
   return (neuron0x817e160()*-0.762866);
}

double NNvtx::synapse0x817f690() {
   return (neuron0x817e720()*0.649556);
}

double NNvtx::synapse0x817f6d0() {
   return (neuron0x817ece0()*-1.09259);
}

