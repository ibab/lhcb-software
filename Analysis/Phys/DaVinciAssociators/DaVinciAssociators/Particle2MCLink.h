// $Id: Particle2MCLink.h,v 1.1 2004-06-11 15:26:13 phicharp Exp $
#ifndef DAVINCIASSOCIATORS_PARTICLE2MCLINK_H 
#define DAVINCIASSOCIATORS_PARTICLE2MCLINK_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IRegistry.h"

#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Linker/LinkerWithKey.h"

/** @class Particle2MCLink Particle2MCLink.h 
    DaVinciAssociators/Particle2MCLink.h
 *  
 *
 *  @author Philippe Charpentier 
 *  @date   2004-04-29
 */

class MCParticle;
class Particle;
class ProtoParticle;


namespace Particle2MCMethod {
  enum {
    No = 0, WithChi2, Chi2, Links, Composite, ChargedPP, NeutralPP, Max 
  } ;

  static const std::string extension[Max] = {
    "",
    "/WithChi2",
    "/Chi2",
    "/Links",
    "/Composite",
    "/PP2MC",
    "/PP2MC"
  };
static const std::string algType[Max] = {
    "",
    "Particle2MCWithChi2",
    "Particle2MCChi2",
    "Particle2MCLinks",
    "CompositeParticle2MCLinks",
    "ChargedPP2MC",
    "NeutralPP2MC"
  };
}

class Object2MCLink 
{
 public:
  Object2MCLink( const Algorithm* myAlg,
              const int method, 
              const std::vector<std::string>& containerList)
    : m_myAlg(myAlg)
    , m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myAlg))
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo(myAlg->eventSvc(),NULL,"")
    , m_linkerTable(myAlg->eventSvc(),NULL,"") {};
  
  Object2MCLink( const Algorithm* myAlg,
              const int method, 
              const std::string& container)
    : m_myAlg(myAlg)
    , m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myAlg))
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(myAlg->eventSvc(),NULL,"")
    , m_linkerTable(myAlg->eventSvc(),NULL,"") {};
  
  Object2MCLink( const Algorithm* myAlg,
                 const std::string& algType, 
                 const std::string& extension,
                 const std::vector<std::string>& 
                 containerList)
    : m_myAlg(myAlg)
    , m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myAlg))
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(containerList) 
    , m_linkTo(myAlg->eventSvc(),NULL,"")
    , m_linkerTable(myAlg->eventSvc(),NULL,"") {};
  
  Object2MCLink( const Algorithm* myAlg,
                 const std::string& algType, 
                 const std::string& extension,
                 const std::string& container)
    : m_myAlg(myAlg)
    , m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myAlg))
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container)) 
    , m_linkTo(myAlg->eventSvc(),NULL,"")
    , m_linkerTable(myAlg->eventSvc(),NULL,"") {};
  
  Object2MCLink( const Algorithm* myAlg )
    : m_myAlg(myAlg)
    , m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myAlg))
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>()) 
    , m_linkTo(myAlg->eventSvc(),NULL,"")
    , m_linkerTable(myAlg->eventSvc(),NULL,"") {};
  
  virtual ~Object2MCLink() {};  ///< Desctructor

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
    
  

  typedef LinkedTo<MCParticle>          To;
  typedef LinkerWithKey<MCParticle>     Linker;
  Linker* linkerTable( const std::string& name) ;
  Linker* linkerTable( const std::string& name, To& test) ;

  MCParticle* firstMCP( const KeyedObject<int>* obj) ;
  MCParticle* firstMCP( const KeyedObject<int>* obj, double& weight) ;
  MCParticle* nextMCP() { 
    return m_linkTo.next(); };
  MCParticle* nextMCP( double& weight) { 
    MCParticle* mcPart =  m_linkTo.next();
    weight = NULL != mcPart ? m_linkTo.weight() : 0.;
    return mcPart;
  };
  double      weightMCP() { 
    return m_linkTo.weight(); };
  int         associatedMCP( const KeyedObject<int>* obj) ;
  MCParticle* first ( const KeyedObject<int>* obj ) {
    return firstMCP( obj ); };
  MCParticle* first ( const KeyedObject<int>* obj, double & weight ) {
    return firstMCP( obj, weight ); };
  MCParticle* next() {
    return m_linkTo.next(); } ;
  MCParticle* next( double& weight) {
    return nextMCP(weight); } ;
  double weight() {
    return m_linkTo.weight(); } ;
  bool notFound();
  bool notFound( const std::string& contname);

 protected:
  void createLinks( const std::string& contName = "") ;
  
  const Algorithm*             m_myAlg;
  const GaudiAlgorithm*        m_myGaudiAlg;
  std::string                  m_extension;
  std::string                  m_linkerAlgType;
  IAlgorithm*                  m_linkerAlg;
  std::vector<std::string>     m_containerList;

  To                                m_linkTo;
  Linker                            m_linkerTable;
  
  // Private methods
  To*
  getLink( const std::string& contName ) ;
  StatusCode 
  locateAlgorithm( const std::string& algType,
                   const std::string& algType,
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

template <class OBJ2MCP=Particle>
class Object2FromMC : public Object2MCLink {

public: 
  /// Standard constructors
  Object2FromMC(const Algorithm* myAlg)
    : Object2MCLink( myAlg )
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const Algorithm* myAlg,
                 const int method, 
                 const std::vector<std::string>& containerList)
    : Object2MCLink( myAlg, method, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  
  Object2FromMC( const Algorithm* myAlg,
                 const int method, 
                 const std::string& container ) 
    : Object2MCLink( myAlg, method, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {};
  

  virtual ~Object2FromMC() {}; ///< Destructor

  typedef LinkedFrom<OBJ2MCP>  From;
  typedef typename std::vector<From>::iterator FromIterator;
  
  bool        isAssociated( const KeyedObject<int>* obj) 
  {
    const OBJ2MCP* part = dynamic_cast<const OBJ2MCP*>(obj);
    if( NULL != part ) return NULL != firstMCP(part);
    const MCParticle* mcPart = dynamic_cast<const MCParticle*>(obj);
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
    if( NULL == m_myAlg || "" == m_linkerAlgType ||
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
      From test(  m_myAlg->eventSvc(), NULL, name);
      if( test.notFound() ) {
        createLinks( name );
        test = From(  m_myAlg->eventSvc(), NULL, name);
      }
      m_linkFromList.push_back( test );
    }
    m_linkFrom = m_linkFromList.end();
  };

};

typedef Object2FromMC<ProtoParticle> ProtoParticle2MCLink;
typedef Object2FromMC<>              Particle2MCLink;

#endif // DAVINCIASSOCIATORS_PARTICLE2MCLINK_H
