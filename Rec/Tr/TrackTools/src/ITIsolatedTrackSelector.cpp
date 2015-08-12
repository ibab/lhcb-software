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
#include "GaudiKernel/SystemOfUnits.h"

#include "Kernel/STLexicalCaster.h"

// track interfaces
#include "TrackInterfaces/IHitExpectation.h"

// Tsa
#include "ITIsolatedTrackSelector.h"
#include "LoKi/select.h"
#include "TrackInterfaces/ISTClusterCollector.h"
// Boost
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#include <functional>
using std::bind;
using namespace std::placeholders;
#else
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;
#endif

#include <boost/lambda/lambda.hpp>

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;
using namespace LHCb;
using namespace Gaudi;
using namespace std;

DECLARE_TOOL_FACTORY( ITIsolatedTrackSelector )

//-----------------------------------------------------------------------------

ITIsolatedTrackSelector::ITIsolatedTrackSelector( const string& type,
						  const string& name,
						  const IInterface* parent ):
  GaudiHistoTool ( type, name, parent )
{

  // interface
  declareInterface< ITrackSelector >( this );

  declareProperty( "MaxHitNbr", m_maxHitNbr =  2 );
  declareProperty( "MinITHits", m_minNumITHits = 6 );
  declareProperty( "ExpectedHitsTool", m_expectedHitsTool =
                   "ITHitExpectation" );
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

  m_expectedHits  = tool< IHitExpectation >( "ITHitExpectation",
					     m_expectedHitsTool );
 
  return sc;
}

bool ITIsolatedTrackSelector::accept ( const Track& aTrack ) const
{
  const vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  vector<LHCb::LHCbID> expectedHits;;
  vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
  LoKi::select(ids.begin(), ids.end(), back_inserter(itHits),
	       bind(&LHCbID::isIT,_1));

  plot( itHits.size(), "ITHits", "Number of real IT its added by pat reco",
	-.5, 30.5, 31 );

  if ( itHits.size() < m_minNumITHits )
    {
      plot( -1., "selection", "Selection -1 is too few, 0 is too much",
	    -1.5, 1.5, 3 );
      return false;
    }

  Category type = ITCategory(itHits);

  ISTClusterCollector::Hits output;

  m_collectors[type-1] -> execute( aTrack, output );

  m_expectedHits -> collect( aTrack, expectedHits );

  plot(output.size(), ST::toString(type) + "surround",
       "Surrounding hits for type " + ST::toString(type), -.5, 20.5, 21);

  plot(output.size(), "ALLsurround", "Surrounding hits", -.5, 20.5, 21);

  plot( output.size() - expectedHits.size(), "criteria",
	"Isolation criteria (found minus expected hits)",
	-4.5, 10.5, 15 );

  if ( output.size() < expectedHits.size() )
    {
      plot( 1., "selection", "Selection -1 is too few, 0 is too much",
	    -1.5, 1.5, 3 );
      return true;
    }
  else if ( ( output.size() - expectedHits.size() ) > m_maxHitNbr ) 
    {
      plot( 0., "selection", "Selection -1 is too few, 0 is too much",
	    -1.5, 1.5, 3 );
      return false;
    }
  else
    {
      plot( 1., "selection", "Selection -1 is too few, 0 is too much",
	    -1.5, 1.5, 3 ); 
      return true;
    }
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
