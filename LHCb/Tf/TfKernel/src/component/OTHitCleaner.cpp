// $Id: OTHitCleaner.cpp,v 1.3 2007-08-20 11:07:07 jonrob Exp $
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
  declareProperty( "MaxClusterSize", m_maxclustersize = 5 );
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

  info() << "Max OT cluster size     = " << m_maxclustersize << endreq;
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
  // clean out large clusters
  removeClusters( begin, end, tmpHits ); // CRJ disabled due to possible bug
  // clean hot modules
  //output.reserve(startSize) ;
  //removeHotModules( begin, end, output );
  if ( !tmpHits.empty() )
  {
    output.reserve(tmpHits.size()) ;
    removeHotModules( tmpHits.begin(), tmpHits.end(), output );
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

  // we'll do this by module

  OTHits::const_iterator lastend = begin ;
  OTHits::const_iterator nextend = begin ;
  ++nextend ;

  size_t numhits = std::distance(begin,end) ;
  std::vector<bool> selectedhits(numhits,false) ;

  while( nextend != end) {
    // first select hits in this module
    do { ++nextend ; }
    while(nextend != end && &((*nextend)->module()) == &((*lastend)->module()) ) ;

    // now divide the hits by mono layer. I wanted to nmake this a
    // container of iterators but the compilor doesn't like it.
    typedef std::vector<size_t> IndexContainer ;
    IndexContainer indexmono[2] ;
    for( OTHits::const_iterator it = lastend; it != nextend ; ++it)
      indexmono[ (*it)->monolayer() ].push_back(std::distance(begin,it)) ;

    // process each monolayer
    for(unsigned int imono=0; imono<2; ++imono)
      if( !indexmono[imono].empty() ) 
      {
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
            abs( (*nexthit)->straw() - (*thishit)->straw()) != 1 ;
          if(endofcluster) {
            // flip the bools if the cluster is accepted
            if( std::distance(lastindex,nextindex) <= m_maxclustersize)
              for( ; lastindex!=nextindex; ++lastindex)
                selectedhits[*lastindex] = true ;
            // otherwise, just update the index
            else
              lastindex = nextindex ;
          }
        }
      }
    lastend = nextend ;
  }

  for ( size_t index = 0; index < selectedhits.size(); ++index)
  {
    if( selectedhits[index] ) output.push_back( *(lastend + index) ) ;
  }

}

void OTHitCleaner::removeHotModules( const OTHits::const_iterator begin,
                                     const OTHits::const_iterator end,
                                     OTHits& output ) const
{ 
  // Might be possible to improve this and make it faster ...

  typedef std::map<const DeOTModule*, std::vector<const OTHit*> > ModuleMap;
  ModuleMap mMap;

  // loop over hits and group per module
  for ( OTHits::const_iterator iH = begin; iH != end; ++iH )
  {
    mMap[ &(*iH)->module() ].push_back( *iH );
  }

  // Loop over hits and add to output container if not too hot
  for ( ModuleMap::const_iterator iM = mMap.begin(); iM != mMap.end(); ++iM )
  {
    // occupancy
    const double occ = (double)(iM->second.size()) / (double)(iM->first->nChannels());
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Module " << iM->first->uniqueModule() << " has occupancy " << occ << endreq;
    }
    if ( occ <= m_maxModuleOcc )
    {
      output.insert( output.end(), iM->second.begin(), iM->second.end() );
    }
    else if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "DeOTModule " << iM->first->uniqueModule() 
              << " suppressed due to high occupancy = " << occ 
              << " > " << m_maxModuleOcc << " maximum" << endreq;
    }
  }

}

//=============================================================================

// CRJ : Don't think this is used
/*
  template<class ForwardIterator, class OutputContainer, class BinaryPredicate>
  inline void removeClusters( ForwardIterator begin,
  ForwardIterator end,
  OutputContainer& output,
  BinaryPredicate neighbour )
  {
  ForwardIterator lastend = begin ;
  ForwardIterator next    = begin ;
  while(next != end) {
  ForwardIterator current = next++ ;
  bool endofcluster = next==end || !neighbour( *current, *next) ;
  if(endofcluster) {
  if( std::distance(lastend,next) <= m_maxclustersize )
  for( ; lastend!= next ; ++lastend)
  output.push_back(*lastend) ;
  else
  lastend = next ;
  }
  }
  }
*/
