
#ifndef GlobalPID_C
#define GlobalPID_C

#include "GlobalPID.h"

#include <TH2.h>
#include <TStyle.h>
#include <TText.h>
#include <TLatex.h>
#include "TGraphErrors.h"
#include "TArrow.h"

#include <sstream>

GlobalPID::PIDType GlobalPID::getMcType() const
{
  if      ( 0    == MCParticleType                            ) { return NoMCInfo; }
  else if ( 11   == MCParticleType || 11    == MCParticleType ) { return Electron; }
  else if ( 13   == MCParticleType || -13   == MCParticleType ) { return Muon;     }
  else if ( 211  == MCParticleType || -211  == MCParticleType ) { return Pion;     }
  else if ( 321  == MCParticleType || -321  == MCParticleType ) { return Kaon;     }
  else if ( 2212 == MCParticleType || -2212 == MCParticleType ) { return Proton;   }
  else                                                   { return UnknownParticle; }
}

bool GlobalPID::passPIDDetSelection() const
{
  if ( config.mustHaveAnyRICH && !hasRichInfo()    ) return false;
  if ( config.mustHaveAerogel && !hasAerogelInfo() ) return false;
  if ( config.mustHaveR1Gas   && !hasRich1Gas()    ) return false;
  if ( config.mustHaveR2Gas   && !hasRich2Gas()    ) return false;
  if ( config.mustHaveECAL    && !hasEcalInfo()    ) return false;
  if ( config.mustHaveHCAL    && !hasHcalInfo()    ) return false;
  if ( config.mustHaveBREM    && !hasBremInfo()    ) return false;
  if ( config.mustHavePRS     && !hasPrsInfo()     ) return false;
  if ( config.mustHaveECALorPRS && !hasPrsInfo() && !hasEcalInfo() ) return false;
  if ( config.mustHaveECALorPRSorRICH && !hasRichInfo() &&
       !hasPrsInfo() && !hasEcalInfo() ) return false;
  if ( config.mustHaveMUON    && !hasMuonInfo()    ) return false;
  return true;
}

bool GlobalPID::passBasicPIDDet() const
{
  if ( config.mustHaveAnyRICH || config.mustHaveAerogel ||
       config.mustHaveR1Gas || config.mustHaveR2Gas )
  {
    if ( !hasRichInfo() ) return false;
  }
  if ( config.mustHaveECAL    && !hasEcalInfo()    ) return false;
  if ( config.mustHaveHCAL    && !hasHcalInfo()    ) return false;
  if ( config.mustHaveBREM    && !hasBremInfo()    ) return false;
  if ( config.mustHavePRS     && !hasPrsInfo()     ) return false;
  if ( config.mustHaveECALorPRS && !hasPrsInfo() && !hasEcalInfo() ) return false;
  if ( config.mustHaveECALorPRSorRICH && !hasRichInfo() && !hasPrsInfo() && !hasEcalInfo() ) return false;
  // cannot apply muon sel due to fact protos only have info for IDed muons
  // must, for the moment, assume all protos have muons info
  if ( config.mustHaveMUON && !hasMuonInfo()    ) return false;
  return true;
}

bool GlobalPID::passTrackSelection() const
{
  // Get the MC type
  const PIDType mcType = getMcType();

  // skip particles without MC
  if ( mcType == NoMCInfo || mcType == UnknownParticle ) return false;

  // track type
  if ( config.trackType != TrackType ) return false;

  // momentum cuts
  if ( TrackP  > config.maxP  || TrackP  < config.minP  ||
       TrackPt > config.maxPt || TrackPt < config.minPt ) return false;

  //std::cout<<" Pass Track momentum cut "<< std::endl;


  //Angle Cuts
  const double aAngle = TrackP > 0.0 ? fabs(1000.0*asin(TrackPt/TrackP)) : 0.0;
  if ( aAngle< config.minAngle ) return false;
  if ( aAngle> config.maxAngle ) return false;
  //  std::cout<<" Pass Track angle cut "<< std::endl;


  // track quality cuts. Not to be used for upgrade since tracking does not have these yet.

  if ( TrackGhostProb  > config.maxGhostProb       ||
       TrackGhostProb  < config.minGhostProb       ||
       TrackLikelihood > config.maxTrackLikelihood ||
       TrackLikelihood < config.minTrackLikelihood ) return false;


  return true;
}

