
#include <sstream>
#include <tuple>
#include <vector>

void DataMCCompare()
{

  // Location of ROOT files
  const std::string rootDir = "/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/StripFiltered/";

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
  TFile * data_f = TFile::Open( (rootDir+"2015-S23r1-ANNPID-BHADRONCOMPLETEEVENT.root").c_str() );
  TTree * data   = (TTree*)gDirectory->Get("ANNPID/DecayTree");
  TFile * mc_f   = TFile::Open( (rootDir+"Incb-DevMCJun2015-S23r1-ANNPID-Pythia8.root").c_str() );
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
  gStyle->SetOptStat(0);

  // variable name, min, max
  typedef std::vector< std::tuple<std::string,double,double,unsigned int> > Variables;

  const Variables vars =
    {
      std::make_tuple( "NumRich1Hits", -0.5, 10000.5, 101 ),
      std::make_tuple( "NumRich2Hits", -0.5, 10000.5, 101 ),
      std::make_tuple( "NumSPDHits", -0.5, 1000.5, 201 ),
      std::make_tuple( "NumLongTracks", -0.5, 175.5, 176 ),

      std::make_tuple( "TrackP", 0, 100000, 100 ),
      std::make_tuple( "TrackPt", 0, 10000, 100 ),
      std::make_tuple( "TrackChi2PerDof", 0, 3.5, 100 ),
      std::make_tuple( "TrackNumDof", -0.5, 50.5 , 51 ),
      std::make_tuple( "TrackGhostProbability", 0, 1, 100 ),
      std::make_tuple( "TrackFitMatchChi2", 0, 10, 100 ),
      std::make_tuple( "TrackFitVeloChi2", 0, 10, 100 ),
      std::make_tuple( "TrackFitVeloNDoF", -0.5, 20.5, 21 ),
      std::make_tuple( "TrackFitTChi2", 0, 50, 100 ),
      std::make_tuple( "TrackFitTNDoF", -0.5, 30.5, 31 ),

      std::make_tuple( "RichUsedR1Gas", -0.5, 1.5, 2 ),
      std::make_tuple( "RichUsedR2Gas", -0.5, 1.5, 2 ),
      std::make_tuple( "RichAbovePiThres", -0.5, 1.5, 2 ),
      std::make_tuple( "RichAboveKaThres", -0.5, 1.5, 2 ),
      std::make_tuple( "RichDLLe", -50, 50 , 100 ),
      std::make_tuple( "RichDLLmu",-50, 50 , 100 ),
      std::make_tuple( "RichDLLk", -50, 50 , 100 ),
      std::make_tuple( "RichDLLp", -50, 50 , 100 ),
      std::make_tuple( "RichDLLbt", -50, 50 , 100 ),

      std::make_tuple( "EcalPIDe", -10, 10, 100 ),
      std::make_tuple( "EcalPIDmu", -10, 10, 100 ),
      std::make_tuple( "HcalPIDe", -10, 10, 100 ),
      std::make_tuple( "HcalPIDmu", -10, 10, 100 ),
      std::make_tuple( "PrsPIDe", -10, 10, 100 ),
      std::make_tuple( "InAccBrem", -0.5, 1.5, 2 ),
      std::make_tuple( "BremPIDe", -10, 10, 100 ),

      std::make_tuple( "MuonBkgLL", -10, 10, 100 ),
      std::make_tuple( "MuonMuLL", -10, 10, 100 ),
      std::make_tuple( "MuonNShared", -0.5, 30.5 , 31 ),
      std::make_tuple( "InAccMuon", -0.5, 1.5, 2 ),
      std::make_tuple( "MuonIsMuon", -0.5, 1.5, 2 ),
      std::make_tuple( "MuonIsLooseMuon", -0.5, 1.5, 2 ),

      std::make_tuple( "piplus_MC15TuneV1_ProbNNe", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC15TuneV1_ProbNNmu", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC15TuneV1_ProbNNpi", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC15TuneV1_ProbNNk", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC15TuneV1_ProbNNp", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC15TuneV1_ProbNNghost", 0, 1 , 100 ),
  
      std::make_tuple( "piplus_MC12TuneV4_ProbNNe", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC12TuneV4_ProbNNmu", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC12TuneV4_ProbNNpi" ,0, 1 , 100 ),
      std::make_tuple( "piplus_MC12TuneV4_ProbNNk", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC12TuneV4_ProbNNp", 0, 1 , 100 ),
      std::make_tuple( "piplus_MC12TuneV4_ProbNNghost", 0, 1, 100 )

    };

      

  
  for ( const auto & tvar : vars )
  {
    const auto & var = std::get<0>(tvar);
    std::cout << "Making plots for " << var << std::endl;
    
    const double min = std::get<1>(tvar);
    const double max = std::get<2>(tvar);
    std::ostringstream tmp1;
    tmp1 << var << " > " << min << " && " << var << " < " << max;
    TCut varSel = tmp1.str().c_str();
    //std::cout << tmp1.str() << std::endl;
    //TCut varSel = "";

    const auto nbins = std::get<3>(tvar);
    const std::string hParams = "("+std::to_string(nbins)+","+std::to_string(min)+","+std::to_string(max)+")";
    
    data->Draw( (var+">>dataH"+var+hParams).c_str(), varSel && tkSel, "prof" );
    TH1F * hdata = (TH1F*) gDirectory->Get(("dataH"+var).c_str());
    //hdata->Sumw2();
    const double dentries = hdata->GetEntries();
    //std::cout << "data entries = " << dentries << std::endl;
    hdata->Scale( 1.0/dentries );
    hdata->SetMarkerColor(kRed);
    hdata->SetLineColor(kRed);
    
    mc->Draw( (var+">>mcH"+var+hParams).c_str(), varSel && tkSel, "prof" );
    TH1F * hmc = (TH1F*) gDirectory->Get(("mcH"+var).c_str());
    //hmc->Sumw2();
    const double mcentries = hmc->GetEntries(); //hmc->Integral(min,max);
    //std::cout << "mc entries = " << mcentries << std::endl;
    hmc->Scale( 1.0/mcentries );
    hmc->SetMarkerColor(kBlue);
    hmc->SetLineColor(kBlue);

    // which plot has biggest bin entry ? Need to draw first.
    auto plots = ( hdata->GetBinContent(hdata->GetMaximumBin()) >
                   hmc->GetBinContent(hmc->GetMaximumBin()) ? 
                   std::make_pair(hdata,hmc) : std::make_pair(hmc,hdata) );

    // Legion for plot
    TLegend * legend = new TLegend(0.865,0.9,0.935,0.85);
    legend->SetTextSize(0.02);
    legend->SetMargin(0.12);

    legend->AddEntry( hdata, "Data", "p" );
    legend->AddEntry( hmc, year.c_str(), "p" );

    plots.first->SetTitle( (year+" "+var).c_str() );

    c->SetLogy(false);
    plots.first->Draw();
    plots.second->Draw("SAME");
    legend->Draw();
    c->SaveAs( ("pdf/"+year+"-"+var+"_liny.pdf").c_str() );

    c->SetLogy(true);
    plots.first->Draw();
    plots.second->Draw("SAME");
    legend->Draw();
    c->SaveAs( ("pdf/"+year+"-"+var+"_logy.pdf").c_str() );

    delete legend;
    
  }


}
