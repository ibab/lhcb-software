// $Id: PrintDuplicates.cpp,v 1.1 2012-10-11 11:56:34 elsasser Exp $

// local
#include "PrintDuplicates.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrintDuplicates )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintDuplicates::PrintDuplicates( const std::string& name,
                                  ISvcLocator* pSvcLocator )
: DaVinciAlgorithm ( name , pSvcLocator ),
  m_printDecay ( NULL )
{
  declareProperty( "MaxPrintoutsPerTESLoc", m_maxPrints = 1 );
  declareProperty( "DeepCheck", m_deepCheck = true );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
/// Destructor
//=============================================================================
PrintDuplicates::~PrintDuplicates() {}

//#############################################################################
/// Execution
//#############################################################################
StatusCode PrintDuplicates::execute()
{

  typedef std::map< Hash32, LHCb::Particle::ConstVector > PartHashMap;
  typedef std::map< std::string, PartHashMap > LocHashMap;

  // local map for this event
  LocHashMap hashMap;

  // Loop over particles and compute hashes
  for ( LHCb::Particle::Range::const_iterator ip = particles().begin();
        ip != particles().end(); ++ip )
  {
    // sanity check
    if ( !*ip ) continue;
    // compute the hash for this decay tree
    const Hash32 h = getLHCbIDsHash( *ip );
    // save this entry
    (hashMap[tesLocation(*ip)])[h] .push_back ( *ip );
  }

  // Look for duplicates within TES locations
  for ( LocHashMap::const_iterator iL = hashMap.begin();
        iL != hashMap.end(); ++iL )
  {
    // The TES location
    const std::string& loc = iL->first;

    // Loop over the distinct trees within this TES location
    for ( PartHashMap::const_iterator iPH = iL->second.begin();
          iPH != iL->second.end(); ++iPH )
    {
      // do we have any duplicates
      if ( iPH->second.size() > 1 )
      {
        // make a local copy of the duplicates vector, for futher processing
        LHCb::Particle::ConstVector dups = iPH->second;

        // Check the hash values for daughters ?
        if ( !m_deepCheck ) { report(loc,dups); }
        else
        {
          FilteredDups filtDups;
          deepHashCheck( iPH->second, filtDups );
          for ( FilteredDups::const_iterator iD = filtDups.begin();
                iD != filtDups.end(); ++iD )
          {
            report(loc,*iD);
          }
        }

      }
    }

  } // loop over TES locations

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

void PrintDuplicates::report( const std::string & loc,
                              const LHCb::Particle::ConstVector & parts )
{
  std::ostringstream mess;
  mess << "Found " << parts.size()
       << " duplicate Particles in '" << loc << "'";
  Warning( mess.str(), StatusCode::FAILURE, m_maxPrints+1 );
  if ( m_countPerLoc[mess.str()]++ < m_maxPrints )
  {
    printDecay()->printTree( parts.begin(), parts.end() );
  }
}

void PrintDuplicates::deepHashCheck( const LHCb::Particle::ConstVector & parts,
                                     FilteredDups& filtDups ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Performing deep hash comparison" << endmsg;

  // First, check the daughters seperately
  typedef std::map< Hashes64, LHCb::Particle::ConstVector > HashCount;
  HashCount hashCount;

  // Loop over particles
  for ( LHCb::Particle::ConstVector::const_iterator iP = parts.begin();
        iP != parts.end(); ++iP )
  {
    // hashes for this particle
    Hashes64 hashes;

    // Get the deep hashes
    getDeepHashes( *iP, hashes );

    // Sort
    std::sort( hashes.begin(), hashes.end() );

    // Print ...
    if ( msgLevel(MSG::DEBUG) ) debug() << " -> Deep Hashes : " << hashes << endmsg;

    // Save in the map
    (hashCount[hashes]).push_back(*iP);
  }

  // Save any duplicates left
  for ( HashCount::const_iterator iH = hashCount.begin(); 
        iH != hashCount.end(); ++iH )
  {
    if ( iH->second.size() > 1 ) filtDups.push_back( iH->second );
  } 

}

void
PrintDuplicates::getDeepHashes( const LHCb::Particle * p,
                                Hashes64& hashes,
                                unsigned int depth ) const
{
  // protect against infinite recursion
  if ( depth > 999999 )
  { Warning( "Infinite recursion in getDeepHashes" ).ignore(); return; }

  if ( p->isBasicParticle() )
  {

    // Fill PID type and Particle hash into a single 64 bit number
    union PIDData
    {
      struct
      {
        Hash32 pidType : 32;
        Hash32 hash    : 32;
      } packed;
      Hash64 raw;
    } data;
    data.packed.pidType = p->particleID().pid();
    data.packed.hash    = getLHCbIDsHash(p);

    // combine with the overall hash
    hashes.push_back( data.raw );

  }
  else
  {

    // Add the hash for composite to the list
    hashes.push_back( (Hash64)getLHCbIDsHash(p) );

    // loop over daughters
    for ( SmartRefVector<LHCb::Particle>::const_iterator id = p->daughters().begin();
          id != p->daughters().end(); ++id )
    {
      getDeepHashes(*id,hashes,++depth);
    }

  }

}
