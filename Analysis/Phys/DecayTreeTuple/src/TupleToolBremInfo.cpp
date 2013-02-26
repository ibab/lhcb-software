// $Id: TupleToolBremInfo.cpp
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "TupleToolBremInfo.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"
#include "GaudiKernel/IRegistry.h" //
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloMomentum.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolBremInfo
//
//
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolBremInfo::TupleToolBremInfo( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : TupleToolBase ( type, name , parent ),
    m_parts(),
    m_pids()
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("Particle",m_parts);
  m_parts.push_back("e+");
}

StatusCode TupleToolBremInfo::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_adder = tool<IBremAdder>("BremAdder","BremAdder" ,this );

  if ( !m_parts.empty() )
  {
    LHCb::IParticlePropertySvc* ppsvc =
      svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
    for ( std::vector<std::string>::const_iterator p = m_parts.begin();
          m_parts.end() != p; ++p )
    {
      const LHCb::ParticleProperty* pp = ppsvc->find( *p );
      if (pp) { m_pids.push_back( pp->pdgID().abspid() ); }
    }
  }

  return sc;
}

//=============================================================================
StatusCode TupleToolBremInfo::fill(const Particle*,
                                   const Particle* P,
                                   const std::string& head,
                                   Tuples::Tuple& tuple )
{
  if( !P ) return StatusCode::SUCCESS;

  const std::string prefix = fullName(head);

  bool ok=false;
  for ( std::vector<unsigned int>::const_iterator pid = m_pids.begin();
        m_pids.end() != pid; ++pid )
  {
    if (  P->particleID().abspid() == *pid ) { ok=true; break; }
  }
  if ( !ok ) return StatusCode::SUCCESS;

  LHCb::CaloMomentum brem =
    m_adder->bremMomentum( P, Gaudi::Utils::toString(P->particleID().pid()) );

  bool filltuple = true;
  filltuple &= tuple->column( prefix+"_BremMultiplicity", brem.multiplicity() );
  filltuple &= tuple->column( prefix+"_BremP"           , brem.momentum());
  filltuple &= tuple->column( prefix+"_BremOrigin"      , brem.referencePoint());
  return StatusCode(filltuple);
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolBremInfo )
