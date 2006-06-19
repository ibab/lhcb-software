// $Id: Particle2VertexIPSAsct.cpp,v 1.4 2006-06-19 14:49:44 jpalac Exp $
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
  declareProperty( "MaxToBeAssociated", m_max=-1 );
}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::Container& particles,
                              const LHCb::PrimVertex::Container& vertices,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::Container, LHCb::PrimVertex::Container>( particles,
                                                                        vertices,
                                                                        pIPTool  );

}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::ConstVector& particles,
                              const LHCb::PrimVertex::ConstVector& vertices,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::ConstVector,
    LHCb::PrimVertex::ConstVector>( particles,
                                    vertices,
                                    pIPTool  );

}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::Container::const_iterator pBegin,
                              const LHCb::Particle::Container::const_iterator pEnd,
                              const LHCb::PrimVertex::Container::const_iterator  vBegin,
                              const LHCb::PrimVertex::Container::const_iterator  vEnd,
                              const IGeomDispCalculator* pIPTool) const
{

  return table<LHCb::Particle::Container::const_iterator,
    LHCb::PrimVertex::Container::const_iterator>(pBegin,
                                                 pEnd,
                                                 vBegin,
                                                 vEnd,
                                                 pIPTool);
}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                              const LHCb::Particle::ConstVector::const_iterator pEnd,
                              const LHCb::PrimVertex::ConstVector::const_iterator   vBegin,
                              const LHCb::PrimVertex::ConstVector::const_iterator   vEnd,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::ConstVector::const_iterator,
    LHCb::PrimVertex::ConstVector::const_iterator>(pBegin,
                                                   pEnd,
                                                   vBegin,
                                                   vEnd,
                                                   pIPTool);
}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::Container& particles,
                              const LHCb::Vertex::Container& vertices,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::Container, LHCb::Vertex::Container>( particles,
                                                                    vertices,
                                                                    pIPTool  );

}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::ConstVector& particles,
                              const LHCb::Vertex::ConstVector& vertices,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::ConstVector,
    LHCb::Vertex::ConstVector>( particles,
                                vertices,
                                pIPTool  );

}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::Container::const_iterator pBegin,
                              const LHCb::Particle::Container::const_iterator pEnd,
                              const LHCb::Vertex::Container::const_iterator  vBegin,
                              const LHCb::Vertex::Container::const_iterator  vEnd,
                              const IGeomDispCalculator* pIPTool) const
{

  return table<LHCb::Particle::Container::const_iterator,
    LHCb::Vertex::Container::const_iterator>(pBegin,
                                             pEnd,
                                             vBegin,
                                             vEnd,
                                             pIPTool);
}
//=============================================================================
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                              const LHCb::Particle::ConstVector::const_iterator pEnd,
                              const LHCb::Vertex::ConstVector::const_iterator   vBegin,
                              const LHCb::Vertex::ConstVector::const_iterator   vEnd,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<LHCb::Particle::ConstVector::const_iterator,
    LHCb::Vertex::ConstVector::const_iterator>(pBegin,
                                               pEnd,
                                               vBegin,
                                               vEnd,
                                               pIPTool);
}

//=============================================================================
template <typename FROM, typename TO>
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const FROM& particles,
                              const TO& vertices,
                              const IGeomDispCalculator* pIPTool) const
{
  return table<typename FROM::const_iterator,
    typename TO::const_iterator>( particles.begin(),
                                  particles.end(),
                                  vertices.begin(),
                                  vertices.end(),
                                  pIPTool );

}
//=============================================================================
template <typename FROMITER, typename TOITER>
Particle2VertexIPSAsct::Table
Particle2VertexIPSAsct::table(const FROMITER pBegin,
                              const FROMITER pEnd,
                              const TOITER   vBegin,
                              const TOITER   vEnd,
                              const IGeomDispCalculator* pIPTool) const
{

  Table table;

  for (FROMITER ip = pBegin; ip!=pEnd; ++ip) {
    for (TOITER iv = vBegin; iv!=vEnd; ++iv){

      if( (*iv)->isPrimary() ) continue;
      double x,sx;
      StatusCode sc = pIPTool->calcImpactPar(**ip,**iv, x, sx);
      if (!sc.isSuccess()) continue;
      double w = std::fabs(m_useSignificance?x/sx:x);
      std::cout << **iv << std::endl;
      debug() << " p = @" << **ip
              << " v = @ " << **iv
              << " w = " << w
              << endreq;
      if (m_max<=0 || w<m_max ) table.relate(*ip,*iv,w);
    }
  }
  return table;
}
//=============================================================================
// Destructor
//=============================================================================
Particle2VertexIPSAsct::~Particle2VertexIPSAsct() {}

//=============================================================================
