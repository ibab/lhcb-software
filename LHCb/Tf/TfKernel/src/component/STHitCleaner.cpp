// $Id: STHitCleaner.cpp,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "STHitCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STHitCleaner
//
// 2007-07-20 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( STHitCleaner );

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

  typedef std::pair<unsigned int,std::vector<const STHit*> > BeetleInfo;
  typedef std::map< const DeSTSector*, BeetleInfo > BeetleMap;
  BeetleMap bMap;

  // loop over hits and group per sector
  for ( STHits::const_iterator iH = begin; iH != end; ++iH )
  {
    BeetleInfo & beetle = bMap[ &(*iH)->sector() ];
    beetle.second.push_back( *iH );
    beetle.first += (*iH)->cluster().pseudoSize();
  }

  // Loop over hits and add to output container if not too hot
  for ( BeetleMap::const_iterator iB = bMap.begin(); iB != bMap.end(); ++iB )
  {
    const unsigned occ = (iB->second.first);
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Sector " << iB->first->id() << " has occupancy " << occ << endreq;
    }
    if ( occ <= m_maxBeetleOcc )
    {
      output.insert( output.end(), iB->second.second.begin(), iB->second.second.end() );
    }
    else if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "DeSTSector " << iB->first->id()
              << " suppressed due to high occupancy = " << occ << endreq;
    }
  }

}

//=============================================================================