void GlobalPID::makeCurve(const Long64_t nTracks)
{
  if ( fChain == 0 ) return;

  // reset track count
  tksInAcc.clear();

  // PID map
  PIDStepData pidData;

  // number of steps
  const double dllStep = (config.maxCut-config.minCut)/(double)(config.nSteps-1);

  // loop over all entries in the ntuple
  Long64_t nentries = fChain->GetEntriesFast();
  if ( nTracks > 0 && nTracks < nentries ) nentries = nTracks;
  std::cout << "Looping over " << nentries << " TTree entries ..." << std::flush;
  Long64_t nbytes(0), nb(0);
  for ( Long64_t jentry = 0; jentry<nentries; ++jentry )
  {
    const Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;
    //------------------------------------------------------------------------------

    // pid det selection
    if ( !passBasicPIDDet()    ) continue;

    // run the track selection
    if ( !passTrackSelection() ) continue;

    // Get the MC type
    const PIDType mcType = getMcType();

    // Get the physical cut variable
    const double thisVar = selectionVar();

    // count tracks in acceptance by type
    ++tksInAcc[mcType];

    // loop over DLL cuts
    double cut(config.minCut);
    for ( unsigned int iStep = 0; iStep < config.nSteps; ++iStep, cut += dllStep )
    {
      const bool selected = ( testCut(thisVar,cut) && passPIDDetSelection() && passSecondaryCut() );
      PIDData & data = pidData[cut];
      if ( selected )
      {
        ++data[mcType].first;
      }
      else
      {
        // work around for muon ID.
        // since only IDed muons are stored loose appropriate normalisation
        if ( Muon != config.idType || Muon != mcType || hasMuonInfo() )
          ++data[mcType].second;
      }
    }

    //------------------------------------------------------------------------------
  }
  std::cout << " Done." << std::endl
            << "Creating " << config.title << std::endl;

  // figure file name
  std::string figName = imageName();
  if ( !config.superImpose )
  {
    bool goOn(true);
    while ( goOn )
    {
      CanvasData::iterator i = figures.find(figName);
      if ( i != figures.end() )
      {
        figName += "_EXTRA";
      } else { goOn = false; }
    }
  }

  // make a canvas for this plot
  static TCanvas * canvas(NULL);
  if ( !canvas || !config.superImpose )
  {
    canvas = new TCanvas( figName.c_str(), figName.c_str(), 1400, 1000 );
    canvas->SetLogx( config.logX );
    canvas->SetLogy( config.logY );
  }

  // save this canvas to the list
  if ( !config.superImpose )
  {
    figures[figName] = canvas;
  }

  const unsigned int labelRate = pidData.size()/config.maxLabels;

  std::vector<double> ideff, idefferr, misideff, misidefferr;
  Labels labels;
  static unsigned int labelOffset = 0;
  unsigned int iLabel(999);
  for ( PIDStepData::iterator iStep = pidData.begin();
        iStep != pidData.end(); ++iStep, ++iLabel )
  {
    // ID
    const double selPart    = ((iStep->second)[config.idType]).first;
    const double totPart    = selPart + ((iStep->second)[config.idType]).second;
    const double partEff    = 100 * ( totPart>0 ? selPart/totPart : 0 );
    const double partEffErr = 100 * poisError( selPart, totPart );

    // misID
    const double selPartMisID    = ((iStep->second)[config.misidType]).first;
    const double totPartMisID    = selPartMisID + ((iStep->second)[config.misidType]).second;
    const double partMisIDEff    = 100 * ( totPartMisID>0 ? selPartMisID/totPartMisID : 0 );
    const double partMisIDEffErr = 100 * poisError( selPartMisID, totPartMisID );

    if ( partEff >= config.minIDeff &&
         partEff <= config.maxIDeff &&
         partMisIDEff >= config.minMisIDeff &&
         partMisIDEff <= config.maxMisIDeff )
    {
      ideff.push_back( partEff );
      idefferr.push_back( partEffErr );
      misideff.push_back( partMisIDEff );
      misidefferr.push_back( partMisIDEffErr );
    }

    // label ?
    if ( config.writeCutValues && iLabel >= labelRate )
    {
      iLabel = labelOffset;
      std::ostringstream label;
      //label << "cut=";
      label << iStep->first;
      labels.push_back( Label(partEff, partMisIDEff, label.str()) );
    }
  }
  labelOffset += 1;

  if ( config.useFixedGraphRange && !config.superImpose )
  {
    TPad* XYpad = new TPad("XYpad","XYpad",0.0,0.0,1.,1.,0,0,0);
    XYpad->SetFillStyle(0);
    XYpad->SetFrameFillStyle(0);
    XYpad->SetLogx( config.logX );
    XYpad->SetLogy( config.logY );
    XYpad->Draw();
    XYpad->cd();
    TH1F* temp = XYpad->DrawFrame(config.minGraphX, config.minGraphY, config.maxGraphX, config.maxGraphY);
    temp->SetTitle( config.title.c_str() );
    temp->GetXaxis()->SetTitle( (name(config.idType)+" ID Efficiency / %").c_str()    );
    temp->GetYaxis()->SetTitle( (name(config.misidType)+" MisID Efficiency / %").c_str() );
  }

  if ( !ideff.empty() )
  {

    // make a graph
    TGraphErrors * g = new TGraphErrors( ideff.size(),
                                         &*ideff.begin(), &*misideff.begin(),
                                         &*idefferr.begin(), &*misidefferr.begin() );
    g->SetMarkerStyle(21);
    g->SetMarkerSize(0.5);
    g->SetMarkerColor(config.color);
    g->SetLineColor(config.color);

    if ( !config.superImpose )
    {
      g->SetTitle( config.title.c_str() );
      g->GetXaxis()->SetTitle( (name(config.idType)+" ID Efficiency / %").c_str()       );
      g->GetYaxis()->SetTitle( (name(config.misidType)+" MisID Efficiency / %").c_str() );
    }
    else
    {
      g->SetTitle( "" );
      g->GetXaxis()->SetTitle( "" );
      g->GetYaxis()->SetTitle( "" );
    }
    if ( config.useFixedGraphRange )
    {
      g->Draw("LP");
    }
    else
    {
      if ( !config.superImpose )
      {
        g->Draw("ALP");
      }
      else
      {
        g->Draw("LP");
      }
    }

    // labels
    for ( Labels::const_iterator iL = labels.begin(); iL != labels.end(); ++iL )
    {
      const double xScale(1.00), yScale(0.96);
      TText * text = new TText();
      text->SetTextSize(0.012);
      text->SetTextColor(config.color);
      text->DrawText( xScale*(iL->x), yScale*(iL->y), iL->label.c_str() );
      //TArrow * arrow = new TArrow( xScale*(iL->x), yScale*(iL->y), iL->x, iL->y, 0.02, "|>" );
      //arrow->SetLineColor(config.color);
      //arrow->SetFillColor(config.color);
      //arrow->Draw();
    }


    // Fill a label box
    fillLabelBox();

  }
  else
  {
    std::cout << "WARNING : Zero data points found for "
              << name(config.idType) << " v " << name(config.misidType)
              << " -> No graph created"
              << std::endl;
  }

}

