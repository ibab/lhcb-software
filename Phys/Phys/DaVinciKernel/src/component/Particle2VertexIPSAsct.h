// $Id: Particle2VertexIPSAsct.h,v 1.3 2006-06-16 13:28:04 jonrob Exp $
#ifndef PARTICLE2VERTEXIPSASCT_H
#define PARTICLE2VERTEXIPSASCT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2VertexAsct.h"            // Interface

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

  typedef IParticle2VertexAsct::Table Table;
  /// Standard constructor
  Particle2VertexIPSAsct( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  // Return the relation table linking particles to PVs
  Table table(const LHCb::Particle::Container& particles,
              const LHCb::Vertex::Container& vertices,
              const IGeomDispCalculator* pIPTool) const;

  Table table(const LHCb::Particle::ConstVector& particles,
              const LHCb::Vertex::ConstVector& vertices,
              const IGeomDispCalculator* pIPTool) const;

  Table table(const LHCb::Particle::Container::const_iterator pBegin,
              const LHCb::Particle::Container::const_iterator pEnd,
              const LHCb::Vertex::Container::const_iterator   vBegin,
              const LHCb::Vertex::Container::const_iterator   vEnd,
              const IGeomDispCalculator* pIPTool) const;

  Table table(const LHCb::Particle::ConstVector::const_iterator pBegin,
              const LHCb::Particle::ConstVector::const_iterator pEnd,
              const LHCb::Vertex::ConstVector::const_iterator   vBegin,
              const LHCb::Vertex::ConstVector::const_iterator   vEnd,
              const IGeomDispCalculator* pIPTool) const;

  template <typename FROMITER, typename TOITER>
  Table table(const FROMITER pBegin,
              const FROMITER pEnd,
              const TOITER vBegin,
              const TOITER vEnd,
              const IGeomDispCalculator* pIPTool) const;

  template <typename FROM, typename TO>
  Table table(const FROM& particles,
              const TO& vertices,
              const IGeomDispCalculator* pIPTool) const;


  virtual ~Particle2VertexIPSAsct( ); ///< Destructor

protected:

private:

  double m_max;            ///< maximum value to enter table..
  bool m_useSignificance;

};
#endif // PARTICLE2VERTEXIPSASCT_H
