#include "NNpionS.h"
#include <cmath>

double NNpionS::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
   input0 = (in0 - 0)/1;
   input1 = (in1 - 0)/1;
   input2 = (in2 - 0)/1;
   input3 = (in3 - 0)/1;
   input4 = (in4 - 0)/1;
   input5 = (in5 - 0)/1;
   input6 = (in6 - 0)/1;
   input7 = (in7 - 0)/1;
   input8 = (in8 - 0)/1;
   input9 = (in9 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x87906a8();
     default:
         return 0.;
   }
}

double NNpionS::neuron0x877ecd0() {
   return input0;
}

double NNpionS::neuron0x877eec8() {
   return input1;
}

double NNpionS::neuron0x877f0c0() {
   return input2;
}

double NNpionS::neuron0x878d538() {
   return input3;
}

double NNpionS::neuron0x878d710() {
   return input4;
}

double NNpionS::neuron0x878d908() {
   return input5;
}

double NNpionS::neuron0x878db00() {
   return input6;
}

double NNpionS::neuron0x878dcf8() {
   return input7;
}

double NNpionS::neuron0x878def0() {
   return input8;
}

double NNpionS::neuron0x878e0e8() {
   return input9;
}

double NNpionS::input0x878e3e0() {
   double input = -0.36318;
   input += synapse0x851bc28();
   input += synapse0x878e570();
   input += synapse0x878e598();
   input += synapse0x878e5c0();
   input += synapse0x878e5e8();
   input += synapse0x878e610();
   input += synapse0x878e638();
   input += synapse0x878e660();
   input += synapse0x878e688();
   input += synapse0x878e6b0();
   return input;
}

