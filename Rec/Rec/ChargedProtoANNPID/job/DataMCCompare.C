
#include <sstream>

void compare()
{


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
    
    // 2015
    const std::string year = "MC2015";
    TFile * data_f = TFile::Open("2015-ANNPID-BHADRONCOMPLETEEVENT-MagDown.root");
    TTree * data   = (TTree*)gDirectory->Get("ANNPID/DecayTree");
    TFile * mc_f   = TFile::Open("Incb-MC2015-ANNPID-Pythia8.root");
    TTree * mc     = (TTree*)gDirectory->Get("ANNPID/DecayTree");

    // // 2012
    // const std::string year = "MC2012";
    // TFile * data_f = TFile::Open("2012-ANNPID-BHADRONCOMPLETEEVENT-MagDown.root");
    // TTree * data   = (TTree*)gDirectory->Get("ANNPID/DecayTree");
    // TFile * mc_f   = TFile::Open("Incb-MC2012-ANNPID-Pythia8.root");
    // TTree * mc     = (TTree*)gDirectory->Get("ANNPID/DecayTree");
    
    TCut tkSel = "TrackType==3";
    
    double min(0.0), max(1.0);
    std::ostringstream tmp1;
    tmp1 << var << " > " << min << " && " << var << " < " << max;
    
    //TCut varSel = tmp1.str().c_str();
    TCut varSel = "";
    
    TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );
    c->SetLogy(true);
    
    data->Draw( (var+">>dataH").c_str(), varSel && tkSel, "prof" );
    TH1F * a = (TH1F*) gDirectory->Get("dataH");
    a->Scale( 1.0/( a->GetEntries() ) );
    a->SetMarkerColor(kRed);
    a->SetLineColor(kRed);
    
    mc->Draw( (var+">>mcH").c_str(), varSel && tkSel, "prof" );
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
