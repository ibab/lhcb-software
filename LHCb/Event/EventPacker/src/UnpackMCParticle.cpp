
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"
#include "Kernel/StandardPacker.h"
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
    debug() << "Size of PackedMCParticles = " << dst->end() - dst->begin() << endmsg;

  LHCb::MCParticles* newMCParticles = new LHCb::MCParticles();
  put( newMCParticles, m_outputName );

  StandardPacker pack;

  newMCParticles->reserve( dst->size() );
  for ( std::vector<LHCb::PackedMCParticle>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS )
  {
    const LHCb::PackedMCParticle& src = (*itS);

    LHCb::MCParticle* part = new LHCb::MCParticle( );
    newMCParticles->insert( part, src.key );

    const double px = pack.energy( src.px );
    const double py = pack.energy( src.py );
    const double pz = pack.energy( src.pz );
    const double mass = src.mass;
    const double E = std::sqrt( (px*px) + (py*py) + (pz*pz) + (mass*mass) );
    part->setMomentum( Gaudi::LorentzVector( px, py, pz , E ) );

    const LHCb::ParticleID PID(src.PID);
    part->setParticleID( PID );

    int hintID(0), key(0);
    pack.hintAndKey( src.originVertex, dst, newMCParticles, hintID, key );
    SmartRef<LHCb::MCVertex> ref( newMCParticles, hintID, key );
    part->setOriginVertex( ref );

    for ( std::vector<int>::const_iterator itI = src.endVertices.begin();
          src.endVertices.end() != itI ; ++itI )
    {
      pack.hintAndKey( *itI, dst, newMCParticles, hintID, key );
      SmartRef<LHCb::MCVertex> refV( newMCParticles, hintID, key );
      part->addToEndVertices( refV );
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
