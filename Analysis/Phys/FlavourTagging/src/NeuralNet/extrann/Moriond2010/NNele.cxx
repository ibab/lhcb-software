#include "NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x2440ab0();
     default:
         return 0.;
   }
}

double NNele::neuron0x243c890() {
   return input0;
}

double NNele::neuron0x243cba0() {
   return input1;
}

double NNele::neuron0x243ceb0() {
   return input2;
}

double NNele::neuron0x243d1f0() {
   return input3;
}

double NNele::neuron0x243d530() {
   return input4;
}

double NNele::neuron0x243d870() {
   return input5;
}

double NNele::input0x243dcf0() {
   double input = -0.283243;
   input += synapse0x215a360();
   input += synapse0x21a62c0();
   input += synapse0x243df70();
   input += synapse0x243dfb0();
   input += synapse0x243dff0();
   input += synapse0x243e030();
   return input;
}

double NNele::neuron0x243dcf0() {
   double input = input0x243dcf0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243e070() {
   double input = 0.525843;
   input += synapse0x243e380();
   input += synapse0x243e3c0();
   input += synapse0x243e400();
   input += synapse0x243e440();
   input += synapse0x243e480();
   input += synapse0x243e4c0();
   return input;
}

double NNele::neuron0x243e070() {
   double input = input0x243e070();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243e500() {
   double input = -1.21098;
   input += synapse0x243e810();
   input += synapse0x243e850();
   input += synapse0x243e890();
   input += synapse0x243e8d0();
   input += synapse0x243e910();
   input += synapse0x242ba50();
   return input;
}

double NNele::neuron0x243e500() {
   double input = input0x243e500();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243ea60() {
   double input = 2.02974;
   input += synapse0x243ece0();
   input += synapse0x243ed20();
   input += synapse0x243ed60();
   input += synapse0x243eda0();
   input += synapse0x243ede0();
   input += synapse0x243ee20();
   return input;
}

double NNele::neuron0x243ea60() {
   double input = input0x243ea60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243ee60() {
   double input = -0.655483;
   input += synapse0x243f170();
   input += synapse0x243f1b0();
   input += synapse0x243f1f0();
   input += synapse0x243f230();
   input += synapse0x243f270();
   input += synapse0x243f2b0();
   return input;
}

double NNele::neuron0x243ee60() {
   double input = input0x243ee60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243f2f0() {
   double input = -0.832765;
   input += synapse0x243f600();
   input += synapse0x243f640();
   input += synapse0x243f680();
   input += synapse0x21f3770();
   input += synapse0x2400f40();
   input += synapse0x215a550();
   return input;
}

double NNele::neuron0x243f2f0() {
   double input = input0x243f2f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243f8d0() {
   double input = -0.0354653;
   input += synapse0x243fb50();
   input += synapse0x243fb90();
   input += synapse0x243fbd0();
   input += synapse0x243fc10();
   input += synapse0x243fc50();
   input += synapse0x243fc90();
   return input;
}

double NNele::neuron0x243f8d0() {
   double input = input0x243f8d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x243fcd0() {
   double input = 0.708993;
   input += synapse0x243ffe0();
   input += synapse0x2440020();
   input += synapse0x2440060();
   input += synapse0x24400a0();
   input += synapse0x24400e0();
   input += synapse0x2440120();
   return input;
}

double NNele::neuron0x243fcd0() {
   double input = input0x243fcd0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x2440160() {
   double input = 0.532082;
   input += synapse0x2440470();
   input += synapse0x24404b0();
   input += synapse0x24404f0();
   input += synapse0x2440530();
   input += synapse0x2440570();
   input += synapse0x24405b0();
   return input;
}

double NNele::neuron0x2440160() {
   double input = input0x2440160();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x24405f0() {
   double input = 0.817401;
   input += synapse0x2440930();
   input += synapse0x2440970();
   input += synapse0x24409b0();
   input += synapse0x24409f0();
   input += synapse0x2440a30();
   input += synapse0x2440a70();
   return input;
}

double NNele::neuron0x24405f0() {
   double input = input0x24405f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x2440ab0() {
   double input = -2.14436;
   input += synapse0x2440df0();
   input += synapse0x2440e30();
   input += synapse0x2440e70();
   input += synapse0x2440eb0();
   input += synapse0x2440ef0();
   input += synapse0x243e950();
   input += synapse0x243e990();
   input += synapse0x243e9d0();
   input += synapse0x243ea10();
   input += synapse0x242bc60();
   return input;
}

double NNele::neuron0x2440ab0() {
   double input = input0x2440ab0();
   return (input * 0.468685)+0.674168;
}

double NNele::synapse0x215a360() {
   return (neuron0x243c890()*0.19769);
}

double NNele::synapse0x21a62c0() {
   return (neuron0x243cba0()*-0.0458592);
}

double NNele::synapse0x243df70() {
   return (neuron0x243ceb0()*-2.41819);
}

double NNele::synapse0x243dfb0() {
   return (neuron0x243d1f0()*0.0816226);
}

double NNele::synapse0x243dff0() {
   return (neuron0x243d530()*0.0964214);
}

double NNele::synapse0x243e030() {
   return (neuron0x243d870()*2.7169);
}

double NNele::synapse0x243e380() {
   return (neuron0x243c890()*0.3833);
}

double NNele::synapse0x243e3c0() {
   return (neuron0x243cba0()*0.679921);
}

double NNele::synapse0x243e400() {
   return (neuron0x243ceb0()*0.280447);
}

double NNele::synapse0x243e440() {
   return (neuron0x243d1f0()*-0.923503);
}

double NNele::synapse0x243e480() {
   return (neuron0x243d530()*-1.37836);
}

double NNele::synapse0x243e4c0() {
   return (neuron0x243d870()*1.59682);
}

double NNele::synapse0x243e810() {
   return (neuron0x243c890()*0.81729);
}

double NNele::synapse0x243e850() {
   return (neuron0x243cba0()*1.24361);
}

double NNele::synapse0x243e890() {
   return (neuron0x243ceb0()*-3.60977);
}

double NNele::synapse0x243e8d0() {
   return (neuron0x243d1f0()*-4.04122);
}

double NNele::synapse0x243e910() {
   return (neuron0x243d530()*-1.08706);
}

double NNele::synapse0x242ba50() {
   return (neuron0x243d870()*1.30658);
}

double NNele::synapse0x243ece0() {
   return (neuron0x243c890()*2.34389);
}

double NNele::synapse0x243ed20() {
   return (neuron0x243cba0()*1.96274);
}

double NNele::synapse0x243ed60() {
   return (neuron0x243ceb0()*4.7308);
}

double NNele::synapse0x243eda0() {
   return (neuron0x243d1f0()*-0.300776);
}

double NNele::synapse0x243ede0() {
   return (neuron0x243d530()*-2.00708);
}

double NNele::synapse0x243ee20() {
   return (neuron0x243d870()*7.00865);
}

double NNele::synapse0x243f170() {
   return (neuron0x243c890()*0.555721);
}

double NNele::synapse0x243f1b0() {
   return (neuron0x243cba0()*0.298054);
}

double NNele::synapse0x243f1f0() {
   return (neuron0x243ceb0()*1.44508);
}

double NNele::synapse0x243f230() {
   return (neuron0x243d1f0()*-0.846507);
}

double NNele::synapse0x243f270() {
   return (neuron0x243d530()*0.154057);
}

double NNele::synapse0x243f2b0() {
   return (neuron0x243d870()*-0.574164);
}

double NNele::synapse0x243f600() {
   return (neuron0x243c890()*-4.69789);
}

double NNele::synapse0x243f640() {
   return (neuron0x243cba0()*1.0576);
}

double NNele::synapse0x243f680() {
   return (neuron0x243ceb0()*-0.409768);
}

double NNele::synapse0x21f3770() {
   return (neuron0x243d1f0()*-2.09043);
}

double NNele::synapse0x2400f40() {
   return (neuron0x243d530()*-1.10052);
}

double NNele::synapse0x215a550() {
   return (neuron0x243d870()*-0.21493);
}

double NNele::synapse0x243fb50() {
   return (neuron0x243c890()*1.43477);
}

double NNele::synapse0x243fb90() {
   return (neuron0x243cba0()*0.83666);
}

double NNele::synapse0x243fbd0() {
   return (neuron0x243ceb0()*2.03645);
}

double NNele::synapse0x243fc10() {
   return (neuron0x243d1f0()*-1.31448);
}

double NNele::synapse0x243fc50() {
   return (neuron0x243d530()*2.23885);
}

double NNele::synapse0x243fc90() {
   return (neuron0x243d870()*-3.68643);
}

double NNele::synapse0x243ffe0() {
   return (neuron0x243c890()*1.05766);
}

double NNele::synapse0x2440020() {
   return (neuron0x243cba0()*-0.216125);
}

double NNele::synapse0x2440060() {
   return (neuron0x243ceb0()*0.76849);
}

double NNele::synapse0x24400a0() {
   return (neuron0x243d1f0()*0.568757);
}

double NNele::synapse0x24400e0() {
   return (neuron0x243d530()*0.304663);
}

double NNele::synapse0x2440120() {
   return (neuron0x243d870()*0.317652);
}

double NNele::synapse0x2440470() {
   return (neuron0x243c890()*-0.717184);
}

double NNele::synapse0x24404b0() {
   return (neuron0x243cba0()*0.914453);
}

double NNele::synapse0x24404f0() {
   return (neuron0x243ceb0()*-1.88538);
}

double NNele::synapse0x2440530() {
   return (neuron0x243d1f0()*-0.450201);
}

double NNele::synapse0x2440570() {
   return (neuron0x243d530()*-0.580801);
}

double NNele::synapse0x24405b0() {
   return (neuron0x243d870()*-0.870145);
}

double NNele::synapse0x2440930() {
   return (neuron0x243c890()*0.747922);
}

double NNele::synapse0x2440970() {
   return (neuron0x243cba0()*2.01781);
}

double NNele::synapse0x24409b0() {
   return (neuron0x243ceb0()*2.48982);
}

double NNele::synapse0x24409f0() {
   return (neuron0x243d1f0()*-1.68993);
}

double NNele::synapse0x2440a30() {
   return (neuron0x243d530()*-0.092615);
}

double NNele::synapse0x2440a70() {
   return (neuron0x243d870()*0.958648);
}

double NNele::synapse0x2440df0() {
   return (neuron0x243dcf0()*-0.570421);
}

double NNele::synapse0x2440e30() {
   return (neuron0x243e070()*1.56316);
}

double NNele::synapse0x2440e70() {
   return (neuron0x243e500()*-3.38651);
}

double NNele::synapse0x2440eb0() {
   return (neuron0x243ea60()*3.13414);
}

double NNele::synapse0x2440ef0() {
   return (neuron0x243ee60()*-1.99102);
}

double NNele::synapse0x243e950() {
   return (neuron0x243f2f0()*2.83452);
}

double NNele::synapse0x243e990() {
   return (neuron0x243f8d0()*1.04663);
}

double NNele::synapse0x243e9d0() {
   return (neuron0x243fcd0()*-1.81663);
}

double NNele::synapse0x243ea10() {
   return (neuron0x2440160()*-2.37812);
}

double NNele::synapse0x242bc60() {
   return (neuron0x24405f0()*0.570296);
}

