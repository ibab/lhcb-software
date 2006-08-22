// $Id: IParticle2VertexAsct.h,v 1.8 2006-08-22 14:13:24 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
#define DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/Particle.h"
#include "Event/VertexBase.h"

class IGeomDispCalculator;

static const InterfaceID IID_IParticle2VertexAsct ( "IParticle2VertexAsct", 1, 0 );

/** @class IParticle2VertexAsct IParticle2VertexAsct.h Kernel/IParticle2VertexAsct.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2006-06-06
 */

class IParticle2VertexAsct : virtual public IAlgTool
{

public:


  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2VertexAsct; }

  virtual double weight( const LHCb::Particle*      particle,
                         const LHCb::VertexBase*    vertex,
                         const IGeomDispCalculator* pIPTool    ) const = 0;

};
#endif // DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
