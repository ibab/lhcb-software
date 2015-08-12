//-----------------------------------------------------------------------------
/** @file ITTrackConfirmation.cpp
 *
 *  Implementation file for reconstruction tool : ITTrackConfirmation
 *
 *  CVS Log :-
 *  $Id: ITTrackConfirmation.cpp,v 1.2 2009-07-06 18:29:28 jonrob Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "ITTrackConfirmation.h"

#include "TrackInterfaces/ITrackCaloMatch.h"
#include "Event/Track.h"
#include "CaloUtils/Calo2Track.h"

// Boost
#include <boost/foreach.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// Loki
#include "LoKi/select.h"

using namespace LHCb;
using namespace boost::lambda;

DECLARE_TOOL_FACTORY( ITTrackConfirmation )

//-----------------------------------------------------------------------------

ITTrackConfirmation::ITTrackConfirmation( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiHistoTool ( type, name, parent )
{

  // interface
  declareProperty("selfConfirm",m_selfConfirm = true);
  declareProperty("caloConfirm",m_caloConfirm = true);
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = 1000 );
  declareProperty("MinCaloEnergy", m_minCaloEnergy =0.0*Gaudi::Units::MeV);

  declareInterface<ITrackSelector>(this);

}

ITTrackConfirmation::~ITTrackConfirmation() { }

StatusCode ITTrackConfirmation::initialize()
{
  const StatusCode sc = GaudiHistoTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_trackCaloMatch = tool<ITrackCaloMatch>("TrackCaloMatch");

  return sc;
}

bool ITTrackConfirmation::accept ( const Track& aTrack ) const
{

 // chi2 test 
 const double chi2 = aTrack.chi2PerDoF();
 if (chi2 > m_maxChi2Cut) return false;

 // confirm your self
 if (m_selfConfirm){
   if (selfConfirm(aTrack) == true) return true;
 }

 // use the calorimeter to confirm
 if (m_caloConfirm){
   double energy = m_trackCaloMatch->energy(aTrack);
   plot(energy/Gaudi::Units::GeV, "energy in calo", 0., 100., 400);
   if (energy > m_minCaloEnergy) return true;
 }

 return false;
}


bool ITTrackConfirmation::selfConfirm(const LHCb::Track& aTrack) const {

 // a track can confirm itself if it passes thro 3 stations

 const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
 std::vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
 LoKi::select(ids.begin(), ids.end(), std::back_inserter(itHits), bind(&LHCbID::isIT,_1));
 
 // count stations
 std::vector<unsigned int> stations;
 BOOST_FOREACH( LHCb::LHCbID id, itHits ){
   stations.push_back(id.stID().station());
 } 

 // number of unique elements
 std::sort(stations.begin(), stations.end());
 stations.erase(std::unique(stations.begin(),stations.end()), stations.end());
 plot(stations.size(), "# stations", -0.5, 10.5, 11);  

 return stations.size() == 3 ? true : false ;
}

