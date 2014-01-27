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
  declareProperty( "EnergyPrecision", m_dpPrec = 2 );
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

  typedef std::map< std::pair<Hash32,double>, LHCb::Particle::ConstVector > PartHashMap;
  typedef std::map< std::string, PartHashMap > LocHashMap;

  // local map for this event
  LocHashMap hashMap;

  const unsigned int dpScale = std::pow(10,m_dpPrec);

  // Loop over particles and compute hashes
  for ( LHCb::Particle::Range::const_iterator ip = particles().begin();
        ip != particles().end(); ++ip )
  {
    // sanity check
    if ( !*ip ) continue;
    // compute the hash for this decay tree
    const Hash32 h = getLHCbIDsHash( *ip );
    // current have to use energy to take PID swaps into account.
    // Would be better to have the option to include this in the hash. To Do.
    const double e = boost::math::round( dpScale * (*ip)->momentum().e() ) / dpScale;
    // save this energy/hash pair
    (hashMap[tesLocation(*ip)])[std::make_pair(h,e)] .push_back ( *ip );
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
        // Check the hash values for daughters ?
        const bool isDup = !m_deepCheck || deepHashCheck(iPH->second);
        if ( isDup )
        {
          std::ostringstream mess;
          mess << "Found " << iPH->second.size()
               << " duplicate Particle(s) in '" << loc << "'";
          Warning( mess.str(), StatusCode::FAILURE, m_maxPrints+1 );
          if ( m_countPerLoc[mess.str()]++ < m_maxPrints )
          {
            printDecay()->printTree( iPH->second.begin(), iPH->second.end() );
          }
        }
        // else
        // {
        //   std::ostringstream mess;
        //   mess << "Found " << iPH->second.size()
        //        << " similar decays in '" << loc << "'";
        //   Warning( mess.str(), StatusCode::FAILURE, m_maxPrints+1 );
        // }
      }
    }

  } // loop over TES locations

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

bool PrintDuplicates::deepHashCheck( const LHCb::Particle::ConstVector & parts ) const
{
  bool isDuplicate = false;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Performing deep Daughters Hash comparison" << endmsg;

  {
    // First, check the daughters seperately
    std::map< Hashes32, unsigned int > hashCount;

    // Loop over particles
    for ( LHCb::Particle::ConstVector::const_iterator iP = parts.begin();
          iP != parts.end(); ++iP )
    {
      Hashes32 hashes;
      getDauHashes( *iP, hashes );
      std::sort( hashes.begin(), hashes.end() );
      if ( msgLevel(MSG::DEBUG) ) debug() << " -> Daughter Hashes : " << hashes << endmsg;
      isDuplicate = ++(hashCount[hashes]) > 1;
      if ( isDuplicate ) break;
    }
  }

  if ( isDuplicate )
  {
    // Got this far. Now (re)test for PID switches
    isDuplicate = false;

    std::map< Hash64, unsigned int > pidHashCount;

    // Loop over particles
    for ( LHCb::Particle::ConstVector::const_iterator iP = parts.begin();
          iP != parts.end(); ++iP )
    {
      const Hash64 pidh = getPIDHash(*iP);
      if ( msgLevel(MSG::DEBUG) ) debug() << " -> PID Hash : " << pidh << endmsg;
      isDuplicate = ++(pidHashCount[pidh]) > 1;
      if ( isDuplicate ) break;
    }
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << "Duplicate = " << isDuplicate << endmsg;

  return isDuplicate;
}

PrintDuplicates::Hash64
PrintDuplicates::getPIDHash( const LHCb::Particle * p,
                             unsigned int depth ) const
{
  Hash64 h = 0;

  // protect against infinite recursion
  if ( depth > 999999 )
  { Warning( "Infinite recursion in getPIDHash" ).ignore(); return h; }

  if ( p->isBasicParticle() )
  {

    // Fill PID type and Particle hash into a single 64 bit number
    union PIDData
    {
      struct
      {
        boost::uint32_t pidType : 32;
        boost::uint32_t hash    : 32;
      } packed;
      boost::uint64_t raw;
    } data;
    data.packed.pidType = p->particleID().pid();
    data.packed.hash    = getLHCbIDsHash(p);

    // combine with the overall hash
    boost::hash_combine( h, data.raw );

  }
  else
  {
    // loop over daughters
    const SmartRefVector<LHCb::Particle>& daughters = p->daughters();
    for ( SmartRefVector<LHCb::Particle>::const_iterator id = daughters.begin();
          id != daughters.end(); ++id )
    {
      boost::hash_combine( h, getPIDHash(*id,++depth) );
    }
  }

  return h;
}

void PrintDuplicates::getDauHashes( const LHCb::Particle * p,
                                    Hashes32& hashes,
                                    unsigned int depth ) const
{
  // protect against infinite recursion
  if ( depth > 999999 )
  { Warning( "Infinite recursion in getDauHashes" ).ignore(); return; }

  // loop of daughters
  const SmartRefVector<LHCb::Particle>& daughters = p->daughters();
  for ( SmartRefVector<LHCb::Particle>::const_iterator id = daughters.begin();
        id != daughters.end(); ++id )
  {
    if ( !(*id)->isBasicParticle() )
    {
      // Add the hash for this to the list
      hashes.push_back( getLHCbIDsHash(*id) );
      // then fill for its daughters
      getDauHashes( *id, hashes, ++depth );
    }
  }

}
