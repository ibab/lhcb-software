// $Id: Particle2VertexIPSAsct.h,v 1.5 2006-06-20 12:08:23 jpalac Exp $
#ifndef PARTICLE2VERTEXIPSASCT_H
#define PARTICLE2VERTEXIPSASCT_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
// from DaVinci
#include "Kernel/IParticle2VertexAsct.h"
/** @class Particle2VertexIPSAsct Particle2VertexIPSAsct.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2006-06-06
 */

class Particle2VertexIPSAsct : public GaudiTool,
                               virtual public IParticle2VertexAsct

{

public:

  typedef IParticle2VertexAsct::TablePV TablePV;
  typedef IParticle2VertexAsct::TableV TableV;

  /// Standard constructor


  Particle2VertexIPSAsct( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);



  TableV table(const LHCb::Particle::Container::const_iterator pBegin,
               const LHCb::Particle::Container::const_iterator pEnd,
               const LHCb::Vertex::Container::const_iterator   vBegin,
               const LHCb::Vertex::Container::const_iterator   vEnd,
               const IGeomDispCalculator* pIPTool) const
  {
    return this->table<LHCb::Particle::Container::const_iterator,
                       LHCb::Vertex::Container::const_iterator,
                       TableV>(pBegin, 
                               pEnd, 
                               vBegin, 
                               vEnd, 
                               pIPTool);           
  }


  TableV table(const LHCb::Particle::ConstVector::const_iterator pBegin,
               const LHCb::Particle::ConstVector::const_iterator pEnd,
               const LHCb::Vertex::ConstVector::const_iterator   vBegin,
               const LHCb::Vertex::ConstVector::const_iterator   vEnd,
               const IGeomDispCalculator* pIPTool) const
  {
    return this->table<LHCb::Particle::ConstVector::const_iterator,
                       LHCb::Vertex::ConstVector::const_iterator,
                       TableV>(pBegin, 
                               pEnd, 
                               vBegin, 
                               vEnd, 
                               pIPTool);  
  }

  TablePV table(const LHCb::Particle::Container::const_iterator pBegin,
                const LHCb::Particle::Container::const_iterator pEnd,
                const LHCb::PrimVertex::Container::const_iterator   vBegin,
                const LHCb::PrimVertex::Container::const_iterator   vEnd,
                const IGeomDispCalculator* pIPTool) const
  {
    return this->table<LHCb::Particle::Container::const_iterator,
                       LHCb::PrimVertex::Container::const_iterator,
                       TablePV>(pBegin, 
                                pEnd, 
                                vBegin, 
                                vEnd, 
                                pIPTool);           
  }


  TablePV table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                const LHCb::Particle::ConstVector::const_iterator pEnd,
                const LHCb::PrimVertex::ConstVector::const_iterator   vBegin,
                const LHCb::PrimVertex::ConstVector::const_iterator   vEnd,
                const IGeomDispCalculator* pIPTool) const
  {
    return this->table<LHCb::Particle::ConstVector::const_iterator,
                       LHCb::PrimVertex::ConstVector::const_iterator,
                       TablePV>(pBegin, 
                                pEnd, 
                                vBegin, 
                                vEnd, 
                                pIPTool);  
  }


  template <typename FROMITER, typename TOITER, typename TABLE>
  TABLE table(const FROMITER pBegin,
              const FROMITER pEnd,
              const TOITER vBegin,
              const TOITER vEnd,
              const IGeomDispCalculator* pIPTool) const;
  
  

  virtual ~Particle2VertexIPSAsct( ); ///< Destructor

private:

protected:

private:

  double m_max;            ///< maximum value to enter table..
  bool m_useSignificance;

};
#endif // PARTICLE2VERTEXIPSASCT_H
