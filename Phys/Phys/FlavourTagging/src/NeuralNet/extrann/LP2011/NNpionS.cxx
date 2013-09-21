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
         return neuron0x4618100();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x4aa9340() {
   return input0;
}

double NNpionS::neuron0x4aa94a0() {
   return input1;
}

double NNpionS::neuron0x45f8560() {
   return input2;
}

double NNpionS::neuron0x45f86c0() {
   return input3;
}

double NNpionS::neuron0x431e1d0() {
   return input4;
}

double NNpionS::neuron0x431e360() {
   return input5;
}

double NNpionS::neuron0x431e4f0() {
   return input6;
}

double NNpionS::input0x2bdf670() {
   double input = -0.130277;
   input += synapse0x3ab3e00();
   input += synapse0x2bdf7d0();
   input += synapse0x2bdf810();
   input += synapse0x2bdf850();
   input += synapse0x2bdf890();
   input += synapse0x2bdf8d0();
   input += synapse0x2bdf910();
   return input;
}

double NNpionS::neuron0x2bdf670() {
   double input = input0x2bdf670();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x2bdf950() {
   double input = -0.244119;
   input += synapse0x2bdfab0();
   input += synapse0x2bdfaf0();
   input += synapse0x2bdfb30();
   input += synapse0x2bdfb70();
   input += synapse0x3a8e500();
   input += synapse0x3a8e5e0();
   input += synapse0x3acf980();
   return input;
}

double NNpionS::neuron0x2bdf950() {
   double input = input0x2bdf950();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x44d61b0() {
   double input = -0.0427897;
   input += synapse0x44d6310();
   input += synapse0x44d6350();
   input += synapse0x44d6390();
   input += synapse0x3acfa60();
   input += synapse0x3acfb40();
   input += synapse0x3acfc20();
   input += synapse0x3acfd00();
   return input;
}

double NNpionS::neuron0x44d61b0() {
   double input = input0x44d61b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x44d63d0() {
   double input = 0.638919;
   input += synapse0x44d6530();
   input += synapse0x44d6570();
   input += synapse0x44d65b0();
   input += synapse0x44d65f0();
   input += synapse0x44d6630();
   input += synapse0x44d6670();
   input += synapse0x44d66b0();
   return input;
}

double NNpionS::neuron0x44d63d0() {
   double input = input0x44d63d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x44d66f0() {
   double input = 0.985763;
   input += synapse0x3acfda0();
   input += synapse0x3acfe80();
   input += synapse0x3acff60();
   input += synapse0x3ad0000();
   input += synapse0x3ad00a0();
   input += synapse0x3ad0140();
   input += synapse0x3ad01e0();
   return input;
}

double NNpionS::neuron0x44d66f0() {
   double input = input0x44d66f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x4659b70() {
   double input = -0.275148;
   input += synapse0x4659cd0();
   input += synapse0x4659d10();
   input += synapse0x4659d50();
   input += synapse0x4659d90();
   input += synapse0x4659dd0();
   input += synapse0x4659e10();
   input += synapse0x4659e50();
   return input;
}

double NNpionS::neuron0x4659b70() {
   double input = input0x4659b70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x4659e90() {
   double input = -0.107208;
   input += synapse0x4659ff0();
   input += synapse0x465a030();
   input += synapse0x465a070();
   input += synapse0x3ad02c0();
   input += synapse0x3ad0360();
   input += synapse0x3ad0400();
   input += synapse0x3ad04a0();
   return input;
}

double NNpionS::neuron0x4659e90() {
   double input = input0x4659e90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x4617de0() {
   double input = -0.277569;
   input += synapse0x4617f40();
   input += synapse0x4617f80();
   input += synapse0x4617fc0();
   input += synapse0x4618000();
   input += synapse0x4618040();
   input += synapse0x4618080();
   input += synapse0x46180c0();
   return input;
}

double NNpionS::neuron0x4617de0() {
   double input = input0x4617de0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x4618100() {
   double input = -0.327603;
   input += synapse0x4618260();
   input += synapse0x46182a0();
   input += synapse0x46182e0();
   input += synapse0x4618320();
   input += synapse0x4618360();
   input += synapse0x46183a0();
   input += synapse0x46183e0();
   input += synapse0x4618420();
   return input;
}

double NNpionS::neuron0x4618100() {
   double input = input0x4618100();
   return (input * 0.496827)+0.556239;
}

double NNpionS::synapse0x3ab3e00() {
   return (neuron0x4aa9340()*0.91782);
}

double NNpionS::synapse0x2bdf7d0() {
   return (neuron0x4aa94a0()*-1.28067);
}

double NNpionS::synapse0x2bdf810() {
   return (neuron0x45f8560()*0.185795);
}

double NNpionS::synapse0x2bdf850() {
   return (neuron0x45f86c0()*0.473196);
}

double NNpionS::synapse0x2bdf890() {
   return (neuron0x431e1d0()*-3.54224);
}

double NNpionS::synapse0x2bdf8d0() {
   return (neuron0x431e360()*0.193146);
}

double NNpionS::synapse0x2bdf910() {
   return (neuron0x431e4f0()*0.642886);
}

double NNpionS::synapse0x2bdfab0() {
   return (neuron0x4aa9340()*-0.206495);
}

double NNpionS::synapse0x2bdfaf0() {
   return (neuron0x4aa94a0()*0.340573);
}

double NNpionS::synapse0x2bdfb30() {
   return (neuron0x45f8560()*0.494794);
}

double NNpionS::synapse0x2bdfb70() {
   return (neuron0x45f86c0()*0.254341);
}

double NNpionS::synapse0x3a8e500() {
   return (neuron0x431e1d0()*-0.456271);
}

double NNpionS::synapse0x3a8e5e0() {
   return (neuron0x431e360()*-0.204143);
}

double NNpionS::synapse0x3acf980() {
   return (neuron0x431e4f0()*0.00219258);
}

double NNpionS::synapse0x44d6310() {
   return (neuron0x4aa9340()*0.643693);
}

double NNpionS::synapse0x44d6350() {
   return (neuron0x4aa94a0()*-0.197524);
}

double NNpionS::synapse0x44d6390() {
   return (neuron0x45f8560()*0.444806);
}

double NNpionS::synapse0x3acfa60() {
   return (neuron0x45f86c0()*-0.33582);
}

double NNpionS::synapse0x3acfb40() {
   return (neuron0x431e1d0()*-0.846106);
}

double NNpionS::synapse0x3acfc20() {
   return (neuron0x431e360()*-0.200914);
}

double NNpionS::synapse0x3acfd00() {
   return (neuron0x431e4f0()*1.81522);
}

double NNpionS::synapse0x44d6530() {
   return (neuron0x4aa9340()*0.00473846);
}

double NNpionS::synapse0x44d6570() {
   return (neuron0x4aa94a0()*0.219643);
}

double NNpionS::synapse0x44d65b0() {
   return (neuron0x45f8560()*0.149561);
}

double NNpionS::synapse0x44d65f0() {
   return (neuron0x45f86c0()*-0.3258);
}

double NNpionS::synapse0x44d6630() {
   return (neuron0x431e1d0()*0.255938);
}

double NNpionS::synapse0x44d6670() {
   return (neuron0x431e360()*-0.631191);
}

double NNpionS::synapse0x44d66b0() {
   return (neuron0x431e4f0()*0.587663);
}

double NNpionS::synapse0x3acfda0() {
   return (neuron0x4aa9340()*2.95504);
}

double NNpionS::synapse0x3acfe80() {
   return (neuron0x4aa94a0()*-1.51391);
}

double NNpionS::synapse0x3acff60() {
   return (neuron0x45f8560()*0.267549);
}

double NNpionS::synapse0x3ad0000() {
   return (neuron0x45f86c0()*-0.0567116);
}

double NNpionS::synapse0x3ad00a0() {
   return (neuron0x431e1d0()*-3.10462);
}

double NNpionS::synapse0x3ad0140() {
   return (neuron0x431e360()*1.41365);
}

double NNpionS::synapse0x3ad01e0() {
   return (neuron0x431e4f0()*4.81217);
}

double NNpionS::synapse0x4659cd0() {
   return (neuron0x4aa9340()*0.171489);
}

double NNpionS::synapse0x4659d10() {
   return (neuron0x4aa94a0()*0.0105438);
}

double NNpionS::synapse0x4659d50() {
   return (neuron0x45f8560()*-0.121998);
}

double NNpionS::synapse0x4659d90() {
   return (neuron0x45f86c0()*0.491658);
}

double NNpionS::synapse0x4659dd0() {
   return (neuron0x431e1d0()*-1.30851);
}

double NNpionS::synapse0x4659e10() {
   return (neuron0x431e360()*-1.41299);
}

double NNpionS::synapse0x4659e50() {
   return (neuron0x431e4f0()*-0.331852);
}

double NNpionS::synapse0x4659ff0() {
   return (neuron0x4aa9340()*-0.102577);
}

double NNpionS::synapse0x465a030() {
   return (neuron0x4aa94a0()*-0.355367);
}

double NNpionS::synapse0x465a070() {
   return (neuron0x45f8560()*0.280432);
}

double NNpionS::synapse0x3ad02c0() {
   return (neuron0x45f86c0()*0.44947);
}

double NNpionS::synapse0x3ad0360() {
   return (neuron0x431e1d0()*-0.137014);
}

double NNpionS::synapse0x3ad0400() {
   return (neuron0x431e360()*0.441919);
}

double NNpionS::synapse0x3ad04a0() {
   return (neuron0x431e4f0()*-0.939669);
}

double NNpionS::synapse0x4617f40() {
   return (neuron0x4aa9340()*-1.83635);
}

double NNpionS::synapse0x4617f80() {
   return (neuron0x4aa94a0()*0.27966);
}

double NNpionS::synapse0x4617fc0() {
   return (neuron0x45f8560()*-0.357896);
}

double NNpionS::synapse0x4618000() {
   return (neuron0x45f86c0()*0.109848);
}

double NNpionS::synapse0x4618040() {
   return (neuron0x431e1d0()*1.86969);
}

double NNpionS::synapse0x4618080() {
   return (neuron0x431e360()*0.768377);
}

double NNpionS::synapse0x46180c0() {
   return (neuron0x431e4f0()*-2.45794);
}

double NNpionS::synapse0x4618260() {
   return (neuron0x2bdf670()*-1.92675);
}

double NNpionS::synapse0x46182a0() {
   return (neuron0x2bdf950()*0.363466);
}

double NNpionS::synapse0x46182e0() {
   return (neuron0x44d61b0()*0.818816);
}

double NNpionS::synapse0x4618320() {
   return (neuron0x44d63d0()*1.43224);
}

double NNpionS::synapse0x4618360() {
   return (neuron0x44d66f0()*-1.67907);
}

double NNpionS::synapse0x46183a0() {
   return (neuron0x4659b70()*3.95114);
}

double NNpionS::synapse0x46183e0() {
   return (neuron0x4659e90()*-0.832443);
}

double NNpionS::synapse0x4618420() {
   return (neuron0x4617de0()*-1.26983);
}

