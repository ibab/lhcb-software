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
         return neuron0xa496358();
     default:
         return 0.;
   }
}

double NNkaon::neuron0xa477ac0() {
   return input0;
}

double NNkaon::neuron0xa477c08() {
   return input1;
}

double NNkaon::neuron0xa477e00() {
   return input2;
}

double NNkaon::neuron0xa495d90() {
   return input3;
}

double NNkaon::neuron0xa495f88() {
   return input4;
}

double NNkaon::neuron0xa496180() {
   return input5;
}

double NNkaon::input0xa496488() {
   double input = 0.443514;
   input += synapse0xa477a48();
   input += synapse0xa496618();
   input += synapse0xa496640();
   input += synapse0xa496668();
   input += synapse0xa496690();
   input += synapse0xa4966b8();
   return input;
}

double NNkaon::neuron0xa496488() {
   double input = input0xa496488();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa4966e0() {
   double input = -0.633816;
   input += synapse0xa4968b8();
   input += synapse0xa4968e0();
   input += synapse0xa496908();
   input += synapse0xa496930();
   input += synapse0xa496958();
   input += synapse0xa496980();
   return input;
}

double NNkaon::neuron0xa4966e0() {
   double input = input0xa4966e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa4969a8() {
   double input = 0.685285;
   input += synapse0xa496b80();
   input += synapse0xa496ba8();
   input += synapse0xa496bd0();
   input += synapse0xa496bf8();
   input += synapse0xa496c20();
   input += synapse0xa496cd0();
   return input;
}

double NNkaon::neuron0xa4969a8() {
   double input = input0xa4969a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa496cf8() {
   double input = -0.227163;
   input += synapse0xa496e88();
   input += synapse0xa496eb0();
   input += synapse0xa496ed8();
   input += synapse0xa496f00();
   input += synapse0xa496f28();
   input += synapse0xa496f50();
   return input;
}

double NNkaon::neuron0xa496cf8() {
   double input = input0xa496cf8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa496f78() {
   double input = 0.594407;
   input += synapse0xa497150();
   input += synapse0xa497178();
   input += synapse0xa4971a0();
   input += synapse0xa4971c8();
   input += synapse0xa4971f0();
   input += synapse0xa497218();
   return input;
}

double NNkaon::neuron0xa496f78() {
   double input = input0xa496f78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa497240() {
   double input = 0.376742;
   input += synapse0xa497438();
   input += synapse0xa497460();
   input += synapse0xa497488();
   input += synapse0xa477ff8();
   input += synapse0xa478020();
   input += synapse0xa478048();
   return input;
}

double NNkaon::neuron0xa497240() {
   double input = input0xa497240();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa4975b8() {
   double input = 0.382671;
   input += synapse0xa4977b0();
   input += synapse0xa4977d8();
   input += synapse0xa497800();
   input += synapse0xa497828();
   input += synapse0xa497850();
   input += synapse0xa497878();
   return input;
}

double NNkaon::neuron0xa4975b8() {
   double input = input0xa4975b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa4978a0() {
   double input = 0.26176;
   input += synapse0xa497a98();
   input += synapse0xa497ac0();
   input += synapse0xa497ae8();
   input += synapse0xa497b10();
   input += synapse0xa497b38();
   input += synapse0xa497b60();
   return input;
}

double NNkaon::neuron0xa4978a0() {
   double input = input0xa4978a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa497b88() {
   double input = 1.03289;
   input += synapse0xa497d80();
   input += synapse0xa497da8();
   input += synapse0xa497dd0();
   input += synapse0xa497df8();
   input += synapse0xa497e20();
   input += synapse0xa497e48();
   return input;
}

double NNkaon::neuron0xa497b88() {
   double input = input0xa497b88();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa497e70() {
   double input = 0.0449542;
   input += synapse0xa498068();
   input += synapse0xa498090();
   input += synapse0xa4980b8();
   input += synapse0xa4980e0();
   input += synapse0xa498108();
   input += synapse0xa498130();
   return input;
}

double NNkaon::neuron0xa497e70() {
   double input = input0xa497e70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaon::input0xa496358() {
   double input = -0.743094;
   input += synapse0xa4981e8();
   input += synapse0xa498210();
   input += synapse0xa498238();
   input += synapse0xa498260();
   input += synapse0xa498288();
   input += synapse0xa477938();
   input += synapse0xa42ef28();
   input += synapse0xa42eeb0();
   input += synapse0xa43b8b0();
   input += synapse0xa477a70();
   return input;
}

double NNkaon::neuron0xa496358() {
   double input = input0xa496358();
   return (input * 0.478483)+0.645099;
}

double NNkaon::synapse0xa477a48() {
   return (neuron0xa477ac0()*-1.6071);
}

double NNkaon::synapse0xa496618() {
   return (neuron0xa477c08()*-0.631503);
}

double NNkaon::synapse0xa496640() {
   return (neuron0xa477e00()*1.07223);
}

double NNkaon::synapse0xa496668() {
   return (neuron0xa495d90()*0.772805);
}

double NNkaon::synapse0xa496690() {
   return (neuron0xa495f88()*-0.477932);
}

double NNkaon::synapse0xa4966b8() {
   return (neuron0xa496180()*-0.725508);
}

double NNkaon::synapse0xa4968b8() {
   return (neuron0xa477ac0()*-0.0264566);
}

double NNkaon::synapse0xa4968e0() {
   return (neuron0xa477c08()*0.946678);
}

double NNkaon::synapse0xa496908() {
   return (neuron0xa477e00()*1.1812);
}

double NNkaon::synapse0xa496930() {
   return (neuron0xa495d90()*-0.609431);
}

double NNkaon::synapse0xa496958() {
   return (neuron0xa495f88()*-0.234401);
}

double NNkaon::synapse0xa496980() {
   return (neuron0xa496180()*-0.137977);
}

double NNkaon::synapse0xa496b80() {
   return (neuron0xa477ac0()*1.14232);
}

double NNkaon::synapse0xa496ba8() {
   return (neuron0xa477c08()*1.81211);
}

double NNkaon::synapse0xa496bd0() {
   return (neuron0xa477e00()*2.34952);
}

double NNkaon::synapse0xa496bf8() {
   return (neuron0xa495d90()*-4.26491);
}

double NNkaon::synapse0xa496c20() {
   return (neuron0xa495f88()*-1.19308);
}

double NNkaon::synapse0xa496cd0() {
   return (neuron0xa496180()*0.297606);
}

double NNkaon::synapse0xa496e88() {
   return (neuron0xa477ac0()*-0.50432);
}

double NNkaon::synapse0xa496eb0() {
   return (neuron0xa477c08()*-3.25364);
}

double NNkaon::synapse0xa496ed8() {
   return (neuron0xa477e00()*-1.3277);
}

double NNkaon::synapse0xa496f00() {
   return (neuron0xa495d90()*-4.46978);
}

double NNkaon::synapse0xa496f28() {
   return (neuron0xa495f88()*-0.167805);
}

double NNkaon::synapse0xa496f50() {
   return (neuron0xa496180()*0.765508);
}

double NNkaon::synapse0xa497150() {
   return (neuron0xa477ac0()*-0.930347);
}

double NNkaon::synapse0xa497178() {
   return (neuron0xa477c08()*-0.579583);
}

double NNkaon::synapse0xa4971a0() {
   return (neuron0xa477e00()*0.93213);
}

double NNkaon::synapse0xa4971c8() {
   return (neuron0xa495d90()*0.566753);
}

double NNkaon::synapse0xa4971f0() {
   return (neuron0xa495f88()*-0.294173);
}

double NNkaon::synapse0xa497218() {
   return (neuron0xa496180()*-0.105952);
}

double NNkaon::synapse0xa497438() {
   return (neuron0xa477ac0()*0.431159);
}

double NNkaon::synapse0xa497460() {
   return (neuron0xa477c08()*0.313728);
}

double NNkaon::synapse0xa497488() {
   return (neuron0xa477e00()*0.245075);
}

double NNkaon::synapse0xa477ff8() {
   return (neuron0xa495d90()*-0.15955);
}

double NNkaon::synapse0xa478020() {
   return (neuron0xa495f88()*0.459042);
}

double NNkaon::synapse0xa478048() {
   return (neuron0xa496180()*0.493392);
}

double NNkaon::synapse0xa4977b0() {
   return (neuron0xa477ac0()*0.0490159);
}

double NNkaon::synapse0xa4977d8() {
   return (neuron0xa477c08()*-1.22234);
}

double NNkaon::synapse0xa497800() {
   return (neuron0xa477e00()*-0.568875);
}

double NNkaon::synapse0xa497828() {
   return (neuron0xa495d90()*0.739603);
}

double NNkaon::synapse0xa497850() {
   return (neuron0xa495f88()*0.473503);
}

double NNkaon::synapse0xa497878() {
   return (neuron0xa496180()*0.449276);
}

double NNkaon::synapse0xa497a98() {
   return (neuron0xa477ac0()*0.793241);
}

double NNkaon::synapse0xa497ac0() {
   return (neuron0xa477c08()*-1.27013);
}

double NNkaon::synapse0xa497ae8() {
   return (neuron0xa477e00()*-1.05437);
}

double NNkaon::synapse0xa497b10() {
   return (neuron0xa495d90()*-3.17512);
}

double NNkaon::synapse0xa497b38() {
   return (neuron0xa495f88()*-0.0781801);
}

double NNkaon::synapse0xa497b60() {
   return (neuron0xa496180()*0.115739);
}

double NNkaon::synapse0xa497d80() {
   return (neuron0xa477ac0()*0.544013);
}

double NNkaon::synapse0xa497da8() {
   return (neuron0xa477c08()*1.14802);
}

double NNkaon::synapse0xa497dd0() {
   return (neuron0xa477e00()*0.487935);
}

double NNkaon::synapse0xa497df8() {
   return (neuron0xa495d90()*-0.698443);
}

double NNkaon::synapse0xa497e20() {
   return (neuron0xa495f88()*-1.58639);
}

double NNkaon::synapse0xa497e48() {
   return (neuron0xa496180()*0.299621);
}

double NNkaon::synapse0xa498068() {
   return (neuron0xa477ac0()*-1.33425);
}

double NNkaon::synapse0xa498090() {
   return (neuron0xa477c08()*-0.515702);
}

double NNkaon::synapse0xa4980b8() {
   return (neuron0xa477e00()*0.214287);
}

double NNkaon::synapse0xa4980e0() {
   return (neuron0xa495d90()*0.622275);
}

double NNkaon::synapse0xa498108() {
   return (neuron0xa495f88()*-1.30858);
}

double NNkaon::synapse0xa498130() {
   return (neuron0xa496180()*-0.603096);
}

double NNkaon::synapse0xa4981e8() {
   return (neuron0xa496488()*0.320911);
}

double NNkaon::synapse0xa498210() {
   return (neuron0xa4966e0()*-1.10186);
}

double NNkaon::synapse0xa498238() {
   return (neuron0xa4969a8()*1.96395);
}

double NNkaon::synapse0xa498260() {
   return (neuron0xa496cf8()*-2.92195);
}

double NNkaon::synapse0xa498288() {
   return (neuron0xa496f78()*0.0148886);
}

double NNkaon::synapse0xa477938() {
   return (neuron0xa497240()*-0.0900043);
}

double NNkaon::synapse0xa42ef28() {
   return (neuron0xa4975b8()*1.44409);
}

double NNkaon::synapse0xa42eeb0() {
   return (neuron0xa4978a0()*-1.2542);
}

double NNkaon::synapse0xa43b8b0() {
   return (neuron0xa497b88()*-1.35699);
}

double NNkaon::synapse0xa477a70() {
   return (neuron0xa497e70()*1.07147);
}