void GlobalPID::recreateCombinedDLL()
{
  // reset combined DLLS
  double NewCombDLLe  = 0;
  double NewCombDLLmu = 0;
  double NewCombDLLpi = 0;
  double NewCombDLLk  = 0;
  double NewCombDLLp  = 0;
  // RICH
  if ( hasRichInfo() )
  {
    NewCombDLLe  += 7.0*tanh(RichDLLe/40.0);
    NewCombDLLmu += 7.0*tanh(RichDLLmu/5.0);
    NewCombDLLpi += RichDLLpi;
    NewCombDLLk  += RichDLLk;
    NewCombDLLp  += RichDLLp;
    //NewCombDLLpi += 7.0*tanh(RichDLLpi/40.0);
    //NewCombDLLk  += 7.0*tanh(RichDLLk/40.0);
    //NewCombDLLp  += 7.0*tanh(RichDLLp/40.0);
  }
  // CALO
  if ( hasEcalInfo() )
  {
    NewCombDLLe  += EcalPIDe;
    NewCombDLLmu += EcalPIDmu;
  }
  if ( hasHcalInfo() )
  {
    NewCombDLLe  += HcalPIDe;
    NewCombDLLmu += HcalPIDmu;
  }
  if ( hasPrsInfo()  ) { NewCombDLLe += PrsPIDe;  }
  if ( hasBremInfo() ) { NewCombDLLe += BremPIDe; }
  // Muon
  if ( hasMuonInfo() )
  {
    NewCombDLLe  += MuonBkgLL;
    NewCombDLLmu += MuonMuLL;
    NewCombDLLpi += MuonBkgLL;
    NewCombDLLk  += MuonBkgLL;
    NewCombDLLp  += MuonBkgLL;
  }
  //std::cout << "Old CombDLLs     = " << CombDLLe << " " << CombDLLmu << " "
  //          << CombDLLpi << " " << CombDLLk << " " << CombDLLp << std::endl;
  //std::cout << "New CombDLLs     = " << NewCombDLLe << " " << NewCombDLLmu << " "
  //          << NewCombDLLpi << " " << NewCombDLLk << " " << NewCombDLLp << std::endl;
  CombDLLe=NewCombDLLe;
  CombDLLmu=NewCombDLLmu;
  CombDLLpi=NewCombDLLpi;
  CombDLLk=NewCombDLLk;
  CombDLLp=NewCombDLLp;
  //std::cout << "Updated CombDLLs = " << CombDLLe << " " << CombDLLmu << " "
  //          << CombDLLpi << " " << CombDLLk << " " << CombDLLp << std::endl;
}

