// $Id: MCParticleCloner.h,v 1.4 2008-04-23 13:01:55 jpalac Exp $
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
 *  using an ICloneMCVertex, and the origin vertex's LHCb::Particle products.
 *  The end vertices receive a special treatment. By default, only SmartRefs
 *  are stored, so an XML catalogue to the original DST is required for access.
 *  It is possible to clone and store end vertices that are also decay 
 *  vertices using the <b>CloneDecayVertices</b> property.
 *  All LHCb::Vertex cloning is performed by the ICloneVertex implementation,
 *  which is set via the ICloneMCVertex property.
 *
 * <b>Extra options</b>
 * CloneDecayVertices: bool. Clone any of the LHCb::MCParticle's endVertices
 * if they are decay vertices, that is, if their LHCb::MCVertex::isDecay() 
 * method returns true. Default: false.
 * ICloneMCVertex: string.
 *
 * @see ICloneMCVertex
 * @see MCVertexCloner
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

  bool m_cloneDecayVertices;

};
#endif // MCPARTICLECLONER_H
