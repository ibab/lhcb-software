#include "NNvtx.h"
#include <cmath>

double NNvtx::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9,double in10) {
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
   input10 = (in10 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x1dc0af60();
     default:
         return 0.;
   }
}

double NNvtx::neuron0x1dd0fb60() {
   return input0;
}

double NNvtx::neuron0x1d17d2e0() {
   return input1;
}

double NNvtx::neuron0x1cfd9fd0() {
   return input2;
}

double NNvtx::neuron0x1cfda2e0() {
   return input3;
}

double NNvtx::neuron0x1cdaa580() {
   return input4;
}

double NNvtx::neuron0x1cdaa890() {
   return input5;
}

double NNvtx::neuron0x1d0dc6b0() {
   return input6;
}

double NNvtx::neuron0x1d0dc960() {
   return input7;
}

double NNvtx::neuron0x1cdc8ae0() {
   return input8;
}

double NNvtx::neuron0x1cdc8e20() {
   return input9;
}

double NNvtx::neuron0x1d1187c0() {
   return input10;
}

double NNvtx::input0x1d118c40() {
   double input = 0.320865;
   input += synapse0x1cde3090();
   input += synapse0x1ce67b50();
   input += synapse0x1dd0fde0();
   input += synapse0x1d118ec0();
   input += synapse0x1d118f00();
   input += synapse0x1d118f40();
   input += synapse0x1d118f80();
   input += synapse0x1d118fc0();
   input += synapse0x1d119000();
   input += synapse0x1d119040();
   input += synapse0x1d119080();
   return input;
}

