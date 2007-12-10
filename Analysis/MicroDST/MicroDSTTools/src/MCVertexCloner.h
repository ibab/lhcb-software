// $Id: MCVertexCloner.h,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
#ifndef MCVERTEXCLONER_H 
#define MCVERTEXCLONER_H 1

// Include files
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneMCVertex.h>            // Interface
#include <MicroDST/ICloneMCParticle.h> 
/** @class MCVertexCloner MCVertexCloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class MCVertexCloner : public MicroDSTTool, 
                       virtual public ICloneMCVertex {
public: 
  /// Standard constructor
  MCVertexCloner( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~MCVertexCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::MCVertex* operator() (const LHCb::MCVertex* mcVertex);

protected:

private:

   LHCb::MCVertex* clone(const LHCb::MCVertex* mcVertex);

private:

  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> BasicMCVtxCloner;
  
  
  ICloneMCParticle* m_particleCloner;

};
#endif // MCVERTEXCLONER_H
