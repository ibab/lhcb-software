#include "NNpionS.h"
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
         return neuron0x87805d0();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x8510b08() {
   return input0;
}

double NNpionS::neuron0x850ee78() {
   return input1;
}

double NNpionS::neuron0x850f070() {
   return input2;
}

double NNpionS::neuron0x850f268() {
   return input3;
}

double NNpionS::neuron0x850f460() {
   return input4;
}

double NNpionS::neuron0x877d900() {
   return input5;
}

double NNpionS::neuron0x877da90() {
   return input6;
}

double NNpionS::neuron0x877dc68() {
   return input7;
}

double NNpionS::neuron0x877de40() {
   return input8;
}

double NNpionS::neuron0x877e018() {
   return input9;
}

double NNpionS::input0x877e310() {
   double input = 0.427184;
   input += synapse0x8422870();
   input += synapse0x877e4a0();
   input += synapse0x877e4c8();
   input += synapse0x877e4f0();
   input += synapse0x877e518();
   input += synapse0x877e540();
   input += synapse0x877e568();
   input += synapse0x877e590();
   input += synapse0x877e5b8();
   input += synapse0x877e5e0();
   return input;
}

double NNpionS::neuron0x877e310() {
   double input = input0x877e310();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877e608() {
   double input = -0.526411;
   input += synapse0x877e7e0();
   input += synapse0x877e808();
   input += synapse0x877e830();
   input += synapse0x877e858();
   input += synapse0x877e880();
   input += synapse0x877e8a8();
   input += synapse0x877e8d0();
   input += synapse0x877e980();
   input += synapse0x877e9a8();
   input += synapse0x877e9d0();
   return input;
}

double NNpionS::neuron0x877e608() {
   double input = input0x877e608();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877e9f8() {
   double input = 1.19187;
   input += synapse0x877eb88();
   input += synapse0x877ebb0();
   input += synapse0x877ebd8();
   input += synapse0x877ec00();
   input += synapse0x877ec28();
   input += synapse0x877ec50();
   input += synapse0x877ec78();
   input += synapse0x877eca0();
   input += synapse0x877ecc8();
   input += synapse0x877ecf0();
   return input;
}

double NNpionS::neuron0x877e9f8() {
   double input = input0x877e9f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877ed18() {
   double input = -0.472499;
   input += synapse0x877eef0();
   input += synapse0x877ef18();
   input += synapse0x877ef40();
   input += synapse0x877e8f8();
   input += synapse0x877e920();
   input += synapse0x877e948();
   input += synapse0x877f070();
   input += synapse0x877f098();
   input += synapse0x877f0c0();
   input += synapse0x877f0e8();
   return input;
}

double NNpionS::neuron0x877ed18() {
   double input = input0x877ed18();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877f110() {
   double input = -0.0537484;
   input += synapse0x877f2e8();
   input += synapse0x877f310();
   input += synapse0x877f338();
   input += synapse0x877f360();
   input += synapse0x877f388();
   input += synapse0x877f3b0();
   input += synapse0x877f3d8();
   input += synapse0x877f400();
   input += synapse0x877f428();
   input += synapse0x877f450();
   return input;
}

double NNpionS::neuron0x877f110() {
   double input = input0x877f110();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877f478() {
   double input = 0.333347;
   input += synapse0x877f650();
   input += synapse0x877f678();
   input += synapse0x877f6a0();
   input += synapse0x877f6c8();
   input += synapse0x877f6f0();
   input += synapse0x877f718();
   input += synapse0x877f740();
   input += synapse0x877f768();
   input += synapse0x877f790();
   input += synapse0x877f7b8();
   return input;
}

double NNpionS::neuron0x877f478() {
   double input = input0x877f478();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877f7e0() {
   double input = -1.2247;
   input += synapse0x877fa40();
   input += synapse0x877fa68();
   input += synapse0x877fa90();
   input += synapse0x877fab8();
   input += synapse0x877fae0();
   input += synapse0x8510a00();
   input += synapse0x86d8c88();
   input += synapse0x86d8b40();
   input += synapse0x86d8c10();
   input += synapse0x8422898();
   return input;
}

double NNpionS::neuron0x877f7e0() {
   double input = input0x877f7e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877ef68() {
   double input = -0.37847;
   input += synapse0x86d8e38();
   input += synapse0x877fd58();
   input += synapse0x877fd80();
   input += synapse0x877fda8();
   input += synapse0x877fdd0();
   input += synapse0x877fdf8();
   input += synapse0x877fe20();
   input += synapse0x877fe48();
   input += synapse0x877fe70();
   input += synapse0x877fe98();
   return input;
}

double NNpionS::neuron0x877ef68() {
   double input = input0x877ef68();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x877fec0() {
   double input = 0.213181;
   input += synapse0x87800b8();
   input += synapse0x87800e0();
   input += synapse0x8780108();
   input += synapse0x8780130();
   input += synapse0x8780158();
   input += synapse0x8780180();
   input += synapse0x87801a8();
   input += synapse0x87801d0();
   input += synapse0x87801f8();
   input += synapse0x8780220();
   return input;
}

double NNpionS::neuron0x877fec0() {
   double input = input0x877fec0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8780248() {
   double input = 0.181653;
   input += synapse0x8780440();
   input += synapse0x8780468();
   input += synapse0x8780490();
   input += synapse0x87804b8();
   input += synapse0x87804e0();
   input += synapse0x8780508();
   input += synapse0x8780530();
   input += synapse0x8780558();
   input += synapse0x8780580();
   input += synapse0x87805a8();
   return input;
}

double NNpionS::neuron0x8780248() {
   double input = input0x8780248();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x87805d0() {
   double input = 0.058848;
   input += synapse0x87806d0();
   input += synapse0x87806f8();
   input += synapse0x8780720();
   input += synapse0x8780748();
   input += synapse0x8780770();
   input += synapse0x8780798();
   input += synapse0x87807c0();
   input += synapse0x87807e8();
   input += synapse0x8780810();
   input += synapse0x8780838();
   return input;
}

double NNpionS::neuron0x87805d0() {
   double input = input0x87805d0();
   return (input * 0.496711)+0.557253;
}

double NNpionS::synapse0x8422870() {
   return (neuron0x8510b08()*0.16886);
}

double NNpionS::synapse0x877e4a0() {
   return (neuron0x850ee78()*-0.130277);
}

double NNpionS::synapse0x877e4c8() {
   return (neuron0x850f070()*-0.693299);
}

double NNpionS::synapse0x877e4f0() {
   return (neuron0x850f268()*-0.114422);
}

double NNpionS::synapse0x877e518() {
   return (neuron0x850f460()*-0.0847816);
}

double NNpionS::synapse0x877e540() {
   return (neuron0x877d900()*0.00898321);
}

double NNpionS::synapse0x877e568() {
   return (neuron0x877da90()*0.578285);
}

double NNpionS::synapse0x877e590() {
   return (neuron0x877dc68()*0.284512);
}

double NNpionS::synapse0x877e5b8() {
   return (neuron0x877de40()*0.747665);
}

double NNpionS::synapse0x877e5e0() {
   return (neuron0x877e018()*-0.497184);
}

double NNpionS::synapse0x877e7e0() {
   return (neuron0x8510b08()*-0.532796);
}

double NNpionS::synapse0x877e808() {
   return (neuron0x850ee78()*2.30451);
}

double NNpionS::synapse0x877e830() {
   return (neuron0x850f070()*2.42089);
}

double NNpionS::synapse0x877e858() {
   return (neuron0x850f268()*0.976057);
}

double NNpionS::synapse0x877e880() {
   return (neuron0x850f460()*1.81459);
}

double NNpionS::synapse0x877e8a8() {
   return (neuron0x877d900()*1.08194);
}

double NNpionS::synapse0x877e8d0() {
   return (neuron0x877da90()*-2.4854);
}

double NNpionS::synapse0x877e980() {
   return (neuron0x877dc68()*0.807295);
}

double NNpionS::synapse0x877e9a8() {
   return (neuron0x877de40()*-0.0894749);
}

double NNpionS::synapse0x877e9d0() {
   return (neuron0x877e018()*-0.125206);
}

double NNpionS::synapse0x877eb88() {
   return (neuron0x8510b08()*2.13257);
}

double NNpionS::synapse0x877ebb0() {
   return (neuron0x850ee78()*0.678993);
}

double NNpionS::synapse0x877ebd8() {
   return (neuron0x850f070()*-1.25949);
}

double NNpionS::synapse0x877ec00() {
   return (neuron0x850f268()*-1.24247);
}

double NNpionS::synapse0x877ec28() {
   return (neuron0x850f460()*-0.0144085);
}

double NNpionS::synapse0x877ec50() {
   return (neuron0x877d900()*1.23205);
}

double NNpionS::synapse0x877ec78() {
   return (neuron0x877da90()*1.72381);
}

double NNpionS::synapse0x877eca0() {
   return (neuron0x877dc68()*1.89331);
}

double NNpionS::synapse0x877ecc8() {
   return (neuron0x877de40()*0.312563);
}

double NNpionS::synapse0x877ecf0() {
   return (neuron0x877e018()*0.481014);
}

double NNpionS::synapse0x877eef0() {
   return (neuron0x8510b08()*-0.046452);
}

double NNpionS::synapse0x877ef18() {
   return (neuron0x850ee78()*0.0236429);
}

double NNpionS::synapse0x877ef40() {
   return (neuron0x850f070()*0.57963);
}

double NNpionS::synapse0x877e8f8() {
   return (neuron0x850f268()*0.121481);
}

double NNpionS::synapse0x877e920() {
   return (neuron0x850f460()*-0.227885);
}

double NNpionS::synapse0x877e948() {
   return (neuron0x877d900()*-0.418915);
}

double NNpionS::synapse0x877f070() {
   return (neuron0x877da90()*-0.770198);
}

double NNpionS::synapse0x877f098() {
   return (neuron0x877dc68()*-0.138549);
}

double NNpionS::synapse0x877f0c0() {
   return (neuron0x877de40()*-0.0966881);
}

double NNpionS::synapse0x877f0e8() {
   return (neuron0x877e018()*0.231432);
}

double NNpionS::synapse0x877f2e8() {
   return (neuron0x8510b08()*-0.472074);
}

double NNpionS::synapse0x877f310() {
   return (neuron0x850ee78()*0.592385);
}

double NNpionS::synapse0x877f338() {
   return (neuron0x850f070()*-0.429303);
}

double NNpionS::synapse0x877f360() {
   return (neuron0x850f268()*-0.347055);
}

double NNpionS::synapse0x877f388() {
   return (neuron0x850f460()*-0.0445637);
}

double NNpionS::synapse0x877f3b0() {
   return (neuron0x877d900()*-0.0532026);
}

double NNpionS::synapse0x877f3d8() {
   return (neuron0x877da90()*0.561536);
}

double NNpionS::synapse0x877f400() {
   return (neuron0x877dc68()*0.227698);
}

double NNpionS::synapse0x877f428() {
   return (neuron0x877de40()*-0.673294);
}

double NNpionS::synapse0x877f450() {
   return (neuron0x877e018()*0.1165);
}

double NNpionS::synapse0x877f650() {
   return (neuron0x8510b08()*0.121814);
}

double NNpionS::synapse0x877f678() {
   return (neuron0x850ee78()*-0.568826);
}

double NNpionS::synapse0x877f6a0() {
   return (neuron0x850f070()*0.416346);
}

double NNpionS::synapse0x877f6c8() {
   return (neuron0x850f268()*0.263345);
}

double NNpionS::synapse0x877f6f0() {
   return (neuron0x850f460()*1.10694);
}

double NNpionS::synapse0x877f718() {
   return (neuron0x877d900()*-0.798195);
}

double NNpionS::synapse0x877f740() {
   return (neuron0x877da90()*-0.907191);
}

double NNpionS::synapse0x877f768() {
   return (neuron0x877dc68()*0.345947);
}

double NNpionS::synapse0x877f790() {
   return (neuron0x877de40()*-0.723573);
}

double NNpionS::synapse0x877f7b8() {
   return (neuron0x877e018()*1.12145);
}

double NNpionS::synapse0x877fa40() {
   return (neuron0x8510b08()*-0.676638);
}

double NNpionS::synapse0x877fa68() {
   return (neuron0x850ee78()*0.659132);
}

double NNpionS::synapse0x877fa90() {
   return (neuron0x850f070()*0.746185);
}

double NNpionS::synapse0x877fab8() {
   return (neuron0x850f268()*0.364405);
}

double NNpionS::synapse0x877fae0() {
   return (neuron0x850f460()*0.862252);
}

double NNpionS::synapse0x8510a00() {
   return (neuron0x877d900()*-0.389419);
}

double NNpionS::synapse0x86d8c88() {
   return (neuron0x877da90()*0.482083);
}

double NNpionS::synapse0x86d8b40() {
   return (neuron0x877dc68()*-1.18551);
}

double NNpionS::synapse0x86d8c10() {
   return (neuron0x877de40()*-0.17241);
}

double NNpionS::synapse0x8422898() {
   return (neuron0x877e018()*1.4784);
}

double NNpionS::synapse0x86d8e38() {
   return (neuron0x8510b08()*-0.193338);
}

double NNpionS::synapse0x877fd58() {
   return (neuron0x850ee78()*-0.270521);
}

double NNpionS::synapse0x877fd80() {
   return (neuron0x850f070()*0.458788);
}

double NNpionS::synapse0x877fda8() {
   return (neuron0x850f268()*-0.65507);
}

double NNpionS::synapse0x877fdd0() {
   return (neuron0x850f460()*-0.909985);
}

double NNpionS::synapse0x877fdf8() {
   return (neuron0x877d900()*-0.187334);
}

double NNpionS::synapse0x877fe20() {
   return (neuron0x877da90()*-1.22153);
}

double NNpionS::synapse0x877fe48() {
   return (neuron0x877dc68()*-1.12241);
}

double NNpionS::synapse0x877fe70() {
   return (neuron0x877de40()*0.383325);
}

double NNpionS::synapse0x877fe98() {
   return (neuron0x877e018()*0.193797);
}

double NNpionS::synapse0x87800b8() {
   return (neuron0x8510b08()*-0.321671);
}

double NNpionS::synapse0x87800e0() {
   return (neuron0x850ee78()*0.807616);
}

double NNpionS::synapse0x8780108() {
   return (neuron0x850f070()*-0.851736);
}

double NNpionS::synapse0x8780130() {
   return (neuron0x850f268()*-0.244679);
}

double NNpionS::synapse0x8780158() {
   return (neuron0x850f460()*1.27389);
}

double NNpionS::synapse0x8780180() {
   return (neuron0x877d900()*1.12484);
}

double NNpionS::synapse0x87801a8() {
   return (neuron0x877da90()*1.72504);
}

double NNpionS::synapse0x87801d0() {
   return (neuron0x877dc68()*-0.43222);
}

double NNpionS::synapse0x87801f8() {
   return (neuron0x877de40()*0.630672);
}

double NNpionS::synapse0x8780220() {
   return (neuron0x877e018()*-1.39814);
}

double NNpionS::synapse0x8780440() {
   return (neuron0x8510b08()*-0.00580139);
}

double NNpionS::synapse0x8780468() {
   return (neuron0x850ee78()*0.423684);
}

double NNpionS::synapse0x8780490() {
   return (neuron0x850f070()*1.58731);
}

double NNpionS::synapse0x87804b8() {
   return (neuron0x850f268()*0.584427);
}

double NNpionS::synapse0x87804e0() {
   return (neuron0x850f460()*0.402354);
}

double NNpionS::synapse0x8780508() {
   return (neuron0x877d900()*-0.758758);
}

double NNpionS::synapse0x8780530() {
   return (neuron0x877da90()*-0.990485);
}

double NNpionS::synapse0x8780558() {
   return (neuron0x877dc68()*-0.546954);
}

double NNpionS::synapse0x8780580() {
   return (neuron0x877de40()*0.171742);
}

double NNpionS::synapse0x87805a8() {
   return (neuron0x877e018()*0.412655);
}

double NNpionS::synapse0x87806d0() {
   return (neuron0x877e310()*0.342265);
}

double NNpionS::synapse0x87806f8() {
   return (neuron0x877e608()*1.53256);
}

double NNpionS::synapse0x8780720() {
   return (neuron0x877e9f8()*-1.50807);
}

double NNpionS::synapse0x8780748() {
   return (neuron0x877ed18()*0.167198);
}

double NNpionS::synapse0x8780770() {
   return (neuron0x877f110()*-0.369559);
}

double NNpionS::synapse0x8780798() {
   return (neuron0x877f478()*1.07961);
}

double NNpionS::synapse0x87807c0() {
   return (neuron0x877f7e0()*-0.930918);
}

double NNpionS::synapse0x87807e8() {
   return (neuron0x877ef68()*1.85851);
}

double NNpionS::synapse0x8780810() {
   return (neuron0x877fec0()*-0.637025);
}

double NNpionS::synapse0x8780838() {
   return (neuron0x8780248()*-0.87313);
}

