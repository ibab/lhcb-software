// $Id: VertexCloner.h,v 1.3 2009-07-30 10:03:01 jpalac Exp $
#ifndef MICRODST_VERTEXCLONER_H
#define MICRODST_VERTEXCLONER_H 1

#include "ObjectClonerBase.h"

#include <MicroDST/ICloneVertex.h>
#include <MicroDST/ICloneParticle.h>

/** @class VertexCloner VertexCloner.h src/VertexCloner.h
 *
 *
 *  Implementation of the ICloneVertex interface.
 *
 *  Clone an LHCb::Vertex and it's outgoing LHCb::Particles.
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class VertexCloner : public extends1<ObjectClonerBase,ICloneVertex> 
{

public:

  /// Standard constructor
  VertexCloner( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  virtual ~VertexCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::Vertex* operator() (const LHCb::Vertex* vertex);

private:

  LHCb::Vertex* clone(const LHCb::Vertex* vertex);

private:

  typedef MicroDST::BasicItemCloner<LHCb::Vertex> BasicVertexCloner;

private:

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

};

#endif // MICRODST_VERTEXCLONER_H
