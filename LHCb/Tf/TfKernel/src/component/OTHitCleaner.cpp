// $Id: OTHitCleaner.cpp,v 1.5 2007-08-28 12:03:58 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "OTHitCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTHitCleaner
//
// 2007-07-20 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTHitCleaner );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTHitCleaner::OTHitCleaner( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type , name , parent )
{
  // interface
  declareInterface<IOTHitCleaner>(this);
  // JOs
  declareProperty( "MaxClusterSize", m_maxClusterSize = 5 );
  declareProperty( "MaxModuleOccupancy", m_maxModuleOcc = 0.4 );
}

//=============================================================================
// Destructor
//=============================================================================
OTHitCleaner::~OTHitCleaner() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode OTHitCleaner::initialize ( )
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Max OT cluster size     = " << m_maxClusterSize << endreq;
  info() << "Max OT module occupancy = " << m_maxModuleOcc << endreq;

  return sc;
}

unsigned int OTHitCleaner::cleanHits( const OTHits::const_iterator begin,
                                      const OTHits::const_iterator end,
                                      OTHits& output ) const
{
  // reserve some space
  const unsigned int startSize = std::distance(begin,end);
  OTHits tmpHits; // nice if this could be avoided...
  tmpHits.reserve(startSize) ;

  // first remove hot modules (faster)
  removeHotModules( begin, end, tmpHits );

  // clean out large clusters
  if( m_maxClusterSize >0 ) {
    output.reserve( tmpHits.size() ) ;
    removeClusters( tmpHits.begin(), tmpHits.end(), output ); 
  } else {
    output.swap( tmpHits ) ;
  } 

  // how many hits where cleaned
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Selected " << output.size() << " out of " 
            << std::distance(begin,end) << " OTHits" << endreq;
  return startSize - output.size();
}

void OTHitCleaner::removeClusters( const OTHits::const_iterator begin,
                                   const OTHits::const_iterator end,
                                   OTHits& output ) const
{
  // what makes this complicated is that it needs to go by mono
  // layer. I'll try to do it such that I don't need to think about
  // it. The result is pretty horrible.

  OTHits::const_iterator currentend = begin ;
  size_t numhits = std::distance(begin,end) ;
  std::vector<bool> selectedhits(numhits,false) ;

  while( currentend != end) {
    // first select hits in this module
    OTHits::const_iterator currentbegin = currentend ;
    const DeOTModule& module = (*currentbegin)->module() ;
    do { ++currentend ; } while(currentend != end && &((*currentend)->module()) == &module ) ;
    
    // now divide the hits by mono layer. I wanted to nmake this a
    // container of iterators but the compilor doesn't like it.
    typedef std::vector<size_t> IndexContainer ;
    IndexContainer indexmono[2] ;
    for( OTHits::const_iterator it = currentbegin; it != currentend ; ++it)
      indexmono[ (*it)->monolayer() ].push_back(std::distance(begin,it)) ;

    // process each monolayer
    for(unsigned int imono=0; imono<2; ++imono)
      if( !indexmono[imono].empty() ) {
        // count cluster size. if cluster small enough, accept
        // hits. what a mess!
        IndexContainer::const_iterator lastindex = indexmono[imono].begin() ;
        IndexContainer::const_iterator endindex  = indexmono[imono].end() ;
        IndexContainer::const_iterator nextindex = lastindex ;
        OTHits::const_iterator nexthit = begin ;
        std::advance(nexthit, * nextindex ) ;
        while( nextindex != endindex ) {
          size_t thisindex = *nextindex ;
          OTHits::const_iterator thishit = nexthit ;
          ++nextindex ;
          std::advance(nexthit, (*nextindex - thisindex ) ) ;
          bool endofcluster = nextindex==endindex ||
            abs( (int)((*nexthit)->straw() - (*thishit)->straw()) ) != 1 ;
          if(endofcluster) {
            // flip the bools if the cluster is accepted
            if( std::distance(lastindex,nextindex) <= m_maxClusterSize)
              for( ; lastindex!=nextindex; ++lastindex)
                selectedhits[*lastindex] = true ;
            // otherwise, just update the index
            else
              lastindex = nextindex ;
          }
        }
      }
  }
  
  // push everything in the output list
  for ( size_t index = 0; index < selectedhits.size(); ++index)
    if( selectedhits[index] ) output.push_back( *(begin + index) ) ;
}

void OTHitCleaner::removeHotModules( const OTHits::const_iterator begin,
                                     const OTHits::const_iterator end,
                                     OTHits& output ) const
{ 

  OTHits::const_iterator currentend = begin ;
  while( currentend != end) {

    // first select hits in this module
    OTHits::const_iterator currentbegin = currentend ;
    const DeOTModule& module = (*currentbegin)->module() ;
    do { ++currentend ; } while(currentend != end && &((*currentend)->module()) == &module ) ;
    
    const double occ = (double)(std::distance(currentbegin,currentend)) / (double)(module.nChannels());
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Module " << module.uniqueModule() << " has occupancy " << occ << endreq;
    
    if( occ <= m_maxModuleOcc )
      output.insert( output.end(), currentbegin, currentend ) ;
    else if ( msgLevel(MSG::DEBUG) )
      debug() << "DeOTModule " << module.uniqueModule() 
              << " suppressed due to high occupancy = " << occ 
              << " > " << m_maxModuleOcc << " maximum" << endreq;
  }
}

//=============================================================================