std::string GlobalPID::name( const GlobalPID::PIDType type ) const
{
  switch ( type )
  {
  case Electron : return "Electron";
  case Muon     : return "Muon";
  case Pion     : return "Pion";
  case Kaon     : return "Kaon";
  case Proton   : return "Proton";
  default       : return "Unknown";
  }
}

std::string GlobalPID::name( const GlobalPID::Variable var ) const
{
  switch ( var )
  {
  case richDLLe  : return "RichDLLe";
  case richDLLmu : return "RichDLLmu";
  case richDLLpi : return "RichDLLpi";
  case richDLLk  : return "RichDLLk";
  case richDLLp  : return "RichDLLp";
  case combDLLe  : return "CombDLLe";
  case combDLLmu : return "CombDLLmu";
  case combDLLpi : return "CombDLLpi";
  case combDLLk  : return "CombDLLk";
  case combDLLp  : return "CombDLLp";
  case muonMuLL  : return "MuonMuLL";
  case muonBkgLL : return "MuonBkgLL";
  case ecalPIDe  : return "EcalPIDe";
  case hcalPIDe  : return "HcalPIDe";
  case ecalPIDmu : return "EcalPIDmu";
  case hcalPIDmu : return "HcalPIDmu";
  case prsPIDe   : return "PrsPIDe";
  case bremPIDe  : return "BremPIDe";
  case caloPIDe  : return "EcalPIDe+HcalPIDe+PrsPIDe+BremPIDe";
  case caloPIDmu : return "EcalPIDmu+HcalPIDmu";
  case muonNShared : return "MuonNShared";
  default        : return "Unknown";
  }
}

