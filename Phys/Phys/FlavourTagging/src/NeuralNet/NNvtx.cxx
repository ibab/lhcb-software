#include "NNvtx.h"
#include <cmath>

double NNvtx::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   input7 = (in7 - 0)/1;
   switch(index) {
     case 0:
         return neuron0xe4b2f50();
     default:
         return 0.;
   }
}

double NNvtx::neuron0xec3e8b0() {
   return input0;
}

double NNvtx::neuron0xea6fe00() {
   return input1;
}

double NNvtx::neuron0xea6ff60() {
   return input2;
}

double NNvtx::neuron0xea700c0() {
   return input3;
}

double NNvtx::neuron0xe472bf0() {
   return input4;
}

double NNvtx::neuron0xe472d80() {
   return input5;
}

double NNvtx::neuron0xe472f10() {
   return input6;
}

double NNvtx::neuron0xf2faf40() {
   return input7;
}

double NNvtx::input0xf2fb210() {
   double input = 0.113024;
   input += synapse0xe246af0();
   input += synapse0xf2fb370();
   input += synapse0xf2fb3b0();
   input += synapse0xf2fb3f0();
   input += synapse0xf2fb430();
   input += synapse0xf2fb470();
   input += synapse0xf2fb4b0();
   input += synapse0xe24c4d0();
   return input;
}

