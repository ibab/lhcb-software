#include "weights/NNkaonS.h"
#include <cmath>

double NNkaonS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x87972a8();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x8793cd8() {
   return input0;
}

double NNkaonS::neuron0x8793e68() {
   return input1;
}

double NNkaonS::neuron0x8794060() {
   return input2;
}

double NNkaonS::neuron0x8794258() {
   return input3;
}

double NNkaonS::neuron0x8794450() {
   return input4;
}

double NNkaonS::neuron0x8794648() {
   return input5;
}

double NNkaonS::neuron0x8794840() {
   return input6;
}

double NNkaonS::neuron0x8794a38() {
   return input7;
}

double NNkaonS::neuron0x8794c30() {
   return input8;
}

double NNkaonS::neuron0x8794e28() {
   return input9;
}

double NNkaonS::input0x8795120() {
   double input = -0.540756;
   input += synapse0x8775970();
   input += synapse0x87952b0();
   input += synapse0x87952d8();
   input += synapse0x8795300();
   input += synapse0x8795328();
   input += synapse0x8795350();
   input += synapse0x8795378();
   input += synapse0x87953a0();
   input += synapse0x87953c8();
   input += synapse0x87953f0();
   return input;
}

double NNkaonS::neuron0x8795120() {
   double input = input0x8795120();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795418() {
   double input = 0.694263;
   input += synapse0x87955f0();
   input += synapse0x8795618();
   input += synapse0x8795640();
   input += synapse0x8795668();
   input += synapse0x8795690();
   input += synapse0x87956b8();
   input += synapse0x87956e0();
   input += synapse0x8795790();
   input += synapse0x87957b8();
   input += synapse0x87957e0();
   return input;
}

double NNkaonS::neuron0x8795418() {
   double input = input0x8795418();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795808() {
   double input = -0.452133;
   input += synapse0x8795998();
   input += synapse0x87959c0();
   input += synapse0x87959e8();
   input += synapse0x8795a10();
   input += synapse0x8795a38();
   input += synapse0x8795a60();
   input += synapse0x8795a88();
   input += synapse0x8795ab0();
   input += synapse0x8795ad8();
   input += synapse0x8795b00();
   return input;
}

double NNkaonS::neuron0x8795808() {
   double input = input0x8795808();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795b28() {
   double input = -0.445563;
   input += synapse0x8795d00();
   input += synapse0x8795d28();
   input += synapse0x8795d50();
   input += synapse0x8422750();
   input += synapse0x8422778();
   input += synapse0x8795708();
   input += synapse0x8795730();
   input += synapse0x8795758();
   input += synapse0x8775f28();
   input += synapse0x8775f50();
   return input;
}

double NNkaonS::neuron0x8795b28() {
   double input = input0x8795b28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795e80() {
   double input = -1.07751;
   input += synapse0x8796058();
   input += synapse0x8796080();
   input += synapse0x87960a8();
   input += synapse0x87960d0();
   input += synapse0x87960f8();
   input += synapse0x8796120();
   input += synapse0x8796148();
   input += synapse0x8796170();
   input += synapse0x8796198();
   input += synapse0x87961c0();
   return input;
}

double NNkaonS::neuron0x8795e80() {
   double input = input0x8795e80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87961e8() {
   double input = 0.0011682;
   input += synapse0x87963c0();
   input += synapse0x87963e8();
   input += synapse0x8796410();
   input += synapse0x8796438();
   input += synapse0x8796460();
   input += synapse0x8796488();
   input += synapse0x87964b0();
   input += synapse0x87964d8();
   input += synapse0x8796500();
   input += synapse0x8796528();
   return input;
}

double NNkaonS::neuron0x87961e8() {
   double input = input0x87961e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796550() {
   double input = -1.45349;
   input += synapse0x87967b0();
   input += synapse0x87967d8();
   input += synapse0x8796800();
   input += synapse0x8796828();
   input += synapse0x8796850();
   input += synapse0x8775da0();
   input += synapse0x8775998();
   input += synapse0x8422ad8();
   input += synapse0x8422a60();
   input += synapse0x86c7e98();
   return input;
}

double NNkaonS::neuron0x8796550() {
   double input = input0x8796550();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795d78() {
   double input = 0.729142;
   input += synapse0x8422c88();
   input += synapse0x8775fc0();
   input += synapse0x8775fe8();
   input += synapse0x8796a80();
   input += synapse0x8796aa8();
   input += synapse0x8796ad0();
   input += synapse0x8796af8();
   input += synapse0x8796b20();
   input += synapse0x8796b48();
   input += synapse0x8796b70();
   return input;
}

double NNkaonS::neuron0x8795d78() {
   double input = input0x8795d78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796b98() {
   double input = -0.144021;
   input += synapse0x8796d90();
   input += synapse0x8796db8();
   input += synapse0x8796de0();
   input += synapse0x8796e08();
   input += synapse0x8796e30();
   input += synapse0x8796e58();
   input += synapse0x8796e80();
   input += synapse0x8796ea8();
   input += synapse0x8796ed0();
   input += synapse0x8796ef8();
   return input;
}

double NNkaonS::neuron0x8796b98() {
   double input = input0x8796b98();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796f20() {
   double input = -0.966289;
   input += synapse0x8797118();
   input += synapse0x8797140();
   input += synapse0x8797168();
   input += synapse0x8797190();
   input += synapse0x87971b8();
   input += synapse0x87971e0();
   input += synapse0x8797208();
   input += synapse0x8797230();
   input += synapse0x8797258();
   input += synapse0x8797280();
   return input;
}

double NNkaonS::neuron0x8796f20() {
   double input = input0x8796f20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87972a8() {
   double input = -0.615;
   input += synapse0x87974a0();
   input += synapse0x87974c8();
   input += synapse0x87974f0();
   input += synapse0x8797518();
   input += synapse0x8797540();
   input += synapse0x8797568();
   input += synapse0x8797590();
   input += synapse0x87975b8();
   input += synapse0x87975e0();
   input += synapse0x8797608();
   return input;
}

double NNkaonS::neuron0x87972a8() {
   double input = input0x87972a8();
   return (input * 0.4776)+0.647979;
}

double NNkaonS::synapse0x8775970() {
   return (neuron0x8793cd8()*-1.58181);
}

double NNkaonS::synapse0x87952b0() {
   return (neuron0x8793e68()*0.484718);
}

double NNkaonS::synapse0x87952d8() {
   return (neuron0x8794060()*0.472624);
}

double NNkaonS::synapse0x8795300() {
   return (neuron0x8794258()*0.118121);
}

double NNkaonS::synapse0x8795328() {
   return (neuron0x8794450()*-0.0933204);
}

double NNkaonS::synapse0x8795350() {
   return (neuron0x8794648()*-0.509342);
}

double NNkaonS::synapse0x8795378() {
   return (neuron0x8794840()*-0.169877);
}

double NNkaonS::synapse0x87953a0() {
   return (neuron0x8794a38()*-0.0249473);
}

double NNkaonS::synapse0x87953c8() {
   return (neuron0x8794c30()*-0.217403);
}

double NNkaonS::synapse0x87953f0() {
   return (neuron0x8794e28()*0.825989);
}

double NNkaonS::synapse0x87955f0() {
   return (neuron0x8793cd8()*0.641913);
}

double NNkaonS::synapse0x8795618() {
   return (neuron0x8793e68()*-0.52287);
}

double NNkaonS::synapse0x8795640() {
   return (neuron0x8794060()*-0.635855);
}

double NNkaonS::synapse0x8795668() {
   return (neuron0x8794258()*-0.397023);
}

double NNkaonS::synapse0x8795690() {
   return (neuron0x8794450()*0.29534);
}

double NNkaonS::synapse0x87956b8() {
   return (neuron0x8794648()*-0.0975812);
}

double NNkaonS::synapse0x87956e0() {
   return (neuron0x8794840()*-0.575415);
}

double NNkaonS::synapse0x8795790() {
   return (neuron0x8794a38()*0.0180211);
}

double NNkaonS::synapse0x87957b8() {
   return (neuron0x8794c30()*-0.149475);
}

double NNkaonS::synapse0x87957e0() {
   return (neuron0x8794e28()*0.151127);
}

double NNkaonS::synapse0x8795998() {
   return (neuron0x8793cd8()*-0.675541);
}

double NNkaonS::synapse0x87959c0() {
   return (neuron0x8793e68()*-0.0613024);
}

double NNkaonS::synapse0x87959e8() {
   return (neuron0x8794060()*-0.219659);
}

double NNkaonS::synapse0x8795a10() {
   return (neuron0x8794258()*-0.14038);
}

double NNkaonS::synapse0x8795a38() {
   return (neuron0x8794450()*-0.0500671);
}

double NNkaonS::synapse0x8795a60() {
   return (neuron0x8794648()*0.119712);
}

double NNkaonS::synapse0x8795a88() {
   return (neuron0x8794840()*-0.57263);
}

double NNkaonS::synapse0x8795ab0() {
   return (neuron0x8794a38()*-0.559487);
}

double NNkaonS::synapse0x8795ad8() {
   return (neuron0x8794c30()*-0.222398);
}

double NNkaonS::synapse0x8795b00() {
   return (neuron0x8794e28()*-0.176308);
}

double NNkaonS::synapse0x8795d00() {
   return (neuron0x8793cd8()*-0.54806);
}

double NNkaonS::synapse0x8795d28() {
   return (neuron0x8793e68()*0.613002);
}

double NNkaonS::synapse0x8795d50() {
   return (neuron0x8794060()*0.997131);
}

double NNkaonS::synapse0x8422750() {
   return (neuron0x8794258()*-0.417292);
}

double NNkaonS::synapse0x8422778() {
   return (neuron0x8794450()*0.304398);
}

double NNkaonS::synapse0x8795708() {
   return (neuron0x8794648()*-0.54258);
}

double NNkaonS::synapse0x8795730() {
   return (neuron0x8794840()*-0.951575);
}

double NNkaonS::synapse0x8795758() {
   return (neuron0x8794a38()*-0.147567);
}

double NNkaonS::synapse0x8775f28() {
   return (neuron0x8794c30()*-0.200116);
}

double NNkaonS::synapse0x8775f50() {
   return (neuron0x8794e28()*0.35224);
}

double NNkaonS::synapse0x8796058() {
   return (neuron0x8793cd8()*-1.14756);
}

double NNkaonS::synapse0x8796080() {
   return (neuron0x8793e68()*0.375772);
}

double NNkaonS::synapse0x87960a8() {
   return (neuron0x8794060()*0.5035);
}

double NNkaonS::synapse0x87960d0() {
   return (neuron0x8794258()*-0.308219);
}

double NNkaonS::synapse0x87960f8() {
   return (neuron0x8794450()*-0.386764);
}

double NNkaonS::synapse0x8796120() {
   return (neuron0x8794648()*-0.0829564);
}

double NNkaonS::synapse0x8796148() {
   return (neuron0x8794840()*2.78759);
}

double NNkaonS::synapse0x8796170() {
   return (neuron0x8794a38()*-0.249373);
}

double NNkaonS::synapse0x8796198() {
   return (neuron0x8794c30()*1.51993);
}

double NNkaonS::synapse0x87961c0() {
   return (neuron0x8794e28()*1.45908);
}

double NNkaonS::synapse0x87963c0() {
   return (neuron0x8793cd8()*-1.04889);
}

double NNkaonS::synapse0x87963e8() {
   return (neuron0x8793e68()*-0.128274);
}

double NNkaonS::synapse0x8796410() {
   return (neuron0x8794060()*0.884877);
}

double NNkaonS::synapse0x8796438() {
   return (neuron0x8794258()*0.369564);
}

double NNkaonS::synapse0x8796460() {
   return (neuron0x8794450()*-0.280517);
}

double NNkaonS::synapse0x8796488() {
   return (neuron0x8794648()*-0.673813);
}

double NNkaonS::synapse0x87964b0() {
   return (neuron0x8794840()*0.78337);
}

double NNkaonS::synapse0x87964d8() {
   return (neuron0x8794a38()*0.171439);
}

double NNkaonS::synapse0x8796500() {
   return (neuron0x8794c30()*-0.978553);
}

double NNkaonS::synapse0x8796528() {
   return (neuron0x8794e28()*0.159492);
}

double NNkaonS::synapse0x87967b0() {
   return (neuron0x8793cd8()*-3.76667);
}

double NNkaonS::synapse0x87967d8() {
   return (neuron0x8793e68()*-0.16952);
}

double NNkaonS::synapse0x8796800() {
   return (neuron0x8794060()*0.800822);
}

double NNkaonS::synapse0x8796828() {
   return (neuron0x8794258()*-0.306094);
}

double NNkaonS::synapse0x8796850() {
   return (neuron0x8794450()*-1.50809);
}

double NNkaonS::synapse0x8775da0() {
   return (neuron0x8794648()*-0.803184);
}

double NNkaonS::synapse0x8775998() {
   return (neuron0x8794840()*-1.21406);
}

double NNkaonS::synapse0x8422ad8() {
   return (neuron0x8794a38()*-0.832555);
}

double NNkaonS::synapse0x8422a60() {
   return (neuron0x8794c30()*-2.17221);
}

double NNkaonS::synapse0x86c7e98() {
   return (neuron0x8794e28()*0.635636);
}

double NNkaonS::synapse0x8422c88() {
   return (neuron0x8793cd8()*2.1098);
}

double NNkaonS::synapse0x8775fc0() {
   return (neuron0x8793e68()*-0.310458);
}

double NNkaonS::synapse0x8775fe8() {
   return (neuron0x8794060()*-2.75108);
}

double NNkaonS::synapse0x8796a80() {
   return (neuron0x8794258()*0.261377);
}

double NNkaonS::synapse0x8796aa8() {
   return (neuron0x8794450()*1.49416);
}

double NNkaonS::synapse0x8796ad0() {
   return (neuron0x8794648()*0.140362);
}

double NNkaonS::synapse0x8796af8() {
   return (neuron0x8794840()*0.950064);
}

double NNkaonS::synapse0x8796b20() {
   return (neuron0x8794a38()*0.137997);
}

double NNkaonS::synapse0x8796b48() {
   return (neuron0x8794c30()*-0.912786);
}

double NNkaonS::synapse0x8796b70() {
   return (neuron0x8794e28()*-1.89167);
}

double NNkaonS::synapse0x8796d90() {
   return (neuron0x8793cd8()*1.82058);
}

double NNkaonS::synapse0x8796db8() {
   return (neuron0x8793e68()*0.21867);
}

double NNkaonS::synapse0x8796de0() {
   return (neuron0x8794060()*-0.628426);
}

double NNkaonS::synapse0x8796e08() {
   return (neuron0x8794258()*-0.000377809);
}

double NNkaonS::synapse0x8796e30() {
   return (neuron0x8794450()*1.23792);
}

double NNkaonS::synapse0x8796e58() {
   return (neuron0x8794648()*0.593945);
}

double NNkaonS::synapse0x8796e80() {
   return (neuron0x8794840()*-1.35821);
}

double NNkaonS::synapse0x8796ea8() {
   return (neuron0x8794a38()*0.645787);
}

double NNkaonS::synapse0x8796ed0() {
   return (neuron0x8794c30()*1.32135);
}

double NNkaonS::synapse0x8796ef8() {
   return (neuron0x8794e28()*0.227025);
}

double NNkaonS::synapse0x8797118() {
   return (neuron0x8793cd8()*0.782696);
}

double NNkaonS::synapse0x8797140() {
   return (neuron0x8793e68()*0.135967);
}

double NNkaonS::synapse0x8797168() {
   return (neuron0x8794060()*-0.699053);
}

double NNkaonS::synapse0x8797190() {
   return (neuron0x8794258()*-0.293686);
}

double NNkaonS::synapse0x87971b8() {
   return (neuron0x8794450()*0.923144);
}

double NNkaonS::synapse0x87971e0() {
   return (neuron0x8794648()*0.409907);
}

double NNkaonS::synapse0x8797208() {
   return (neuron0x8794840()*-0.26428);
}

double NNkaonS::synapse0x8797230() {
   return (neuron0x8794a38()*-0.415895);
}

double NNkaonS::synapse0x8797258() {
   return (neuron0x8794c30()*0.932953);
}

double NNkaonS::synapse0x8797280() {
   return (neuron0x8794e28()*-1.35653);
}

double NNkaonS::synapse0x87974a0() {
   return (neuron0x8795120()*0.959812);
}

double NNkaonS::synapse0x87974c8() {
   return (neuron0x8795418()*0.724814);
}

double NNkaonS::synapse0x87974f0() {
   return (neuron0x8795808()*0.856032);
}

double NNkaonS::synapse0x8797518() {
   return (neuron0x8795b28()*2.09086);
}

double NNkaonS::synapse0x8797540() {
   return (neuron0x8795e80()*-3.69201);
}

double NNkaonS::synapse0x8797568() {
   return (neuron0x87961e8()*-0.833962);
}

double NNkaonS::synapse0x8797590() {
   return (neuron0x8796550()*3.1773);
}

double NNkaonS::synapse0x87975b8() {
   return (neuron0x8795d78()*-2.9652);
}

double NNkaonS::synapse0x87975e0() {
   return (neuron0x8796b98()*2.20199);
}

double NNkaonS::synapse0x8797608() {
   return (neuron0x8796f20()*1.02518);
}