std::string GlobalPID::name( const GlobalPID::TrkType type ) const
{
  switch ( type )
  {
  case Velo       : return "Velo";
  case VeloR      : return "VeloR";
  case Long       : return "Long";
  case Upstream   : return "Upstream";
  case Downstream : return "Downstream";
  case Ttrack     : return "Ttrack";
  default         : return "Unknown";
  }
}

double GlobalPID::caloTotalPIDe() const
{
  double val(0);
  if ( hasEcalInfo() ) val += EcalPIDe;
  if ( hasHcalInfo() ) val += HcalPIDe;
  if ( hasPrsInfo()  ) val += PrsPIDe;
  if ( hasBremInfo() ) val += BremPIDe;
  return val;
}

double GlobalPID::caloTotalPIDmu() const
{
  double val(0);
  if ( hasEcalInfo() ) val += EcalPIDmu;
  if ( hasHcalInfo() ) val += HcalPIDmu;
  return val;
}

double GlobalPID::variable( const Variable var ) const
{
  switch ( var )
  {
  case richDLLe  : return RichDLLe;
  case richDLLmu : return RichDLLmu;
  case richDLLpi : return RichDLLpi;
  case richDLLk  : return RichDLLk;
  case richDLLp  : return RichDLLp;
  case combDLLe  : return CombDLLe;
  case combDLLmu : return combinedMuonDLL();
  case combDLLpi : return CombDLLpi;
  case combDLLk  : return CombDLLk;
  case combDLLp  : return CombDLLp;
  case muonMuLL  : return muonDLL();
  case muonBkgLL : return MuonBkgLL;
  case ecalPIDe  : return EcalPIDe;
  case hcalPIDe  : return HcalPIDe;
  case ecalPIDmu : return EcalPIDmu;
  case hcalPIDmu : return HcalPIDmu;
  case prsPIDe   : return PrsPIDe;
  case bremPIDe  : return BremPIDe;
  case caloPIDe  : return caloTotalPIDe();
  case caloPIDmu : return caloTotalPIDmu();
  case muonNShared : return muonSharedHits();
  default        : return 0;
  }
}

void GlobalPID::fillLabelBox() const
{
  ostringstream line0,line1,line2,line3,line5,line6;
  line0 << config.subtitle;
  line1 << name(config.var1);
  if ( config.var2 != NoVar ) line1 << "-" << name(config.var2);
  line1 << " " << config.cutType << " cut";
  line2 << name(config.secvar1);
  if ( config.secvar2 != NoVar ) line2 << "-" << name(config.secvar2);
  line2 << " " << config.secCutType << " " << config.secondaryCut;
  line3 << name(config.trackType) << " tracks | ";
  line3 << config.minP/1000  << "<P(GeV)<" << config.maxP/1000;
  line3 << " | " << config.minPt/1000 << "<Pt(GeV)<" << config.maxPt/1000;
  if ( config.minAngle > -9999 || config.maxAngle < 9999 )
  { line3 << " | " << config.minAngle << "<TkAng(mrad)<"<< config.maxAngle; }

  line5 << "Required Dets : ";
  if ( config.mustHaveAerogel )   line5 << "Aero ";
  if ( config.mustHaveR1Gas   )   line5 << "R1Gas ";
  if ( config.mustHaveR2Gas   )   line5 << "R2Gas ";
  if ( config.mustHaveAnyRICH )   line5 << "AnyRICH ";
  if ( config.mustHaveECAL    )   line5 << "ECAL ";
  if ( config.mustHaveHCAL    )   line5 << "HCAL ";
  if ( config.mustHavePRS     )   line5 << "PRS ";
  if ( config.mustHaveBREM    )   line5 << "Brem ";
  if ( config.mustHaveECALorPRS ) line5 << "ECAL or PRS ";
  if ( config.mustHaveECALorPRSorRICH ) line5 << "ECAL or PRS or RICH ";
  if ( config.mustHaveMUON    )   line5 << "MUON ";

  line6 << tksInAcc[config.idType] << " " << name(config.idType) << "s in Acceptance";

  TLatex * text = new TLatex();
  text->SetTextColor(config.color);
  text->SetNDC(true);
  text->SetTextSize(config.textSize);
  const double xOffset(0.12), yInc(config.textSize);
  static double yOffset(0);
  if ( !config.superImpose ) yOffset = 0.88;
  if ( !config.subtitle.empty() )
  { text->DrawText( xOffset, yOffset-=yInc, line0.str().c_str() ); }
  text->DrawText( xOffset, yOffset-=yInc, line1.str().c_str() );
  if ( config.secvar1 != NoVar )
  { text->DrawText( xOffset, yOffset-=yInc, line2.str().c_str() ); }
  text->DrawText( xOffset, yOffset-=yInc, line3.str().c_str() );
  text->DrawText( xOffset, yOffset-=yInc, line5.str().c_str() );
  text->DrawText( xOffset, yOffset-=yInc, line6.str().c_str() );

  yOffset-=yInc/2;
}

