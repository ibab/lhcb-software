
// STL
//#include <random> // For flags tests. To be removed.

// Event model
#include "Event/MCParticle.h"
#include "Event/StandardPacker.h"
#include "Event/PackedMCParticle.h"

// local
#include "UnpackMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( UnpackMCParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  UnpackMCParticle::UnpackMCParticle( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedMCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::MCParticleLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
UnpackMCParticle::~UnpackMCParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackMCParticle::execute() 
{

  // CRJ : If packed data does not exist just return. Needed for packing of
  //     : spillover which is not neccessarily available for each event
  if ( !m_alwaysOutput && !exist<LHCb::PackedMCParticles>(m_inputName) )
    return StatusCode::SUCCESS;

  LHCb::PackedMCParticles* dst =
    getOrCreate<LHCb::PackedMCParticles,LHCb::PackedMCParticles>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedMCParticles = " << dst->mcParts().size() << endmsg;

  LHCb::MCParticles* newMCParticles = new LHCb::MCParticles();
  put( newMCParticles, m_outputName );

  StandardPacker pack(this);

  // Packing version
  const char pVer = dst->packingVersion();

  // random generator for private tests of flags. 
  //static std::default_random_engine gen;
  //static std::uniform_real_distribution<float> uniform(0,1);

  newMCParticles->reserve( dst->mcParts().size() );
  for ( const LHCb::PackedMCParticle& src : dst->mcParts() )
  {

    LHCb::MCParticle* part = new LHCb::MCParticle( );
    newMCParticles->insert( part, src.key );

    const double px = pack.energy( src.px );
    const double py = pack.energy( src.py );
    const double pz = pack.energy( src.pz );
    const double mass = src.mass;
    const double E = std::sqrt( (px*px) + (py*py) + (pz*pz) + (mass*mass) );
    part->setMomentum( Gaudi::LorentzVector( px, py, pz , E ) );

    part->setParticleID( LHCb::ParticleID(src.PID) );

    part->setFlags( src.flags );
    // for testing, randomly set 'fromSignal' to true 5% of the time.
    //part->setFromSignal( uniform(gen) > 0.95 );

    int hintID(0), key(0);
    if ( ( 0==pVer && pack.hintAndKey32( src.originVertex, dst, newMCParticles, hintID, key ) ) ||
         ( 0!=pVer && pack.hintAndKey64( src.originVertex, dst, newMCParticles, hintID, key ) ) )
    {
      SmartRef<LHCb::MCVertex> ref( newMCParticles, hintID, key );
      part->setOriginVertex( ref );
    }
    else { Error( "Corrupt MCParticle Origin MCVertex SmartRef detected" ).ignore(); }

    for ( const auto& I : src.endVertices )
    {
      if ( ( 0==pVer && pack.hintAndKey32( I, dst, newMCParticles, hintID, key ) ) ||
           ( 0!=pVer && pack.hintAndKey64( I, dst, newMCParticles, hintID, key ) ) )
      {
        SmartRef<LHCb::MCVertex> refV( newMCParticles, hintID, key );
        part->addToEndVertices( refV );
      }
      else { Error( "Corrupt MCParticle End MCVertex SmartRef detected" ).ignore(); }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
