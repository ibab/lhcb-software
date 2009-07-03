
//-----------------------------------------------------------------------------
/** @file ITIsolatedTrackSelector.cpp
 *
 *  : ITIsolatedTrackSelector
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author J Luisier Johan.Luisier@cern.ch
 *  @date   26/06/2009
 */
//-----------------------------------------------------------------------------
#include "Kernel/ITNames.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Kernel/STLexicalCaster.h"

// Tsa
#include "ITIsolatedTrackSelector.h"
#include "LoKi/select.h"
#include "TrackInterfaces/ISTClusterCollector.h"
// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;
using namespace std;

DECLARE_TOOL_FACTORY( ITIsolatedTrackSelector );

//-----------------------------------------------------------------------------

ITIsolatedTrackSelector::ITIsolatedTrackSelector( const string& type,
						  const string& name,
						  const IInterface* parent ):
  GaudiHistoTool ( type, name, parent )
{

  // interface
  declareInterface< ITrackSelector >( this );

  declareProperty("MaxHitNbr", m_maxHitNbr = 2);
  declareProperty("MinITHits", m_minNumITHits = 6);
}

ITIsolatedTrackSelector::~ITIsolatedTrackSelector()
{}

StatusCode ITIsolatedTrackSelector::initialize()
{
  const StatusCode sc = GaudiHistoTool::initialize();
  if ( sc.isFailure() ) return sc;

  string name("");

  for (unsigned int i = 0 ; i < 5; i++) 
    {
      name = "Mixed";
      if ( i < 4 ) name = ITNames().BoxToString( i + 1 );
      name += "Collector";
      m_collectors.push_back( tool< ISTClusterCollector >( "STClusterCollector", name ) );
    }
 
  return sc;
}

bool ITIsolatedTrackSelector::accept ( const Track& aTrack ) const
{


  const vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
  LoKi::select(ids.begin(), ids.end(), back_inserter(itHits), bind(&LHCbID::isIT,_1));


  plot(itHits.size(), "ITHits", "ITHits", -.5, 20.5, 21);

  if (itHits.size() < m_minNumITHits) return false;

  Category type = ITCategory(itHits);

  ISTClusterCollector::Hits output;


  m_collectors[type-1] -> execute( aTrack, output );

  plot(output.size(), ST::toString(type) + "surround",
       "Surrounding hits" + ST::toString(type), -.5, 99.5, 100);

  if ( output.size() > m_maxHitNbr ) return false;

  return true;
}

ITIsolatedTrackSelector::Category ITIsolatedTrackSelector::ITCategory(const vector<LHCb::LHCbID>& ids) const
{
  typedef map<unsigned int, unsigned int> BoxMap;
  BoxMap nBox;
  BOOST_FOREACH(LHCb::LHCbID id, ids)
    {
      nBox[ id.stID().detRegion() ] += 1;
    } // for each
  
  return nBox.size() == 1 ? Category(nBox.begin() -> first) : Mixed ;
}