std::string GlobalPID::imageName() const
{
  if ( config.imageFile.empty() )
  {
    ostringstream title;
    title << name(config.var1);
    if ( config.var2 != NoVar ) title << "-" << name(config.var2);
    if ( config.secvar1 != NoVar )
    {
      title << "_" << name(config.secvar1);
      if ( config.secvar1 != NoVar )
      {
        title << "-" << name(config.secvar2);
      }
      title << "GT" << config.secondaryCut;
    }
    title << "_" << name(config.trackType);
    title << "_P" <<  config.minP/1000 << "-" << config.maxP/1000;
    title << "_Pt" <<  config.minPt/1000 << "-" << config.maxPt/1000;
    return title.str();
  }
  else
  {
    return config.imageFile;
  }
}

void GlobalPID::configReset()
{
  config = Configuration();
}

void GlobalPID::reset()
{
  // delete the current canvases
  for ( CanvasData::iterator i = figures.begin(); i != figures.end(); ++i )
  {
    delete i->second;
  }
  figures.clear();
  // reset the config
  configReset();
}

void GlobalPID::saveFigures( const std::string & type )
{
  // print to file each canvas
  for ( CanvasData::iterator i = figures.begin(); i != figures.end(); ++i )
  {
    saveImage( i->second, (i->first+"."+type).c_str() );
  }
}

//--------------------------------------------------------------------------------------------

void GlobalPID::loadTTree( const std::string & filename )
{
  resetFile();
  std::cout << "Opening file : " << filename << std::endl;
  m_file = new TFile(filename.c_str());
  m_file->cd((filename+":/ChargedProtoTuple").c_str());
  fChain = (TTree*)gDirectory->Get("protoPtuple");
  Init( fChain );
}

void GlobalPID::resetFile()
{
  if ( m_file ) { m_file->Close(); delete m_file; }
}

GlobalPID::GlobalPID() : fChain(NULL), m_file(NULL) { }

GlobalPID::GlobalPID(const std::string & filename)
  : fChain(NULL), m_file(NULL)
{
  loadTTree(filename);
}

GlobalPID::~GlobalPID()
{
  resetFile();
}

