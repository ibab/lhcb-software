#include "weights/NNpionS.h"
#include <cmath>

double NNpionS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x8789f70();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x87868c0() {
   return input0;
}

double NNpionS::neuron0x8786a70() {
   return input1;
}

double NNpionS::neuron0x8786c68() {
   return input2;
}

double NNpionS::neuron0x8786e60() {
   return input3;
}

double NNpionS::neuron0x8787058() {
   return input4;
}

double NNpionS::neuron0x8787250() {
   return input5;
}

double NNpionS::neuron0x8787448() {
   return input6;
}

double NNpionS::neuron0x8787640() {
   return input7;
}

double NNpionS::neuron0x8787838() {
   return input8;
}

double NNpionS::neuron0x8787a30() {
   return input9;
}

double NNpionS::input0x8787d28() {
   double input = -1.002;
   input += synapse0x8768690();
   input += synapse0x8787eb8();
   input += synapse0x8787ee0();
   input += synapse0x8787f08();
   input += synapse0x8787f30();
   input += synapse0x8787f58();
   input += synapse0x8787f80();
   input += synapse0x8787fa8();
   input += synapse0x8787fd0();
   input += synapse0x8787ff8();
   return input;
}

double NNpionS::neuron0x8787d28() {
   double input = input0x8787d28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788020() {
   double input = 0.0480501;
   input += synapse0x87881f8();
   input += synapse0x8788220();
   input += synapse0x8788248();
   input += synapse0x8788270();
   input += synapse0x8788298();
   input += synapse0x87882c0();
   input += synapse0x87882e8();
   input += synapse0x8788398();
   input += synapse0x87883c0();
   input += synapse0x87883e8();
   return input;
}

double NNpionS::neuron0x8788020() {
   double input = input0x8788020();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788410() {
   double input = -0.90514;
   input += synapse0x87885a0();
   input += synapse0x87885c8();
   input += synapse0x87885f0();
   input += synapse0x8788618();
   input += synapse0x8788640();
   input += synapse0x8788668();
   input += synapse0x8788690();
   input += synapse0x87886b8();
   input += synapse0x87886e0();
   input += synapse0x8788708();
   return input;
}

double NNpionS::neuron0x8788410() {
   double input = input0x8788410();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788730() {
   double input = -0.216557;
   input += synapse0x8788908();
   input += synapse0x8788930();
   input += synapse0x8788958();
   input += synapse0x8518c68();
   input += synapse0x8518c90();
   input += synapse0x8788310();
   input += synapse0x8788338();
   input += synapse0x8788360();
   input += synapse0x8788a88();
   input += synapse0x8788ab0();
   return input;
}

double NNpionS::neuron0x8788730() {
   double input = input0x8788730();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788ad8() {
   double input = -0.718739;
   input += synapse0x8788cb0();
   input += synapse0x8788cd8();
   input += synapse0x8788d00();
   input += synapse0x8788d28();
   input += synapse0x8788d50();
   input += synapse0x8788d78();
   input += synapse0x8788da0();
   input += synapse0x8788dc8();
   input += synapse0x8788df0();
   input += synapse0x8788e18();
   return input;
}

double NNpionS::neuron0x8788ad8() {
   double input = input0x8788ad8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788e40() {
   double input = -0.47483;
   input += synapse0x8789018();
   input += synapse0x8789040();
   input += synapse0x8789068();
   input += synapse0x8789090();
   input += synapse0x87890b8();
   input += synapse0x87890e0();
   input += synapse0x8789108();
   input += synapse0x8789130();
   input += synapse0x8789158();
   input += synapse0x8789180();
   return input;
}

double NNpionS::neuron0x8788e40() {
   double input = input0x8788e40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x87891a8() {
   double input = -0.318013;
   input += synapse0x8789408();
   input += synapse0x8789430();
   input += synapse0x8789458();
   input += synapse0x8789480();
   input += synapse0x87894a8();
   input += synapse0x8768aa8();
   input += synapse0x87686b8();
   input += synapse0x8518fd0();
   input += synapse0x8768a30();
   input += synapse0x8518f58();
   return input;
}

double NNpionS::neuron0x87891a8() {
   double input = input0x87891a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8788980() {
   double input = -0.0726077;
   input += synapse0x871b5b0();
   input += synapse0x8519180();
   input += synapse0x8789720();
   input += synapse0x8789748();
   input += synapse0x8789770();
   input += synapse0x8789798();
   input += synapse0x87897c0();
   input += synapse0x87897e8();
   input += synapse0x8789810();
   input += synapse0x8789838();
   return input;
}

double NNpionS::neuron0x8788980() {
   double input = input0x8788980();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8789860() {
   double input = 0.0751885;
   input += synapse0x8789a58();
   input += synapse0x8789a80();
   input += synapse0x8789aa8();
   input += synapse0x8789ad0();
   input += synapse0x8789af8();
   input += synapse0x8789b20();
   input += synapse0x8789b48();
   input += synapse0x8789b70();
   input += synapse0x8789b98();
   input += synapse0x8789bc0();
   return input;
}

double NNpionS::neuron0x8789860() {
   double input = input0x8789860();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8789be8() {
   double input = 0.689305;
   input += synapse0x8789de0();
   input += synapse0x8789e08();
   input += synapse0x8789e30();
   input += synapse0x8789e58();
   input += synapse0x8789e80();
   input += synapse0x8789ea8();
   input += synapse0x8789ed0();
   input += synapse0x8789ef8();
   input += synapse0x8789f20();
   input += synapse0x8789f48();
   return input;
}

double NNpionS::neuron0x8789be8() {
   double input = input0x8789be8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8789f70() {
   double input = -0.315196;
   input += synapse0x878a168();
   input += synapse0x878a190();
   input += synapse0x878a1b8();
   input += synapse0x878a1e0();
   input += synapse0x878a208();
   input += synapse0x878a230();
   input += synapse0x878a258();
   input += synapse0x878a280();
   input += synapse0x878a2a8();
   input += synapse0x878a2d0();
   return input;
}

double NNpionS::neuron0x8789f70() {
   double input = input0x8789f70();
   return (input * 0.496689)+0.557443;
}

double NNpionS::synapse0x8768690() {
   return (neuron0x87868c0()*0.306044);
}

double NNpionS::synapse0x8787eb8() {
   return (neuron0x8786a70()*0.461011);
}

double NNpionS::synapse0x8787ee0() {
   return (neuron0x8786c68()*-0.442481);
}

double NNpionS::synapse0x8787f08() {
   return (neuron0x8786e60()*-0.18983);
}

double NNpionS::synapse0x8787f30() {
   return (neuron0x8787058()*0.917022);
}

double NNpionS::synapse0x8787f58() {
   return (neuron0x8787250()*-0.264831);
}

double NNpionS::synapse0x8787f80() {
   return (neuron0x8787448()*0.434427);
}

double NNpionS::synapse0x8787fa8() {
   return (neuron0x8787640()*-0.360589);
}

double NNpionS::synapse0x8787fd0() {
   return (neuron0x8787838()*-0.0812085);
}

double NNpionS::synapse0x8787ff8() {
   return (neuron0x8787a30()*0.514848);
}

double NNpionS::synapse0x87881f8() {
   return (neuron0x87868c0()*-0.0808604);
}

double NNpionS::synapse0x8788220() {
   return (neuron0x8786a70()*0.0310892);
}

double NNpionS::synapse0x8788248() {
   return (neuron0x8786c68()*0.431497);
}

double NNpionS::synapse0x8788270() {
   return (neuron0x8786e60()*-0.111637);
}

double NNpionS::synapse0x8788298() {
   return (neuron0x8787058()*-0.478805);
}

double NNpionS::synapse0x87882c0() {
   return (neuron0x8787250()*0.423215);
}

double NNpionS::synapse0x87882e8() {
   return (neuron0x8787448()*-1.57532);
}

double NNpionS::synapse0x8788398() {
   return (neuron0x8787640()*0.121302);
}

double NNpionS::synapse0x87883c0() {
   return (neuron0x8787838()*-0.575461);
}

double NNpionS::synapse0x87883e8() {
   return (neuron0x8787a30()*0.0408064);
}

double NNpionS::synapse0x87885a0() {
   return (neuron0x87868c0()*0.746167);
}

double NNpionS::synapse0x87885c8() {
   return (neuron0x8786a70()*-0.294221);
}

double NNpionS::synapse0x87885f0() {
   return (neuron0x8786c68()*0.460539);
}

double NNpionS::synapse0x8788618() {
   return (neuron0x8786e60()*-0.273347);
}

double NNpionS::synapse0x8788640() {
   return (neuron0x8787058()*1.24161);
}

double NNpionS::synapse0x8788668() {
   return (neuron0x8787250()*-0.688133);
}

double NNpionS::synapse0x8788690() {
   return (neuron0x8787448()*-1.73096);
}

double NNpionS::synapse0x87886b8() {
   return (neuron0x8787640()*1.95021);
}

double NNpionS::synapse0x87886e0() {
   return (neuron0x8787838()*1.21334);
}

double NNpionS::synapse0x8788708() {
   return (neuron0x8787a30()*0.109924);
}

double NNpionS::synapse0x8788908() {
   return (neuron0x87868c0()*0.0909755);
}

double NNpionS::synapse0x8788930() {
   return (neuron0x8786a70()*-0.910872);
}

double NNpionS::synapse0x8788958() {
   return (neuron0x8786c68()*-1.74668);
}

double NNpionS::synapse0x8518c68() {
   return (neuron0x8786e60()*-0.0460948);
}

double NNpionS::synapse0x8518c90() {
   return (neuron0x8787058()*-0.584599);
}

double NNpionS::synapse0x8788310() {
   return (neuron0x8787250()*-0.723478);
}

double NNpionS::synapse0x8788338() {
   return (neuron0x8787448()*2.12195);
}

double NNpionS::synapse0x8788360() {
   return (neuron0x8787640()*0.363126);
}

double NNpionS::synapse0x8788a88() {
   return (neuron0x8787838()*-0.228446);
}

double NNpionS::synapse0x8788ab0() {
   return (neuron0x8787a30()*-1.98786);
}

double NNpionS::synapse0x8788cb0() {
   return (neuron0x87868c0()*-0.804238);
}

double NNpionS::synapse0x8788cd8() {
   return (neuron0x8786a70()*-0.136571);
}

double NNpionS::synapse0x8788d00() {
   return (neuron0x8786c68()*0.483275);
}

double NNpionS::synapse0x8788d28() {
   return (neuron0x8786e60()*0.507483);
}

double NNpionS::synapse0x8788d50() {
   return (neuron0x8787058()*0.300805);
}

double NNpionS::synapse0x8788d78() {
   return (neuron0x8787250()*0.415337);
}

double NNpionS::synapse0x8788da0() {
   return (neuron0x8787448()*-0.88165);
}

double NNpionS::synapse0x8788dc8() {
   return (neuron0x8787640()*-1.0622);
}

double NNpionS::synapse0x8788df0() {
   return (neuron0x8787838()*-0.961004);
}

double NNpionS::synapse0x8788e18() {
   return (neuron0x8787a30()*-0.309439);
}

double NNpionS::synapse0x8789018() {
   return (neuron0x87868c0()*-0.310938);
}

double NNpionS::synapse0x8789040() {
   return (neuron0x8786a70()*-0.404118);
}

double NNpionS::synapse0x8789068() {
   return (neuron0x8786c68()*-0.644434);
}

double NNpionS::synapse0x8789090() {
   return (neuron0x8786e60()*-0.263192);
}

double NNpionS::synapse0x87890b8() {
   return (neuron0x8787058()*0.403574);
}

double NNpionS::synapse0x87890e0() {
   return (neuron0x8787250()*-0.393815);
}

double NNpionS::synapse0x8789108() {
   return (neuron0x8787448()*1.30319);
}

double NNpionS::synapse0x8789130() {
   return (neuron0x8787640()*0.332945);
}

double NNpionS::synapse0x8789158() {
   return (neuron0x8787838()*0.343496);
}

double NNpionS::synapse0x8789180() {
   return (neuron0x8787a30()*0.12852);
}

double NNpionS::synapse0x8789408() {
   return (neuron0x87868c0()*-0.294396);
}

double NNpionS::synapse0x8789430() {
   return (neuron0x8786a70()*-0.00301968);
}

double NNpionS::synapse0x8789458() {
   return (neuron0x8786c68()*0.952862);
}

double NNpionS::synapse0x8789480() {
   return (neuron0x8786e60()*0.335314);
}

double NNpionS::synapse0x87894a8() {
   return (neuron0x8787058()*-0.270406);
}

double NNpionS::synapse0x8768aa8() {
   return (neuron0x8787250()*-0.239559);
}

double NNpionS::synapse0x87686b8() {
   return (neuron0x8787448()*-0.728792);
}

double NNpionS::synapse0x8518fd0() {
   return (neuron0x8787640()*0.326248);
}

double NNpionS::synapse0x8768a30() {
   return (neuron0x8787838()*0.589585);
}

double NNpionS::synapse0x8518f58() {
   return (neuron0x8787a30()*0.566069);
}

double NNpionS::synapse0x871b5b0() {
   return (neuron0x87868c0()*-0.464163);
}

double NNpionS::synapse0x8519180() {
   return (neuron0x8786a70()*-0.142627);
}

double NNpionS::synapse0x8789720() {
   return (neuron0x8786c68()*0.245877);
}

double NNpionS::synapse0x8789748() {
   return (neuron0x8786e60()*0.447386);
}

double NNpionS::synapse0x8789770() {
   return (neuron0x8787058()*-0.560955);
}

double NNpionS::synapse0x8789798() {
   return (neuron0x8787250()*-0.354786);
}

double NNpionS::synapse0x87897c0() {
   return (neuron0x8787448()*-1.25686);
}

double NNpionS::synapse0x87897e8() {
   return (neuron0x8787640()*-0.498371);
}

double NNpionS::synapse0x8789810() {
   return (neuron0x8787838()*-0.0213383);
}

double NNpionS::synapse0x8789838() {
   return (neuron0x8787a30()*-0.0101639);
}

double NNpionS::synapse0x8789a58() {
   return (neuron0x87868c0()*0.178272);
}

double NNpionS::synapse0x8789a80() {
   return (neuron0x8786a70()*0.202519);
}

double NNpionS::synapse0x8789aa8() {
   return (neuron0x8786c68()*0.178574);
}

double NNpionS::synapse0x8789ad0() {
   return (neuron0x8786e60()*0.146881);
}

double NNpionS::synapse0x8789af8() {
   return (neuron0x8787058()*0.546808);
}

double NNpionS::synapse0x8789b20() {
   return (neuron0x8787250()*0.30463);
}

double NNpionS::synapse0x8789b48() {
   return (neuron0x8787448()*-0.105874);
}

double NNpionS::synapse0x8789b70() {
   return (neuron0x8787640()*-0.267053);
}

double NNpionS::synapse0x8789b98() {
   return (neuron0x8787838()*0.326491);
}

double NNpionS::synapse0x8789bc0() {
   return (neuron0x8787a30()*-0.272692);
}

double NNpionS::synapse0x8789de0() {
   return (neuron0x87868c0()*0.769024);
}

double NNpionS::synapse0x8789e08() {
   return (neuron0x8786a70()*-0.262654);
}

double NNpionS::synapse0x8789e30() {
   return (neuron0x8786c68()*-0.517203);
}

double NNpionS::synapse0x8789e58() {
   return (neuron0x8786e60()*0.481112);
}

double NNpionS::synapse0x8789e80() {
   return (neuron0x8787058()*0.485222);
}

double NNpionS::synapse0x8789ea8() {
   return (neuron0x8787250()*0.490717);
}

double NNpionS::synapse0x8789ed0() {
   return (neuron0x8787448()*0.917141);
}

double NNpionS::synapse0x8789ef8() {
   return (neuron0x8787640()*0.920949);
}

double NNpionS::synapse0x8789f20() {
   return (neuron0x8787838()*0.0230226);
}

double NNpionS::synapse0x8789f48() {
   return (neuron0x8787a30()*0.0898824);
}

double NNpionS::synapse0x878a168() {
   return (neuron0x8787d28()*-0.774414);
}

double NNpionS::synapse0x878a190() {
   return (neuron0x8788020()*1.21918);
}

double NNpionS::synapse0x878a1b8() {
   return (neuron0x8788410()*1.43636);
}

double NNpionS::synapse0x878a1e0() {
   return (neuron0x8788730()*-1.59477);
}

double NNpionS::synapse0x878a208() {
   return (neuron0x8788ad8()*1.16984);
}

double NNpionS::synapse0x878a230() {
   return (neuron0x8788e40()*-0.797003);
}

double NNpionS::synapse0x878a258() {
   return (neuron0x87891a8()*-0.400184);
}

double NNpionS::synapse0x878a280() {
   return (neuron0x8788980()*1.26322);
}

double NNpionS::synapse0x878a2a8() {
   return (neuron0x8789860()*-0.266948);
}

double NNpionS::synapse0x878a2d0() {
   return (neuron0x8789be8()*-0.847019);
}

