// $Id: MCMatchObjP2MCRelator.h,v 1.11 2009-06-30 08:46:46 jpalac Exp $
#ifndef MCMATCHOBJP2MCRELATOR_H 
#define MCMATCHOBJP2MCRELATOR_H 1

// Include files
#include <functional>
// from Gaudi
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
class MCMatchObjP2MCRelator : public P2MCPBase

{
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

  LoKi::MCMatch matcher() const;

  void addTables(LoKi::MCMatchObj* matcher) const;
  
  void printMCPIDs(const LHCb::MCParticle::ConstVector& mcps) const
  {
    always() << "==================================" << endmsg;
    for (LHCb::MCParticle::ConstVector::const_iterator mcp = mcps.begin();
         mcp!=mcps.end(); ++mcp) {
      always() << "\tMCP PID " << (*mcp)->particleID().pid() << endmsg;
    }
  }
  

private:

  LoKi::IReporter* m_reporter;
  mutable LoKi::MCMatchObj* m_matcher;
  typedef std::vector<std::string> Addresses ;
  Addresses m_tables;

};


#endif // MCMATCHOBJP2MCPRELATOR_H
