// $Id: VertexCloner.h,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
#ifndef MICRODST_VERTEXCLONER_H 
#define MICRODST_VERTEXCLONER_H 1

// Include files
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneVertex.h>            // Interface
#include <MicroDST/ICloneParticle.h>
#include <MicroDST/Functors.hpp>

/** @class VertexCloner VertexCloner.h src/VertexCloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class VertexCloner : public MicroDSTTool, 
                     virtual public ICloneVertex {
public: 
  /// Standard constructor
  VertexCloner( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~VertexCloner( ); ///< Destructor

  StatusCode initialize();

  StatusCode finalize();

  virtual LHCb::Vertex* operator() (const LHCb::Vertex* vertex);

private:

  LHCb::Vertex* clone(const LHCb::Vertex* vertex);

private:


  typedef MicroDST::BasicItemCloner<LHCb::Vertex> BasicVertexCloner;


  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

};
#endif // MICRODST_VERTEXCLONER_H