double NNvtx::neuron0x1d118c40() {
   double input = input0x1d118c40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d1190c0() {
   double input = 0.10543;
   input += synapse0x1d1193d0();
   input += synapse0x1d119410();
   input += synapse0x1d119450();
   input += synapse0x1d119490();
   input += synapse0x1d1194d0();
   input += synapse0x1d119510();
   input += synapse0x1d17d560();
   input += synapse0x1cdc9160();
   input += synapse0x1d21f930();
   input += synapse0x1d21f970();
   input += synapse0x1d0dcca0();
   return input;
}

double NNvtx::neuron0x1d1190c0() {
   double input = input0x1d1190c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1ce79460() {
   double input = -0.0139851;
   input += synapse0x1ce79770();
   input += synapse0x1ce797b0();
   input += synapse0x1ce797f0();
   input += synapse0x1ce79830();
   input += synapse0x1ce79870();
   input += synapse0x1ce798b0();
   input += synapse0x1ce798f0();
   input += synapse0x1ce79930();
   input += synapse0x1ce79970();
   input += synapse0x1ce799b0();
   input += synapse0x1ce799f0();
   return input;
}

double NNvtx::neuron0x1ce79460() {
   double input = input0x1ce79460();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1ce79c40() {
   double input = -0.233414;
   input += synapse0x1ce67b10();
   input += synapse0x1ce79ec0();
   input += synapse0x1ce79f00();
   input += synapse0x1ce79f40();
   input += synapse0x1ce79f80();
   input += synapse0x1ce79fc0();
   input += synapse0x1ce7a000();
   input += synapse0x1ce7a040();
   input += synapse0x1ce7a080();
   input += synapse0x1ce7a0c0();
   input += synapse0x1ce7a100();
   return input;
}

double NNvtx::neuron0x1ce79c40() {
   double input = input0x1ce79c40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1ce7a140() {
   double input = -0.188614;
   input += synapse0x1d14ede0();
   input += synapse0x1d36ba80();
   input += synapse0x1cf98dd0();
   input += synapse0x1cf98e70();
   input += synapse0x1cf5df60();
   input += synapse0x1cf5dfa0();
   input += synapse0x1cde2d70();
   input += synapse0x1cde2db0();
   input += synapse0x1ce7a2a0();
   input += synapse0x1ce7a2e0();
   input += synapse0x1dc0a840();
   return input;
}

double NNvtx::neuron0x1ce7a140() {
   double input = input0x1ce7a140();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1dc0a880() {
   double input = -0.00774662;
   input += synapse0x1d21f780();
   input += synapse0x1d21f7c0();
   input += synapse0x1dc0aca0();
   input += synapse0x1dc0ace0();
   input += synapse0x1dc0ad20();
   input += synapse0x1dc0ad60();
   input += synapse0x1dc0ada0();
   input += synapse0x1dc0ade0();
   input += synapse0x1dc0ae20();
   input += synapse0x1dc0ae60();
   input += synapse0x1ce79a30();
   return input;
}

double NNvtx::neuron0x1dc0a880() {
   double input = input0x1dc0a880();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1ce79a70() {
   double input = 0.195594;
   input += synapse0x1dc0b460();
   input += synapse0x1dc0b4a0();
   input += synapse0x1dc0b4e0();
   input += synapse0x1dc0b520();
   input += synapse0x1dc0b560();
   input += synapse0x1dc0b5a0();
   input += synapse0x1dc0b5e0();
   input += synapse0x1dc0b620();
   input += synapse0x1dc0b660();
   input += synapse0x1dc0b6a0();
   input += synapse0x1ce79bd0();
   return input;
}

double NNvtx::neuron0x1ce79a70() {
   double input = input0x1ce79a70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d10e960() {
   double input = 0.0727287;
   input += synapse0x1d10ec70();
   input += synapse0x1d10ecb0();
   input += synapse0x1d10ecf0();
   input += synapse0x1d10ed30();
   input += synapse0x1d10ed70();
   input += synapse0x1d10edb0();
   input += synapse0x1d10edf0();
   input += synapse0x1d10ee30();
   input += synapse0x1d10ee70();
   input += synapse0x1d10eeb0();
   input += synapse0x1d10eef0();
   return input;
}

double NNvtx::neuron0x1d10e960() {
   double input = input0x1d10e960();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d10ef30() {
   double input = -0.141861;
   input += synapse0x1d10f240();
   input += synapse0x1d10f280();
   input += synapse0x1d10f2c0();
   input += synapse0x1d10f300();
   input += synapse0x1d10f340();
   input += synapse0x1d10f380();
   input += synapse0x1d10f3c0();
   input += synapse0x1d10f400();
   input += synapse0x1d10f440();
   input += synapse0x1d10f480();
   input += synapse0x1d10f4c0();
   return input;
}

double NNvtx::neuron0x1d10ef30() {
   double input = input0x1d10ef30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d10f500() {
   double input = 0.0579296;
   input += synapse0x1d0b9170();
   input += synapse0x1d0b91b0();
   input += synapse0x1d0b91f0();
   input += synapse0x1d0b9230();
   input += synapse0x1d0b9270();
   input += synapse0x1d0b92b0();
   input += synapse0x1d0b92f0();
   input += synapse0x1d0b9330();
   input += synapse0x1d0b9370();
   input += synapse0x1d0b93b0();
   input += synapse0x1d0b93f0();
   return input;
}

double NNvtx::neuron0x1d10f500() {
   double input = input0x1d10f500();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d0b9430() {
   double input = 0.243131;
   input += synapse0x1d0b9740();
   input += synapse0x1d0b9780();
   input += synapse0x1d0b97c0();
   input += synapse0x1d0b9800();
   input += synapse0x1d0b9840();
   input += synapse0x1d0b9880();
   input += synapse0x1d0b98c0();
   input += synapse0x1d0b9900();
   input += synapse0x1d0b9940();
   input += synapse0x1d0b9980();
   input += synapse0x1d0b99c0();
   return input;
}

double NNvtx::neuron0x1d0b9430() {
   double input = input0x1d0b9430();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1d0b9a00() {
   double input = 0.344038;
   input += synapse0x1d0b9d40();
   input += synapse0x1d0b9d80();
   input += synapse0x1d0b9dc0();
   input += synapse0x1d0b9e00();
   input += synapse0x1d0b9e40();
   input += synapse0x1d0b9e80();
   input += synapse0x1d0b9ec0();
   input += synapse0x1d0b9f00();
   input += synapse0x1dc0aea0();
   input += synapse0x1dc0aee0();
   input += synapse0x1dc0af20();
   return input;
}

double NNvtx::neuron0x1d0b9a00() {
   double input = input0x1d0b9a00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0x1dc0af60() {
   double input = -0.0873575;
   input += synapse0x1d118b00();
   input += synapse0x1d118b40();
   input += synapse0x1d118b80();
   input += synapse0x1d118bc0();
   input += synapse0x1d118c00();
   input += synapse0x1d0ba750();
   input += synapse0x1d0ba790();
   input += synapse0x1d0ba7d0();
   input += synapse0x1d0ba810();
   input += synapse0x1d0ba850();
   input += synapse0x1d0ba890();
   input += synapse0x1d0ba8d0();
   return input;
}

double NNvtx::neuron0x1dc0af60() {
   double input = input0x1dc0af60();
   return (input * 0.489201)+0.603356;
}

double NNvtx::synapse0x1cde3090() {
   return (neuron0x1dd0fb60()*0.310348);
}

double NNvtx::synapse0x1ce67b50() {
   return (neuron0x1d17d2e0()*-0.516894);
}

double NNvtx::synapse0x1dd0fde0() {
   return (neuron0x1cfd9fd0()*-0.321889);
}

double NNvtx::synapse0x1d118ec0() {
   return (neuron0x1cfda2e0()*-0.314883);
}

double NNvtx::synapse0x1d118f00() {
   return (neuron0x1cdaa580()*-0.0186633);
}

double NNvtx::synapse0x1d118f40() {
   return (neuron0x1cdaa890()*-0.542191);
}

double NNvtx::synapse0x1d118f80() {
   return (neuron0x1d0dc6b0()*-0.0685937);
}

double NNvtx::synapse0x1d118fc0() {
   return (neuron0x1d0dc960()*0.111526);
}

double NNvtx::synapse0x1d119000() {
   return (neuron0x1cdc8ae0()*-0.375987);
}

double NNvtx::synapse0x1d119040() {
   return (neuron0x1cdc8e20()*0.0503818);
}

double NNvtx::synapse0x1d119080() {
   return (neuron0x1d1187c0()*-0.106637);
}

double NNvtx::synapse0x1d1193d0() {
   return (neuron0x1dd0fb60()*0.70886);
}

double NNvtx::synapse0x1d119410() {
   return (neuron0x1d17d2e0()*0.115343);
}

double NNvtx::synapse0x1d119450() {
   return (neuron0x1cfd9fd0()*0.198397);
}

double NNvtx::synapse0x1d119490() {
   return (neuron0x1cfda2e0()*-0.125685);
}

double NNvtx::synapse0x1d1194d0() {
   return (neuron0x1cdaa580()*-0.0223828);
}

double NNvtx::synapse0x1d119510() {
   return (neuron0x1cdaa890()*0.321081);
}

double NNvtx::synapse0x1d17d560() {
   return (neuron0x1d0dc6b0()*0.196935);
}

double NNvtx::synapse0x1cdc9160() {
   return (neuron0x1d0dc960()*0.510667);
}

double NNvtx::synapse0x1d21f930() {
   return (neuron0x1cdc8ae0()*-0.136536);
}

double NNvtx::synapse0x1d21f970() {
   return (neuron0x1cdc8e20()*-0.270068);
}

double NNvtx::synapse0x1d0dcca0() {
   return (neuron0x1d1187c0()*-0.208095);
}

double NNvtx::synapse0x1ce79770() {
   return (neuron0x1dd0fb60()*-0.262059);
}

double NNvtx::synapse0x1ce797b0() {
   return (neuron0x1d17d2e0()*-0.071736);
}

double NNvtx::synapse0x1ce797f0() {
   return (neuron0x1cfd9fd0()*-0.180542);
}

double NNvtx::synapse0x1ce79830() {
   return (neuron0x1cfda2e0()*0.0135273);
}

double NNvtx::synapse0x1ce79870() {
   return (neuron0x1cdaa580()*0.750627);
}

double NNvtx::synapse0x1ce798b0() {
   return (neuron0x1cdaa890()*-0.641142);
}

double NNvtx::synapse0x1ce798f0() {
   return (neuron0x1d0dc6b0()*-0.395974);
}

double NNvtx::synapse0x1ce79930() {
   return (neuron0x1d0dc960()*-0.255759);
}

double NNvtx::synapse0x1ce79970() {
   return (neuron0x1cdc8ae0()*-0.45475);
}

double NNvtx::synapse0x1ce799b0() {
   return (neuron0x1cdc8e20()*0.453507);
}

double NNvtx::synapse0x1ce799f0() {
   return (neuron0x1d1187c0()*0.00595097);
}

double NNvtx::synapse0x1ce67b10() {
   return (neuron0x1dd0fb60()*-0.260135);
}

double NNvtx::synapse0x1ce79ec0() {
   return (neuron0x1d17d2e0()*-0.0763837);
}

double NNvtx::synapse0x1ce79f00() {
   return (neuron0x1cfd9fd0()*-0.219237);
}

double NNvtx::synapse0x1ce79f40() {
   return (neuron0x1cfda2e0()*-0.48568);
}

double NNvtx::synapse0x1ce79f80() {
   return (neuron0x1cdaa580()*-0.185368);
}

double NNvtx::synapse0x1ce79fc0() {
   return (neuron0x1cdaa890()*0.154811);
}

double NNvtx::synapse0x1ce7a000() {
   return (neuron0x1d0dc6b0()*0.492118);
}

double NNvtx::synapse0x1ce7a040() {
   return (neuron0x1d0dc960()*0.237142);
}

double NNvtx::synapse0x1ce7a080() {
   return (neuron0x1cdc8ae0()*0.327501);
}

double NNvtx::synapse0x1ce7a0c0() {
   return (neuron0x1cdc8e20()*0.503178);
}

double NNvtx::synapse0x1ce7a100() {
   return (neuron0x1d1187c0()*0.208993);
}

double NNvtx::synapse0x1d14ede0() {
   return (neuron0x1dd0fb60()*-0.480773);
}

double NNvtx::synapse0x1d36ba80() {
   return (neuron0x1d17d2e0()*0.338372);
}

double NNvtx::synapse0x1cf98dd0() {
   return (neuron0x1cfd9fd0()*-0.139981);
}

double NNvtx::synapse0x1cf98e70() {
   return (neuron0x1cfda2e0()*0.18857);
}

double NNvtx::synapse0x1cf5df60() {
   return (neuron0x1cdaa580()*0.0862334);
}

double NNvtx::synapse0x1cf5dfa0() {
   return (neuron0x1cdaa890()*-0.383264);
}

double NNvtx::synapse0x1cde2d70() {
   return (neuron0x1d0dc6b0()*0.364752);
}

double NNvtx::synapse0x1cde2db0() {
   return (neuron0x1d0dc960()*-0.00486311);
}

double NNvtx::synapse0x1ce7a2a0() {
   return (neuron0x1cdc8ae0()*0.0386824);
}

double NNvtx::synapse0x1ce7a2e0() {
   return (neuron0x1cdc8e20()*0.354448);
}

double NNvtx::synapse0x1dc0a840() {
   return (neuron0x1d1187c0()*-0.359725);
}

double NNvtx::synapse0x1d21f780() {
   return (neuron0x1dd0fb60()*-0.114704);
}

double NNvtx::synapse0x1d21f7c0() {
   return (neuron0x1d17d2e0()*-0.434788);
}

double NNvtx::synapse0x1dc0aca0() {
   return (neuron0x1cfd9fd0()*-0.305886);
}

double NNvtx::synapse0x1dc0ace0() {
   return (neuron0x1cfda2e0()*-0.118288);
}

double NNvtx::synapse0x1dc0ad20() {
   return (neuron0x1cdaa580()*0.388985);
}

double NNvtx::synapse0x1dc0ad60() {
   return (neuron0x1cdaa890()*0.17793);
}

double NNvtx::synapse0x1dc0ada0() {
   return (neuron0x1d0dc6b0()*0.260148);
}

double NNvtx::synapse0x1dc0ade0() {
   return (neuron0x1d0dc960()*0.17777);
}

double NNvtx::synapse0x1dc0ae20() {
   return (neuron0x1cdc8ae0()*-0.389531);
}

double NNvtx::synapse0x1dc0ae60() {
   return (neuron0x1cdc8e20()*0.0115119);
}

double NNvtx::synapse0x1ce79a30() {
   return (neuron0x1d1187c0()*-0.260807);
}

double NNvtx::synapse0x1dc0b460() {
   return (neuron0x1dd0fb60()*0.370101);
}

double NNvtx::synapse0x1dc0b4a0() {
   return (neuron0x1d17d2e0()*0.366054);
}

double NNvtx::synapse0x1dc0b4e0() {
   return (neuron0x1cfd9fd0()*0.338526);
}

double NNvtx::synapse0x1dc0b520() {
   return (neuron0x1cfda2e0()*0.34937);
}

double NNvtx::synapse0x1dc0b560() {
   return (neuron0x1cdaa580()*-0.162002);
}

double NNvtx::synapse0x1dc0b5a0() {
   return (neuron0x1cdaa890()*-0.349668);
}

double NNvtx::synapse0x1dc0b5e0() {
   return (neuron0x1d0dc6b0()*-0.469948);
}

double NNvtx::synapse0x1dc0b620() {
   return (neuron0x1d0dc960()*-0.307707);
}

double NNvtx::synapse0x1dc0b660() {
   return (neuron0x1cdc8ae0()*-0.418472);
}

double NNvtx::synapse0x1dc0b6a0() {
   return (neuron0x1cdc8e20()*0.227814);
}

double NNvtx::synapse0x1ce79bd0() {
   return (neuron0x1d1187c0()*0.0522693);
}

double NNvtx::synapse0x1d10ec70() {
   return (neuron0x1dd0fb60()*-0.13775);
}

double NNvtx::synapse0x1d10ecb0() {
   return (neuron0x1d17d2e0()*-0.17227);
}

double NNvtx::synapse0x1d10ecf0() {
   return (neuron0x1cfd9fd0()*-0.0514374);
}

double NNvtx::synapse0x1d10ed30() {
   return (neuron0x1cfda2e0()*-0.266717);
}

double NNvtx::synapse0x1d10ed70() {
   return (neuron0x1cdaa580()*-0.375165);
}

double NNvtx::synapse0x1d10edb0() {
   return (neuron0x1cdaa890()*-0.735456);
}

double NNvtx::synapse0x1d10edf0() {
   return (neuron0x1d0dc6b0()*0.485885);
}

double NNvtx::synapse0x1d10ee30() {
   return (neuron0x1d0dc960()*-0.464149);
}

double NNvtx::synapse0x1d10ee70() {
   return (neuron0x1cdc8ae0()*-0.166686);
}

double NNvtx::synapse0x1d10eeb0() {
   return (neuron0x1cdc8e20()*0.00116963);
}

double NNvtx::synapse0x1d10eef0() {
   return (neuron0x1d1187c0()*0.732977);
}

double NNvtx::synapse0x1d10f240() {
   return (neuron0x1dd0fb60()*-0.0168345);
}

double NNvtx::synapse0x1d10f280() {
   return (neuron0x1d17d2e0()*-0.431569);
}

double NNvtx::synapse0x1d10f2c0() {
   return (neuron0x1cfd9fd0()*-0.0902817);
}

double NNvtx::synapse0x1d10f300() {
   return (neuron0x1cfda2e0()*0.599017);
}

double NNvtx::synapse0x1d10f340() {
   return (neuron0x1cdaa580()*1.73737);
}

double NNvtx::synapse0x1d10f380() {
   return (neuron0x1cdaa890()*0.0470695);
}

double NNvtx::synapse0x1d10f3c0() {
   return (neuron0x1d0dc6b0()*-0.413445);
}

double NNvtx::synapse0x1d10f400() {
   return (neuron0x1d0dc960()*-0.0788935);
}

double NNvtx::synapse0x1d10f440() {
   return (neuron0x1cdc8ae0()*-0.339119);
}

double NNvtx::synapse0x1d10f480() {
   return (neuron0x1cdc8e20()*-0.0332687);
}

double NNvtx::synapse0x1d10f4c0() {
   return (neuron0x1d1187c0()*-0.649069);
}

double NNvtx::synapse0x1d0b9170() {
   return (neuron0x1dd0fb60()*0.300471);
}

double NNvtx::synapse0x1d0b91b0() {
   return (neuron0x1d17d2e0()*0.0379497);
}

double NNvtx::synapse0x1d0b91f0() {
   return (neuron0x1cfd9fd0()*0.0453556);
}

double NNvtx::synapse0x1d0b9230() {
   return (neuron0x1cfda2e0()*0.326756);
}

double NNvtx::synapse0x1d0b9270() {
   return (neuron0x1cdaa580()*0.494814);
}

double NNvtx::synapse0x1d0b92b0() {
   return (neuron0x1cdaa890()*0.140291);
}

double NNvtx::synapse0x1d0b92f0() {
   return (neuron0x1d0dc6b0()*0.433627);
}

double NNvtx::synapse0x1d0b9330() {
   return (neuron0x1d0dc960()*-0.442906);
}

double NNvtx::synapse0x1d0b9370() {
   return (neuron0x1cdc8ae0()*0.491098);
}

double NNvtx::synapse0x1d0b93b0() {
   return (neuron0x1cdc8e20()*-0.303394);
}

double NNvtx::synapse0x1d0b93f0() {
   return (neuron0x1d1187c0()*0.354021);
}

double NNvtx::synapse0x1d0b9740() {
   return (neuron0x1dd0fb60()*0.49963);
}

double NNvtx::synapse0x1d0b9780() {
   return (neuron0x1d17d2e0()*0.399565);
}

double NNvtx::synapse0x1d0b97c0() {
   return (neuron0x1cfd9fd0()*0.428351);
}

double NNvtx::synapse0x1d0b9800() {
   return (neuron0x1cfda2e0()*-0.139667);
}

double NNvtx::synapse0x1d0b9840() {
   return (neuron0x1cdaa580()*-0.417562);
}

double NNvtx::synapse0x1d0b9880() {
   return (neuron0x1cdaa890()*0.27511);
}

double NNvtx::synapse0x1d0b98c0() {
   return (neuron0x1d0dc6b0()*-0.25256);
}

double NNvtx::synapse0x1d0b9900() {
   return (neuron0x1d0dc960()*0.298143);
}

double NNvtx::synapse0x1d0b9940() {
   return (neuron0x1cdc8ae0()*0.492383);
}

double NNvtx::synapse0x1d0b9980() {
   return (neuron0x1cdc8e20()*-0.230045);
}

double NNvtx::synapse0x1d0b99c0() {
   return (neuron0x1d1187c0()*0.164924);
}

double NNvtx::synapse0x1d0b9d40() {
   return (neuron0x1dd0fb60()*-0.0961538);
}

double NNvtx::synapse0x1d0b9d80() {
   return (neuron0x1d17d2e0()*-0.451474);
}

double NNvtx::synapse0x1d0b9dc0() {
   return (neuron0x1cfd9fd0()*-0.151534);
}

double NNvtx::synapse0x1d0b9e00() {
   return (neuron0x1cfda2e0()*-0.472966);
}

double NNvtx::synapse0x1d0b9e40() {
   return (neuron0x1cdaa580()*-0.497489);
}

double NNvtx::synapse0x1d0b9e80() {
   return (neuron0x1cdaa890()*-0.188206);
}

double NNvtx::synapse0x1d0b9ec0() {
   return (neuron0x1d0dc6b0()*0.225734);
}

double NNvtx::synapse0x1d0b9f00() {
   return (neuron0x1d0dc960()*-0.392493);
}

double NNvtx::synapse0x1dc0aea0() {
   return (neuron0x1cdc8ae0()*0.394387);
}

double NNvtx::synapse0x1dc0aee0() {
   return (neuron0x1cdc8e20()*0.32559);
}

double NNvtx::synapse0x1dc0af20() {
   return (neuron0x1d1187c0()*-0.418441);
}

double NNvtx::synapse0x1d118b00() {
   return (neuron0x1d118c40()*-0.0628007);
}

double NNvtx::synapse0x1d118b40() {
   return (neuron0x1d1190c0()*0.0926834);
}

double NNvtx::synapse0x1d118b80() {
   return (neuron0x1ce79460()*-0.248179);
}

double NNvtx::synapse0x1d118bc0() {
   return (neuron0x1ce79c40()*0.00993967);
}

double NNvtx::synapse0x1d118c00() {
   return (neuron0x1ce7a140()*0.329376);
}

double NNvtx::synapse0x1d0ba750() {
   return (neuron0x1dc0a880()*-0.18356);
}

double NNvtx::synapse0x1d0ba790() {
   return (neuron0x1ce79a70()*-0.162699);
}

double NNvtx::synapse0x1d0ba7d0() {
   return (neuron0x1d10e960()*-0.13037);
}

double NNvtx::synapse0x1d0ba810() {
   return (neuron0x1d10ef30()*0.421138);
}

double NNvtx::synapse0x1d0ba850() {
   return (neuron0x1d10f500()*0.407652);
}

double NNvtx::synapse0x1d0ba890() {
   return (neuron0x1d0b9430()*-0.445417);
}

double NNvtx::synapse0x1d0ba8d0() {
   return (neuron0x1d0b9a00()*0.0392749);
}

