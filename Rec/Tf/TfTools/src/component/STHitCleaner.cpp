// Include files

// local
#include "STHitCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STHitCleaner
//
// 2007-07-20 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( STHitCleaner )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STHitCleaner::STHitCleaner( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type , name , parent )
{
  // interface
  declareInterface<ISTHitCleaner>(this);
  // JOs
  declareProperty( "MaxSectorOccupancy", m_maxBeetleOcc = 48 );
}

//=============================================================================
// Destructor
//=============================================================================
STHitCleaner::~STHitCleaner() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode STHitCleaner::initialize ( )
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Max ST sector occupancy = " << m_maxBeetleOcc << endreq;

  return sc;
}

unsigned int STHitCleaner::cleanHits( const STHits::const_iterator begin,
                                      const STHits::const_iterator end,
                                      STHits& output ) const
{
  // reserve some space
  const unsigned int startSize = std::distance(begin,end);
  output.reserve(startSize) ;
  // clean out hot beetles
  removeHotBeetles( begin, end, output );
  // how many hits where cleaned
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Selected " << output.size() << " out of "
            << std::distance(begin,end) << " STHits" << endreq;
  return startSize - output.size();
}

void STHitCleaner::removeHotBeetles( const STHits::const_iterator begin,
                                     const STHits::const_iterator end,
                                     STHits& output ) const
{
  STHits::const_iterator currentend = begin ;
  while( currentend != end) {
    
    // select hits in this sector and add up total size
    STHits::const_iterator currentbegin = currentend ;
    const DeSTSector& sector = (*currentbegin)->sector() ;
    unsigned int occ(0) ;
    do { 
      occ += (*currentend)->cluster().pseudoSize();
      ++currentend ; 
    } while(currentend != end && &((*currentend)->sector()) == &sector ) ;
    
    // add to output container if not too hot
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Sector " << sector.id() << " has occupancy " << occ << endreq;
    }
    if ( occ <= m_maxBeetleOcc )
    {
      output.insert( output.end(), currentbegin, currentend );
    }
    else if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "DeSTSector " << sector.id()
              << " suppressed due to high occupancy = " << occ 
              << " > " << m_maxBeetleOcc << " maximum"
              << endreq;
    }
  }

}

//=============================================================================
