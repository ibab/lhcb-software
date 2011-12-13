// $Id: MCVertexCloner.h,v 1.4 2009-07-30 10:03:01 jpalac Exp $
#ifndef MCVERTEXCLONER_H
#define MCVERTEXCLONER_H 1

// Include files
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneMCVertex.h>            // Interface
#include <MicroDST/ICloneMCParticle.h>
/** @class MCVertexCloner MCVertexCloner.h
 *
 *  MicroDSTTool to clone an LHCb::MCVertex and place it in a TES location
 *  parallel to that of the parent. It clones and stores the decay products
 *  of the MCVertex using an using an ICloneMCParticle.
 *  All LHCb::MCParticle cloning is performed by the ICloneMCParticle
 *  implementation, which is set via the ICloneMCParticle property.
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class MCVertexCloner : public extends1<MicroDSTTool, ICloneMCVertex>
{

public:

  /// Standard constructor
  MCVertexCloner( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~MCVertexCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::MCVertex* operator() (const LHCb::MCVertex* mcVertex);

public:

  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> BasicCloner;

private:

  LHCb::MCVertex* clone(const LHCb::MCVertex* mcVertex);

  void cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products,
                          LHCb::MCVertex* clonedVertex);

private:

  ICloneMCParticle* m_particleCloner;
  std::string m_particleClonerName;

};

#endif // MCVERTEXCLONER_H
