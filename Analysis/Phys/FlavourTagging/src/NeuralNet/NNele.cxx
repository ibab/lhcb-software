#include "NNele.h"
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
         return neuron0x876eed8();
     default:
         return 0.;
   }
}

double NNele::neuron0x851b830() {
   return input0;
}

double NNele::neuron0x851b978() {
   return input1;
}

double NNele::neuron0x851bb50() {
   return input2;
}

double NNele::neuron0x876e7a0() {
   return input3;
}

double NNele::neuron0x876e930() {
   return input4;
}

double NNele::neuron0x876eb08() {
   return input5;
}

double NNele::neuron0x876ed00() {
   return input6;
}

double NNele::input0x876eff8() {
   double input = 0.375697;
   input += synapse0x851fa38();
   input += synapse0x876f188();
   input += synapse0x876f1b0();
   input += synapse0x876f1d8();
   input += synapse0x876f200();
   input += synapse0x876f228();
   input += synapse0x876f250();
   return input;
}

double NNele::neuron0x876eff8() {
   double input = input0x876eff8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876f278() {
   double input = -0.606585;
   input += synapse0x876f450();
   input += synapse0x876f478();
   input += synapse0x876f4a0();
   input += synapse0x876f4c8();
   input += synapse0x876f4f0();
   input += synapse0x876f518();
   input += synapse0x876f540();
   return input;
}

double NNele::neuron0x876f278() {
   double input = input0x876f278();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876f568() {
   double input = -0.492589;
   input += synapse0x876f740();
   input += synapse0x876f768();
   input += synapse0x876f790();
   input += synapse0x876f840();
   input += synapse0x876f868();
   input += synapse0x876f890();
   input += synapse0x876f8b8();
   return input;
}

double NNele::neuron0x876f568() {
   double input = input0x876f568();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876f8e0() {
   double input = 1.19475;
   input += synapse0x876fa70();
   input += synapse0x876fa98();
   input += synapse0x876fac0();
   input += synapse0x876fae8();
   input += synapse0x876fb10();
   input += synapse0x876fb38();
   input += synapse0x876fb60();
   return input;
}

double NNele::neuron0x876f8e0() {
   double input = input0x876f8e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876fb88() {
   double input = 0.204268;
   input += synapse0x876fd60();
   input += synapse0x876fd88();
   input += synapse0x876fdb0();
   input += synapse0x876fdd8();
   input += synapse0x876fe00();
   input += synapse0x876f7b8();
   input += synapse0x876f7e0();
   return input;
}

double NNele::neuron0x876fb88() {
   double input = input0x876fb88();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876ff30() {
   double input = 0.0417528;
   input += synapse0x876f808();
   input += synapse0x8770108();
   input += synapse0x8770130();
   input += synapse0x8770158();
   input += synapse0x8770180();
   input += synapse0x87701a8();
   input += synapse0x87701d0();
   return input;
}

double NNele::neuron0x876ff30() {
   double input = input0x876ff30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x87701f8() {
   double input = 0.369475;
   input += synapse0x87703f0();
   input += synapse0x8770418();
   input += synapse0x8770440();
   input += synapse0x8770468();
   input += synapse0x8770490();
   input += synapse0x87704b8();
   input += synapse0x87704e0();
   return input;
}

double NNele::neuron0x87701f8() {
   double input = input0x87701f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8770508() {
   double input = -1.29216;
   input += synapse0x8770700();
   input += synapse0x8770728();
   input += synapse0x8770750();
   input += synapse0x8770778();
   input += synapse0x87707a0();
   input += synapse0x87707c8();
   input += synapse0x87707f0();
   return input;
}

double NNele::neuron0x8770508() {
   double input = input0x8770508();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8770818() {
   double input = 0.709177;
   input += synapse0x8770a10();
   input += synapse0x8770a38();
   input += synapse0x8770a60();
   input += synapse0x8770a88();
   input += synapse0x8770ab0();
   input += synapse0x8770ad8();
   input += synapse0x8770b00();
   return input;
}

double NNele::neuron0x8770818() {
   double input = input0x8770818();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8770b28() {
   double input = -0.1655;
   input += synapse0x8770da8();
   input += synapse0x8770dd0();
   input += synapse0x851b800();
   input += synapse0x851fa60();
   input += synapse0x851fda8();
   input += synapse0x851fd30();
   input += synapse0x87441f8();
   return input;
}

double NNele::neuron0x8770b28() {
   double input = input0x8770b28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x876eed8() {
   double input = 0.0348324;
   input += synapse0x8771000();
   input += synapse0x8771028();
   input += synapse0x8771050();
   input += synapse0x8771078();
   input += synapse0x87710a0();
   input += synapse0x87710c8();
   input += synapse0x87710f0();
   input += synapse0x8771118();
   input += synapse0x8771140();
   input += synapse0x8771168();
   return input;
}

double NNele::neuron0x876eed8() {
   double input = input0x876eed8();
   return (input * 0.467162)+0.678213;
}

double NNele::synapse0x851fa38() {
   return (neuron0x851b830()*-0.791752);
}

double NNele::synapse0x876f188() {
   return (neuron0x851b978()*-0.365074);
}

double NNele::synapse0x876f1b0() {
   return (neuron0x851bb50()*0.582985);
}

double NNele::synapse0x876f1d8() {
   return (neuron0x876e7a0()*0.54934);
}

double NNele::synapse0x876f200() {
   return (neuron0x876e930()*-0.262519);
}

double NNele::synapse0x876f228() {
   return (neuron0x876eb08()*0.433209);
}

double NNele::synapse0x876f250() {
   return (neuron0x876ed00()*0.244576);
}

double NNele::synapse0x876f450() {
   return (neuron0x851b830()*-0.504551);
}

double NNele::synapse0x876f478() {
   return (neuron0x851b978()*-0.707189);
}

double NNele::synapse0x876f4a0() {
   return (neuron0x851bb50()*-0.619716);
}

double NNele::synapse0x876f4c8() {
   return (neuron0x876e7a0()*1.3137);
}

double NNele::synapse0x876f4f0() {
   return (neuron0x876e930()*-0.701142);
}

double NNele::synapse0x876f518() {
   return (neuron0x876eb08()*0.37601);
}

double NNele::synapse0x876f540() {
   return (neuron0x876ed00()*0.272797);
}

double NNele::synapse0x876f740() {
   return (neuron0x851b830()*-2.17239);
}

double NNele::synapse0x876f768() {
   return (neuron0x851b978()*-1.23845);
}

double NNele::synapse0x876f790() {
   return (neuron0x851bb50()*4.3248);
}

double NNele::synapse0x876f840() {
   return (neuron0x876e7a0()*2.53778);
}

double NNele::synapse0x876f868() {
   return (neuron0x876e930()*-1.28425);
}

double NNele::synapse0x876f890() {
   return (neuron0x876eb08()*0.644365);
}

double NNele::synapse0x876f8b8() {
   return (neuron0x876ed00()*-0.238763);
}

double NNele::synapse0x876fa70() {
   return (neuron0x851b830()*-1.2036);
}

double NNele::synapse0x876fa98() {
   return (neuron0x851b978()*0.220692);
}

double NNele::synapse0x876fac0() {
   return (neuron0x851bb50()*1.16721);
}

double NNele::synapse0x876fae8() {
   return (neuron0x876e7a0()*-0.00036276);
}

double NNele::synapse0x876fb10() {
   return (neuron0x876e930()*-0.414394);
}

double NNele::synapse0x876fb38() {
   return (neuron0x876eb08()*-0.266964);
}

double NNele::synapse0x876fb60() {
   return (neuron0x876ed00()*-0.80293);
}

double NNele::synapse0x876fd60() {
   return (neuron0x851b830()*0.124844);
}

double NNele::synapse0x876fd88() {
   return (neuron0x851b978()*0.332048);
}

double NNele::synapse0x876fdb0() {
   return (neuron0x851bb50()*0.316804);
}

double NNele::synapse0x876fdd8() {
   return (neuron0x876e7a0()*0.395948);
}

double NNele::synapse0x876fe00() {
   return (neuron0x876e930()*0.76214);
}

double NNele::synapse0x876f7b8() {
   return (neuron0x876eb08()*0.0851639);
}

double NNele::synapse0x876f7e0() {
   return (neuron0x876ed00()*-0.0787721);
}

double NNele::synapse0x876f808() {
   return (neuron0x851b830()*0.0526935);
}

double NNele::synapse0x8770108() {
   return (neuron0x851b978()*-0.393815);
}

double NNele::synapse0x8770130() {
   return (neuron0x851bb50()*-1.58749);
}

double NNele::synapse0x8770158() {
   return (neuron0x876e7a0()*-0.141446);
}

double NNele::synapse0x8770180() {
   return (neuron0x876e930()*-1.10192);
}

double NNele::synapse0x87701a8() {
   return (neuron0x876eb08()*0.400333);
}

double NNele::synapse0x87701d0() {
   return (neuron0x876ed00()*0.206351);
}

double NNele::synapse0x87703f0() {
   return (neuron0x851b830()*-0.495041);
}

double NNele::synapse0x8770418() {
   return (neuron0x851b978()*0.14928);
}

double NNele::synapse0x8770440() {
   return (neuron0x851bb50()*-0.0233698);
}

double NNele::synapse0x8770468() {
   return (neuron0x876e7a0()*0.984966);
}

double NNele::synapse0x8770490() {
   return (neuron0x876e930()*-0.681344);
}

double NNele::synapse0x87704b8() {
   return (neuron0x876eb08()*0.283613);
}

double NNele::synapse0x87704e0() {
   return (neuron0x876ed00()*-0.0235534);
}

double NNele::synapse0x8770700() {
   return (neuron0x851b830()*-0.211688);
}

double NNele::synapse0x8770728() {
   return (neuron0x851b978()*-1.41346);
}

double NNele::synapse0x8770750() {
   return (neuron0x851bb50()*0.839662);
}

double NNele::synapse0x8770778() {
   return (neuron0x876e7a0()*2.0417);
}

double NNele::synapse0x87707a0() {
   return (neuron0x876e930()*-0.538894);
}

double NNele::synapse0x87707c8() {
   return (neuron0x876eb08()*0.854943);
}

double NNele::synapse0x87707f0() {
   return (neuron0x876ed00()*0.630432);
}

double NNele::synapse0x8770a10() {
   return (neuron0x851b830()*-0.129009);
}

double NNele::synapse0x8770a38() {
   return (neuron0x851b978()*0.764444);
}

double NNele::synapse0x8770a60() {
   return (neuron0x851bb50()*0.167918);
}

double NNele::synapse0x8770a88() {
   return (neuron0x876e7a0()*-1.4361);
}

double NNele::synapse0x8770ab0() {
   return (neuron0x876e930()*0.0406966);
}

double NNele::synapse0x8770ad8() {
   return (neuron0x876eb08()*-0.377394);
}

double NNele::synapse0x8770b00() {
   return (neuron0x876ed00()*0.134348);
}

double NNele::synapse0x8770da8() {
   return (neuron0x851b830()*0.16026);
}

double NNele::synapse0x8770dd0() {
   return (neuron0x851b978()*-0.028134);
}

double NNele::synapse0x851b800() {
   return (neuron0x851bb50()*0.251142);
}

double NNele::synapse0x851fa60() {
   return (neuron0x876e7a0()*-0.127783);
}

double NNele::synapse0x851fda8() {
   return (neuron0x876e930()*-0.242197);
}

double NNele::synapse0x851fd30() {
   return (neuron0x876eb08()*-0.421844);
}

double NNele::synapse0x87441f8() {
   return (neuron0x876ed00()*-0.200513);
}

double NNele::synapse0x8771000() {
   return (neuron0x876eff8()*-1.11166);
}

double NNele::synapse0x8771028() {
   return (neuron0x876f278()*1.18336);
}

double NNele::synapse0x8771050() {
   return (neuron0x876f568()*1.93622);
}

double NNele::synapse0x8771078() {
   return (neuron0x876f8e0()*-1.52341);
}

double NNele::synapse0x87710a0() {
   return (neuron0x876fb88()*0.801372);
}

double NNele::synapse0x87710c8() {
   return (neuron0x876ff30()*-1.13109);
}

double NNele::synapse0x87710f0() {
   return (neuron0x87701f8()*-0.0653332);
}

double NNele::synapse0x8771118() {
   return (neuron0x8770508()*-1.95418);
}

double NNele::synapse0x8771140() {
   return (neuron0x8770818()*0.822138);
}

double NNele::synapse0x8771168() {
   return (neuron0x8770b28()*0.00048302);
}

