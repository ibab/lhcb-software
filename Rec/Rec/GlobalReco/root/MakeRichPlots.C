#include "TROOT.h"
#include "TSystem.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH1F.h"

void MakeRichPlots()
{

  const std::string imageType = "pdf";

  const std::string dir = "/Users/chris/LHCb/RootFiles/Run2";
  //const std::string dir = "/usera/jonesc/LHCbCMake/lhcb-head/BrunelDevNightly/output/Run2";

  // load the file
  //const std::string nametag = "Quartic";
  const std::string nametag = "Quartic-NoAmbig";
  //const std::string nametag = "Esti-All";
  TFile * f = TFile::Open((dir+"/"+nametag+"/50000events/protoparticles.tuples.root").c_str());

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut detOK = "RichUsedAero || RichUsedR1Gas || RichUsedR2Gas";

  std::string trackSelS = "TrackType==3 && TrackP>3000 && TrackP<100000 && TrackChi2PerDof<5";
  //trackSelS += " && TrackGhostProb<0.9"; // add ghost prob cut
  //trackSelS += " && TrackLikelihood>-40"; // Likelihood
  //trackSelS += " && (TrackCloneDist>5000 || TrackCloneDist<0)"; // clone distance
  TCut trackSel = trackSelS.c_str();

  TCut realPr = "abs(MCParticleType) == 2212";
  TCut realK  = "abs(MCParticleType) == 321";
  TCut realPi = "abs(MCParticleType) == 211";

  TCut prAboveThres = "RichAbovePrThres";
  TCut kAboveThres  = "RichAboveKaThres";
  TCut piAboveThres = "RichAbovePiThres";

  TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );

  std::vector<double> cuts;
  cuts.push_back(-5);
  cuts.push_back(-2.5);
  cuts.push_back(0);
  cuts.push_back(2.5);
  cuts.push_back(5);

  // Kaon ID plots
  for ( std::vector<double>::const_iterator iC = cuts.begin();
        iC != cuts.end(); ++iC )
  {
    std::ostringstream cC; 
    cC << *iC;

    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>kIDEff").c_str(),     
                detOK && realK  && trackSel && kAboveThres, "prof" );
    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(), 
                detOK && realPi && trackSel && piAboveThres, "prof" );
    TH1F * kIDEff     = (TH1F*) gDirectory->Get("kIDEff");
    TH1F * piMisIDEff = (TH1F*) gDirectory->Get("piMisIDEff");

    if ( kIDEff && piMisIDEff )
    {
      kIDEff->SetTitle( ( "DLLk>"+cC.str() + " && " + trackSelS ).c_str() );
      kIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
      kIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
      kIDEff->Draw();
      kIDEff->SetMarkerColor(kRed);
      kIDEff->SetLineColor(kRed);
      
      piMisIDEff->Draw("SAME");
      piMisIDEff->SetMarkerColor(kBlue);
      piMisIDEff->SetLineColor(kBlue);
      
      c->SaveAs( (nametag+"-KaonID-DLL"+cC.str()+"."+imageType).c_str() );
    }
    else
    {
      std::cerr << "Error making kaon plots" << std::endl;
    }

  }

  // Proton ID plots
  for ( std::vector<double>::const_iterator iC = cuts.begin();
        iC != cuts.end(); ++iC )
  {
    std::ostringstream cC; 
    cC << *iC;

    tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>prIDEff").c_str(),     
                detOK && realPr && trackSel && prAboveThres, "prof" );
    tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(), 
                detOK && realPi && trackSel && piAboveThres, "prof" );
    TH1F * prIDEff    = (TH1F*) gDirectory->Get("prIDEff");
    TH1F * piMisIDEff = (TH1F*) gDirectory->Get("piMisIDEff");

    if ( prIDEff && piMisIDEff )
    {
      prIDEff->SetTitle( ( "DLLp>"+cC.str() + " && " + trackSelS ).c_str() );
      prIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
      prIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
      prIDEff->Draw();
      prIDEff->SetMarkerColor(kRed);
      prIDEff->SetLineColor(kRed);
      
      piMisIDEff->Draw("SAME");
      piMisIDEff->SetMarkerColor(kBlue);
      piMisIDEff->SetLineColor(kBlue);
      
      c->SaveAs( (nametag+"-ProtonID-DLL"+cC.str()+"."+imageType).c_str() );   
    }
    else
    {
      std::cerr << "Error making proton plots" << std::endl;
    }

  }

}
