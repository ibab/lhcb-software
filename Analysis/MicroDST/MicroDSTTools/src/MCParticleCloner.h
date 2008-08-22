// $Id: MCParticleCloner.h,v 1.6 2008-08-22 12:24:33 jpalac Exp $
#ifndef MCPARTICLECLONER_H 
#define MCPARTICLECLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneMCParticle.h>            // Interface
#include <MicroDST/ICloneMCVertex.h>
/** @class MCParticleCloner MCParticleCloner.h
 *  
 *  MicroDSTTool to clone an LHCb::MCParticle and place it in a TES location
 *  parallel to that of the parent. It clones and stores the origin vertex
 *  using simple copy, and copies the origin vertex's LHCb::MCParticle products.
 *  End vertices receive a special treatment. By default, only SmartRefs
 *  are stored, so an XML catalogue to the original DST is required for access.
 *  It is possible to clone and store end vertices that are also decay 
 *  vertices setting the <b>ICloneMCVertex</b> property to an existing 
 *  implementation of ICloneMCVertex.
 *
 *  <b>Options</b>
 *  ICloneMCVertex: string. Implementation of the ICloneMCVertex used to clone
 *  decay end vertices, that is, end vertices with 
 *  LHCb::MCVertex::isDecay() = true. 
 *  Default: "NONE". No cloning of decay vertices performed. Only SmartRefs
 *  are copied.
 *
 *  @see ICloneMCVertex
 *  @see MCVertexCloner
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class MCParticleCloner : public MicroDSTTool, 
                         virtual public ICloneMCParticle {
public: 
  /// Standard constructor
  MCParticleCloner( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~MCParticleCloner( ); ///< Destructor

  StatusCode initialize();

  StatusCode finalize();

  virtual LHCb::MCParticle* operator() (const LHCb::MCParticle* mcp);
  
private:

  LHCb::MCParticle* clone(const LHCb::MCParticle* mcp);

  inline bool cloneOriginVertex(const LHCb::MCVertex* vertex) 
  {
    return vertex && (vertex->isDecay() || vertex->isPrimary() );
  }
  
  void cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices,
                          LHCb::MCParticle* clonedParticle);
  
private:

  typedef MicroDST::BasicItemCloner<LHCb::MCParticle> BasicMCPCloner;

  ICloneMCVertex* m_vertexCloner;

  std::string m_vertexClonerName;

};
#endif // MCPARTICLECLONER_H
