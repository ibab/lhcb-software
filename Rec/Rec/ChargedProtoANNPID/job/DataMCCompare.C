
#include <sstream>

void DataMCCompare()
{

  // Location of ROOT files
  const std::string rootDir = "/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/";

  // Stripping lines
  const auto stripLines = {
    "StrippingB02DKD2HHHBeauty2CharmLineDecision",   
    "StrippingB02DKD2Pi0HHHMergedBeauty2CharmLineDecision",   
    "StrippingB02DKD2Pi0HHHResolvedBeauty2CharmLineDecision",   
    "StrippingB02DKPiPiD2HHHPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiD2HHHBeauty2CharmLineDecision",   
    "StrippingB02DPiD2Pi0HHHMergedBeauty2CharmLineDecision",   
    "StrippingB02DPiD2Pi0HHHResolvedBeauty2CharmLineDecision",   
    "StrippingB02DPiNoIPD2HHHCFPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiNoIPDs2HHHPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiPiPiD2HHHPIDBeauty2CharmLineDecision",
    "StrippingB02DPiWSD2HHHBeauty2CharmLineDecision",
    "StrippingB0d2DTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuInvVertForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB2CharmlessQ2B3piSelectionLineDecision",
    "StrippingB2CharmlessQ2B4piSelectionLineDecision",
    "StrippingB2D0D0KD02HHD02HHBeauty2CharmLineDecision",
    "StrippingB2D0KD2HHBeauty2CharmLineDecision",
    "StrippingB2D0PiD2HHBeauty2CharmLineDecision",
    "StrippingB2D0PiD2HHHHTIGHTBeauty2CharmLineDecision",
    "StrippingB2D0PiNoIPD2KPIPIDBeauty2CharmLineDecision",
    "StrippingB2D0PiPiPiD2HHTIGHTBeauty2CharmLineDecision",
    "StrippingB2DDphi_B2D0D0PhiLineDecision",
    "StrippingBc2JpsiTauNuForB2XTauNuAllLinesDecision",
    "StrippingBc2JpsiTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuWSForB2XTauNuAllLinesDecision",
    "StrippingD02HHTopoTOSLineDecision",
    "StrippingHb2Charged2BodyB2Charged2BodyLineDecision",
    "StrippingLb2LcPiNoIPLc2PKPiBeauty2CharmLineDecision",
    "StrippingLb2LcTauNuForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuInvVertForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingLb2XicPiNoIPXic2PKPiBeauty2CharmLineDecision",
    "StrippingOmegab2Omegac0PiNoIPOmegac02PKKPiBeauty2CharmLineDecision",
    "StrippingPseudoDoubleTopoLineDecision",
    "StrippingXib2Xic0PiNoIPXic02PKKPiBeauty2CharmLineDecision"
  };
  std::string stripCutS = "(";
  for ( const std::string stripLine : stripLines )
  {
    if ( stripCutS != "(" ) { stripCutS += " ||"; }
    stripCutS += " "+stripLine;
  }
  stripCutS += " )";
  TCut stripSel( stripCutS.c_str() );
  //std::cout << stripCutS << std::endl;

  // 2015
  const std::string year = "MC2015";
  TFile * data_f = TFile::Open( (rootDir+"2015-ANNPID-BHADRONCOMPLETEEVENT.root").c_str() );
  TTree * data   = (TTree*)gDirectory->Get("ANNPID/DecayTree");
  TFile * mc_f   = TFile::Open( (rootDir+"Incb-MC2015-ANNPID-Pythia8.root").c_str() );
  TTree * mc     = (TTree*)gDirectory->Get("ANNPID/DecayTree");
  if ( !mc || !data ) return;
  
  // // 2012
  // const std::string year = "MC2012";
  // TFile * data_f = TFile::Open( (rootDir+"2012-ANNPID-BHADRONCOMPLETEEVENT-MagDown.root").c_str() );
  // TTree * data   = (TTree*)gDirectory->Get("ANNPID/DecayTree");
  // TFile * mc_f   = TFile::Open( (rootDir+"Incb-MC2012-ANNPID-Pythia8.root").c_str() );
  // TTree * mc     = (TTree*)gDirectory->Get("ANNPID/DecayTree");

  TCut tkSel = "TrackType==3";

  TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );

  for ( std::string var : {

        "NumRich1Hits",
        "NumRich2Hits",
        "NumSPDHits",
        "NumLongTracks",

        "TrackP",
        "TrackPt",
        "TrackChi2PerDof",
        "TrackNumDof",
        "TrackGhostProbability",
        "TrackFitMatchChi2",
        "TrackFitVeloChi2",
        "TrackFitVeloNDoF",
        "TrackFitTChi2",
        "TrackFitTNDoF",
        "RichUsedR1Gas",
        "RichUsedR2Gas",
        "RichAbovePiThres",
        "RichAboveKaThres",
        "RichDLLe",
        "RichDLLmu",
        "RichDLLk",
        "RichDLLp",
        "RichDLLbt",
        "MuonBkgLL",
        "MuonMuLL",
        "MuonIsMuon",
        "MuonNShared",
        "InAccMuon",
        "MuonIsLooseMuon",
        "EcalPIDe",
        "EcalPIDmu",
        "HcalPIDe",
        "HcalPIDmu",
        "PrsPIDe",
        "InAccBrem",
        "BremPIDe",
      
      "piplus_MC15TuneV1_ProbNNe",
      "piplus_MC15TuneV1_ProbNNmu",
      "piplus_MC15TuneV1_ProbNNpi",
      "piplus_MC15TuneV1_ProbNNk",
      "piplus_MC15TuneV1_ProbNNp",
      "piplus_MC15TuneV1_ProbNNghost",
  
      "piplus_MC12TuneV4_ProbNNe",
      "piplus_MC12TuneV4_ProbNNmu",
      "piplus_MC12TuneV4_ProbNNpi",
      "piplus_MC12TuneV4_ProbNNk",
      "piplus_MC12TuneV4_ProbNNp",
      "piplus_MC12TuneV4_ProbNNghost"
        
        } )
  {
    std::cout << "Making plots for " << var << std::endl;
    
    double min(0.0), max(1.0);
    std::ostringstream tmp1;
    tmp1 << var << " > " << min << " && " << var << " < " << max;
    
    //TCut varSel = tmp1.str().c_str();
    TCut varSel = "";
    
    c->SetLogy(true);
    
    data->Draw( (var+">>dataH").c_str(), stripSel && varSel && tkSel, "prof" );
    TH1F * a = (TH1F*) gDirectory->Get("dataH");
    a->Scale( 1.0/( a->GetEntries() ) );
    a->SetMarkerColor(kRed);
    a->SetLineColor(kRed);
    
    mc->Draw( (var+">>mcH").c_str(), stripSel && varSel && tkSel, "prof" );
    TH1F * b = (TH1F*) gDirectory->Get("mcH");
    b->Scale( 1.0/( b->GetEntries() ) );
    b->SetMarkerColor(kBlue);
    b->SetLineColor(kBlue);
    b->SetTitle( (year+" "+var).c_str() );
    
    b->Draw();
    a->Draw("SAME");
    
    
    c->SaveAs( (year+"-"+var+".pdf").c_str() );
    
  }


}