double NNpionS::neuron0x878e3e0() {
   double input = input0x878e3e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878e6d8() {
   double input = -0.799651;
   input += synapse0x878e8b0();
   input += synapse0x878e8d8();
   input += synapse0x878e900();
   input += synapse0x878e928();
   input += synapse0x878e950();
   input += synapse0x878e978();
   input += synapse0x878e9a0();
   input += synapse0x878ea50();
   input += synapse0x878ea78();
   input += synapse0x878eaa0();
   return input;
}

double NNpionS::neuron0x878e6d8() {
   double input = input0x878e6d8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878eac8() {
   double input = -0.542103;
   input += synapse0x878ec58();
   input += synapse0x878ec80();
   input += synapse0x878eca8();
   input += synapse0x878ecd0();
   input += synapse0x878ecf8();
   input += synapse0x878ed20();
   input += synapse0x878ed48();
   input += synapse0x878ed70();
   input += synapse0x878ed98();
   input += synapse0x878edc0();
   return input;
}

double NNpionS::neuron0x878eac8() {
   double input = input0x878eac8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878ede8() {
   double input = 0.117925;
   input += synapse0x878efc0();
   input += synapse0x878efe8();
   input += synapse0x878f010();
   input += synapse0x878e9c8();
   input += synapse0x878e9f0();
   input += synapse0x878ea18();
   input += synapse0x878f140();
   input += synapse0x878f168();
   input += synapse0x878f190();
   input += synapse0x878f1b8();
   return input;
}

double NNpionS::neuron0x878ede8() {
   double input = input0x878ede8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878f1e0() {
   double input = 0.422924;
   input += synapse0x878f3b8();
   input += synapse0x878f3e0();
   input += synapse0x878f408();
   input += synapse0x878f430();
   input += synapse0x878f458();
   input += synapse0x878f480();
   input += synapse0x878f4a8();
   input += synapse0x878f4d0();
   input += synapse0x878f4f8();
   input += synapse0x878f520();
   return input;
}

double NNpionS::neuron0x878f1e0() {
   double input = input0x878f1e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878f548() {
   double input = 0.414671;
   input += synapse0x878f720();
   input += synapse0x878f748();
   input += synapse0x878f770();
   input += synapse0x878f798();
   input += synapse0x878f7c0();
   input += synapse0x878f7e8();
   input += synapse0x878f810();
   input += synapse0x878f838();
   input += synapse0x878f860();
   input += synapse0x878f888();
   return input;
}

double NNpionS::neuron0x878f548() {
   double input = input0x878f548();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878f8b0() {
   double input = -0.609542;
   input += synapse0x878fb10();
   input += synapse0x878fb38();
   input += synapse0x878fb60();
   input += synapse0x878fb88();
   input += synapse0x878fbb0();
   input += synapse0x851bfc0();
   input += synapse0x851be68();
   input += synapse0x851bf48();
   input += synapse0x851bc50();
   input += synapse0x8373358();
   return input;
}

double NNpionS::neuron0x878f8b0() {
   double input = input0x878f8b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878f038() {
   double input = 0.25463;
   input += synapse0x851c170();
   input += synapse0x878fe70();
   input += synapse0x878fe98();
   input += synapse0x878fec0();
   input += synapse0x878fee8();
   input += synapse0x878ff10();
   input += synapse0x878ff38();
   input += synapse0x878ff60();
   input += synapse0x878ff88();
   input += synapse0x878ffb0();
   return input;
}

double NNpionS::neuron0x878f038() {
   double input = input0x878f038();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x878ffd8() {
   double input = -0.571286;
   input += synapse0x87901b0();
   input += synapse0x87901d8();
   input += synapse0x8790200();
   input += synapse0x8790228();
   input += synapse0x8790250();
   input += synapse0x8790278();
   input += synapse0x87902a0();
   input += synapse0x87902c8();
   input += synapse0x87902f0();
   input += synapse0x8790318();
   return input;
}

double NNpionS::neuron0x878ffd8() {
   double input = input0x878ffd8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x8790340() {
   double input = -0.0663442;
   input += synapse0x8790518();
   input += synapse0x8790540();
   input += synapse0x8790568();
   input += synapse0x8790590();
   input += synapse0x87905b8();
   input += synapse0x87905e0();
   input += synapse0x8790608();
   input += synapse0x8790630();
   input += synapse0x8790658();
   input += synapse0x8790680();
   return input;
}

double NNpionS::neuron0x8790340() {
   double input = input0x8790340();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double NNpionS::input0x87906a8() {
   double input = 0.746413;
   input += synapse0x87907a8();
   input += synapse0x87907d0();
   input += synapse0x87907f8();
   input += synapse0x8790820();
   input += synapse0x8790848();
   input += synapse0x8790870();
   input += synapse0x8790898();
   input += synapse0x87908c0();
   input += synapse0x87908e8();
   input += synapse0x8790910();
   return input;
}

double NNpionS::neuron0x87906a8() {
   double input = input0x87906a8();
   return (input * 0.497417)+0.550759;
}

double NNpionS::synapse0x851bc28() {
   return (neuron0x877ecd0()*-0.0643267);
}

double NNpionS::synapse0x878e570() {
   return (neuron0x877eec8()*0.0143793);
}

double NNpionS::synapse0x878e598() {
   return (neuron0x877f0c0()*1.01049);
}

double NNpionS::synapse0x878e5c0() {
   return (neuron0x878d538()*-0.574054);
}

double NNpionS::synapse0x878e5e8() {
   return (neuron0x878d710()*0.0337118);
}

double NNpionS::synapse0x878e610() {
   return (neuron0x878d908()*-0.359034);
}

double NNpionS::synapse0x878e638() {
   return (neuron0x878db00()*-0.928867);
}

double NNpionS::synapse0x878e660() {
   return (neuron0x878dcf8()*0.321188);
}

double NNpionS::synapse0x878e688() {
   return (neuron0x878def0()*0.82188);
}

double NNpionS::synapse0x878e6b0() {
   return (neuron0x878e0e8()*0.551884);
}

double NNpionS::synapse0x878e8b0() {
   return (neuron0x877ecd0()*0.0816521);
}

double NNpionS::synapse0x878e8d8() {
   return (neuron0x877eec8()*0.372512);
}

double NNpionS::synapse0x878e900() {
   return (neuron0x877f0c0()*1.21655);
}

double NNpionS::synapse0x878e928() {
   return (neuron0x878d538()*-0.0868727);
}

double NNpionS::synapse0x878e950() {
   return (neuron0x878d710()*0.156466);
}

double NNpionS::synapse0x878e978() {
   return (neuron0x878d908()*-0.709461);
}

double NNpionS::synapse0x878e9a0() {
   return (neuron0x878db00()*-2.01732);
}

double NNpionS::synapse0x878ea50() {
   return (neuron0x878dcf8()*0.144782);
}

double NNpionS::synapse0x878ea78() {
   return (neuron0x878def0()*-1.06898);
}

double NNpionS::synapse0x878eaa0() {
   return (neuron0x878e0e8()*-0.49697);
}

double NNpionS::synapse0x878ec58() {
   return (neuron0x877ecd0()*0.0934889);
}

double NNpionS::synapse0x878ec80() {
   return (neuron0x877eec8()*-0.341764);
}

double NNpionS::synapse0x878eca8() {
   return (neuron0x877f0c0()*-0.844884);
}

double NNpionS::synapse0x878ecd0() {
   return (neuron0x878d538()*-0.00828399);
}

double NNpionS::synapse0x878ecf8() {
   return (neuron0x878d710()*0.549359);
}

double NNpionS::synapse0x878ed20() {
   return (neuron0x878d908()*-0.381487);
}

double NNpionS::synapse0x878ed48() {
   return (neuron0x878db00()*0.998892);
}

double NNpionS::synapse0x878ed70() {
   return (neuron0x878dcf8()*-0.15799);
}

double NNpionS::synapse0x878ed98() {
   return (neuron0x878def0()*-0.0489331);
}

double NNpionS::synapse0x878edc0() {
   return (neuron0x878e0e8()*0.78136);
}

double NNpionS::synapse0x878efc0() {
   return (neuron0x877ecd0()*0.167784);
}

double NNpionS::synapse0x878efe8() {
   return (neuron0x877eec8()*-0.265971);
}

double NNpionS::synapse0x878f010() {
   return (neuron0x877f0c0()*0.101638);
}

double NNpionS::synapse0x878e9c8() {
   return (neuron0x878d538()*0.214857);
}

double NNpionS::synapse0x878e9f0() {
   return (neuron0x878d710()*0.618409);
}

double NNpionS::synapse0x878ea18() {
   return (neuron0x878d908()*-0.256797);
}

double NNpionS::synapse0x878f140() {
   return (neuron0x878db00()*-1.2392);
}

double NNpionS::synapse0x878f168() {
   return (neuron0x878dcf8()*0.83862);
}

double NNpionS::synapse0x878f190() {
   return (neuron0x878def0()*-0.21446);
}

double NNpionS::synapse0x878f1b8() {
   return (neuron0x878e0e8()*1.38538);
}

double NNpionS::synapse0x878f3b8() {
   return (neuron0x877ecd0()*1.36225);
}

double NNpionS::synapse0x878f3e0() {
   return (neuron0x877eec8()*0.579384);
}

double NNpionS::synapse0x878f408() {
   return (neuron0x877f0c0()*0.0492804);
}

double NNpionS::synapse0x878f430() {
   return (neuron0x878d538()*-0.272982);
}

double NNpionS::synapse0x878f458() {
   return (neuron0x878d710()*-0.590022);
}

double NNpionS::synapse0x878f480() {
   return (neuron0x878d908()*-0.920104);
}

double NNpionS::synapse0x878f4a8() {
   return (neuron0x878db00()*0.497004);
}

double NNpionS::synapse0x878f4d0() {
   return (neuron0x878dcf8()*1.46261);
}

double NNpionS::synapse0x878f4f8() {
   return (neuron0x878def0()*0.268791);
}

double NNpionS::synapse0x878f520() {
   return (neuron0x878e0e8()*0.102607);
}

double NNpionS::synapse0x878f720() {
   return (neuron0x877ecd0()*-0.160686);
}

double NNpionS::synapse0x878f748() {
   return (neuron0x877eec8()*0.23481);
}

double NNpionS::synapse0x878f770() {
   return (neuron0x877f0c0()*-0.420153);
}

double NNpionS::synapse0x878f798() {
   return (neuron0x878d538()*0.234305);
}

double NNpionS::synapse0x878f7c0() {
   return (neuron0x878d710()*-0.368377);
}

double NNpionS::synapse0x878f7e8() {
   return (neuron0x878d908()*-0.0701801);
}

double NNpionS::synapse0x878f810() {
   return (neuron0x878db00()*0.366066);
}

double NNpionS::synapse0x878f838() {
   return (neuron0x878dcf8()*-0.435071);
}

double NNpionS::synapse0x878f860() {
   return (neuron0x878def0()*-0.224612);
}

double NNpionS::synapse0x878f888() {
   return (neuron0x878e0e8()*-0.57267);
}

double NNpionS::synapse0x878fb10() {
   return (neuron0x877ecd0()*0.256911);
}

double NNpionS::synapse0x878fb38() {
   return (neuron0x877eec8()*-0.598651);
}

double NNpionS::synapse0x878fb60() {
   return (neuron0x877f0c0()*-0.946018);
}

double NNpionS::synapse0x878fb88() {
   return (neuron0x878d538()*-0.0546893);
}

double NNpionS::synapse0x878fbb0() {
   return (neuron0x878d710()*0.422214);
}

double NNpionS::synapse0x851bfc0() {
   return (neuron0x878d908()*-0.572661);
}

double NNpionS::synapse0x851be68() {
   return (neuron0x878db00()*1.28394);
}

double NNpionS::synapse0x851bf48() {
   return (neuron0x878dcf8()*-0.305192);
}

double NNpionS::synapse0x851bc50() {
   return (neuron0x878def0()*-0.852496);
}

double NNpionS::synapse0x8373358() {
   return (neuron0x878e0e8()*0.908193);
}

double NNpionS::synapse0x851c170() {
   return (neuron0x877ecd0()*0.183179);
}

double NNpionS::synapse0x878fe70() {
   return (neuron0x877eec8()*0.254996);
}

double NNpionS::synapse0x878fe98() {
   return (neuron0x877f0c0()*0.192118);
}

double NNpionS::synapse0x878fec0() {
   return (neuron0x878d538()*0.360314);
}

double NNpionS::synapse0x878fee8() {
   return (neuron0x878d710()*0.624555);
}

double NNpionS::synapse0x878ff10() {
   return (neuron0x878d908()*-0.0950559);
}

double NNpionS::synapse0x878ff38() {
   return (neuron0x878db00()*1.57931);
}

double NNpionS::synapse0x878ff60() {
   return (neuron0x878dcf8()*0.0292482);
}

double NNpionS::synapse0x878ff88() {
   return (neuron0x878def0()*-0.406067);
}

double NNpionS::synapse0x878ffb0() {
   return (neuron0x878e0e8()*0.0844941);
}

double NNpionS::synapse0x87901b0() {
   return (neuron0x877ecd0()*0.123073);
}

double NNpionS::synapse0x87901d8() {
   return (neuron0x877eec8()*0.429348);
}

double NNpionS::synapse0x8790200() {
   return (neuron0x877f0c0()*0.569342);
}

double NNpionS::synapse0x8790228() {
   return (neuron0x878d538()*0.0724532);
}

double NNpionS::synapse0x8790250() {
   return (neuron0x878d710()*-0.529732);
}

double NNpionS::synapse0x8790278() {
   return (neuron0x878d908()*-0.436947);
}

double NNpionS::synapse0x87902a0() {
   return (neuron0x878db00()*-1.51869);
}

double NNpionS::synapse0x87902c8() {
   return (neuron0x878dcf8()*-0.107749);
}

double NNpionS::synapse0x87902f0() {
   return (neuron0x878def0()*0.619975);
}

double NNpionS::synapse0x8790318() {
   return (neuron0x878e0e8()*-0.233284);
}

double NNpionS::synapse0x8790518() {
   return (neuron0x877ecd0()*-0.385533);
}

double NNpionS::synapse0x8790540() {
   return (neuron0x877eec8()*-0.128819);
}

double NNpionS::synapse0x8790568() {
   return (neuron0x877f0c0()*-1.1085);
}

double NNpionS::synapse0x8790590() {
   return (neuron0x878d538()*0.141044);
}

double NNpionS::synapse0x87905b8() {
   return (neuron0x878d710()*-0.258719);
}

double NNpionS::synapse0x87905e0() {
   return (neuron0x878d908()*-0.578301);
}

double NNpionS::synapse0x8790608() {
   return (neuron0x878db00()*2.04319);
}

double NNpionS::synapse0x8790630() {
   return (neuron0x878dcf8()*0.261001);
}

double NNpionS::synapse0x8790658() {
   return (neuron0x878def0()*-1.02419);
}

double NNpionS::synapse0x8790680() {
   return (neuron0x878e0e8()*-1.83205);
}

double NNpionS::synapse0x87907a8() {
   return (neuron0x878e3e0()*-0.25821);
}

double NNpionS::synapse0x87907d0() {
   return (neuron0x878e6d8()*2.18516);
}

double NNpionS::synapse0x87907f8() {
   return (neuron0x878eac8()*-0.341912);
}

double NNpionS::synapse0x8790820() {
   return (neuron0x878ede8()*1.23832);
}

double NNpionS::synapse0x8790848() {
   return (neuron0x878f1e0()*-1.39757);
}

double NNpionS::synapse0x8790870() {
   return (neuron0x878f548()*0.0466835);
}

double NNpionS::synapse0x8790898() {
   return (neuron0x878f8b0()*-0.991831);
}

double NNpionS::synapse0x87908c0() {
   return (neuron0x878f038()*-1.04392);
}

double NNpionS::synapse0x87908e8() {
   return (neuron0x878ffd8()*0.788475);
}

double NNpionS::synapse0x8790910() {
   return (neuron0x8790340()*-1.112);
}

