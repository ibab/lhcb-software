#include "DC06_NNkaon.h"
#include <cmath>

double DC06_NNkaon::value(int index,double in0,double in1,double in2,double in3) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x8b705f0();
     default:
         return 0.;
   }
}

double DC06_NNkaon::neuron0x8b6fe98() {
   return input0;
}

double DC06_NNkaon::neuron0x8b70028() {
   return input1;
}

double DC06_NNkaon::neuron0x8b70200() {
   return input2;
}

double DC06_NNkaon::neuron0x8b703f8() {
   return input3;
}

double DC06_NNkaon::input0x8b70710() {
   double input = 0.19115;
   input += synapse0x8a5b038();
   input += synapse0x8b708a0();
   input += synapse0x8b708c8();
   input += synapse0x8b708f0();
   return input;
}

double DC06_NNkaon::neuron0x8b70710() {
   double input = input0x8b70710();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8b70918() {
   double input = 1.71353;
   input += synapse0x8b70af0();
   input += synapse0x8b70b18();
   input += synapse0x8b70b40();
   input += synapse0x8b70b68();
   return input;
}

double DC06_NNkaon::neuron0x8b70918() {
   double input = input0x8b70918();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8b70b90() {
   double input = -0.117837;
   input += synapse0x8bbc6f8();
   input += synapse0x8bbc720();
   input += synapse0x8bbc748();
   input += synapse0x8bbc770();
   return input;
}

double DC06_NNkaon::neuron0x8b70b90() {
   double input = input0x8b70b90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbc798() {
   double input = 0.660576;
   input += synapse0x8bbc970();
   input += synapse0x8bbc998();
   input += synapse0x8bbc9c0();
   input += synapse0x8bbc9e8();
   return input;
}

double DC06_NNkaon::neuron0x8bbc798() {
   double input = input0x8bbc798();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbca10() {
   double input = -0.799721;
   input += synapse0x8bbcc08();
   input += synapse0x8bbccb8();
   input += synapse0x8bbcce0();
   input += synapse0x8bbcd08();
   return input;
}

double DC06_NNkaon::neuron0x8bbca10() {
   double input = input0x8bbca10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbcd30() {
   double input = 2.06852;
   input += synapse0x8bbcee0();
   input += synapse0x8bbcf08();
   input += synapse0x8bbcf30();
   input += synapse0x8bbcf58();
   return input;
}

double DC06_NNkaon::neuron0x8bbcd30() {
   double input = input0x8bbcd30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbcf80() {
   double input = -2.63021;
   input += synapse0x8bbd178();
   input += synapse0x8bbd1a0();
   input += synapse0x8bbd1c8();
   input += synapse0x8bbd1f0();
   return input;
}

double DC06_NNkaon::neuron0x8bbcf80() {
   double input = input0x8bbcf80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbd218() {
   double input = -0.705812;
   input += synapse0x8bbd410();
   input += synapse0x8bbd438();
   input += synapse0x8bbd460();
   input += synapse0x8bbd488();
   return input;
}

double DC06_NNkaon::neuron0x8bbd218() {
   double input = input0x8bbd218();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbd4b0() {
   double input = 0.0182506;
   input += synapse0x8bbd6a8();
   input += synapse0x8b70d20();
   input += synapse0x8bbcc30();
   input += synapse0x8bbcc58();
   return input;
}

double DC06_NNkaon::neuron0x8bbd4b0() {
   double input = input0x8bbd4b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbd7d8() {
   double input = 1.66681;
   input += synapse0x8bbd9d0();
   input += synapse0x8bbd9f8();
   input += synapse0x8bbda20();
   input += synapse0x8bbda48();
   return input;
}

double DC06_NNkaon::neuron0x8bbd7d8() {
   double input = input0x8bbd7d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbda70() {
   double input = 0.42694;
   input += synapse0x8bbdc70();
   input += synapse0x8bbdc98();
   input += synapse0x8bbdcc0();
   input += synapse0x8bbdce8();
   return input;
}

double DC06_NNkaon::neuron0x8bbda70() {
   double input = input0x8bbda70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8bbdd10() {
   double input = -0.950298;
   input += synapse0x8bbdf10();
   input += synapse0x8bbdf38();
   input += synapse0x8bbdf60();
   input += synapse0x8bbdf88();
   return input;
}

double DC06_NNkaon::neuron0x8bbdd10() {
   double input = input0x8bbdd10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double DC06_NNkaon::input0x8b705f0() {
   double input = 0.465975;
   input += synapse0x8bbe088();
   input += synapse0x8bbe0b0();
   input += synapse0x8bbe0d8();
   input += synapse0x8bbe100();
   input += synapse0x8bbe128();
   input += synapse0x8bbe150();
   input += synapse0x8bbe178();
   input += synapse0x8bbe1a0();
   input += synapse0x8bbe1c8();
   input += synapse0x8bbe1f0();
   input += synapse0x8bbe218();
   input += synapse0x8bbe240();
   return input;
}

double DC06_NNkaon::neuron0x8b705f0() {
   double input = input0x8b705f0();
   return (input * 0.475144)+0.655685;
}

double DC06_NNkaon::synapse0x8a5b038() {
   return (neuron0x8b6fe98()*0.0719854);
}

double DC06_NNkaon::synapse0x8b708a0() {
   return (neuron0x8b70028()*-0.149071);
}

double DC06_NNkaon::synapse0x8b708c8() {
   return (neuron0x8b70200()*0.00532869);
}

double DC06_NNkaon::synapse0x8b708f0() {
   return (neuron0x8b703f8()*3.89692);
}

double DC06_NNkaon::synapse0x8b70af0() {
   return (neuron0x8b6fe98()*-0.0529867);
}

double DC06_NNkaon::synapse0x8b70b18() {
   return (neuron0x8b70028()*1.05124);
}

double DC06_NNkaon::synapse0x8b70b40() {
   return (neuron0x8b70200()*1.93487);
}

double DC06_NNkaon::synapse0x8b70b68() {
   return (neuron0x8b703f8()*1.48519);
}

double DC06_NNkaon::synapse0x8bbc6f8() {
   return (neuron0x8b6fe98()*0.670359);
}

double DC06_NNkaon::synapse0x8bbc720() {
   return (neuron0x8b70028()*0.200403);
}

double DC06_NNkaon::synapse0x8bbc748() {
   return (neuron0x8b70200()*-0.396428);
}

double DC06_NNkaon::synapse0x8bbc770() {
   return (neuron0x8b703f8()*-0.022225);
}

double DC06_NNkaon::synapse0x8bbc970() {
   return (neuron0x8b6fe98()*0.417032);
}

double DC06_NNkaon::synapse0x8bbc998() {
   return (neuron0x8b70028()*-0.80757);
}

double DC06_NNkaon::synapse0x8bbc9c0() {
   return (neuron0x8b70200()*0.636774);
}

double DC06_NNkaon::synapse0x8bbc9e8() {
   return (neuron0x8b703f8()*-9.20379);
}

double DC06_NNkaon::synapse0x8bbcc08() {
   return (neuron0x8b6fe98()*-0.24418);
}

double DC06_NNkaon::synapse0x8bbccb8() {
   return (neuron0x8b70028()*-1.40186);
}

double DC06_NNkaon::synapse0x8bbcce0() {
   return (neuron0x8b70200()*0.940559);
}

double DC06_NNkaon::synapse0x8bbcd08() {
   return (neuron0x8b703f8()*-1.30669);
}

double DC06_NNkaon::synapse0x8bbcee0() {
   return (neuron0x8b6fe98()*-0.536569);
}

double DC06_NNkaon::synapse0x8bbcf08() {
   return (neuron0x8b70028()*-0.443858);
}

double DC06_NNkaon::synapse0x8bbcf30() {
   return (neuron0x8b70200()*1.62326);
}

double DC06_NNkaon::synapse0x8bbcf58() {
   return (neuron0x8b703f8()*-0.0812424);
}

double DC06_NNkaon::synapse0x8bbd178() {
   return (neuron0x8b6fe98()*0.731411);
}

double DC06_NNkaon::synapse0x8bbd1a0() {
   return (neuron0x8b70028()*1.17377);
}

double DC06_NNkaon::synapse0x8bbd1c8() {
   return (neuron0x8b70200()*-2.85516);
}

double DC06_NNkaon::synapse0x8bbd1f0() {
   return (neuron0x8b703f8()*0.560241);
}

double DC06_NNkaon::synapse0x8bbd410() {
   return (neuron0x8b6fe98()*-0.0416664);
}

double DC06_NNkaon::synapse0x8bbd438() {
   return (neuron0x8b70028()*-0.169316);
}

double DC06_NNkaon::synapse0x8bbd460() {
   return (neuron0x8b70200()*-2.63207);
}

double DC06_NNkaon::synapse0x8bbd488() {
   return (neuron0x8b703f8()*-2.89515);
}

double DC06_NNkaon::synapse0x8bbd6a8() {
   return (neuron0x8b6fe98()*0.681645);
}

double DC06_NNkaon::synapse0x8b70d20() {
   return (neuron0x8b70028()*0.0917903);
}

double DC06_NNkaon::synapse0x8bbcc30() {
   return (neuron0x8b70200()*0.574566);
}

double DC06_NNkaon::synapse0x8bbcc58() {
   return (neuron0x8b703f8()*-0.853526);
}

double DC06_NNkaon::synapse0x8bbd9d0() {
   return (neuron0x8b6fe98()*-0.63505);
}

double DC06_NNkaon::synapse0x8bbd9f8() {
   return (neuron0x8b70028()*0.7265);
}

double DC06_NNkaon::synapse0x8bbda20() {
   return (neuron0x8b70200()*3.7844);
}

double DC06_NNkaon::synapse0x8bbda48() {
   return (neuron0x8b703f8()*0.712672);
}

double DC06_NNkaon::synapse0x8bbdc70() {
   return (neuron0x8b6fe98()*-0.776822);
}

double DC06_NNkaon::synapse0x8bbdc98() {
   return (neuron0x8b70028()*0.486728);
}

double DC06_NNkaon::synapse0x8bbdcc0() {
   return (neuron0x8b70200()*0.587454);
}

double DC06_NNkaon::synapse0x8bbdce8() {
   return (neuron0x8b703f8()*-0.302391);
}

double DC06_NNkaon::synapse0x8bbdf10() {
   return (neuron0x8b6fe98()*1.15871);
}

double DC06_NNkaon::synapse0x8bbdf38() {
   return (neuron0x8b70028()*-0.932074);
}

double DC06_NNkaon::synapse0x8bbdf60() {
   return (neuron0x8b70200()*-1.36636);
}

double DC06_NNkaon::synapse0x8bbdf88() {
   return (neuron0x8b703f8()*-1.09099);
}

double DC06_NNkaon::synapse0x8bbe088() {
   return (neuron0x8b70710()*-2.25184);
}

double DC06_NNkaon::synapse0x8bbe0b0() {
   return (neuron0x8b70918()*-0.941766);
}

double DC06_NNkaon::synapse0x8bbe0d8() {
   return (neuron0x8b70b90()*0.117637);
}

double DC06_NNkaon::synapse0x8bbe100() {
   return (neuron0x8bbc798()*-1.15741);
}

double DC06_NNkaon::synapse0x8bbe128() {
   return (neuron0x8bbca10()*-1.28408);
}

double DC06_NNkaon::synapse0x8bbe150() {
   return (neuron0x8bbcd30()*2.33852);
}

double DC06_NNkaon::synapse0x8bbe178() {
   return (neuron0x8bbcf80()*-2.05451);
}

double DC06_NNkaon::synapse0x8bbe1a0() {
   return (neuron0x8bbd218()*-1.74432);
}

double DC06_NNkaon::synapse0x8bbe1c8() {
   return (neuron0x8bbd4b0()*-0.303977);
}

double DC06_NNkaon::synapse0x8bbe1f0() {
   return (neuron0x8bbd7d8()*0.421002);
}

double DC06_NNkaon::synapse0x8bbe218() {
   return (neuron0x8bbda70()*0.713672);
}

double DC06_NNkaon::synapse0x8bbe240() {
   return (neuron0x8bbdd10()*1.18422);
}

