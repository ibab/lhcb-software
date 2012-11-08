// $Id: MCMatchObjP2MCRelator.h,v 1.16 2009-10-27 14:08:49 ibelyaev Exp $
#ifndef MCMATCHOBJP2MCRELATOR_H
#define MCMATCHOBJP2MCRELATOR_H 1

// Include files
#include <functional>
#include <map>
#include <string>

// from Gaudi
#include <GaudiKernel/IIncidentListener.h>
#include <GaudiKernel/IIncidentSvc.h>
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
class MCMatchObjP2MCRelator : public extends1<P2MCPBase, IIncidentListener>
{

public:

  /// Standard constructor
  MCMatchObjP2MCRelator( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  virtual ~MCMatchObjP2MCRelator( ); ///< Destructor

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;

public:

  virtual bool isMatched( const LHCb::Particle* particle,
                          const LHCb::MCParticle* mcParticle ) const ;

public :

  virtual void handle( const Incident& );

private:

  inline LoKi::MCMatch matcher() const
  {
    return LoKi::MCMatch( m_matcher );
  }

  void addTables() const;

  void addTable(const LHCb::Particle* particle) const;

  void loadTable(const std::string & address) const;

  void printMCPIDs(const LHCb::MCParticle::ConstVector& mcps) const
  {
    always() << "==================================" << endmsg;
    for (LHCb::MCParticle::ConstVector::const_iterator mcp = mcps.begin();
         mcp!=mcps.end(); ++mcp)
    {
      always() << "\tMCP PID " << (*mcp)->particleID().pid() << endmsg;
    }
  }

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj Data object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const DataObject * pObj ) const
  {
    return ( !pObj ? "Null DataObject !" :
             (pObj->registry() ? pObj->registry()->identifier() : "UnRegistered") );
  }

private:

  LoKi::IReporter* m_reporter;
  LoKi::MCMatchObj* m_matcher;

  typedef std::vector<std::string> Addresses ;
  Addresses m_tables;

  mutable bool m_loaded;
  mutable std::map<std::string,bool> m_pLocDoneMap;

};

#endif // MCMATCHOBJP2MCPRELATOR_H
