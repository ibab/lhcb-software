// $Id: MCMatchObjP2MCRelator.h,v 1.5 2009-03-10 18:12:06 jpalac Exp $
#ifndef MCMATCHOBJP2MCRELATOR_H 
#define MCMATCHOBJP2MCRELATOR_H 1

// Include files
#include <functional>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "Kernel/IParticle2MCAssociator.h"            // Interface
//#include "Kernel/Particle2MCAssociatorBase.h"
#include "P2MCP/IP2MCP.h"  
#include "P2MCP/P2MCPBase.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCParticles.h"
// forward declarations
namespace LoKi 
{
  class IReporter;
}
/** @class MCMatchObjP2MCRelator MCMatchObjP2MCRelator.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-04
 */
class MCMatchObjP2MCRelator : public P2MCPBase,
                              virtual public IP2MCP {
public: 
  /// Standard constructor
  MCMatchObjP2MCRelator( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~MCMatchObjP2MCRelator( ); ///< Destructor

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;

  virtual bool isMatched(const LHCb::Particle* particle, 
                         const LHCb::MCParticle* mcParticle) const ;

private:

  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::ConstVector& mcParticles) const;
  
  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::Container* mcParticles) const;

  LoKi::MCMatch matcher() const;

  void addTables(LoKi::MCMatchObj* matcher) const;

  struct MCSortLogic : public std::binary_function<const LHCb::MCParticle*,const LHCb::MCParticle* , bool>
  {
    
    bool operator() ( const LHCb::MCParticle* p1 , 
                      const LHCb::MCParticle* p2 ) const
      {
        LoKi::MCParticles::FromMCDecayTree fromDecay( p1 ) ;
        return fromDecay ( p2 );
      }
  };

  
  void printMCPIDs(const LHCb::MCParticle::ConstVector& mcps) const
  {
    for (LHCb::MCParticle::ConstVector::const_iterator mcp = mcps.begin();
         mcp!=mcps.end(); ++mcp) {
      std::cout << "\tMCP PID " << (*mcp)->particleID().pid() << std::endl;
    }
  }
  

private:

  LoKi::IReporter* m_reporter;
  mutable LoKi::MCMatchObj* m_matcher;
  typedef std::vector<std::string> Addresses ;
  Addresses m_PP2MC;

};


#endif // MCMATCHOBJP2MCPRELATOR_H
