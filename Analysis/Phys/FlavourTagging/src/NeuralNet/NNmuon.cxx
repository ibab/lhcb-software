#include "NNmuon.h"
#include <cmath>

double NNmuon::value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0xab15ed0();
     default:
         return 0.;
   }
}

double NNmuon::neuron0xab153a8() {
   return input0;
}

double NNmuon::neuron0xab154f0() {
   return input1;
}

double NNmuon::neuron0xab156e8() {
   return input2;
}

double NNmuon::neuron0xab158e0() {
   return input3;
}

double NNmuon::neuron0xab15ad8() {
   return input4;
}

double NNmuon::neuron0xab15cd0() {
   return input5;
}

double NNmuon::input0xab15fd8() {
   double input = -1.18821;
   input += synapse0xab15330();
   input += synapse0xab16168();
   input += synapse0xab16190();
   input += synapse0xab161b8();
   input += synapse0xab161e0();
   input += synapse0xab16208();
   return input;
}

double NNmuon::neuron0xab15fd8() {
   double input = input0xab15fd8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab16230() {
   double input = 0.820694;
   input += synapse0xab16408();
   input += synapse0xab16430();
   input += synapse0xab16458();
   input += synapse0xab16480();
   input += synapse0xab164a8();
   input += synapse0xab164d0();
   return input;
}

double NNmuon::neuron0xab16230() {
   double input = input0xab16230();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab164f8() {
   double input = -1.0834;
   input += synapse0xab166d0();
   input += synapse0xab166f8();
   input += synapse0xab16720();
   input += synapse0xab16748();
   input += synapse0xab16770();
   input += synapse0xab16820();
   return input;
}

double NNmuon::neuron0xab164f8() {
   double input = input0xab164f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab16848() {
   double input = -0.000156854;
   input += synapse0xab169d8();
   input += synapse0xab16a00();
   input += synapse0xab16a28();
   input += synapse0xab16a50();
   input += synapse0xab16a78();
   input += synapse0xab16aa0();
   return input;
}

double NNmuon::neuron0xab16848() {
   double input = input0xab16848();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab16ac8() {
   double input = -0.432602;
   input += synapse0xab16ca0();
   input += synapse0xab16cc8();
   input += synapse0xab16cf0();
   input += synapse0xab16d18();
   input += synapse0xab16d40();
   input += synapse0xab16d68();
   return input;
}

double NNmuon::neuron0xab16ac8() {
   double input = input0xab16ac8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab16d90() {
   double input = -0.964287;
   input += synapse0xab16f88();
   input += synapse0xab16fb0();
   input += synapse0xab16fd8();
   input += synapse0xab16798();
   input += synapse0xab167c0();
   input += synapse0xab167e8();
   return input;
}

double NNmuon::neuron0xab16d90() {
   double input = input0xab16d90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab17108() {
   double input = 0.738587;
   input += synapse0xab17300();
   input += synapse0xab17328();
   input += synapse0xab17350();
   input += synapse0xab17378();
   input += synapse0xab173a0();
   input += synapse0xab173c8();
   return input;
}

double NNmuon::neuron0xab17108() {
   double input = input0xab17108();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab173f0() {
   double input = -0.528775;
   input += synapse0xab175e8();
   input += synapse0xab17610();
   input += synapse0xab17638();
   input += synapse0xab17660();
   input += synapse0xab17688();
   input += synapse0xab176b0();
   return input;
}

double NNmuon::neuron0xab173f0() {
   double input = input0xab173f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab176d8() {
   double input = 0.54763;
   input += synapse0xab178d0();
   input += synapse0xab178f8();
   input += synapse0xab17920();
   input += synapse0xab17948();
   input += synapse0xab17970();
   input += synapse0xab17998();
   return input;
}

double NNmuon::neuron0xab176d8() {
   double input = input0xab176d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab179c0() {
   double input = -0.203257;
   input += synapse0xab17bb8();
   input += synapse0xab17be0();
   input += synapse0xab17c08();
   input += synapse0xab17c30();
   input += synapse0xab17c58();
   input += synapse0xab17c80();
   return input;
}

double NNmuon::neuron0xab179c0() {
   double input = input0xab179c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNmuon::input0xab15ed0() {
   double input = 1.06169;
   input += synapse0xab17da0();
   input += synapse0xab17dc8();
   input += synapse0xab17df0();
   input += synapse0xab17e18();
   input += synapse0xab17e40();
   input += synapse0xab1a168();
   input += synapse0xaaaee10();
   input += synapse0xaaaed98();
   input += synapse0xaad6238();
   input += synapse0xab15358();
   return input;
}

double NNmuon::neuron0xab15ed0() {
   double input = input0xab15ed0();
   return (input * 0.456577)+0.703808;
}

double NNmuon::synapse0xab15330() {
   return (neuron0xab153a8()*-2.63941);
}

double NNmuon::synapse0xab16168() {
   return (neuron0xab154f0()*-0.152904);
}

double NNmuon::synapse0xab16190() {
   return (neuron0xab156e8()*0.641466);
}

double NNmuon::synapse0xab161b8() {
   return (neuron0xab158e0()*-0.761854);
}

double NNmuon::synapse0xab161e0() {
   return (neuron0xab15ad8()*-0.211504);
}

double NNmuon::synapse0xab16208() {
   return (neuron0xab15cd0()*-0.720235);
}

double NNmuon::synapse0xab16408() {
   return (neuron0xab153a8()*0.883323);
}

double NNmuon::synapse0xab16430() {
   return (neuron0xab154f0()*-0.549741);
}

double NNmuon::synapse0xab16458() {
   return (neuron0xab156e8()*-2.96519);
}

double NNmuon::synapse0xab16480() {
   return (neuron0xab158e0()*-2.95522);
}

double NNmuon::synapse0xab164a8() {
   return (neuron0xab15ad8()*-1.08693);
}

double NNmuon::synapse0xab164d0() {
   return (neuron0xab15cd0()*0.0100934);
}

double NNmuon::synapse0xab166d0() {
   return (neuron0xab153a8()*-1.02094);
}

double NNmuon::synapse0xab166f8() {
   return (neuron0xab154f0()*-0.00695201);
}

double NNmuon::synapse0xab16720() {
   return (neuron0xab156e8()*-3.38574);
}

double NNmuon::synapse0xab16748() {
   return (neuron0xab158e0()*1.5654);
}

double NNmuon::synapse0xab16770() {
   return (neuron0xab15ad8()*0.46399);
}

double NNmuon::synapse0xab16820() {
   return (neuron0xab15cd0()*-2.28972);
}

double NNmuon::synapse0xab169d8() {
   return (neuron0xab153a8()*-0.678644);
}

double NNmuon::synapse0xab16a00() {
   return (neuron0xab154f0()*-0.524961);
}

double NNmuon::synapse0xab16a28() {
   return (neuron0xab156e8()*-4.75199);
}

double NNmuon::synapse0xab16a50() {
   return (neuron0xab158e0()*-2.59088);
}

double NNmuon::synapse0xab16a78() {
   return (neuron0xab15ad8()*-0.851473);
}

double NNmuon::synapse0xab16aa0() {
   return (neuron0xab15cd0()*-0.963707);
}

double NNmuon::synapse0xab16ca0() {
   return (neuron0xab153a8()*-0.520407);
}

double NNmuon::synapse0xab16cc8() {
   return (neuron0xab154f0()*0.00591055);
}

double NNmuon::synapse0xab16cf0() {
   return (neuron0xab156e8()*0.351898);
}

double NNmuon::synapse0xab16d18() {
   return (neuron0xab158e0()*1.41628);
}

double NNmuon::synapse0xab16d40() {
   return (neuron0xab15ad8()*0.313414);
}

double NNmuon::synapse0xab16d68() {
   return (neuron0xab15cd0()*0.521721);
}

double NNmuon::synapse0xab16f88() {
   return (neuron0xab153a8()*-0.159358);
}

double NNmuon::synapse0xab16fb0() {
   return (neuron0xab154f0()*-0.0791881);
}

double NNmuon::synapse0xab16fd8() {
   return (neuron0xab156e8()*1.23696);
}

double NNmuon::synapse0xab16798() {
   return (neuron0xab158e0()*0.631053);
}

double NNmuon::synapse0xab167c0() {
   return (neuron0xab15ad8()*0.319696);
}

double NNmuon::synapse0xab167e8() {
   return (neuron0xab15cd0()*-0.0266746);
}

double NNmuon::synapse0xab17300() {
   return (neuron0xab153a8()*0.174271);
}

double NNmuon::synapse0xab17328() {
   return (neuron0xab154f0()*-0.135022);
}

double NNmuon::synapse0xab17350() {
   return (neuron0xab156e8()*-0.0275693);
}

double NNmuon::synapse0xab17378() {
   return (neuron0xab158e0()*0.126976);
}

double NNmuon::synapse0xab173a0() {
   return (neuron0xab15ad8()*-0.520558);
}

double NNmuon::synapse0xab173c8() {
   return (neuron0xab15cd0()*-0.288999);
}

double NNmuon::synapse0xab175e8() {
   return (neuron0xab153a8()*-0.877804);
}

double NNmuon::synapse0xab17610() {
   return (neuron0xab154f0()*0.0644147);
}

double NNmuon::synapse0xab17638() {
   return (neuron0xab156e8()*-0.192202);
}

double NNmuon::synapse0xab17660() {
   return (neuron0xab158e0()*-0.473735);
}

double NNmuon::synapse0xab17688() {
   return (neuron0xab15ad8()*-0.328614);
}

double NNmuon::synapse0xab176b0() {
   return (neuron0xab15cd0()*-0.25092);
}

double NNmuon::synapse0xab178d0() {
   return (neuron0xab153a8()*0.022936);
}

double NNmuon::synapse0xab178f8() {
   return (neuron0xab154f0()*-0.666041);
}

double NNmuon::synapse0xab17920() {
   return (neuron0xab156e8()*-0.264706);
}

double NNmuon::synapse0xab17948() {
   return (neuron0xab158e0()*0.476342);
}

double NNmuon::synapse0xab17970() {
   return (neuron0xab15ad8()*0.224346);
}

double NNmuon::synapse0xab17998() {
   return (neuron0xab15cd0()*0.180214);
}

double NNmuon::synapse0xab17bb8() {
   return (neuron0xab153a8()*0.0111808);
}

double NNmuon::synapse0xab17be0() {
   return (neuron0xab154f0()*-0.822843);
}

double NNmuon::synapse0xab17c08() {
   return (neuron0xab156e8()*-2.55842);
}

double NNmuon::synapse0xab17c30() {
   return (neuron0xab158e0()*-0.665733);
}

double NNmuon::synapse0xab17c58() {
   return (neuron0xab15ad8()*-0.817437);
}

double NNmuon::synapse0xab17c80() {
   return (neuron0xab15cd0()*-0.782305);
}

double NNmuon::synapse0xab17da0() {
   return (neuron0xab15fd8()*1.3703);
}

double NNmuon::synapse0xab17dc8() {
   return (neuron0xab16230()*-2.43461);
}

double NNmuon::synapse0xab17df0() {
   return (neuron0xab164f8()*-2.35988);
}

double NNmuon::synapse0xab17e18() {
   return (neuron0xab16848()*-3.19644);
}

double NNmuon::synapse0xab17e40() {
   return (neuron0xab16ac8()*-1.27501);
}

double NNmuon::synapse0xab1a168() {
   return (neuron0xab16d90()*-1.69611);
}

double NNmuon::synapse0xaaaee10() {
   return (neuron0xab17108()*0.994775);
}

double NNmuon::synapse0xaaaed98() {
   return (neuron0xab173f0()*0.808367);
}

double NNmuon::synapse0xaad6238() {
   return (neuron0xab176d8()*0.184254);
}

double NNmuon::synapse0xab15358() {
   return (neuron0xab179c0()*-1.57698);
}

