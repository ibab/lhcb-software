#include "NNkaonS.h"
#include <cmath>

double NNkaonS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8a8d810();
     default:
         return 0.;
   }
}

double NNkaonS::neuron0x89f9e80() {
   return input0;
}

double NNkaonS::neuron0x87e5128() {
   return input1;
}

double NNkaonS::neuron0x87e5300() {
   return input2;
}

double NNkaonS::neuron0x8a8d030() {
   return input3;
}

double NNkaonS::neuron0x8a8d228() {
   return input4;
}

double NNkaonS::neuron0x8a8d420() {
   return input5;
}

double NNkaonS::neuron0x8a8d618() {
   return input6;
}

double NNkaonS::input0x8a8d930() {
   double input = 0.415338;
   input += synapse0x8a8e380();
   input += synapse0x8a8dac0();
   input += synapse0x8a8dae8();
   input += synapse0x8a8db10();
   input += synapse0x8a8db38();
   input += synapse0x8a8db60();
   input += synapse0x8a8db88();
   return input;
}

double NNkaonS::neuron0x8a8d930() {
   double input = input0x8a8d930();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8a8dbb0() {
   double input = 0.323676;
   input += synapse0x8a8dd88();
   input += synapse0x8a8ddb0();
   input += synapse0x8a8ddd8();
   input += synapse0x8727ea0();
   input += synapse0x8727ec8();
   input += synapse0x8727ef0();
   input += synapse0x8727f18();
   return input;
}

double NNkaonS::neuron0x8a8dbb0() {
   double input = input0x8a8dbb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8727f40() {
   double input = -0.404842;
   input += synapse0x8728118();
   input += synapse0x8728140();
   input += synapse0x8728168();
   input += synapse0x8728218();
   input += synapse0x8728240();
   input += synapse0x8728268();
   input += synapse0x8728290();
   return input;
}

double NNkaonS::neuron0x8727f40() {
   double input = input0x8727f40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x87282b8() {
   double input = 0.220572;
   input += synapse0x8728448();
   input += synapse0x8728470();
   input += synapse0x8728498();
   input += synapse0x87284c0();
   input += synapse0x87284e8();
   input += synapse0x8728510();
   input += synapse0x8728538();
   return input;
}

double NNkaonS::neuron0x87282b8() {
   double input = input0x87282b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8728560() {
   double input = 0.308054;
   input += synapse0x8728738();
   input += synapse0x8728760();
   input += synapse0x8728788();
   input += synapse0x87287b0();
   input += synapse0x87287d8();
   input += synapse0x8728190();
   input += synapse0x87281b8();
   return input;
}

double NNkaonS::neuron0x8728560() {
   double input = input0x8728560();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8728908() {
   double input = -0.162691;
   input += synapse0x8728ae0();
   input += synapse0x8728b08();
   input += synapse0x8728b30();
   input += synapse0x8728b58();
   input += synapse0x8728b80();
   input += synapse0x8728ba8();
   input += synapse0x8728bd0();
   return input;
}

double NNkaonS::neuron0x8728908() {
   double input = input0x8728908();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8728bf8() {
   double input = 0.219348;
   input += synapse0x8728dd0();
   input += synapse0x8728df8();
   input += synapse0x8728e20();
   input += synapse0x8728e48();
   input += synapse0x8728e70();
   input += synapse0x8728e98();
   input += synapse0x8728ec0();
   return input;
}

double NNkaonS::neuron0x8728bf8() {
   double input = input0x8728bf8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNkaonS::input0x8a8d810() {
   double input = -0.0355467;
   input += synapse0x8728fc0();
   input += synapse0x8728fe8();
   input += synapse0x8729010();
   input += synapse0x8729038();
   input += synapse0x8729060();
   input += synapse0x8729088();
   input += synapse0x87290b0();
   return input;
}

double NNkaonS::neuron0x8a8d810() {
   double input = input0x8a8d810();
   return (input * 0.478734)+0.64427;
}

double NNkaonS::synapse0x8a8e380() {
   return (neuron0x89f9e80()*2.79041);
}

double NNkaonS::synapse0x8a8dac0() {
   return (neuron0x87e5128()*-1.3011);
}

double NNkaonS::synapse0x8a8dae8() {
   return (neuron0x87e5300()*-2.46041);
}

double NNkaonS::synapse0x8a8db10() {
   return (neuron0x8a8d030()*-0.000892037);
}

double NNkaonS::synapse0x8a8db38() {
   return (neuron0x8a8d228()*1.23892);
}

double NNkaonS::synapse0x8a8db60() {
   return (neuron0x8a8d420()*1.30809);
}

double NNkaonS::synapse0x8a8db88() {
   return (neuron0x8a8d618()*-0.0318202);
}

double NNkaonS::synapse0x8a8dd88() {
   return (neuron0x89f9e80()*-2.65178);
}

double NNkaonS::synapse0x8a8ddb0() {
   return (neuron0x87e5128()*0.765837);
}

double NNkaonS::synapse0x8a8ddd8() {
   return (neuron0x87e5300()*1.49049);
}

double NNkaonS::synapse0x8727ea0() {
   return (neuron0x8a8d030()*-0.243483);
}

double NNkaonS::synapse0x8727ec8() {
   return (neuron0x8a8d228()*-0.759855);
}

double NNkaonS::synapse0x8727ef0() {
   return (neuron0x8a8d420()*-1.62199);
}

double NNkaonS::synapse0x8727f18() {
   return (neuron0x8a8d618()*0.325073);
}

double NNkaonS::synapse0x8728118() {
   return (neuron0x89f9e80()*-1.92412);
}

double NNkaonS::synapse0x8728140() {
   return (neuron0x87e5128()*0.758898);
}

double NNkaonS::synapse0x8728168() {
   return (neuron0x87e5300()*1.80895);
}

double NNkaonS::synapse0x8728218() {
   return (neuron0x8a8d030()*0.0967911);
}

double NNkaonS::synapse0x8728240() {
   return (neuron0x8a8d228()*-0.508353);
}

double NNkaonS::synapse0x8728268() {
   return (neuron0x8a8d420()*-0.638236);
}

double NNkaonS::synapse0x8728290() {
   return (neuron0x8a8d618()*-0.782752);
}

double NNkaonS::synapse0x8728448() {
   return (neuron0x89f9e80()*1.69789);
}

double NNkaonS::synapse0x8728470() {
   return (neuron0x87e5128()*-0.506108);
}

double NNkaonS::synapse0x8728498() {
   return (neuron0x87e5300()*-0.810314);
}

double NNkaonS::synapse0x87284c0() {
   return (neuron0x8a8d030()*0.00379044);
}

double NNkaonS::synapse0x87284e8() {
   return (neuron0x8a8d228()*-0.176733);
}

double NNkaonS::synapse0x8728510() {
   return (neuron0x8a8d420()*1.16978);
}

double NNkaonS::synapse0x8728538() {
   return (neuron0x8a8d618()*-1.96936);
}

double NNkaonS::synapse0x8728738() {
   return (neuron0x89f9e80()*3.07847);
}

double NNkaonS::synapse0x8728760() {
   return (neuron0x87e5128()*-1.77424);
}

double NNkaonS::synapse0x8728788() {
   return (neuron0x87e5300()*-2.1069);
}

double NNkaonS::synapse0x87287b0() {
   return (neuron0x8a8d030()*0.474752);
}

double NNkaonS::synapse0x87287d8() {
   return (neuron0x8a8d228()*0.461266);
}

double NNkaonS::synapse0x8728190() {
   return (neuron0x8a8d420()*2.15811);
}

double NNkaonS::synapse0x87281b8() {
   return (neuron0x8a8d618()*0.226231);
}

double NNkaonS::synapse0x8728ae0() {
   return (neuron0x89f9e80()*2.78676);
}

double NNkaonS::synapse0x8728b08() {
   return (neuron0x87e5128()*-0.945016);
}

double NNkaonS::synapse0x8728b30() {
   return (neuron0x87e5300()*-2.41395);
}

double NNkaonS::synapse0x8728b58() {
   return (neuron0x8a8d030()*0.35387);
}

double NNkaonS::synapse0x8728b80() {
   return (neuron0x8a8d228()*0.331165);
}

double NNkaonS::synapse0x8728ba8() {
   return (neuron0x8a8d420()*1.6991);
}

double NNkaonS::synapse0x8728bd0() {
   return (neuron0x8a8d618()*2.93477);
}

double NNkaonS::synapse0x8728dd0() {
   return (neuron0x89f9e80()*-1.35538);
}

double NNkaonS::synapse0x8728df8() {
   return (neuron0x87e5128()*0.454516);
}

double NNkaonS::synapse0x8728e20() {
   return (neuron0x87e5300()*0.957563);
}

double NNkaonS::synapse0x8728e48() {
   return (neuron0x8a8d030()*-0.0731791);
}

double NNkaonS::synapse0x8728e70() {
   return (neuron0x8a8d228()*0.117);
}

double NNkaonS::synapse0x8728e98() {
   return (neuron0x8a8d420()*-0.333278);
}

double NNkaonS::synapse0x8728ec0() {
   return (neuron0x8a8d618()*-0.773837);
}

double NNkaonS::synapse0x8728fc0() {
   return (neuron0x8a8d930()*1.44181);
}

double NNkaonS::synapse0x8728fe8() {
   return (neuron0x8a8dbb0()*-1.3328);
}

double NNkaonS::synapse0x8729010() {
   return (neuron0x8727f40()*0.579371);
}

double NNkaonS::synapse0x8729038() {
   return (neuron0x87282b8()*3.91404);
}

double NNkaonS::synapse0x8729060() {
   return (neuron0x8728560()*0.37054);
}

double NNkaonS::synapse0x8729088() {
   return (neuron0x8728908()*-5.70256);
}

double NNkaonS::synapse0x87290b0() {
   return (neuron0x8728bf8()*0.628377);
}

