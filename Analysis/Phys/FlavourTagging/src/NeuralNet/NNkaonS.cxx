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
         return neuron0x8798808();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x8786e90() {
   return input0;
}

double NNkaonS::neuron0x8787088() {
   return input1;
}

double NNkaonS::neuron0x8787280() {
   return input2;
}

double NNkaonS::neuron0x8787478() {
   return input3;
}

double NNkaonS::neuron0x8795900() {
   return input4;
}

double NNkaonS::neuron0x8795ad8() {
   return input5;
}

double NNkaonS::neuron0x8795cb0() {
   return input6;
}

double NNkaonS::neuron0x8795e88() {
   return input7;
}

double NNkaonS::neuron0x8796080() {
   return input8;
}

double NNkaonS::neuron0x8796278() {
   return input9;
}

double NNkaonS::input0x8796570() {
   double input = 0.375628;
   input += synapse0x85599e0();
   input += synapse0x8796700();
   input += synapse0x8796728();
   input += synapse0x8796750();
   input += synapse0x8796778();
   input += synapse0x87967a0();
   input += synapse0x87967c8();
   input += synapse0x87967f0();
   input += synapse0x8796818();
   input += synapse0x8796840();
   return input;
}

double NNkaonS::neuron0x8796570() {
   double input = input0x8796570();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796868() {
   double input = -0.811007;
   input += synapse0x8796a40();
   input += synapse0x8796a68();
   input += synapse0x8796a90();
   input += synapse0x8796ab8();
   input += synapse0x8796ae0();
   input += synapse0x8796b08();
   input += synapse0x8796b30();
   input += synapse0x8796be0();
   input += synapse0x8796c08();
   input += synapse0x8796c30();
   return input;
}

double NNkaonS::neuron0x8796868() {
   double input = input0x8796868();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796c58() {
   double input = -0.242871;
   input += synapse0x8796de8();
   input += synapse0x8796e10();
   input += synapse0x8796e38();
   input += synapse0x8796e60();
   input += synapse0x8796e88();
   input += synapse0x8796eb0();
   input += synapse0x8796ed8();
   input += synapse0x8796f00();
   input += synapse0x8796f28();
   input += synapse0x8796f50();
   return input;
}

double NNkaonS::neuron0x8796c58() {
   double input = input0x8796c58();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8796f78() {
   double input = 0.474151;
   input += synapse0x8797150();
   input += synapse0x8797178();
   input += synapse0x87971a0();
   input += synapse0x8796b58();
   input += synapse0x8796b80();
   input += synapse0x8796ba8();
   input += synapse0x87972d0();
   input += synapse0x87972f8();
   input += synapse0x8797320();
   input += synapse0x8797348();
   return input;
}

double NNkaonS::neuron0x8796f78() {
   double input = input0x8796f78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8797370() {
   double input = -1.16152;
   input += synapse0x8797548();
   input += synapse0x8797570();
   input += synapse0x8797598();
   input += synapse0x87975c0();
   input += synapse0x87975e8();
   input += synapse0x8797610();
   input += synapse0x8797638();
   input += synapse0x8797660();
   input += synapse0x8797688();
   input += synapse0x87976b0();
   return input;
}

double NNkaonS::neuron0x8797370() {
   double input = input0x8797370();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87976d8() {
   double input = -0.226745;
   input += synapse0x87978b0();
   input += synapse0x87978d8();
   input += synapse0x8797900();
   input += synapse0x8797928();
   input += synapse0x8797950();
   input += synapse0x8797978();
   input += synapse0x87979a0();
   input += synapse0x87979c8();
   input += synapse0x87979f0();
   input += synapse0x8797a18();
   return input;
}

double NNkaonS::neuron0x87976d8() {
   double input = input0x87976d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8797a40() {
   double input = 0.916567;
   input += synapse0x8797ca0();
   input += synapse0x8797cc8();
   input += synapse0x8797cf0();
   input += synapse0x8797d18();
   input += synapse0x8797d40();
   input += synapse0x8559d78();
   input += synapse0x8559c20();
   input += synapse0x8559d00();
   input += synapse0x8559a08();
   input += synapse0x84224f8();
   return input;
}

double NNkaonS::neuron0x8797a40() {
   double input = input0x8797a40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87971c8() {
   double input = -0.723689;
   input += synapse0x8559f28();
   input += synapse0x8508cb0();
   input += synapse0x8797fb8();
   input += synapse0x8797fe0();
   input += synapse0x8798008();
   input += synapse0x8798030();
   input += synapse0x8798058();
   input += synapse0x8798080();
   input += synapse0x87980a8();
   input += synapse0x87980d0();
   return input;
}

double NNkaonS::neuron0x87971c8() {
   double input = input0x87971c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87980f8() {
   double input = -0.467672;
   input += synapse0x87982f0();
   input += synapse0x8798318();
   input += synapse0x8798340();
   input += synapse0x8798368();
   input += synapse0x8798390();
   input += synapse0x87983b8();
   input += synapse0x87983e0();
   input += synapse0x8798408();
   input += synapse0x8798430();
   input += synapse0x8798458();
   return input;
}

double NNkaonS::neuron0x87980f8() {
   double input = input0x87980f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8798480() {
   double input = -0.504529;
   input += synapse0x8798678();
   input += synapse0x87986a0();
   input += synapse0x87986c8();
   input += synapse0x87986f0();
   input += synapse0x8798718();
   input += synapse0x8798740();
   input += synapse0x8798768();
   input += synapse0x8798790();
   input += synapse0x87987b8();
   input += synapse0x87987e0();
   return input;
}

double NNkaonS::neuron0x8798480() {
   double input = input0x8798480();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8798808() {
   double input = -0.171509;
   input += synapse0x8798908();
   input += synapse0x8798930();
   input += synapse0x8798958();
   input += synapse0x8798980();
   input += synapse0x87989a8();
   input += synapse0x87989d0();
   input += synapse0x87989f8();
   input += synapse0x8798a20();
   input += synapse0x8798a48();
   input += synapse0x8798a70();
   return input;
}

double NNkaonS::neuron0x8798808() {
   double input = input0x8798808();
   return (input * 0.480764)+0.637354;
}

double NNkaonS::synapse0x85599e0() {
   return (neuron0x8786e90()*-0.710559);
}

double NNkaonS::synapse0x8796700() {
   return (neuron0x8787088()*-0.00610197);
}

double NNkaonS::synapse0x8796728() {
   return (neuron0x8787280()*0.363865);
}

double NNkaonS::synapse0x8796750() {
   return (neuron0x8787478()*-0.385319);
}

double NNkaonS::synapse0x8796778() {
   return (neuron0x8795900()*-0.503566);
}

double NNkaonS::synapse0x87967a0() {
   return (neuron0x8795ad8()*-0.652573);
}

double NNkaonS::synapse0x87967c8() {
   return (neuron0x8795cb0()*0.0481637);
}

double NNkaonS::synapse0x87967f0() {
   return (neuron0x8795e88()*0.00778118);
}

double NNkaonS::synapse0x8796818() {
   return (neuron0x8796080()*-0.251326);
}

double NNkaonS::synapse0x8796840() {
   return (neuron0x8796278()*0.518219);
}

double NNkaonS::synapse0x8796a40() {
   return (neuron0x8786e90()*-0.48081);
}

double NNkaonS::synapse0x8796a68() {
   return (neuron0x8787088()*0.411585);
}

double NNkaonS::synapse0x8796a90() {
   return (neuron0x8787280()*1.95314);
}

double NNkaonS::synapse0x8796ab8() {
   return (neuron0x8787478()*1.02785);
}

double NNkaonS::synapse0x8796ae0() {
   return (neuron0x8795900()*-1.31921);
}

double NNkaonS::synapse0x8796b08() {
   return (neuron0x8795ad8()*0.347622);
}

double NNkaonS::synapse0x8796b30() {
   return (neuron0x8795cb0()*-1.9971);
}

double NNkaonS::synapse0x8796be0() {
   return (neuron0x8795e88()*0.597943);
}

double NNkaonS::synapse0x8796c08() {
   return (neuron0x8796080()*1.85732);
}

double NNkaonS::synapse0x8796c30() {
   return (neuron0x8796278()*1.70332);
}

double NNkaonS::synapse0x8796de8() {
   return (neuron0x8786e90()*0.622409);
}

double NNkaonS::synapse0x8796e10() {
   return (neuron0x8787088()*-0.120594);
}

double NNkaonS::synapse0x8796e38() {
   return (neuron0x8787280()*-0.246051);
}

double NNkaonS::synapse0x8796e60() {
   return (neuron0x8787478()*0.461027);
}

double NNkaonS::synapse0x8796e88() {
   return (neuron0x8795900()*-1.91516);
}

double NNkaonS::synapse0x8796eb0() {
   return (neuron0x8795ad8()*0.537913);
}

double NNkaonS::synapse0x8796ed8() {
   return (neuron0x8795cb0()*1.42203);
}

double NNkaonS::synapse0x8796f00() {
   return (neuron0x8795e88()*-0.214491);
}

double NNkaonS::synapse0x8796f28() {
   return (neuron0x8796080()*-0.349081);
}

double NNkaonS::synapse0x8796f50() {
   return (neuron0x8796278()*-2.56975);
}

double NNkaonS::synapse0x8797150() {
   return (neuron0x8786e90()*0.464627);
}

double NNkaonS::synapse0x8797178() {
   return (neuron0x8787088()*-0.321208);
}

double NNkaonS::synapse0x87971a0() {
   return (neuron0x8787280()*0.472391);
}

double NNkaonS::synapse0x8796b58() {
   return (neuron0x8787478()*0.21473);
}

double NNkaonS::synapse0x8796b80() {
   return (neuron0x8795900()*0.224917);
}

double NNkaonS::synapse0x8796ba8() {
   return (neuron0x8795ad8()*-0.195295);
}

double NNkaonS::synapse0x87972d0() {
   return (neuron0x8795cb0()*-1.33372);
}

double NNkaonS::synapse0x87972f8() {
   return (neuron0x8795e88()*-0.443139);
}

double NNkaonS::synapse0x8797320() {
   return (neuron0x8796080()*-0.331996);
}

double NNkaonS::synapse0x8797348() {
   return (neuron0x8796278()*-0.198651);
}

double NNkaonS::synapse0x8797548() {
   return (neuron0x8786e90()*-0.13185);
}

double NNkaonS::synapse0x8797570() {
   return (neuron0x8787088()*-0.499494);
}

double NNkaonS::synapse0x8797598() {
   return (neuron0x8787280()*0.826584);
}

double NNkaonS::synapse0x87975c0() {
   return (neuron0x8787478()*-1.17833);
}

double NNkaonS::synapse0x87975e8() {
   return (neuron0x8795900()*-0.581099);
}

double NNkaonS::synapse0x8797610() {
   return (neuron0x8795ad8()*-0.886691);
}

double NNkaonS::synapse0x8797638() {
   return (neuron0x8795cb0()*-3.7478);
}

double NNkaonS::synapse0x8797660() {
   return (neuron0x8795e88()*-0.200936);
}

double NNkaonS::synapse0x8797688() {
   return (neuron0x8796080()*-2.27137);
}

double NNkaonS::synapse0x87976b0() {
   return (neuron0x8796278()*0.525607);
}

double NNkaonS::synapse0x87978b0() {
   return (neuron0x8786e90()*-0.0935354);
}

double NNkaonS::synapse0x87978d8() {
   return (neuron0x8787088()*0.363649);
}

double NNkaonS::synapse0x8797900() {
   return (neuron0x8787280()*-0.162411);
}

double NNkaonS::synapse0x8797928() {
   return (neuron0x8787478()*-0.052346);
}

double NNkaonS::synapse0x8797950() {
   return (neuron0x8795900()*0.260409);
}

double NNkaonS::synapse0x8797978() {
   return (neuron0x8795ad8()*-0.108418);
}

double NNkaonS::synapse0x87979a0() {
   return (neuron0x8795cb0()*-0.858101);
}

double NNkaonS::synapse0x87979c8() {
   return (neuron0x8795e88()*-0.0572249);
}

double NNkaonS::synapse0x87979f0() {
   return (neuron0x8796080()*0.116666);
}

double NNkaonS::synapse0x8797a18() {
   return (neuron0x8796278()*-0.142236);
}

double NNkaonS::synapse0x8797ca0() {
   return (neuron0x8786e90()*3.41328);
}

double NNkaonS::synapse0x8797cc8() {
   return (neuron0x8787088()*-0.0389411);
}

double NNkaonS::synapse0x8797cf0() {
   return (neuron0x8787280()*-0.211162);
}

double NNkaonS::synapse0x8797d18() {
   return (neuron0x8787478()*-0.17281);
}

double NNkaonS::synapse0x8797d40() {
   return (neuron0x8795900()*-0.85464);
}

double NNkaonS::synapse0x8559d78() {
   return (neuron0x8795ad8()*0.583042);
}

double NNkaonS::synapse0x8559c20() {
   return (neuron0x8795cb0()*0.412057);
}

double NNkaonS::synapse0x8559d00() {
   return (neuron0x8795e88()*-0.206601);
}

double NNkaonS::synapse0x8559a08() {
   return (neuron0x8796080()*0.737265);
}

double NNkaonS::synapse0x84224f8() {
   return (neuron0x8796278()*0.353149);
}

double NNkaonS::synapse0x8559f28() {
   return (neuron0x8786e90()*-0.413019);
}

double NNkaonS::synapse0x8508cb0() {
   return (neuron0x8787088()*-1.35553);
}

double NNkaonS::synapse0x8797fb8() {
   return (neuron0x8787280()*-0.906734);
}

double NNkaonS::synapse0x8797fe0() {
   return (neuron0x8787478()*-0.253459);
}

double NNkaonS::synapse0x8798008() {
   return (neuron0x8795900()*-0.00657234);
}

double NNkaonS::synapse0x8798030() {
   return (neuron0x8795ad8()*-0.248891);
}

double NNkaonS::synapse0x8798058() {
   return (neuron0x8795cb0()*1.52816);
}

double NNkaonS::synapse0x8798080() {
   return (neuron0x8795e88()*0.340038);
}

double NNkaonS::synapse0x87980a8() {
   return (neuron0x8796080()*0.427937);
}

double NNkaonS::synapse0x87980d0() {
   return (neuron0x8796278()*1.49505);
}

double NNkaonS::synapse0x87982f0() {
   return (neuron0x8786e90()*-0.480595);
}

double NNkaonS::synapse0x8798318() {
   return (neuron0x8787088()*0.508904);
}

double NNkaonS::synapse0x8798340() {
   return (neuron0x8787280()*0.0514988);
}

double NNkaonS::synapse0x8798368() {
   return (neuron0x8787478()*-0.277881);
}

double NNkaonS::synapse0x8798390() {
   return (neuron0x8795900()*-0.898821);
}

double NNkaonS::synapse0x87983b8() {
   return (neuron0x8795ad8()*-0.322519);
}

double NNkaonS::synapse0x87983e0() {
   return (neuron0x8795cb0()*0.417136);
}

double NNkaonS::synapse0x8798408() {
   return (neuron0x8795e88()*-0.36787);
}

double NNkaonS::synapse0x8798430() {
   return (neuron0x8796080()*0.189121);
}

double NNkaonS::synapse0x8798458() {
   return (neuron0x8796278()*1.01494);
}

double NNkaonS::synapse0x8798678() {
   return (neuron0x8786e90()*-0.0759789);
}

double NNkaonS::synapse0x87986a0() {
   return (neuron0x8787088()*-0.403877);
}

double NNkaonS::synapse0x87986c8() {
   return (neuron0x8787280()*0.234797);
}

double NNkaonS::synapse0x87986f0() {
   return (neuron0x8787478()*0.0699712);
}

double NNkaonS::synapse0x8798718() {
   return (neuron0x8795900()*-0.0763703);
}

double NNkaonS::synapse0x8798740() {
   return (neuron0x8795ad8()*-0.506963);
}

double NNkaonS::synapse0x8798768() {
   return (neuron0x8795cb0()*0.573667);
}

double NNkaonS::synapse0x8798790() {
   return (neuron0x8795e88()*-0.0168553);
}

double NNkaonS::synapse0x87987b8() {
   return (neuron0x8796080()*0.176863);
}

double NNkaonS::synapse0x87987e0() {
   return (neuron0x8796278()*1.22019);
}

double NNkaonS::synapse0x8798908() {
   return (neuron0x8796570()*-0.0797765);
}

double NNkaonS::synapse0x8798930() {
   return (neuron0x8796868()*1.66436);
}

double NNkaonS::synapse0x8798958() {
   return (neuron0x8796c58()*-0.930618);
}

double NNkaonS::synapse0x8798980() {
   return (neuron0x8796f78()*1.03071);
}

double NNkaonS::synapse0x87989a8() {
   return (neuron0x8797370()*2.64592);
}

double NNkaonS::synapse0x87989d0() {
   return (neuron0x87976d8()*0.816491);
}

double NNkaonS::synapse0x87989f8() {
   return (neuron0x8797a40()*-1.31786);
}

double NNkaonS::synapse0x8798a20() {
   return (neuron0x87971c8()*-1.25546);
}

double NNkaonS::synapse0x8798a48() {
   return (neuron0x87980f8()*-0.289599);
}

double NNkaonS::synapse0x8798a70() {
   return (neuron0x8798480()*-0.682968);
}

