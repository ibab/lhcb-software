#include "NNele.h"
#include <cmath>

double NNele::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x875fe38();
     default:
         return 0.;
   }
}

double NNele::neuron0x8750e38() {
   return input0;
}

double NNele::neuron0x8750fc8() {
   return input1;
}

double NNele::neuron0x87511c0() {
   return input2;
}

double NNele::neuron0x875f728() {
   return input3;
}

double NNele::neuron0x875f870() {
   return input4;
}

double NNele::neuron0x875fa68() {
   return input5;
}

double NNele::neuron0x875fc60() {
   return input6;
}

double NNele::input0x875ff58() {
   double input = -1.45343;
   input += synapse0x84a4cb0();
   input += synapse0x87600e8();
   input += synapse0x8760110();
   input += synapse0x8760138();
   input += synapse0x8760160();
   input += synapse0x8760188();
   input += synapse0x87601b0();
   return input;
}

double NNele::neuron0x875ff58() {
   double input = input0x875ff58();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x87601d8() {
   double input = -0.21384;
   input += synapse0x87603b0();
   input += synapse0x87603d8();
   input += synapse0x8760400();
   input += synapse0x8760428();
   input += synapse0x8760450();
   input += synapse0x8760478();
   input += synapse0x87604a0();
   return input;
}

double NNele::neuron0x87601d8() {
   double input = input0x87601d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x87604c8() {
   double input = 0.0343501;
   input += synapse0x87606a0();
   input += synapse0x87606c8();
   input += synapse0x87606f0();
   input += synapse0x87607a0();
   input += synapse0x87607c8();
   input += synapse0x87607f0();
   input += synapse0x8760818();
   return input;
}

double NNele::neuron0x87604c8() {
   double input = input0x87604c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8760840() {
   double input = -0.597751;
   input += synapse0x87609d0();
   input += synapse0x87609f8();
   input += synapse0x8760a20();
   input += synapse0x8760a48();
   input += synapse0x8760a70();
   input += synapse0x8760a98();
   input += synapse0x8760ac0();
   return input;
}

double NNele::neuron0x8760840() {
   double input = input0x8760840();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8760ae8() {
   double input = -1.19777;
   input += synapse0x8760cc0();
   input += synapse0x8760ce8();
   input += synapse0x8760d10();
   input += synapse0x8760d38();
   input += synapse0x8760d60();
   input += synapse0x8760718();
   input += synapse0x8760740();
   return input;
}

double NNele::neuron0x8760ae8() {
   double input = input0x8760ae8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8760e90() {
   double input = -0.963289;
   input += synapse0x8760768();
   input += synapse0x8761068();
   input += synapse0x8761090();
   input += synapse0x87610b8();
   input += synapse0x87610e0();
   input += synapse0x8761108();
   input += synapse0x8761130();
   return input;
}

double NNele::neuron0x8760e90() {
   double input = input0x8760e90();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8761158() {
   double input = -0.184793;
   input += synapse0x8761350();
   input += synapse0x8761378();
   input += synapse0x87613a0();
   input += synapse0x87613c8();
   input += synapse0x87613f0();
   input += synapse0x8761418();
   input += synapse0x8761440();
   return input;
}

double NNele::neuron0x8761158() {
   double input = input0x8761158();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8761468() {
   double input = 0.0903632;
   input += synapse0x8761660();
   input += synapse0x8761688();
   input += synapse0x87616b0();
   input += synapse0x87616d8();
   input += synapse0x8761700();
   input += synapse0x8761728();
   input += synapse0x8761750();
   return input;
}

double NNele::neuron0x8761468() {
   double input = input0x8761468();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8761778() {
   double input = -0.0597434;
   input += synapse0x8761970();
   input += synapse0x8761998();
   input += synapse0x87619c0();
   input += synapse0x87619e8();
   input += synapse0x8761a10();
   input += synapse0x8761a38();
   input += synapse0x8761a60();
   return input;
}

double NNele::neuron0x8761778() {
   double input = input0x8761778();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x8761a88() {
   double input = -0.946919;
   input += synapse0x8761d08();
   input += synapse0x8761d30();
   input += synapse0x84a5020();
   input += synapse0x84a4fa8();
   input += synapse0x836a120();
   input += synapse0x8760d88();
   input += synapse0x8760db0();
   return input;
}

double NNele::neuron0x8761a88() {
   double input = input0x8761a88();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNele::input0x875fe38() {
   double input = 0.0756898;
   input += synapse0x8761fa8();
   input += synapse0x8761fd0();
   input += synapse0x8761ff8();
   input += synapse0x8762020();
   input += synapse0x8762048();
   input += synapse0x8762070();
   input += synapse0x8762098();
   input += synapse0x87620c0();
   input += synapse0x87620e8();
   input += synapse0x8762110();
   return input;
}

double NNele::neuron0x875fe38() {
   double input = input0x875fe38();
   return (input * 0.461045)+0.693488;
}

double NNele::synapse0x84a4cb0() {
   return (neuron0x8750e38()*-0.23385);
}

double NNele::synapse0x87600e8() {
   return (neuron0x8750fc8()*1.43945);
}

double NNele::synapse0x8760110() {
   return (neuron0x87511c0()*-2.87383);
}

double NNele::synapse0x8760138() {
   return (neuron0x875f728()*1.58818);
}

double NNele::synapse0x8760160() {
   return (neuron0x875f870()*-0.995309);
}

double NNele::synapse0x8760188() {
   return (neuron0x875fa68()*2.84651);
}

double NNele::synapse0x87601b0() {
   return (neuron0x875fc60()*0.188495);
}

double NNele::synapse0x87603b0() {
   return (neuron0x8750e38()*-1.2255);
}

double NNele::synapse0x87603d8() {
   return (neuron0x8750fc8()*1.14132);
}

double NNele::synapse0x8760400() {
   return (neuron0x87511c0()*-0.491479);
}

double NNele::synapse0x8760428() {
   return (neuron0x875f728()*2.17173);
}

double NNele::synapse0x8760450() {
   return (neuron0x875f870()*0.838445);
}

double NNele::synapse0x8760478() {
   return (neuron0x875fa68()*0.932496);
}

double NNele::synapse0x87604a0() {
   return (neuron0x875fc60()*0.381101);
}

double NNele::synapse0x87606a0() {
   return (neuron0x8750e38()*0.401123);
}

double NNele::synapse0x87606c8() {
   return (neuron0x8750fc8()*-0.751619);
}

double NNele::synapse0x87606f0() {
   return (neuron0x87511c0()*2.47623);
}

double NNele::synapse0x87607a0() {
   return (neuron0x875f728()*0.134553);
}

double NNele::synapse0x87607c8() {
   return (neuron0x875f870()*2.23521);
}

double NNele::synapse0x87607f0() {
   return (neuron0x875fa68()*-1.86776);
}

double NNele::synapse0x8760818() {
   return (neuron0x875fc60()*0.37071);
}

double NNele::synapse0x87609d0() {
   return (neuron0x8750e38()*0.884753);
}

double NNele::synapse0x87609f8() {
   return (neuron0x8750fc8()*0.553256);
}

double NNele::synapse0x8760a20() {
   return (neuron0x87511c0()*-0.116585);
}

double NNele::synapse0x8760a48() {
   return (neuron0x875f728()*-0.0630867);
}

double NNele::synapse0x8760a70() {
   return (neuron0x875f870()*-0.289766);
}

double NNele::synapse0x8760a98() {
   return (neuron0x875fa68()*0.226704);
}

double NNele::synapse0x8760ac0() {
   return (neuron0x875fc60()*-0.198513);
}

double NNele::synapse0x8760cc0() {
   return (neuron0x8750e38()*-2.40662);
}

double NNele::synapse0x8760ce8() {
   return (neuron0x8750fc8()*1.62106);
}

double NNele::synapse0x8760d10() {
   return (neuron0x87511c0()*0.488296);
}

double NNele::synapse0x8760d38() {
   return (neuron0x875f728()*-0.755848);
}

double NNele::synapse0x8760d60() {
   return (neuron0x875f870()*0.508882);
}

double NNele::synapse0x8760718() {
   return (neuron0x875fa68()*-0.46327);
}

double NNele::synapse0x8760740() {
   return (neuron0x875fc60()*-0.700708);
}

double NNele::synapse0x8760768() {
   return (neuron0x8750e38()*0.140869);
}

double NNele::synapse0x8761068() {
   return (neuron0x8750fc8()*-1.42063);
}

double NNele::synapse0x8761090() {
   return (neuron0x87511c0()*0.737081);
}

double NNele::synapse0x87610b8() {
   return (neuron0x875f728()*-1.46645);
}

double NNele::synapse0x87610e0() {
   return (neuron0x875f870()*0.706007);
}

double NNele::synapse0x8761108() {
   return (neuron0x875fa68()*-0.906365);
}

double NNele::synapse0x8761130() {
   return (neuron0x875fc60()*0.186167);
}

double NNele::synapse0x8761350() {
   return (neuron0x8750e38()*2.82291);
}

double NNele::synapse0x8761378() {
   return (neuron0x8750fc8()*1.64881);
}

double NNele::synapse0x87613a0() {
   return (neuron0x87511c0()*-1.86753);
}

double NNele::synapse0x87613c8() {
   return (neuron0x875f728()*-4.65299);
}

double NNele::synapse0x87613f0() {
   return (neuron0x875f870()*-1.45638);
}

double NNele::synapse0x8761418() {
   return (neuron0x875fa68()*-0.289262);
}

double NNele::synapse0x8761440() {
   return (neuron0x875fc60()*1.9098);
}

double NNele::synapse0x8761660() {
   return (neuron0x8750e38()*0.50635);
}

double NNele::synapse0x8761688() {
   return (neuron0x8750fc8()*1.00303);
}

double NNele::synapse0x87616b0() {
   return (neuron0x87511c0()*-0.704011);
}

double NNele::synapse0x87616d8() {
   return (neuron0x875f728()*0.68812);
}

double NNele::synapse0x8761700() {
   return (neuron0x875f870()*-0.491107);
}

double NNele::synapse0x8761728() {
   return (neuron0x875fa68()*0.881734);
}

double NNele::synapse0x8761750() {
   return (neuron0x875fc60()*0.142941);
}

double NNele::synapse0x8761970() {
   return (neuron0x8750e38()*-0.521064);
}

double NNele::synapse0x8761998() {
   return (neuron0x8750fc8()*-0.641288);
}

double NNele::synapse0x87619c0() {
   return (neuron0x87511c0()*0.0522468);
}

double NNele::synapse0x87619e8() {
   return (neuron0x875f728()*-0.270715);
}

double NNele::synapse0x8761a10() {
   return (neuron0x875f870()*0.412177);
}

double NNele::synapse0x8761a38() {
   return (neuron0x875fa68()*-0.501236);
}

double NNele::synapse0x8761a60() {
   return (neuron0x875fc60()*-0.679228);
}

double NNele::synapse0x8761d08() {
   return (neuron0x8750e38()*-0.463746);
}

double NNele::synapse0x8761d30() {
   return (neuron0x8750fc8()*0.886608);
}

double NNele::synapse0x84a5020() {
   return (neuron0x87511c0()*0.781924);
}

double NNele::synapse0x84a4fa8() {
   return (neuron0x875f728()*2.74391);
}

double NNele::synapse0x836a120() {
   return (neuron0x875f870()*0.919767);
}

double NNele::synapse0x8760d88() {
   return (neuron0x875fa68()*-0.164509);
}

double NNele::synapse0x8760db0() {
   return (neuron0x875fc60()*-1.47535);
}

double NNele::synapse0x8761fa8() {
   return (neuron0x875ff58()*-1.01116);
}

double NNele::synapse0x8761fd0() {
   return (neuron0x87601d8()*-1.23905);
}

double NNele::synapse0x8761ff8() {
   return (neuron0x87604c8()*2.03842);
}

double NNele::synapse0x8762020() {
   return (neuron0x8760840()*0.763362);
}

double NNele::synapse0x8762048() {
   return (neuron0x8760ae8()*1.51148);
}

double NNele::synapse0x8762070() {
   return (neuron0x8760e90()*-1.51186);
}

double NNele::synapse0x8762098() {
   return (neuron0x8761158()*-1.81426);
}

double NNele::synapse0x87620c0() {
   return (neuron0x8761468()*1.12627);
}

double NNele::synapse0x87620e8() {
   return (neuron0x8761778()*-0.859527);
}

double NNele::synapse0x8762110() {
   return (neuron0x8761a88()*-1.75458);
}

