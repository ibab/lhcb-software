#ifndef PDGCODESHH
#define PDGCODESHH

namespace pdg {

  typedef int PDG_CodeType;

  const PDG_CodeType dqrk= 1;
  const PDG_CodeType dbar=-1;
  const PDG_CodeType uqrk= 2;
  const PDG_CodeType ubar=-2;
  const PDG_CodeType sqrk= 3;
  const PDG_CodeType sbar=-3;
  const PDG_CodeType cqrk= 4;
  const PDG_CodeType cbar=-4;
  const PDG_CodeType bqrk= 5;
  const PDG_CodeType bbar=-5;
  const PDG_CodeType tqrk= 6;
  const PDG_CodeType tbar=-6;

  const PDG_CodeType nuel    = 12;
  const PDG_CodeType numul   = 14;
  const PDG_CodeType nutaul  = 16;
  const PDG_CodeType anuel   =-12;
  const PDG_CodeType anumul  =-14;
  const PDG_CodeType anutaul =-16;

  const PDG_CodeType eminus  = 11;
  const PDG_CodeType eplus   =-11;
  const PDG_CodeType muminus = 13;
  const PDG_CodeType muplus  =-13;
  const PDG_CodeType tauminus= 15;
  const PDG_CodeType tauplus =-15;
  const PDG_CodeType gamma   = 22;
  const PDG_CodeType z0      = 23;
  const PDG_CodeType h01     = 25;  // Standard model higgs - all conventions
  const PDG_CodeType h0_pdgConvention    = 25;  // Ligtest neutral susy higgs
  const PDG_CodeType h0_herwigConvention = 26;  // Ligtest neutral susy higgs
  const PDG_CodeType sel     = 1000011;
  const PDG_CodeType asel    =-1000011;
  const PDG_CodeType snuel   = 1000012;
  const PDG_CodeType asnuel  =-1000012;
  const PDG_CodeType smul    = 1000013;
  const PDG_CodeType asmul   =-1000013;
  const PDG_CodeType snumul  = 1000014;
  const PDG_CodeType asnumul =-1000014;
  const PDG_CodeType stau1   = 1000015;
  const PDG_CodeType astau1  =-1000015;
  const PDG_CodeType snutaul = 1000016;
  const PDG_CodeType asnutaul=-1000016;

  const PDG_CodeType ser     = 2000011;
  const PDG_CodeType aser    =-2000011;
  const PDG_CodeType smur    = 2000013;
  const PDG_CodeType asmur   =-2000013;
  const PDG_CodeType stau2   = 2000015;
  const PDG_CodeType astau2  =-2000015;

  const PDG_CodeType ntlino1 = 1000022;
  const PDG_CodeType ntlino2 = 1000023;
  const PDG_CodeType ntlino3 = 1000025;
  const PDG_CodeType ntlino4 = 1000035;

  const PDG_CodeType chgino1plus  =  1000024;
  const PDG_CodeType chgino1minus = -1000024;
  const PDG_CodeType chgino2plus  =  1000037;
  const PDG_CodeType chgino2minus = -1000037;

  const PDG_CodeType squarkdl    = 1000001;
  const PDG_CodeType squarkul    = 1000002;
  const PDG_CodeType squarksl    = 1000003;
  const PDG_CodeType squarkcl    = 1000004;
  const PDG_CodeType squarkb1    = 1000005;
  const PDG_CodeType squarkt1    = 1000006;
  const PDG_CodeType squarkdr    = 2000001;
  const PDG_CodeType squarkur    = 2000002;
  const PDG_CodeType squarksr    = 2000003;
  const PDG_CodeType squarkcr    = 2000004;
  const PDG_CodeType squarkb2    = 2000005;
  const PDG_CodeType squarkt2    = 2000006;
  const PDG_CodeType squarkdbarl =-1000001;
  const PDG_CodeType squarkubarl =-1000002;
  const PDG_CodeType squarksbarl =-1000003;
  const PDG_CodeType squarkcbarl =-1000004;
  const PDG_CodeType squarkbbar1 =-1000005;
  const PDG_CodeType squarktbar1 =-1000006;
  const PDG_CodeType squarkdbarr =-2000001;
  const PDG_CodeType squarkubarr =-2000002;
  const PDG_CodeType squarksbarr =-2000003;
  const PDG_CodeType squarkcbarr =-2000004;
  const PDG_CodeType squarkbbar2 =-2000005;
  const PDG_CodeType squarktbar2 =-2000006;

  const double mass_z =91.187;
  const double width_z= 2.490;

  const bool isQuark(const PDG_CodeType pdg);
  const bool isSquark(const PDG_CodeType pdg);
  const bool isLeftSquark(const PDG_CodeType pdg);
  const bool isRightSquark(const PDG_CodeType pdg);
  const bool isElectron(const PDG_CodeType pdg);
  const bool isMuon(const PDG_CodeType pdg);
  const bool isSelectron(const PDG_CodeType pdg);
  const bool isSmuon(const PDG_CodeType pdg);
  const bool isElectronOrMuon(const PDG_CodeType pdg);
  const bool isSelectronOrSmuon(const PDG_CodeType pdg);
  const bool isOwnAntiParticle(const PDG_CodeType pdg);
  const bool isChargedLepton(const PDG_CodeType pdg);
  const PDG_CodeType antiParticle(const PDG_CodeType pdg);
  const bool isSusyParticle(const PDG_CodeType pdg);
}

#endif