Int_t GlobalPID::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}
Long64_t GlobalPID::LoadTree(Long64_t entry)
{
  // Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (!fChain->InheritsFrom(TChain::Class()))  return centry;
  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void GlobalPID::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normaly not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("TrackP", &TrackP, &b_TrackP);
  fChain->SetBranchAddress("TrackPt", &TrackPt, &b_TrackPt);
  fChain->SetBranchAddress("TrackChi2PerDof", &TrackChi2PerDof, &b_TrackChi2PerDof);
  fChain->SetBranchAddress("TrackNumDof", &TrackNumDof, &b_TrackNumDof);
  fChain->SetBranchAddress("TrackType", &TrackType, &b_TrackType);
  fChain->SetBranchAddress("TrackHistory", &TrackHistory, &b_TrackHistory);
  fChain->SetBranchAddress("TrackGhostProb", &TrackGhostProb, &b_TrackGhostProb);
  fChain->SetBranchAddress("TrackLikelihood", &TrackLikelihood, &b_TrackLikelihood);

  fChain->SetBranchAddress("RichDLLe", &RichDLLe, &b_RichDLLe);
  fChain->SetBranchAddress("RichDLLmu", &RichDLLmu, &b_RichDLLmu);
  fChain->SetBranchAddress("RichDLLpi", &RichDLLpi, &b_RichDLLpi);
  fChain->SetBranchAddress("RichDLLk", &RichDLLk, &b_RichDLLk);
  fChain->SetBranchAddress("RichDLLp", &RichDLLp, &b_RichDLLp);
  fChain->SetBranchAddress("RichDLLbt", &RichDLLbt, &b_RichDLLbt);
  fChain->SetBranchAddress("RichUsedAero", &RichUsedAero, &b_RichUsedAero);
  fChain->SetBranchAddress("RichUsedR1Gas", &RichUsedR1Gas, &b_RichUsedR1Gas);
  fChain->SetBranchAddress("RichUsedR2Gas", &RichUsedR2Gas, &b_RichUsedR2Gas);
  fChain->SetBranchAddress("RichAboveElThres", &RichAboveElThres, &b_RichAboveElThres);
  fChain->SetBranchAddress("RichAboveMuThres", &RichAboveMuThres, &b_RichAboveMuThres);
  fChain->SetBranchAddress("RichAbovePiThres", &RichAbovePiThres, &b_RichAbovePiThres);
  fChain->SetBranchAddress("RichAboveKaThres", &RichAboveKaThres, &b_RichAboveKaThres);
  fChain->SetBranchAddress("RichAbovePrThres", &RichAbovePrThres, &b_RichAbovePrThres);
  fChain->SetBranchAddress("MuonBkgLL", &MuonBkgLL, &b_MuonBkgLL);
  fChain->SetBranchAddress("MuonMuLL", &MuonMuLL, &b_MuonMuLL);
  fChain->SetBranchAddress("MuonNShared", &MuonNShared, &b_MuonNShared);
  fChain->SetBranchAddress("MuonIsLooseMuon", &MuonIsLooseMuon, &b_MuonIsLooseMuon);
  fChain->SetBranchAddress("MuonIsMuon", &MuonIsMuon, &b_MuonIsMuon);
  fChain->SetBranchAddress("MuonInAcc", &MuonInAcc, &b_MuonInAcc);
  fChain->SetBranchAddress("InAccSpd", &InAccSpd, &b_InAccSpd);
  fChain->SetBranchAddress("InAccPrs", &InAccPrs, &b_InAccPrs);
  fChain->SetBranchAddress("InAccEcal", &InAccEcal, &b_InAccEcal);
  fChain->SetBranchAddress("InAccHcal", &InAccHcal, &b_InAccHcal);
  fChain->SetBranchAddress("InAccBrem", &InAccBrem, &b_InAccBrem);
  fChain->SetBranchAddress("CaloTrMatch", &CaloTrMatch, &b_CaloTrMatch);
  fChain->SetBranchAddress("CaloElectronMatch", &CaloElectronMatch, &b_CaloElectronMatch);
  fChain->SetBranchAddress("CaloBremMatch", &CaloBremMatch, &b_CaloBremMatch);
  fChain->SetBranchAddress("CaloChargedSpd", &CaloChargedSpd, &b_CaloChargedSpd);
  fChain->SetBranchAddress("CaloChargedPrs", &CaloChargedPrs, &b_CaloChargedPrs);
  fChain->SetBranchAddress("CaloChargedEcal", &CaloChargedEcal, &b_CaloChargedEcal);
  fChain->SetBranchAddress("CaloSpdE", &CaloSpdE, &b_CaloSpdE);
  fChain->SetBranchAddress("CaloPrsE", &CaloPrsE, &b_CaloPrsE);
  fChain->SetBranchAddress("CaloEcalChi2", &CaloEcalChi2, &b_CaloEcalChi2);
  fChain->SetBranchAddress("CaloClusChi2", &CaloClusChi2, &b_CaloClusChi2);
  fChain->SetBranchAddress("CaloBremChi2", &CaloBremChi2, &b_CaloBremChi2);
  fChain->SetBranchAddress("CaloEcalE", &CaloEcalE, &b_CaloEcalE);
  fChain->SetBranchAddress("CaloHcalE", &CaloHcalE, &b_CaloHcalE);
  fChain->SetBranchAddress("CaloTrajectoryL", &CaloTrajectoryL, &b_CaloTrajectoryL);
  fChain->SetBranchAddress("EcalPIDe", &EcalPIDe, &b_EcalPIDe);
  fChain->SetBranchAddress("HcalPIDe", &HcalPIDe, &b_HcalPIDe);
  fChain->SetBranchAddress("PrsPIDe", &PrsPIDe, &b_PrsPIDe);
  fChain->SetBranchAddress("BremPIDe", &BremPIDe, &b_BremPIDe);
  fChain->SetBranchAddress("EcalPIDmu", &EcalPIDmu, &b_EcalPIDmu);
  fChain->SetBranchAddress("HcalPIDmu", &HcalPIDmu, &b_HcalPIDmu);
  fChain->SetBranchAddress("CombDLLe", &CombDLLe, &b_CombDLLe);
  fChain->SetBranchAddress("CombDLLmu", &CombDLLmu, &b_CombDLLmu);
  fChain->SetBranchAddress("CombDLLpi", &CombDLLpi, &b_CombDLLpi);
  fChain->SetBranchAddress("CombDLLk", &CombDLLk, &b_CombDLLk);
  fChain->SetBranchAddress("CombDLLp", &CombDLLp, &b_CombDLLp);
  fChain->SetBranchAddress("ProbNNe",&ProbNNe,&b_ProbNNe);
  fChain->SetBranchAddress("ProbNNmu",&ProbNNmu,&b_ProbNNmu);
  fChain->SetBranchAddress("ProbNNpi",&ProbNNpi,&b_ProbNNpi);
  fChain->SetBranchAddress("ProbNNk",&ProbNNk,&b_ProbNNk);
  fChain->SetBranchAddress("ProbNNp",&ProbNNp,&b_ProbNNp);
  fChain->SetBranchAddress("ProbNNghost",&ProbNNghost,&b_ProbNNghost);
  fChain->SetBranchAddress("VeloCharge", &VeloCharge, &b_VeloCharge);
  fChain->SetBranchAddress("MCParticleType", &MCParticleType, &b_MCParticleType);
  fChain->SetBranchAddress("MCParticleP", &MCParticleP, &b_MCParticleP);
  fChain->SetBranchAddress("MCParticlePt", &MCParticlePt, &b_MCParticlePt);
  fChain->SetBranchAddress("MCVirtualMass", &MCVirtualMass, &b_MCVirtualMass);

  Notify();
}

Bool_t GlobalPID::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normaly not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}

void GlobalPID::Show(Long64_t entry)
{
  // Print contents of entry.
  // If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

// method to save image files from a canvas
void GlobalPID::saveImage ( TCanvas * canvas,
                            const std::string& filename )
{
  if ( canvas )
  {
    std::string _name = filename;
    typedef std::map<std::string,bool> LocalFileMap;
    static LocalFileMap fileMap;
    bool goOn(true);
    while ( goOn )
    {
      LocalFileMap::iterator iF = fileMap.find(_name);
      if ( iF != fileMap.end() )
      {
        _name = "NEWFIG_"+_name;
      } else { goOn = false; }
    }
    fileMap[_name] = true;
    std::cout << "Printing image " << _name << std::endl;
    canvas->SaveAs( _name.c_str() );
  }
}

void GlobalPID::makeCode()
{
  if ( fChain ) fChain->MakeClass();
}

#endif
