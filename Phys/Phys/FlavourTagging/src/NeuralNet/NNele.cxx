#include "NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3,double in4) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x15e075c0();
     default:
         return 0.;
   }
}

double NNele::neuron0x1573d000() {
   return input0;
}

double NNele::neuron0x1574df40() {
   return input1;
}

double NNele::neuron0x1574e0d0() {
   return input2;
}

double NNele::neuron0x1574e260() {
   return input3;
}

double NNele::neuron0x1574e3f0() {
   return input4;
}

double NNele::input0x1574c070() {
   double input = -0.58054;
   input += synapse0x173bc930();
   input += synapse0x1a42efd0();
   input += synapse0x15cab9e0();
   input += synapse0x1738d9a0();
   input += synapse0x17473a80();
   return input;
}

double NNele::neuron0x1574c070() {
   double input = input0x1574c070();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x1574c200() {
   double input = 0.465004;
   input += synapse0x15d2d100();
   input += synapse0x17a18180();
   input += synapse0x15ca1c70();
   input += synapse0x168cce50();
   input += synapse0x15e49430();
   return input;
}

double NNele::neuron0x1574c200() {
   double input = input0x1574c200();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x1574c390() {
   double input = 0.200541;
   input += synapse0x1921e4e0();
   input += synapse0x15c5f150();
   input += synapse0x15c83410();
   input += synapse0x15ca1a70();
   input += synapse0x174bf300();
   return input;
}

double NNele::neuron0x1574c390() {
   double input = input0x1574c390();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x1574ff10() {
   double input = 0.218054;
   input += synapse0x177a5890();
   input += synapse0x16551b80();
   input += synapse0x164ed070();
   input += synapse0x16d7c5d0();
   input += synapse0x15c8ad10();
   return input;
}

double NNele::neuron0x1574ff10() {
   double input = input0x1574ff10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x157501b0() {
   double input = -0.412196;
   input += synapse0x193406e0();
   input += synapse0x186275d0();
   input += synapse0x18635190();
   input += synapse0x16853020();
   input += synapse0x1989a170();
   return input;
}

double NNele::neuron0x157501b0() {
   double input = input0x157501b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x15750340() {
   double input = 0.0612545;
   input += synapse0x1a446030();
   input += synapse0x16269bd0();
   input += synapse0x19890c00();
   input += synapse0x15cad4b0();
   input += synapse0x15cadd60();
   return input;
}

double NNele::neuron0x15750340() {
   double input = input0x15750340();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x157504d0() {
   double input = -0.119289;
   input += synapse0x1722c940();
   input += synapse0x1a3fc730();
   input += synapse0x15cfca20();
   input += synapse0x15d8ea20();
   input += synapse0x18246350();
   return input;
}

double NNele::neuron0x157504d0() {
   double input = input0x157504d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x15e075c0() {
   double input = -1.02064;
   input += synapse0x1872b7b0();
   input += synapse0x170d7a90();
   input += synapse0x169962d0();
   input += synapse0x16d3d780();
   input += synapse0x18252c80();
   input += synapse0x160676c0();
   input += synapse0x167cac20();
   return input;
}

double NNele::neuron0x15e075c0() {
   double input = input0x15e075c0();
   return (input * 0.481854)+0.633479;
}

double NNele::synapse0x173bc930() {
   return (neuron0x1573d000()*0.489504);
}

double NNele::synapse0x1a42efd0() {
   return (neuron0x1574df40()*0.203496);
}

double NNele::synapse0x15cab9e0() {
   return (neuron0x1574e0d0()*-0.791427);
}

double NNele::synapse0x1738d9a0() {
   return (neuron0x1574e260()*-0.511588);
}

double NNele::synapse0x17473a80() {
   return (neuron0x1574e3f0()*0.244597);
}

double NNele::synapse0x15d2d100() {
   return (neuron0x1573d000()*-1.11092);
}

double NNele::synapse0x17a18180() {
   return (neuron0x1574df40()*-1.38377);
}

double NNele::synapse0x15ca1c70() {
   return (neuron0x1574e0d0()*-0.0464115);
}

double NNele::synapse0x168cce50() {
   return (neuron0x1574e260()*0.247512);
}

double NNele::synapse0x15e49430() {
   return (neuron0x1574e3f0()*-0.853837);
}

double NNele::synapse0x1921e4e0() {
   return (neuron0x1573d000()*1.70919);
}

double NNele::synapse0x15c5f150() {
   return (neuron0x1574df40()*2.14718);
}

double NNele::synapse0x15c83410() {
   return (neuron0x1574e0d0()*1.3494);
}

double NNele::synapse0x15ca1a70() {
   return (neuron0x1574e260()*-1.13463);
}

double NNele::synapse0x174bf300() {
   return (neuron0x1574e3f0()*0.344649);
}

double NNele::synapse0x177a5890() {
   return (neuron0x1573d000()*0.18677);
}

double NNele::synapse0x16551b80() {
   return (neuron0x1574df40()*-0.180687);
}

double NNele::synapse0x164ed070() {
   return (neuron0x1574e0d0()*-0.181156);
}

double NNele::synapse0x16d7c5d0() {
   return (neuron0x1574e260()*-0.0620595);
}

double NNele::synapse0x15c8ad10() {
   return (neuron0x1574e3f0()*0.362024);
}

double NNele::synapse0x193406e0() {
   return (neuron0x1573d000()*1.24684);
}

double NNele::synapse0x186275d0() {
   return (neuron0x1574df40()*1.16719);
}

double NNele::synapse0x18635190() {
   return (neuron0x1574e0d0()*-0.872485);
}

double NNele::synapse0x16853020() {
   return (neuron0x1574e260()*-0.595198);
}

double NNele::synapse0x1989a170() {
   return (neuron0x1574e3f0()*0.0285524);
}

double NNele::synapse0x1a446030() {
   return (neuron0x1573d000()*-0.208473);
}

double NNele::synapse0x16269bd0() {
   return (neuron0x1574df40()*-0.111131);
}

double NNele::synapse0x19890c00() {
   return (neuron0x1574e0d0()*0.181593);
}

double NNele::synapse0x15cad4b0() {
   return (neuron0x1574e260()*-0.131917);
}

double NNele::synapse0x15cadd60() {
   return (neuron0x1574e3f0()*0.27773);
}

double NNele::synapse0x1722c940() {
   return (neuron0x1573d000()*-1.25111);
}

double NNele::synapse0x1a3fc730() {
   return (neuron0x1574df40()*-1.77965);
}

double NNele::synapse0x15cfca20() {
   return (neuron0x1574e0d0()*-0.07696);
}

double NNele::synapse0x15d8ea20() {
   return (neuron0x1574e260()*-0.440112);
}

double NNele::synapse0x18246350() {
   return (neuron0x1574e3f0()*0.163041);
}

double NNele::synapse0x1872b7b0() {
   return (neuron0x1574c070()*0.910084);
}

double NNele::synapse0x170d7a90() {
   return (neuron0x1574c200()*-0.298729);
}

double NNele::synapse0x169962d0() {
   return (neuron0x1574c390()*1.18184);
}

double NNele::synapse0x16d3d780() {
   return (neuron0x1574ff10()*0.192728);
}

double NNele::synapse0x18252c80() {
   return (neuron0x157501b0()*-0.103039);
}

double NNele::synapse0x160676c0() {
   return (neuron0x15750340()*-0.141426);
}

double NNele::synapse0x167cac20() {
   return (neuron0x157504d0()*-1.35274);
}

