// $Id: Particle2MCLinker.h,v 1.4 2006-10-11 15:42:58 jpalac Exp $
#ifndef DAVINCIASSOCIATORS_PARTICLE2MCLINKER_H 
#define DAVINCIASSOCIATORS_PARTICLE2MCLINKER_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/KeyedObject.h"

#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Linker/LinkerWithKey.h"

// DaVinciKernel
#include "Kernel/Particle2MCMethod.h"

/** @class Particle2MCLinker Particle2MCLinker.h 
    DaVinciAssociators/Particle2MCLinker.h
 *  
 *
 *  @author Philippe Charpentier 
 *  @date   2004-04-29
 */

namespace LHCb 
{
  class Particle;
  class MCParticle;
  class ProtoParticle;
}


class Object2MCLinker 
{
 public:
  // Constructors from Algorithm
  Object2MCLinker( const Algorithm* myMother,
                   const int method, 
                   const std::vector<std::string>& containerList)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo( m_evtSvc,NULL,"")
    , m_linkerTable( m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const Algorithm* myMother,
                   const int method, 
                   const std::string& container)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const Algorithm* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::vector<std::string>& 
                   containerList)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const Algorithm* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::string& container)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const Algorithm* myMother )
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>()) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};

  // Now constructors from tools
  Object2MCLinker( const GaudiTool* myMother,
                   const int method, 
                   const std::vector<std::string>& containerList)
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo( m_evtSvc,NULL,"")
    , m_linkerTable( m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const GaudiTool* myMother,
                   const int method, 
                   const std::string& container)
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const GaudiTool* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::vector<std::string>& 
                   containerList)
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const GaudiTool* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::string& container)
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  Object2MCLinker( const GaudiTool* myMother )
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>()) 
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {};
  
  virtual ~Object2MCLinker() {};  ///< Desctructor

  StatusCode setAlgorithm( const int method, 
                           const std::vector<std::string>& containerList);
  StatusCode setAlgorithm( const int method, 
                           const std::string& container) 
  {
    return setAlgorithm( method, std::vector<std::string>(1,container));
  };
  StatusCode setAlgorithm( const std::string& algType,
                           const std::string& extension,
                           const std::vector<std::string>& containerList);
  StatusCode setAlgorithm( const std::string& algType,
                           const std::string& extension,
                           const std::string& container) 
  {
    return setAlgorithm( algType, extension, 
                         std::vector<std::string>(1,container));
  };
    
  

  typedef LinkedTo<LHCb::MCParticle>          To;
  typedef LinkerWithKey<LHCb::MCParticle>     Linker;
  Linker* linkerTable( const std::string& name) ;
  Linker* linkerTable( const std::string& name, To& test) ;

  const LHCb::MCParticle* firstMCP( const KeyedObject<int>* obj) ;
  const LHCb::MCParticle* firstMCP( const KeyedObject<int>* obj, double& weight) ;
  const LHCb::MCParticle* nextMCP() { 
    return m_linkTo.next(); };
  const LHCb::MCParticle* nextMCP( double& weight) { 
    const LHCb::MCParticle* mcPart =  m_linkTo.next();
    weight = NULL != mcPart ? m_linkTo.weight() : 0.;
    return mcPart;
  };
  double      weightMCP() { 
    return m_linkTo.weight(); };
  int         associatedMCP( const KeyedObject<int>* obj) ;
  const LHCb::MCParticle* first ( const KeyedObject<int>* obj ) {
    return firstMCP( obj ); };
  const LHCb::MCParticle* first ( const KeyedObject<int>* obj, double & weight ) {
    return firstMCP( obj, weight ); };
  const LHCb::MCParticle* next() {
    return m_linkTo.next(); } ;
  const LHCb::MCParticle* next( double& weight) {
    return nextMCP(weight); } ;
  double weight() {
    return m_linkTo.weight(); } ;
  bool notFound();
  bool notFound( const std::string& contname);
  bool checkAssociation( const KeyedObject<int>* obj, 
                         const LHCb::MCParticle* mcPart);

 protected:
  
  const GaudiAlgorithm*        m_myGaudiAlg;
  const GaudiTool*             m_myGaudiTool;
  IDataProviderSvc*            m_evtSvc;
  IMessageSvc*                 m_msgSvc;
  ISvcLocator*                 m_svcLoc;
  std::string                  m_name;
  std::string                  m_extension;
  std::string                  m_linkerAlgType;
  IAlgorithm*                  m_linkerAlg;
  std::vector<std::string>     m_containerList;

  To                                m_linkTo;
  Linker                            m_linkerTable;
  
  // Private methods
  void 
  createLinks( const std::string& contName = "") ;

  To* 
  getLink( const std::string& contName ) ;

  StatusCode 
  locateAlgorithm( const std::string& algType,
                   const std::string& algName,
                   IAlgorithm*& alg,
                   const std::vector<std::string>& inputData);
  StatusCode
  setAlgInputData( IAlgorithm*& alg, 
                   const std::vector<std::string>& inputData);
  
  inline std::string 
  containerName( const ContainedObject* obj) const
  {
    if( NULL == obj->parent() || 
        NULL == obj->parent()->registry()) return "";
    return obj->parent()->registry()->identifier();
  }
};