double NNvtx::neuron0xf2fb210() {
   double input = input0xf2fb210();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe46cb00() {
   double input = 0.22274;
   input += synapse0xe46cc60();
   input += synapse0xe46cca0();
   input += synapse0xe46cce0();
   input += synapse0xe46cd20();
   input += synapse0xe46cd60();
   input += synapse0xe46cda0();
   input += synapse0xe46cde0();
   input += synapse0xe46ce20();
   return input;
}

double NNvtx::neuron0xe46cb00() {
   double input = input0xe46cb00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe46ce60() {
   double input = 0.146766;
   input += synapse0xe46cfc0();
   input += synapse0xe24c5d0();
   input += synapse0xe2717a0();
   input += synapse0xe24c6d0();
   input += synapse0xe24c8d0();
   input += synapse0xe24cfd0();
   input += synapse0xe24d520();
   input += synapse0xe271ca0();
   return input;
}

double NNvtx::neuron0xe46ce60() {
   double input = input0xe46ce60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe470ae0() {
   double input = 0.296081;
   input += synapse0xe470c40();
   input += synapse0xe470c80();
   input += synapse0xe470cc0();
   input += synapse0xe470d00();
   input += synapse0xe470d40();
   input += synapse0xe470d80();
   input += synapse0xe470dc0();
   input += synapse0xe470e00();
   return input;
}

double NNvtx::neuron0xe470ae0() {
   double input = input0xe470ae0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe470e40() {
   double input = -0.496096;
   input += synapse0xe470fa0();
   input += synapse0xe4711f0();
   input += synapse0xe271da0();
   input += synapse0xe271ea0();
   input += synapse0xe24d870();
   input += synapse0xe24da70();
   input += synapse0xe2721f0();
   input += synapse0xe24dc70();
   return input;
}

double NNvtx::neuron0xe470e40() {
   double input = input0xe470e40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe46d000() {
   double input = 0.122854;
   input += synapse0xe46d160();
   input += synapse0xe46d1a0();
   input += synapse0xe24e1c0();
   input += synapse0xe24e6c0();
   input += synapse0xe24e910();
   input += synapse0xe2726f0();
   input += synapse0xe24eb10();
   input += synapse0xe24ec10();
   return input;
}

double NNvtx::neuron0xe46d000() {
   double input = input0xe46d000();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe632540() {
   double input = -0.21217;
   input += synapse0xe6326a0();
   input += synapse0xe6326e0();
   input += synapse0xe632720();
   input += synapse0xe632760();
   input += synapse0xe6327a0();
   input += synapse0xe6327e0();
   input += synapse0xe632820();
   input += synapse0xe632860();
   return input;
}

double NNvtx::neuron0xe632540() {
   double input = input0xe632540();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe6328a0() {
   double input = -0.75389;
   input += synapse0xe632a00();
   input += synapse0xe632a40();
   input += synapse0xe632a80();
   input += synapse0xe632ac0();
   input += synapse0xe632b00();
   input += synapse0xe632b40();
   input += synapse0xe632b80();
   input += synapse0xe632bc0();
   return input;
}

double NNvtx::neuron0xe6328a0() {
   double input = input0xe6328a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe632c00() {
   double input = -0.0500817;
   input += synapse0xe2729f0();
   input += synapse0xe272af0();
   input += synapse0xe256b10();
   input += synapse0xe272e90();
   input += synapse0xe272f90();
   input += synapse0xe273090();
   input += synapse0xe25e0f0();
   input += synapse0xe2741b0();
   return input;
}

double NNvtx::neuron0xe632c00() {
   double input = input0xe632c00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe470fe0() {
   double input = -0.214485;
   input += synapse0xe471140();
   input += synapse0xe471180();
   input += synapse0xe25e1f0();
   input += synapse0xe25e6f0();
   input += synapse0xe274510();
   input += synapse0xe25e7f0();
   input += synapse0xe25e9f0();
   input += synapse0xe25ebf0();
   return input;
}

double NNvtx::neuron0xe470fe0() {
   double input = input0xe470fe0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNvtx::input0xe4b2f50() {
   double input = -0.792628;
   input += synapse0xe4b30b0();
   input += synapse0xe4b30f0();
   input += synapse0xe4b3130();
   input += synapse0xe4b3170();
   input += synapse0xe4b31b0();
   input += synapse0xe4b31f0();
   input += synapse0xe4b3230();
   input += synapse0xe4b3270();
   input += synapse0xe4b32b0();
   input += synapse0xe4b32f0();
   return input;
}

double NNvtx::neuron0xe4b2f50() {
   double input = input0xe4b2f50();
   return (input * 0.480859)+0.637019;
}

double NNvtx::synapse0xe246af0() {
   return (neuron0xec3e8b0()*1.04385);
}

double NNvtx::synapse0xf2fb370() {
   return (neuron0xea6fe00()*0.291828);
}

double NNvtx::synapse0xf2fb3b0() {
   return (neuron0xea6ff60()*-0.145193);
}

double NNvtx::synapse0xf2fb3f0() {
   return (neuron0xea700c0()*-0.315399);
}

double NNvtx::synapse0xf2fb430() {
   return (neuron0xe472bf0()*0.204465);
}

double NNvtx::synapse0xf2fb470() {
   return (neuron0xe472d80()*0.106833);
}

double NNvtx::synapse0xf2fb4b0() {
   return (neuron0xe472f10()*0.119035);
}

double NNvtx::synapse0xe24c4d0() {
   return (neuron0xf2faf40()*-0.0333863);
}

double NNvtx::synapse0xe46cc60() {
   return (neuron0xec3e8b0()*-0.228094);
}

double NNvtx::synapse0xe46cca0() {
   return (neuron0xea6fe00()*0.529244);
}

double NNvtx::synapse0xe46cce0() {
   return (neuron0xea6ff60()*-0.136743);
}

double NNvtx::synapse0xe46cd20() {
   return (neuron0xea700c0()*-0.114549);
}

double NNvtx::synapse0xe46cd60() {
   return (neuron0xe472bf0()*-0.0993901);
}

double NNvtx::synapse0xe46cda0() {
   return (neuron0xe472d80()*0.243514);
}

double NNvtx::synapse0xe46cde0() {
   return (neuron0xe472f10()*0.102379);
}

double NNvtx::synapse0xe46ce20() {
   return (neuron0xf2faf40()*-0.416402);
}

double NNvtx::synapse0xe46cfc0() {
   return (neuron0xec3e8b0()*-0.86717);
}

double NNvtx::synapse0xe24c5d0() {
   return (neuron0xea6fe00()*-0.613125);
}

double NNvtx::synapse0xe2717a0() {
   return (neuron0xea6ff60()*0.207244);
}

double NNvtx::synapse0xe24c6d0() {
   return (neuron0xea700c0()*-0.00725826);
}

double NNvtx::synapse0xe24c8d0() {
   return (neuron0xe472bf0()*-0.481632);
}

double NNvtx::synapse0xe24cfd0() {
   return (neuron0xe472d80()*-0.57874);
}

double NNvtx::synapse0xe24d520() {
   return (neuron0xe472f10()*-0.466311);
}

double NNvtx::synapse0xe271ca0() {
   return (neuron0xf2faf40()*0.532939);
}

double NNvtx::synapse0xe470c40() {
   return (neuron0xec3e8b0()*-0.669262);
}

double NNvtx::synapse0xe470c80() {
   return (neuron0xea6fe00()*-0.0482671);
}

double NNvtx::synapse0xe470cc0() {
   return (neuron0xea6ff60()*-0.224742);
}

double NNvtx::synapse0xe470d00() {
   return (neuron0xea700c0()*0.0378034);
}

double NNvtx::synapse0xe470d40() {
   return (neuron0xe472bf0()*2.37278);
}

double NNvtx::synapse0xe470d80() {
   return (neuron0xe472d80()*1.63238);
}

double NNvtx::synapse0xe470dc0() {
   return (neuron0xe472f10()*0.167081);
}

double NNvtx::synapse0xe470e00() {
   return (neuron0xf2faf40()*-0.445996);
}

double NNvtx::synapse0xe470fa0() {
   return (neuron0xec3e8b0()*0.236426);
}

double NNvtx::synapse0xe4711f0() {
   return (neuron0xea6fe00()*-0.0305504);
}

double NNvtx::synapse0xe271da0() {
   return (neuron0xea6ff60()*-0.0949723);
}

double NNvtx::synapse0xe271ea0() {
   return (neuron0xea700c0()*-0.726025);
}

double NNvtx::synapse0xe24d870() {
   return (neuron0xe472bf0()*-0.947857);
}

double NNvtx::synapse0xe24da70() {
   return (neuron0xe472d80()*-0.952238);
}

double NNvtx::synapse0xe2721f0() {
   return (neuron0xe472f10()*-0.0174378);
}

double NNvtx::synapse0xe24dc70() {
   return (neuron0xf2faf40()*-2.01127);
}

double NNvtx::synapse0xe46d160() {
   return (neuron0xec3e8b0()*0.188196);
}

double NNvtx::synapse0xe46d1a0() {
   return (neuron0xea6fe00()*-0.307523);
}

double NNvtx::synapse0xe24e1c0() {
   return (neuron0xea6ff60()*-0.169022);
}

double NNvtx::synapse0xe24e6c0() {
   return (neuron0xea700c0()*1.0359);
}

double NNvtx::synapse0xe24e910() {
   return (neuron0xe472bf0()*1.16354);
}

double NNvtx::synapse0xe2726f0() {
   return (neuron0xe472d80()*-1.51558);
}

double NNvtx::synapse0xe24eb10() {
   return (neuron0xe472f10()*0.229716);
}

double NNvtx::synapse0xe24ec10() {
   return (neuron0xf2faf40()*2.55796);
}

double NNvtx::synapse0xe6326a0() {
   return (neuron0xec3e8b0()*-0.641609);
}

double NNvtx::synapse0xe6326e0() {
   return (neuron0xea6fe00()*-0.0485569);
}

double NNvtx::synapse0xe632720() {
   return (neuron0xea6ff60()*0.243902);
}

double NNvtx::synapse0xe632760() {
   return (neuron0xea700c0()*0.130364);
}

double NNvtx::synapse0xe6327a0() {
   return (neuron0xe472bf0()*0.144469);
}

double NNvtx::synapse0xe6327e0() {
   return (neuron0xe472d80()*-0.193016);
}

double NNvtx::synapse0xe632820() {
   return (neuron0xe472f10()*-0.290424);
}

double NNvtx::synapse0xe632860() {
   return (neuron0xf2faf40()*0.045016);
}

double NNvtx::synapse0xe632a00() {
   return (neuron0xec3e8b0()*0.459901);
}

double NNvtx::synapse0xe632a40() {
   return (neuron0xea6fe00()*0.226305);
}

double NNvtx::synapse0xe632a80() {
   return (neuron0xea6ff60()*-0.0206127);
}

double NNvtx::synapse0xe632ac0() {
   return (neuron0xea700c0()*-0.172016);
}

double NNvtx::synapse0xe632b00() {
   return (neuron0xe472bf0()*-1.81009);
}

double NNvtx::synapse0xe632b40() {
   return (neuron0xe472d80()*-0.335024);
}

double NNvtx::synapse0xe632b80() {
   return (neuron0xe472f10()*0.240629);
}

double NNvtx::synapse0xe632bc0() {
   return (neuron0xf2faf40()*-0.791507);
}

double NNvtx::synapse0xe2729f0() {
   return (neuron0xec3e8b0()*-0.52523);
}

double NNvtx::synapse0xe272af0() {
   return (neuron0xea6fe00()*-0.514635);
}

double NNvtx::synapse0xe256b10() {
   return (neuron0xea6ff60()*-0.394847);
}

double NNvtx::synapse0xe272e90() {
   return (neuron0xea700c0()*-0.245361);
}

double NNvtx::synapse0xe272f90() {
   return (neuron0xe472bf0()*-0.172978);
}

double NNvtx::synapse0xe273090() {
   return (neuron0xe472d80()*0.106919);
}

double NNvtx::synapse0xe25e0f0() {
   return (neuron0xe472f10()*0.311456);
}

double NNvtx::synapse0xe2741b0() {
   return (neuron0xf2faf40()*0.00956663);
}

double NNvtx::synapse0xe471140() {
   return (neuron0xec3e8b0()*0.230688);
}

double NNvtx::synapse0xe471180() {
   return (neuron0xea6fe00()*0.0299436);
}

double NNvtx::synapse0xe25e1f0() {
   return (neuron0xea6ff60()*-0.123749);
}

double NNvtx::synapse0xe25e6f0() {
   return (neuron0xea700c0()*0.725864);
}

double NNvtx::synapse0xe274510() {
   return (neuron0xe472bf0()*0.652281);
}

double NNvtx::synapse0xe25e7f0() {
   return (neuron0xe472d80()*-0.174784);
}

double NNvtx::synapse0xe25e9f0() {
   return (neuron0xe472f10()*0.739229);
}

double NNvtx::synapse0xe25ebf0() {
   return (neuron0xf2faf40()*0.296532);
}

double NNvtx::synapse0xe4b30b0() {
   return (neuron0xf2fb210()*-0.488972);
}

double NNvtx::synapse0xe4b30f0() {
   return (neuron0xe46cb00()*0.282608);
}

double NNvtx::synapse0xe4b3130() {
   return (neuron0xe46ce60()*-0.302899);
}

double NNvtx::synapse0xe4b3170() {
   return (neuron0xe470ae0()*1.07294);
}

double NNvtx::synapse0xe4b31b0() {
   return (neuron0xe470e40()*-1.0476);
}

double NNvtx::synapse0xe4b31f0() {
   return (neuron0xe46d000()*1.18666);
}

double NNvtx::synapse0xe4b3230() {
   return (neuron0xe632540()*-0.313852);
}

double NNvtx::synapse0xe4b3270() {
   return (neuron0xe6328a0()*-0.1662);
}

double NNvtx::synapse0xe4b32b0() {
   return (neuron0xe632c00()*0.412302);
}

double NNvtx::synapse0xe4b32f0() {
   return (neuron0xe470fe0()*-1.07119);
}

