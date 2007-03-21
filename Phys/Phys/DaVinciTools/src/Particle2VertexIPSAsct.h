// $Id: Particle2VertexIPSAsct.h,v 1.1 2007-03-21 13:02:40 pkoppenb Exp $
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

  /// Standard constructor

  Particle2VertexIPSAsct( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual double weight( const LHCb::Particle*      particle,
                         const LHCb::VertexBase*    vertex,
                         const IGeomDispCalculator* pIPTool    ) const;

  virtual ~Particle2VertexIPSAsct( ); ///< Destructor

private:

protected:

private:

  bool m_useSignificance;

};
#endif // PARTICLE2VERTEXIPSASCT_H