template <class OBJ2MCP=LHCb::Particle>
class Object2FromMC : public Object2MCLinker {

public: 
  /// Standard constructors
  Object2FromMC(const Algorithm* myMother)
    : Object2MCLinker( myMother )
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const Algorithm* myMother,
                 const int method, 
                 const std::vector<std::string>& containerList)
    : Object2MCLinker( myMother, method, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const Algorithm* myMother,
                 const int method, 
                 const std::string& container ) 
    : Object2MCLinker( myMother, method, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  Object2FromMC( const Algorithm* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::vector<std::string>& 
                 containerList)
    : Object2MCLinker( myMother, algtype, extension, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  Object2FromMC( const Algorithm* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::string& container)
    : Object2MCLinker( myMother, algtype, extension, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  Object2FromMC(const GaudiTool* myMother)
    : Object2MCLinker( myMother )
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const GaudiTool* myMother,
                 const int method, 
                 const std::vector<std::string>& containerList)
    : Object2MCLinker( myMother, method, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const GaudiTool* myMother,
                 const int method, 
                 const std::string& container ) 
    : Object2MCLinker( myMother, method, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  Object2FromMC( const GaudiTool* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::vector<std::string>& 
                 containerList)
    : Object2MCLinker( myMother, algtype, extension, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  Object2FromMC( const GaudiTool* myMother,
                   const std::string& algType, 
                   const std::string& extension,
                   const std::string& container)
    : Object2MCLinker( myMother, algtype, extension, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};

  virtual ~Object2FromMC() {}; ///< Destructor

  typedef LinkedFrom<OBJ2MCP>  From;
  typedef typename std::vector<From>::iterator FromIterator;
  
  bool        isAssociated( const KeyedObject<int>* obj) 
  {
    const OBJ2MCP* part = dynamic_cast<const OBJ2MCP*>(obj);
    if( NULL != part ) return NULL != firstMCP(part);
    const LHCb::MCParticle* mcPart = dynamic_cast<const LHCb::MCParticle*>(obj);
    if( NULL != mcPart ) return NULL != firstP(mcPart);
    return false;
  };

  OBJ2MCP*     firstP( const KeyedObject<int>* mcPart, double& weight)
  {
    OBJ2MCP* part = firstP(mcPart);
    weight = weightP();
    return part;
  };

  OBJ2MCP*     firstP( const KeyedObject<int>* mcPart)
  {
    if( (NULL == m_myGaudiAlg && NULL == m_myGaudiTool) || "" == m_linkerAlgType ||
        0 == m_containerList.size() ) return NULL;

    createFromLinks();
    for( FromIterator fr = m_linkFromList.begin();
         m_linkFromList.end() != fr; fr++) {
      OBJ2MCP* part = fr->first( mcPart );
      if( NULL != part ) {
        m_linkFrom = fr;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    return NULL;
  };

  OBJ2MCP*     nextP(double& weight)
  {
    OBJ2MCP* part = nextP();
    weight = weightP();
    return part;
  };
  OBJ2MCP*     nextP()
  {
    for( FromIterator fr = m_linkFrom;
         m_linkFromList.end() != fr; fr++) {
      OBJ2MCP* part = fr->next();
      if( NULL != part ) {
        m_linkFrom = fr;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    return NULL;
  };
  
  double      weightP()
  { 
    return m_linkFromList.end() != m_linkFrom ? m_linkFrom->weight() : 0.;
  };

  int         associatedP( const KeyedObject<int>* obj)
  {
    int n = 0;
    for( OBJ2MCP* part = firstP(obj); 
         NULL != part; 
         part = nextP(), n++);
    return n;
  };

protected:

private:
  std::vector<From>        m_linkFromList;
  FromIterator             m_linkFrom;

  void        createFromLinks()
  {
    m_linkFromList.clear();
    for( std::vector<std::string>::const_iterator contIt = 
           m_containerList.begin();
         m_containerList.end() != contIt; contIt++) {
      const std::string name = 
        *contIt + m_extension;
      From test(  m_evtSvc, NULL, name);
      if( test.notFound() ) {
        createLinks( *contIt );
        test = From(  m_evtSvc, NULL, name);
      }
      m_linkFromList.push_back( test );
    }
    m_linkFrom = m_linkFromList.end();
  };

};

typedef Object2FromMC<LHCb::ProtoParticle> ProtoParticle2MCLinker;
typedef Object2FromMC<>                    Particle2MCLinker;

#endif // DAVINCIASSOCIATORS_PARTICLE2MCLINKER_H
