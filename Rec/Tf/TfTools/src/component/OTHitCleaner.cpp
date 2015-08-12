// Include files

// local
#include "OTHitCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTHitCleaner
//
// 2007-07-20 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTHitCleaner )

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
  typedef std::vector<size_t> IndexContainer ;
  IndexContainer indexmono[2] ;
  indexmono[0].reserve(64) ;
  indexmono[1].reserve(64) ;

  while( currentend != end) {
    // first select hits in this module
    OTHits::const_iterator currentbegin = currentend ;
    const DeOTModule& module = (*currentbegin)->module() ;
    do { ++currentend ; } while(currentend != end && &((*currentend)->module()) == &module ) ;
    
    // now divide the hits by mono layer. I wanted to nmake this a
    // container of iterators but the compilor doesn't like it.
    indexmono[0].clear() ;
    indexmono[1].clear() ;
    for( OTHits::const_iterator it = currentbegin; it != currentend ; ++it)
      indexmono[ (*it)->monolayer() ].push_back(std::distance(begin,it)) ;

    // process each monolayer
    for(unsigned int imono=0; imono<2; ++imono)
      if( !indexmono[imono].empty() ) {
	IndexContainer::const_iterator clusterbeginindex = indexmono[imono].begin() ;
	IndexContainer::const_iterator clusterendindex   = indexmono[imono].begin() ;
	IndexContainer::const_iterator endindex  = indexmono[imono].end() ;
	OTHits::const_iterator currenthit(begin) ;
	std::advance(currenthit,*clusterendindex) ;
	while(clusterendindex  != endindex ) {
	  bool endofcluster = ++clusterendindex == endindex ;
	  if( !endofcluster ) {
	    OTHits::const_iterator nexthit(begin) ;
	    std::advance(nexthit,*clusterendindex) ;
	    endofcluster = abs( int((*currenthit)->straw()) - int((*nexthit)->straw()) ) != 1 ;
	    currenthit = nexthit ;
	  }
          if(endofcluster) {
            // flip the bools if the cluster is accepted
            if( std::distance(clusterbeginindex,clusterendindex) <= m_maxClusterSize)
              for( ; clusterbeginindex!=clusterendindex; ++clusterbeginindex)
                selectedhits[*clusterbeginindex] = true ;
	    clusterbeginindex = clusterendindex ;
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
