#include "NNpionS.h"
#include <cmath>

double NNpionS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8c0f228();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x8ba6a10() {
   return input0;
}

double NNpionS::neuron0x8ba6b58() {
   return input1;
}

double NNpionS::neuron0x8ba6d30() {
   return input2;
}

double NNpionS::neuron0x898a738() {
   return input3;
}

double NNpionS::neuron0x898a930() {
   return input4;
}

double NNpionS::neuron0x8c0ee38() {
   return input5;
}

double NNpionS::neuron0x8c0f030() {
   return input6;
}

double NNpionS::input0x890f3a8() {
   double input = -0.721799;
   input += synapse0x85b0208();
   input += synapse0x890f538();
   input += synapse0x890f560();
   input += synapse0x890f588();
   input += synapse0x890f5b0();
   input += synapse0x890f5d8();
   input += synapse0x890f600();
   return input;
}

double NNpionS::neuron0x890f3a8() {
   double input = input0x890f3a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x890f628() {
   double input = 0.204345;
   input += synapse0x890f800();
   input += synapse0x890f828();
   input += synapse0x890f850();
   input += synapse0x890f878();
   input += synapse0x890f8a0();
   input += synapse0x890f8c8();
   input += synapse0x8ba6e78();
   return input;
}

double NNpionS::neuron0x890f628() {
   double input = input0x890f628();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x88f3d68() {
   double input = -0.0351791;
   input += synapse0x88f3f40();
   input += synapse0x88f3f68();
   input += synapse0x88f3f90();
   input += synapse0x88f4040();
   input += synapse0x88f4068();
   input += synapse0x88f4090();
   input += synapse0x88f40b8();
   return input;
}

double NNpionS::neuron0x88f3d68() {
   double input = input0x88f3d68();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x88f40e0() {
   double input = -0.740017;
   input += synapse0x88f4270();
   input += synapse0x898ab28();
   input += synapse0x898ab50();
   input += synapse0x898ab78();
   input += synapse0x891ce80();
   input += synapse0x891cea8();
   input += synapse0x891ced0();
   return input;
}

double NNpionS::neuron0x88f40e0() {
   double input = input0x88f40e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x891cef8() {
   double input = -0.0212334;
   input += synapse0x891d0d0();
   input += synapse0x891d0f8();
   input += synapse0x891d120();
   input += synapse0x891d148();
   input += synapse0x891d170();
   input += synapse0x88f3fb8();
   input += synapse0x88f3fe0();
   return input;
}

double NNpionS::neuron0x891cef8() {
   double input = input0x891cef8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x891d2a0() {
   double input = 0.585576;
   input += synapse0x8c103d8();
   input += synapse0x8c10400();
   input += synapse0x8c10428();
   input += synapse0x8c10450();
   input += synapse0x8c10478();
   input += synapse0x8c104a0();
   input += synapse0x8c104c8();
   return input;
}

double NNpionS::neuron0x891d2a0() {
   double input = input0x891d2a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8c104f0() {
   double input = -0.167308;
   input += synapse0x8c106c8();
   input += synapse0x8c106f0();
   input += synapse0x8c10718();
   input += synapse0x8c10740();
   input += synapse0x8c10768();
   input += synapse0x8c10790();
   input += synapse0x8c107b8();
   return input;
}

double NNpionS::neuron0x8c104f0() {
   double input = input0x8c104f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8c0f228() {
   double input = 0.289476;
   input += synapse0x8c108b8();
   input += synapse0x8c108e0();
   input += synapse0x8ba7fe8();
   input += synapse0x8ba8010();
   input += synapse0x8ba8038();
   input += synapse0x8ba8060();
   input += synapse0x8ba8088();
   return input;
}

double NNpionS::neuron0x8c0f228() {
   double input = input0x8c0f228();
   return (input * 0.497705)+0.547851;
}

double NNpionS::synapse0x85b0208() {
   return (neuron0x8ba6a10()*-0.948375);
}

double NNpionS::synapse0x890f538() {
   return (neuron0x8ba6b58()*-0.458527);
}

double NNpionS::synapse0x890f560() {
   return (neuron0x8ba6d30()*2.29839);
}

double NNpionS::synapse0x890f588() {
   return (neuron0x898a738()*0.404908);
}

double NNpionS::synapse0x890f5b0() {
   return (neuron0x898a930()*0.0400143);
}

double NNpionS::synapse0x890f5d8() {
   return (neuron0x8c0ee38()*-1.60591);
}

double NNpionS::synapse0x890f600() {
   return (neuron0x8c0f030()*-0.267925);
}

double NNpionS::synapse0x890f800() {
   return (neuron0x8ba6a10()*0.411926);
}

double NNpionS::synapse0x890f828() {
   return (neuron0x8ba6b58()*0.593631);
}

double NNpionS::synapse0x890f850() {
   return (neuron0x8ba6d30()*-1.05012);
}

double NNpionS::synapse0x890f878() {
   return (neuron0x898a738()*-0.0388521);
}

double NNpionS::synapse0x890f8a0() {
   return (neuron0x898a930()*0.292324);
}

double NNpionS::synapse0x890f8c8() {
   return (neuron0x8c0ee38()*1.00388);
}

double NNpionS::synapse0x8ba6e78() {
   return (neuron0x8c0f030()*1.71256);
}

double NNpionS::synapse0x88f3f40() {
   return (neuron0x8ba6a10()*0.816497);
}

double NNpionS::synapse0x88f3f68() {
   return (neuron0x8ba6b58()*1.10449);
}

double NNpionS::synapse0x88f3f90() {
   return (neuron0x8ba6d30()*-1.53016);
}

double NNpionS::synapse0x88f4040() {
   return (neuron0x898a738()*0.825792);
}

double NNpionS::synapse0x88f4068() {
   return (neuron0x898a930()*-0.0688984);
}

double NNpionS::synapse0x88f4090() {
   return (neuron0x8c0ee38()*1.30619);
}

double NNpionS::synapse0x88f40b8() {
   return (neuron0x8c0f030()*1.49551);
}

double NNpionS::synapse0x88f4270() {
   return (neuron0x8ba6a10()*0.153601);
}

double NNpionS::synapse0x898ab28() {
   return (neuron0x8ba6b58()*-0.340265);
}

double NNpionS::synapse0x898ab50() {
   return (neuron0x8ba6d30()*0.533843);
}

double NNpionS::synapse0x898ab78() {
   return (neuron0x898a738()*0.0323478);
}

double NNpionS::synapse0x891ce80() {
   return (neuron0x898a930()*-0.134412);
}

double NNpionS::synapse0x891cea8() {
   return (neuron0x8c0ee38()*-0.0691368);
}

double NNpionS::synapse0x891ced0() {
   return (neuron0x8c0f030()*-2.14241);
}

double NNpionS::synapse0x891d0d0() {
   return (neuron0x8ba6a10()*0.392295);
}

double NNpionS::synapse0x891d0f8() {
   return (neuron0x8ba6b58()*-0.0685067);
}

double NNpionS::synapse0x891d120() {
   return (neuron0x8ba6d30()*0.531245);
}

double NNpionS::synapse0x891d148() {
   return (neuron0x898a738()*-0.142545);
}

double NNpionS::synapse0x891d170() {
   return (neuron0x898a930()*0.137211);
}

double NNpionS::synapse0x88f3fb8() {
   return (neuron0x8c0ee38()*0.0931435);
}

double NNpionS::synapse0x88f3fe0() {
   return (neuron0x8c0f030()*-1.07535);
}

double NNpionS::synapse0x8c103d8() {
   return (neuron0x8ba6a10()*2.3091);
}

double NNpionS::synapse0x8c10400() {
   return (neuron0x8ba6b58()*0.298753);
}

double NNpionS::synapse0x8c10428() {
   return (neuron0x8ba6d30()*-2.15746);
}

double NNpionS::synapse0x8c10450() {
   return (neuron0x898a738()*0.772951);
}

double NNpionS::synapse0x8c10478() {
   return (neuron0x898a930()*-0.0102391);
}

double NNpionS::synapse0x8c104a0() {
   return (neuron0x8c0ee38()*1.97767);
}

double NNpionS::synapse0x8c104c8() {
   return (neuron0x8c0f030()*3.95349);
}

double NNpionS::synapse0x8c106c8() {
   return (neuron0x8ba6a10()*-0.354711);
}

double NNpionS::synapse0x8c106f0() {
   return (neuron0x8ba6b58()*-0.091695);
}

double NNpionS::synapse0x8c10718() {
   return (neuron0x8ba6d30()*-0.251027);
}

double NNpionS::synapse0x8c10740() {
   return (neuron0x898a738()*-0.0685042);
}

double NNpionS::synapse0x8c10768() {
   return (neuron0x898a930()*0.445152);
}

double NNpionS::synapse0x8c10790() {
   return (neuron0x8c0ee38()*0.207264);
}

double NNpionS::synapse0x8c107b8() {
   return (neuron0x8c0f030()*0.252589);
}

double NNpionS::synapse0x8c108b8() {
   return (neuron0x890f3a8()*-1.27094);
}

double NNpionS::synapse0x8c108e0() {
   return (neuron0x890f628()*0.115476);
}

double NNpionS::synapse0x8ba7fe8() {
   return (neuron0x88f3d68()*0.220656);
}

double NNpionS::synapse0x8ba8010() {
   return (neuron0x88f40e0()*2.06033);
}

double NNpionS::synapse0x8ba8038() {
   return (neuron0x891cef8()*1.9991);
}

double NNpionS::synapse0x8ba8060() {
   return (neuron0x891d2a0()*-2.27697);
}

double NNpionS::synapse0x8ba8088() {
   return (neuron0x8c104f0()*0.0789374);
}

