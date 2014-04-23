// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT

void doDalitz(int seed = 0){

  gSystem->Load("libPhysics.so");
  gSystem->Load("libRooFit.so");

  gROOT->LoadMacro("twoDStyle.C");
  gROOT->LoadMacro("histoUtils.C");
  twoDStyle();

  std::cout << " doDalitz called with rnd seed = " << seed << endl;

  gROOT->LoadMacro("AbsComplexBrackets.C+");
  gROOT->LoadMacro("AbsComplex.C+");
  gROOT->LoadMacro("AbsComplexPdf.C+");
  gROOT->LoadMacro("dataThrowerAway.C+");
  gROOT->LoadMacro("ComplexProxy.C+");
  gROOT->LoadMacro("ComplexBW.C+");
  gROOT->LoadMacro("Calculate4BodyProps.h+");
  gROOT->LoadMacro("BlattWeisskopf.C+");
  gROOT->LoadMacro("SpinFactor.C+");
  gROOT->LoadMacro("SpinFactors.C+");
  gROOT->LoadMacro("DcyAmplitude.C+");
  gROOT->LoadMacro("DbleAmplitude.C+");
  gROOT->LoadMacro("rhoOmega.C+");
  gROOT->LoadMacro("DbleAmplitudeWithRho.C+");
  //  gROOT->LoadMacro("newMatrixElement.C+");
  gROOT->LoadMacro("Resonance.C+");
  gROOT->LoadMacro("ComplexSum.C+");
  //  gROOT->LoadMacro("ComplexSumPdf.C+");
  gROOT->LoadMacro("ComplexProd.C+");
  gROOT->LoadMacro("doFractions.C+");
  gROOT->LoadMacro("RooAbsPhaseSpace.C+");
  gROOT->LoadMacro("Roo3BodyPhaseSpace.C+");
  gROOT->LoadMacro("DKKpipiResonances.C+");
  //  gROOT->LoadMacro("DKsPiPiResonances.C+");
  gROOT->LoadMacro("fourMxy.C+");
  gROOT->LoadMacro("makeMassPlots4.C+");

  gROOT->LoadMacro("Roo4BodyPhaseSpace.C+");
  gROOT->LoadMacro("RooDalitz.C+");
  gROOT->LoadMacro("RooBinomiDalitz.C+");
  gROOT->LoadMacro("mark.C+");
  gROOT->LoadMacro("Grahm.C+");

  //  gROOT->LoadMacro("test4Body.C+");
  //test4Body();

  gROOT->LoadMacro("simpleDalitz.C+");
  simpleDalitz(seed);

  //  gROOT->LoadMacro("flatPhaseSpace.C+");
  // flatPhaseSpace();

  //  gROOT->LoadMacro("justPhaseSpace.C+");
  //  justPhaseSpace();

  //gROOT->LoadMacro("binomiDalitz.C+");
  //binomiDalitz();


}

