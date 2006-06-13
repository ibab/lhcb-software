
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "TrackSelector.h"


using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackSelector );

TrackSelector::TrackSelector(const std::string& type,
                             const std::string& name,
                             const IInterface* parent):
  GaudiTool(type, name, parent){

  declareProperty("pCut",    m_pCut    = 0.0*Gaudi::Units::GeV );
  declareProperty("ptCut",   m_ptCut   = 0.0*Gaudi::Units::GeV );
  declareProperty("chi2Cut", m_chi2Cut = 1000.);
  declareProperty("hitCut",  m_hitCut = 5.);
  declareProperty("vWeight", m_vWeight = 1.);
  declareProperty("oWeight", m_oWeight = 0.5);
  declareProperty("iWeight", m_iWeight = 1.);

  declareProperty("TrackTypes", m_trTypes );

  declareInterface<ITrackSelector>(this);

}

TrackSelector::~TrackSelector() { }

StatusCode TrackSelector::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Min P Cut            = " << m_pCut << endreq;
  info() << "Min Pt Cut           = " << m_ptCut << endreq;
  info() << "Min Chi-squared Cut  = " << m_chi2Cut << endreq;
  info() << "Min hit cut          = " << m_hitCut << endreq;

  // initialise track type and alg selections
  if ( !m_trTypes.empty() )
  {
    info() << "Selecting track types = " << m_trTypes << endreq;
    for ( TrackTypes::const_iterator iT = m_trTypes.begin();
          iT != m_trTypes.end(); ++iT )
    {
      if      ( *iT == "Velo"       ) { m_selTypes[Track::Velo] = true; }
      else if ( *iT == "VeloR"      ) { m_selTypes[Track::VeloR] = true; }
      else if ( *iT == "Long"       ) { m_selTypes[Track::Long] = true; }
      else if ( *iT == "Upstream"   ) { m_selTypes[Track::Upstream] = true; }
      else if ( *iT == "Downstream" ) { m_selTypes[Track::Downstream] = true; }
      else if ( *iT == "Ttrack"     ) { m_selTypes[Track::Ttrack] = true; }
      else
      {
        return Error( "Unknown track type '"+*iT+"'" );
      }
    }
    // Note, track types not selected above, will automatically NOT be selected
  }

  return sc;
}

bool TrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // simple cuts first

  // chi-squared
  if (aTrack.chi2PerDoF() > m_chi2Cut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi-squared " << aTrack.chi2PerDoF() << " failed cut " << m_chi2Cut << endreq;
    return false;
  }

  // cut p
  if (aTrack.p() < m_pCut)
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << aTrack.p() << " failed cut " << m_pCut << endreq;
    return false;
  }

  // cut on pt
  if (aTrack.pt() < m_ptCut)
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << aTrack.pt() << " failed cut " << m_ptCut << endreq;
    return false;
  }

  // measurements
  const double nMeas = weightedMeasurementSum(aTrack);
  if (nMeas < m_hitCut)
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> #hits " << nMeas << " failed cut " << m_hitCut << endreq;
    return false;
  }

  // track types
  if ( !m_trTypes.empty() && !m_selTypes[aTrack.type()] ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track type " << aTrack.type() << " is rejected" << endreq;
    return false;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

double TrackSelector::weightedMeasurementSum(const Track& aTrack) const
{

  double wSum = 0;
  const std::vector<LHCbID>& ids = aTrack.lhcbIDs();
  for ( std::vector<LHCbID>::const_iterator iter = ids.begin();
        iter != ids.end(); ++iter )
  {
    if (iter->isVelo() == true){
      wSum += m_vWeight;
    }
    else if (iter->isST() == true) {
      wSum += m_iWeight;
    }
    else if (iter->isOT() == true) {
      wSum += m_oWeight;
    }
    else {
      // nothing
    }
  }

  return wSum;
}
