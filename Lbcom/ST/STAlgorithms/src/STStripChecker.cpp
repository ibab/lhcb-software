// Include files 

// local
#include "STStripChecker.h"

#include "Kernel/STTell1Board.h"
#include "Kernel/BeetleRepresentation.h"

#include <algorithm>
#include <boost/foreach.hpp>

#include "AIDA/IProfile1D.h"

using namespace std;
using namespace LHCb;
using namespace ST;
using namespace AIDA;

//-----------------------------------------------------------------------------
// Implementation file for class : STStripChecker
//
// 2009-10-22 : Johan Luisier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STStripChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STStripChecker::STStripChecker( const string& name,
                                ISvcLocator* pSvcLocator)
  : HistoAlgBase ( name , pSvcLocator ), m_counter( 0 )
{
  declareProperty( "SignalToNoise", m_signalToNoiseCut = 8. );
  declareProperty( "HitsNumber"   ,  m_hitsNumber = 1u );

  declareSTConfigProperty( "DataLocation", m_DataLoc,
                           "Raw/IT/Clusters" );
  
  setForcedInit();
}
//=============================================================================
// Destructor
//=============================================================================
STStripChecker::~STStripChecker()
{} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STStripChecker::initialize()
{
  StatusCode sc( HistoAlgBase::initialize() ); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HistoAlgBase

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  initMap();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STStripChecker::execute()
{
  StatusCode sc( SUCCESS );
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  clusterCont = get< STClusters >( m_DataLoc );

  STClusters::const_iterator itClust, ClustBegin(clusterCont -> begin()),
    ClustEnd(clusterCont -> end());

  vector< STChannelID > channelIDs;
  DeSTSector *sector;
  double Noise;
  vector< STChannelID >::const_iterator clusIt, clusBegin, clusEnd;

  for (itClust = ClustBegin; itClust != ClustEnd; itClust++)
    {
      channelIDs = (*itClust) -> channels();

      sector = tracker() -> findSector( channelIDs.front() );

      if ( sector == 0 ) continue;

      clusEnd   = channelIDs.end();
      clusBegin = channelIDs.begin();

      for (clusIt = clusBegin; clusIt != clusEnd; clusIt++)
	{
	  
	  Noise = sector -> noise( *clusIt );
	  
      
	  if ( static_cast<double>( (*itClust) -> adcValue(clusIt - clusBegin )  )>
	       Noise * m_signalToNoiseCut ) 
	    {
	      if ( m_StripsMap.find( *clusIt ) != m_StripsMap.end() )
		m_StripsMap[ *clusIt ]++;
	    }
	}
    }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STStripChecker::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  pair< LHCb::STChannelID, unsigned int > elem;
  BOOST_FOREACH( elem, m_StripsMap )
    {
      printZeros( elem );
    }

  info() << endmsg << "In total, " << m_counter << " strips didn't have a hit"
	 << endmsg;

  vector< DeSTSector* > sectors;

  sectors = tracker() -> sectors();

  vector< DeSTSector* >::const_iterator secIt(sectors.begin()),
    secEnd( sectors.end() );

  unsigned int strip, nbrStrips;

  STChannelID channelID;
  string nick;

  for (; secIt != secEnd; secIt++)
    {
      nbrStrips = (*secIt) -> nStrip();
      nick = (*secIt) -> nickname();
      for (strip = 1; strip < nbrStrips; strip++)
	{
	  channelID = (*secIt) -> stripToChan( strip );
	  profile1D( strip, m_StripsMap[ channelID ], nick,
		     "Number of hits in " + nick,
		     .5, nbrStrips + .5, nbrStrips );
	}
    }

  return HistoAlgBase::finalize();  // must be called after all other actions
}

StatusCode STStripChecker::initMap()
{
  StatusCode sc( SUCCESS );

  vector< DeSTSector* > sectors;

  sectors = tracker() -> sectors();

  vector< DeSTSector* >::const_iterator secIt(sectors.begin()),
    secEnd( sectors.end() );

  unsigned int strip;

  STChannelID channelID;

  for (; secIt != secEnd; secIt++)
    {
      for (strip = 1; strip < (*secIt) -> nStrip(); strip++)
	{
	  channelID = (*secIt) -> stripToChan( strip );
	  if ( (*secIt) -> isOKStrip( channelID ) )
	    m_StripsMap[ channelID ] = 0u;
	}
    }

  return sc;
}

void STStripChecker::printZeros( const pair< STChannelID, unsigned int >& p )
{
  if ( p.second < m_hitsNumber )
  {
    DeSTSector * sector;
    sector = tracker() -> findSector( p.first );

    if ( m_lastSector != sector -> nickname() )
    {
      info() << endmsg;
      m_lastSector = sector -> nickname();
      
      info() << "No hits found in  " << sector -> nickname()
	     << " strip(s) ";
    }
    
    info() << p.first.strip() << ' ';
    m_counter++;
  }
}

//=============================================================================
