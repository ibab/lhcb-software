// $Id: Particle2VertexIPSAsct.cpp,v 1.6 2006-08-22 14:13:24 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from DaVinci
#include "Kernel/IGeomDispCalculator.h"

// local
#include "Particle2VertexIPSAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2VertexIPSAsct
//
// 2006-06-06 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Particle2VertexIPSAsct );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2VertexIPSAsct::Particle2VertexIPSAsct( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticle2VertexAsct>(this);
  declareProperty( "UseSignificance",   m_useSignificance=true );
}
//=============================================================================
double Particle2VertexIPSAsct::weight(const LHCb::Particle*      particle,
                                      const LHCb::VertexBase*    vertex,
                                      const IGeomDispCalculator* pIPTool  ) const 
{
  double    ip(0.);
  double ipErr(0.);
  StatusCode sc = pIPTool->calcImpactPar(*particle,*vertex, ip, ipErr);
  return (sc.isSuccess() ) ? std::fabs(m_useSignificance?ip/ipErr:ip) : 0.;
}

//=============================================================================
// Destructor
//=============================================================================
Particle2VertexIPSAsct::~Particle2VertexIPSAsct() {}

//=============================================================================
