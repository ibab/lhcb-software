#include "NNkaonS.h"
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
         return neuron0x8795508();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x8783ac0() {
   return input0;
}

double NNkaonS::neuron0x8783c70() {
   return input1;
}

double NNkaonS::neuron0x87921c8() {
   return input2;
}

double NNkaonS::neuron0x87923a0() {
   return input3;
}

double NNkaonS::neuron0x8792578() {
   return input4;
}

double NNkaonS::neuron0x8792770() {
   return input5;
}

double NNkaonS::neuron0x8792968() {
   return input6;
}

double NNkaonS::neuron0x8792b60() {
   return input7;
}

double NNkaonS::neuron0x8792d58() {
   return input8;
}

double NNkaonS::neuron0x8792f50() {
   return input9;
}

double NNkaonS::input0x8793248() {
   double input = 1.31415;
   input += synapse0x877ff30();
   input += synapse0x87933d8();
   input += synapse0x8793400();
   input += synapse0x8793428();
   input += synapse0x8793450();
   input += synapse0x8793478();
   input += synapse0x87934a0();
   input += synapse0x87934c8();
   input += synapse0x87934f0();
   input += synapse0x8793518();
   return input;
}

double NNkaonS::neuron0x8793248() {
   double input = input0x8793248();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8793540() {
   double input = -0.228914;
   input += synapse0x8793718();
   input += synapse0x8793740();
   input += synapse0x8793768();
   input += synapse0x8793790();
   input += synapse0x87937b8();
   input += synapse0x87937e0();
   input += synapse0x8793808();
   input += synapse0x87938b8();
   input += synapse0x87938e0();
   input += synapse0x8793908();
   return input;
}

double NNkaonS::neuron0x8793540() {
   double input = input0x8793540();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8793930() {
   double input = 0.758786;
   input += synapse0x8793ac0();
   input += synapse0x8793ae8();
   input += synapse0x8793b10();
   input += synapse0x8793b38();
   input += synapse0x8793b60();
   input += synapse0x8793b88();
   input += synapse0x8793bb0();
   input += synapse0x8793bd8();
   input += synapse0x8793c00();
   input += synapse0x8793c28();
   return input;
}

double NNkaonS::neuron0x8793930() {
   double input = input0x8793930();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8793c50() {
   double input = -0.185668;
   input += synapse0x8793e28();
   input += synapse0x8793e50();
   input += synapse0x8793e78();
   input += synapse0x8793830();
   input += synapse0x8793858();
   input += synapse0x8793880();
   input += synapse0x8793fa8();
   input += synapse0x8793fd0();
   input += synapse0x8793ff8();
   input += synapse0x8794020();
   return input;
}

double NNkaonS::neuron0x8793c50() {
   double input = input0x8793c50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8794048() {
   double input = 0.192543;
   input += synapse0x8794220();
   input += synapse0x8794248();
   input += synapse0x8794270();
   input += synapse0x8794298();
   input += synapse0x87942c0();
   input += synapse0x87942e8();
   input += synapse0x8794310();
   input += synapse0x8794338();
   input += synapse0x8794360();
   input += synapse0x8794388();
   return input;
}

double NNkaonS::neuron0x8794048() {
   double input = input0x8794048();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87943b0() {
   double input = -0.146737;
   input += synapse0x8794588();
   input += synapse0x87945b0();
   input += synapse0x87945d8();
   input += synapse0x8794600();
   input += synapse0x8794628();
   input += synapse0x8794650();
   input += synapse0x8794678();
   input += synapse0x87946a0();
   input += synapse0x87946c8();
   input += synapse0x87946f0();
   return input;
}

double NNkaonS::neuron0x87943b0() {
   double input = input0x87943b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8794718() {
   double input = 0.565759;
   input += synapse0x8794978();
   input += synapse0x87949a0();
   input += synapse0x87949c8();
   input += synapse0x87949f0();
   input += synapse0x8794a18();
   input += synapse0x877ff58();
   input += synapse0x84a7b88();
   input += synapse0x84a7b10();
   input += synapse0x8421e60();
   input += synapse0x84a7a08();
   return input;
}

double NNkaonS::neuron0x8794718() {
   double input = input0x8794718();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8793ea0() {
   double input = 0.487206;
   input += synapse0x84a7d38();
   input += synapse0x84a7a30();
   input += synapse0x8794cd8();
   input += synapse0x8794d00();
   input += synapse0x8794d28();
   input += synapse0x8794d50();
   input += synapse0x8794d78();
   input += synapse0x8794da0();
   input += synapse0x8794dc8();
   input += synapse0x8794df0();
   return input;
}

double NNkaonS::neuron0x8793ea0() {
   double input = input0x8793ea0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8794e18() {
   double input = 0.875988;
   input += synapse0x8794ff0();
   input += synapse0x8795018();
   input += synapse0x8795040();
   input += synapse0x8795068();
   input += synapse0x8795090();
   input += synapse0x87950b8();
   input += synapse0x87950e0();
   input += synapse0x8795108();
   input += synapse0x8795130();
   input += synapse0x8795158();
   return input;
}

double NNkaonS::neuron0x8794e18() {
   double input = input0x8794e18();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795180() {
   double input = -0.378648;
   input += synapse0x8795378();
   input += synapse0x87953a0();
   input += synapse0x87953c8();
   input += synapse0x87953f0();
   input += synapse0x8795418();
   input += synapse0x8795440();
   input += synapse0x8795468();
   input += synapse0x8795490();
   input += synapse0x87954b8();
   input += synapse0x87954e0();
   return input;
}

double NNkaonS::neuron0x8795180() {
   double input = input0x8795180();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8795508() {
   double input = 1.05425;
   input += synapse0x8795608();
   input += synapse0x8795630();
   input += synapse0x8795658();
   input += synapse0x8795680();
   input += synapse0x87956a8();
   input += synapse0x87956d0();
   input += synapse0x87956f8();
   input += synapse0x8795720();
   input += synapse0x8795748();
   input += synapse0x8795770();
   return input;
}

double NNkaonS::neuron0x8795508() {
   double input = input0x8795508();
   return (input * 0.481375)+0.635195;
}

double NNkaonS::synapse0x877ff30() {
   return (neuron0x8783ac0()*4.51422);
}

double NNkaonS::synapse0x87933d8() {
   return (neuron0x8783c70()*0.246396);
}

double NNkaonS::synapse0x8793400() {
   return (neuron0x87921c8()*-0.792241);
}

double NNkaonS::synapse0x8793428() {
   return (neuron0x87923a0()*1.11276);
}

double NNkaonS::synapse0x8793450() {
   return (neuron0x8792578()*0.529257);
}

double NNkaonS::synapse0x8793478() {
   return (neuron0x8792770()*0.19219);
}

double NNkaonS::synapse0x87934a0() {
   return (neuron0x8792968()*2.47185);
}

double NNkaonS::synapse0x87934c8() {
   return (neuron0x8792b60()*-0.264941);
}

double NNkaonS::synapse0x87934f0() {
   return (neuron0x8792d58()*2.87305);
}

double NNkaonS::synapse0x8793518() {
   return (neuron0x8792f50()*1.21651);
}

double NNkaonS::synapse0x8793718() {
   return (neuron0x8783ac0()*-0.255889);
}

double NNkaonS::synapse0x8793740() {
   return (neuron0x8783c70()*0.19691);
}

double NNkaonS::synapse0x8793768() {
   return (neuron0x87921c8()*0.535193);
}

double NNkaonS::synapse0x8793790() {
   return (neuron0x87923a0()*0.344651);
}

double NNkaonS::synapse0x87937b8() {
   return (neuron0x8792578()*-0.25918);
}

double NNkaonS::synapse0x87937e0() {
   return (neuron0x8792770()*-0.461993);
}

double NNkaonS::synapse0x8793808() {
   return (neuron0x8792968()*-2.17805);
}

double NNkaonS::synapse0x87938b8() {
   return (neuron0x8792b60()*0.264837);
}

double NNkaonS::synapse0x87938e0() {
   return (neuron0x8792d58()*-0.238994);
}

double NNkaonS::synapse0x8793908() {
   return (neuron0x8792f50()*-1.26593);
}

double NNkaonS::synapse0x8793ac0() {
   return (neuron0x8783ac0()*0.51879);
}

double NNkaonS::synapse0x8793ae8() {
   return (neuron0x8783c70()*0.166312);
}

double NNkaonS::synapse0x8793b10() {
   return (neuron0x87921c8()*0.200703);
}

double NNkaonS::synapse0x8793b38() {
   return (neuron0x87923a0()*0.0853529);
}

double NNkaonS::synapse0x8793b60() {
   return (neuron0x8792578()*0.771169);
}

double NNkaonS::synapse0x8793b88() {
   return (neuron0x8792770()*0.294367);
}

double NNkaonS::synapse0x8793bb0() {
   return (neuron0x8792968()*-0.776181);
}

double NNkaonS::synapse0x8793bd8() {
   return (neuron0x8792b60()*-0.295897);
}

double NNkaonS::synapse0x8793c00() {
   return (neuron0x8792d58()*-0.719285);
}

double NNkaonS::synapse0x8793c28() {
   return (neuron0x8792f50()*0.939128);
}

double NNkaonS::synapse0x8793e28() {
   return (neuron0x8783ac0()*0.487698);
}

double NNkaonS::synapse0x8793e50() {
   return (neuron0x8783c70()*0.197546);
}

double NNkaonS::synapse0x8793e78() {
   return (neuron0x87921c8()*0.911408);
}

double NNkaonS::synapse0x8793830() {
   return (neuron0x87923a0()*-0.220644);
}

double NNkaonS::synapse0x8793858() {
   return (neuron0x8792578()*0.168798);
}

double NNkaonS::synapse0x8793880() {
   return (neuron0x8792770()*0.162702);
}

double NNkaonS::synapse0x8793fa8() {
   return (neuron0x8792968()*-1.17018);
}

double NNkaonS::synapse0x8793fd0() {
   return (neuron0x8792b60()*0.0252608);
}

double NNkaonS::synapse0x8793ff8() {
   return (neuron0x8792d58()*-0.260327);
}

double NNkaonS::synapse0x8794020() {
   return (neuron0x8792f50()*-0.381248);
}

double NNkaonS::synapse0x8794220() {
   return (neuron0x8783ac0()*0.78413);
}

double NNkaonS::synapse0x8794248() {
   return (neuron0x8783c70()*-0.137903);
}

double NNkaonS::synapse0x8794270() {
   return (neuron0x87921c8()*-0.989663);
}

double NNkaonS::synapse0x8794298() {
   return (neuron0x87923a0()*0.467226);
}

double NNkaonS::synapse0x87942c0() {
   return (neuron0x8792578()*-0.410199);
}

double NNkaonS::synapse0x87942e8() {
   return (neuron0x8792770()*0.460978);
}

double NNkaonS::synapse0x8794310() {
   return (neuron0x8792968()*1.38418);
}

double NNkaonS::synapse0x8794338() {
   return (neuron0x8792b60()*-0.152463);
}

double NNkaonS::synapse0x8794360() {
   return (neuron0x8792d58()*0.7473);
}

double NNkaonS::synapse0x8794388() {
   return (neuron0x8792f50()*-0.0568734);
}

double NNkaonS::synapse0x8794588() {
   return (neuron0x8783ac0()*0.366118);
}

double NNkaonS::synapse0x87945b0() {
   return (neuron0x8783c70()*0.060658);
}

double NNkaonS::synapse0x87945d8() {
   return (neuron0x87921c8()*-0.26024);
}

double NNkaonS::synapse0x8794600() {
   return (neuron0x87923a0()*-0.400524);
}

double NNkaonS::synapse0x8794628() {
   return (neuron0x8792578()*0.0489896);
}

double NNkaonS::synapse0x8794650() {
   return (neuron0x8792770()*-0.295621);
}

double NNkaonS::synapse0x8794678() {
   return (neuron0x8792968()*0.216828);
}

double NNkaonS::synapse0x87946a0() {
   return (neuron0x8792b60()*0.27929);
}

double NNkaonS::synapse0x87946c8() {
   return (neuron0x8792d58()*-0.142582);
}

double NNkaonS::synapse0x87946f0() {
   return (neuron0x8792f50()*-0.00790141);
}

double NNkaonS::synapse0x8794978() {
   return (neuron0x8783ac0()*0.173168);
}

double NNkaonS::synapse0x87949a0() {
   return (neuron0x8783c70()*0.554236);
}

double NNkaonS::synapse0x87949c8() {
   return (neuron0x87921c8()*-0.0568179);
}

double NNkaonS::synapse0x87949f0() {
   return (neuron0x87923a0()*-0.0638318);
}

double NNkaonS::synapse0x8794a18() {
   return (neuron0x8792578()*0.121338);
}

double NNkaonS::synapse0x877ff58() {
   return (neuron0x8792770()*-0.0439839);
}

double NNkaonS::synapse0x84a7b88() {
   return (neuron0x8792968()*-0.908453);
}

double NNkaonS::synapse0x84a7b10() {
   return (neuron0x8792b60()*0.448251);
}

double NNkaonS::synapse0x8421e60() {
   return (neuron0x8792d58()*-0.359945);
}

double NNkaonS::synapse0x84a7a08() {
   return (neuron0x8792f50()*-1.03621);
}

double NNkaonS::synapse0x84a7d38() {
   return (neuron0x8783ac0()*-0.242657);
}

double NNkaonS::synapse0x84a7a30() {
   return (neuron0x8783c70()*0.375328);
}

double NNkaonS::synapse0x8794cd8() {
   return (neuron0x87921c8()*1.3328);
}

double NNkaonS::synapse0x8794d00() {
   return (neuron0x87923a0()*-0.197005);
}

double NNkaonS::synapse0x8794d28() {
   return (neuron0x8792578()*-0.419889);
}

double NNkaonS::synapse0x8794d50() {
   return (neuron0x8792770()*-0.163425);
}

double NNkaonS::synapse0x8794d78() {
   return (neuron0x8792968()*-0.562937);
}

double NNkaonS::synapse0x8794da0() {
   return (neuron0x8792b60()*-0.420306);
}

double NNkaonS::synapse0x8794dc8() {
   return (neuron0x8792d58()*0.1826);
}

double NNkaonS::synapse0x8794df0() {
   return (neuron0x8792f50()*-0.157782);
}

double NNkaonS::synapse0x8794ff0() {
   return (neuron0x8783ac0()*0.980339);
}

double NNkaonS::synapse0x8795018() {
   return (neuron0x8783c70()*0.338052);
}

double NNkaonS::synapse0x8795040() {
   return (neuron0x87921c8()*-2.11811);
}

double NNkaonS::synapse0x8795068() {
   return (neuron0x87923a0()*0.169558);
}

double NNkaonS::synapse0x8795090() {
   return (neuron0x8792578()*1.5969);
}

double NNkaonS::synapse0x87950b8() {
   return (neuron0x8792770()*0.396069);
}

double NNkaonS::synapse0x87950e0() {
   return (neuron0x8792968()*3.56145);
}

double NNkaonS::synapse0x8795108() {
   return (neuron0x8792b60()*0.328415);
}

double NNkaonS::synapse0x8795130() {
   return (neuron0x8792d58()*0.414997);
}

double NNkaonS::synapse0x8795158() {
   return (neuron0x8792f50()*-2.02637);
}

double NNkaonS::synapse0x8795378() {
   return (neuron0x8783ac0()*-0.795098);
}

double NNkaonS::synapse0x87953a0() {
   return (neuron0x8783c70()*-0.203042);
}

double NNkaonS::synapse0x87953c8() {
   return (neuron0x87921c8()*0.287966);
}

double NNkaonS::synapse0x87953f0() {
   return (neuron0x87923a0()*-0.389408);
}

double NNkaonS::synapse0x8795418() {
   return (neuron0x8792578()*-1.56345);
}

double NNkaonS::synapse0x8795440() {
   return (neuron0x8792770()*-0.71054);
}

double NNkaonS::synapse0x8795468() {
   return (neuron0x8792968()*3.61224);
}

double NNkaonS::synapse0x8795490() {
   return (neuron0x8792b60()*0.175416);
}

double NNkaonS::synapse0x87954b8() {
   return (neuron0x8792d58()*-1.5814);
}

double NNkaonS::synapse0x87954e0() {
   return (neuron0x8792f50()*-2.05497);
}

double NNkaonS::synapse0x8795608() {
   return (neuron0x8793248()*-2.69907);
}

double NNkaonS::synapse0x8795630() {
   return (neuron0x8793540()*1.67344);
}

double NNkaonS::synapse0x8795658() {
   return (neuron0x8793930()*0.736732);
}

double NNkaonS::synapse0x8795680() {
   return (neuron0x8793c50()*0.692786);
}

double NNkaonS::synapse0x87956a8() {
   return (neuron0x8794048()*-0.250836);
}

double NNkaonS::synapse0x87956d0() {
   return (neuron0x87943b0()*0.446405);
}

double NNkaonS::synapse0x87956f8() {
   return (neuron0x8794718()*1.23955);
}

double NNkaonS::synapse0x8795720() {
   return (neuron0x8793ea0()*0.742198);
}

double NNkaonS::synapse0x8795748() {
   return (neuron0x8794e18()*-1.55236);
}

double NNkaonS::synapse0x8795770() {
   return (neuron0x8795180()*-1.72154);
}

