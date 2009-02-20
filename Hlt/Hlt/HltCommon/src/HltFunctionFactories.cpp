// $Id: HltFunctionFactories.cpp,v 1.25 2009-02-20 11:47:54 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltFunctions.h"
#include "HltFunctionFactories.h"
#include "HltTFunctionFactory.icpp"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctionFactories
//
// 2007-12-04 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackFunctionFactory );

DECLARE_TOOL_FACTORY( HltVertexFunctionFactory );


//=============================================================================
HltTrackFunctionFactory::HltTrackFunctionFactory( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : HltTFunctionFactory<LHCb::Track> ( type, name , parent )
{
}

HltTrackFunctionFactory::~HltTrackFunctionFactory() {
  // TODO HltTFunctionFactory needs to delete the m_creators
} 

StatusCode HltTrackFunctionFactory::initialize() {

  StatusCode sc = HltTFunctionFactory<LHCb::Track>::initialize();

  declare("L0ET");
  declare<Hlt::PT>("PT");
  declare<Hlt::PT>("PT0");
  declare<Hlt::TrackFlag>("IsBackward");
  declare<Hlt::NumberOfASideVeloHits>("NumberOfASideVeloHits");
  declare<Hlt::NumberOfCSideVeloHits>("NumberOfCSideVeloHits");

  declare<Hlt::rIP,zen::abs_min,Hlt::VertexSelection>("rIP");
  declare<Hlt::IP,zen::abs_min,Hlt::VertexSelection>("IP");
  declare<Hlt::IP,zen::abs_min,Hlt::VertexSelection>("FitIP");

  declare<Hlt::DOCA,zen::abs_min,Hlt::TrackSelection>("DOCA");
  declare<Hlt::MatchIDsFraction,zen::abs_max,Hlt::TrackSelection>("MatchIDsFraction");
  declare<Hlt::DoShareM3,zen::abs_max,Hlt::TrackSelection>("DoShareM3");

  declare<Hlt::FitChi2OverNdf>("FitChi2OverNdf");
  declare<Hlt::FitMuChi2>("FitMuChi2");
  declare<Hlt::FitCleanedChi2OverNdf>("FitCleanedChi2OverNdf");
  
  declare<ITrackBiFunctionTool,zen::abs_min,Hlt::TrackSelection>
    ("Calo2DChi2","HltRZVeloTCaloMatch");

  declare<ITrackBiFunctionTool,zen::abs_min,Hlt::TrackSelection>
    ("Calo3DChi2","HltVeloTCaloMatch");

  declare<ITrackBiFunctionTool,zen::abs_min,Hlt::TrackSelection>
    ("RZVeloTMatch","HltMatchTVeloTracks");

  declare<ITrackFunctionTool>("MuonIDDistance","HltMuonIDDistanceTool");

  declare<ITrackFunctionTool>("DeltaP","HltDeltaPTool");
  declare<ITrackFunctionTool>("IsMuon","HltIsMuonTool");
  declare<ITrackFunctionTool>("IsPhoton","HltIsPhotonTool");
  declare<ITrackFunctionTool>("AntiEleConf","HltAntiEleConf");
  declare<ITrackBiFunctionTool,zen::abs_min,Hlt::TrackSelection>
    ("Ecal2DChi2","HltRZVeloEcalMatch");

  declare<ITrackBiFunctionTool,zen::abs_min,Hlt::TrackSelection>
    ("Ecal3DChi2","HltVeloEcalMatch");

  declare<ITrackFunctionTool>("ptAtOrigin","PtTransporter");

  return sc;
}



//=============================================================================


HltVertexFunctionFactory::HltVertexFunctionFactory( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : HltTFunctionFactory<LHCb::RecVertex> ( type, name , parent )
{
}

HltVertexFunctionFactory::~HltVertexFunctionFactory() {
  // TODO HltTFunctionFactory needs to delete the m_creators
} 

StatusCode HltVertexFunctionFactory::initialize() {

  StatusCode sc = HltTFunctionFactory<LHCb::RecVertex>::initialize();
  
  declare<Hlt::DZ,zen::abs_min,Hlt::VertexSelection>("VertexDz");
  declare<Hlt::DZ,zen::abs_min,Hlt::VertexSelection>("FitVertexDz");
  declare<Hlt::FC,zen::abs_min,Hlt::VertexSelection>("VertexPointing");
  declare<Hlt::FC,zen::abs_min,Hlt::VertexSelection>("FitVertexPointing");
  declare<Hlt::VertexMinIP,zen::abs_min,Hlt::VertexSelection>("VertexMinIP");
  declare<Hlt::VertexMinIP,zen::abs_min,Hlt::VertexSelection>("FitVertexMinIP");
  declare<Hlt::VertexMatchIDsFraction,zen::abs_max,Hlt::VertexSelection>("VertexMatchIDsFraction");
  
  declare<Hlt::VertexAngle>("VertexAngle");
  declare<Hlt::VertexAngle>("FitVertexAngle");
  declare<Hlt::VertexDOCA>("VertexDOCA");
  declare<Hlt::VertexDOCA>("FitVertexDOCA");
  declare<Hlt::VertexDimuonMass>("VertexDimuonMass");
  declare<Hlt::VertexDimuonMass>("FitVertexDimuonMass");
  declare<Hlt::VertexSumPT>("VertexSumPT");
  declare<Hlt::VertexMinPT>("VertexMinPT");
  declare<Hlt::VertexMaxPT>("VertexMaxPT");
  declare<Hlt::VertexMinPT>("FitVertexMinPT");
  declare<Hlt::VertexMaxPT>("FitVertexMaxPT");
  declare<Hlt::VertexMaxChi2OverNdf>("FitVertexMaxChi2OverNdf");
  declare<Hlt::VertexMaxMuChi2>("FitVertexMaxMuChi2");

  declare<Hlt::VertexNumberOfASideTracks>("VertexNumberOfASideTracks");
  declare<Hlt::VertexNumberOfCSideTracks>("VertexNumberOfCSideTracks");

  declare<IRecVertexFunctionTool>("VertexDiElectronMass","VertexDiElectronMassTool");

  return sc;
}



//=============================================================================

