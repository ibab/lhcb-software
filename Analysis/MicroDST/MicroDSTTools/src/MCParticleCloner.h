// $Id: MCParticleCloner.h,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
#ifndef MCPARTICLECLONER_H 
#define MCPARTICLECLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneMCParticle.h>            // Interface
#include <MicroDST/ICloneMCVertex.h>
/** @class MCParticleCloner MCParticleCloner.h
 *  
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

  virtual LHCb::MCParticle* operator() (const LHCb::MCParticle* mcp);
  

protected:

private:

  LHCb::MCParticle* clone(const LHCb::MCParticle* mcp);

  LHCb::MCVertex* cloneVertexTree(const LHCb::MCVertex* mcVertex);
  
private:

  typedef MicroDST::BasicItemCloner<LHCb::MCParticle> BasicMCPCloner;

  ICloneMCVertex* m_vertexCloner;

};
#endif // MCPARTICLECLONER_H
