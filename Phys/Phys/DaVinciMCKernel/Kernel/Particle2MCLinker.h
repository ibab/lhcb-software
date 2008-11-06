// $Id: Particle2MCLinker.h,v 1.6 2008-11-06 17:15:50 cattanem Exp $
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

namespace LHCb
{
  class Particle;
  class MCParticle;
  class ProtoParticle;
}

/** @class Object2MCLinker Particle2MCLinker.h Kernel/Particle2MCLinker.h
    *
    *  Class providing association functionality to MCParticles
    * 
    *  @author Philippe Charpentier
    *  @date   2004-04-29
    */

/* 	The class MCAssociation holds a single MCParticle and an association weight. it is
	used when returning the array of all MCParticles associated to a single
	Particle/ProtoParticle. This is implemented in the rangeFrom method, where the interface
	is identical as it was in DC04.

	Added by V. Gligorov, 23-11-07. 
	
*/

class MCAssociation {
public:

  MCAssociation() 
    :
    m_associatedMCP(0),
    m_weight(0.)
  {}

  MCAssociation(const LHCb::MCParticle* mcp, 
                const double weight)
    :
    m_associatedMCP(mcp),
    m_weight(weight)
  {
  }

  virtual ~MCAssociation() {}
  
  const LHCb::MCParticle* to() const { return m_associatedMCP; }

  double weight() const { return m_weight; }

  StatusCode setTo(const LHCb::MCParticle* associatedMCP) {
    m_associatedMCP = associatedMCP;
    return StatusCode::SUCCESS;
  }

  StatusCode setWeight(double weight)  {
    m_weight = weight;
    return StatusCode::SUCCESS;
  }

private:
  const LHCb::MCParticle* m_associatedMCP;
  double m_weight;
};

template <class SOURCE=LHCb::Particle>
class Object2MCLinker
{
public:
  // Typedef for source type
  typedef SOURCE Source;
  // Typedef for RangeFrom return type
  typedef std::vector<MCAssociation> ToRange;
  typedef ToRange::iterator ToIterator;
  typedef ToRange::const_iterator ToConstIterator;
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
      , m_context (m_myGaudiAlg->context() )
      , m_extension(Particle2MCMethod::extension[method])
      , m_linkerAlgType(Particle2MCMethod::algType[method])
      , m_linkerAlg(NULL)
      , m_containerList(containerList)
      , m_linkTo( m_evtSvc,NULL,"")
    , m_linkerTable( m_evtSvc,NULL,"") { }
  
  Object2MCLinker( const Algorithm* myMother,
                   const int method,
                   const std::string& container)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

  Object2MCLinker( const Algorithm* myMother,
                   const std::string& algType,
                   const std::string& extension,
                   const std::vector<std::string>& containerList)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(containerList)
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

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
    , m_context (m_myGaudiAlg->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

  Object2MCLinker( const Algorithm* myMother )
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(NULL)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>())
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

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
    , m_context (m_myGaudiTool->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(containerList)
    , m_linkTo( m_evtSvc,NULL,"")
    , m_linkerTable( m_evtSvc,NULL,"") {}

  Object2MCLinker( const GaudiTool* myMother,
                   const int method,
                   const std::string& container)
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

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
    , m_context (m_myGaudiTool->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(containerList)
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

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
    , m_context (m_myGaudiTool->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

  Object2MCLinker( const GaudiTool* myMother )
    : m_myGaudiAlg(NULL)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(NULL)
    , m_containerList(std::vector<std::string>())
    , m_linkTo(m_evtSvc,NULL,"")
    , m_linkerTable(m_evtSvc,NULL,"") {}

  virtual ~Object2MCLinker() {};  ///< Desctructor


  StatusCode setAlgorithm( const int method,
                           const std::vector<std::string>& containerList);

  StatusCode setAlgorithm( const int method,
                           const std::string& container)
  {
    return setAlgorithm( method, std::vector<std::string>(1,container));
  }

  StatusCode setAlgorithm( const std::string& algType,
                           const std::string& extension,
                           const std::vector<std::string>& containerList);

  StatusCode setAlgorithm( const std::string& algType,
                           const std::string& extension,
                           const std::string& container)
  {
    return setAlgorithm( algType, extension,
                         std::vector<std::string>(1,container));
  }



  typedef LinkedTo<LHCb::MCParticle>          To;

  typedef LinkerWithKey<LHCb::MCParticle, SOURCE>     Linker;

  Linker* linkerTable( const std::string& name) ;

  Linker* linkerTable( const std::string& name, To& test) ;

  ToRange rangeFrom(const SOURCE* part);

  const LHCb::MCParticle* firstMCP( const SOURCE* obj) ;

  const LHCb::MCParticle* firstMCP( const SOURCE* obj, double& weight) ;

  const LHCb::MCParticle* nextMCP() { return m_linkTo.next(); }

  const LHCb::MCParticle* nextMCP( double& weight) {
    const LHCb::MCParticle* mcPart =  m_linkTo.next();
    weight = NULL != mcPart ? m_linkTo.weight() : 0.;
    return mcPart;
  }

  double      weightMCP() { return m_linkTo.weight(); }

  int         associatedMCP( const SOURCE* obj) ;

  const LHCb::MCParticle* first ( const SOURCE* obj ) {
    return firstMCP( obj );
  }

  const LHCb::MCParticle* first ( const SOURCE* obj, double & weight ) {
    return firstMCP( obj, weight );
  }

  const LHCb::MCParticle* next() { return m_linkTo.next(); }

  const LHCb::MCParticle* next( double& weight) { return nextMCP(weight); }

  double weight() { return m_linkTo.weight(); }
  bool notFound();
  bool notFound( const std::string& contname);
  bool checkAssociation( const SOURCE* obj,
                         const LHCb::MCParticle* mcPart);
  std::string context()const {return m_context;}
  
protected:

  const GaudiAlgorithm*        m_myGaudiAlg;
  const GaudiTool*             m_myGaudiTool;
  IDataProviderSvc*            m_evtSvc;
  IMessageSvc*                 m_msgSvc;
  ISvcLocator*                 m_svcLoc;
  std::string                  m_name;
  std::string                  m_context;
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
class Object2FromMC : public Object2MCLinker<OBJ2MCP> {


public:
  /// Standard constructors
  Object2FromMC(const Algorithm* myMother)
    : Object2MCLinker<OBJ2MCP>( myMother )
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const Algorithm* myMother,
                 const int method,
                 const std::vector<std::string>& containerList)
    : Object2MCLinker<OBJ2MCP>( myMother, method, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const Algorithm* myMother,
                 const int method,
                 const std::string& container )
    : Object2MCLinker<OBJ2MCP>( myMother, method, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const Algorithm* myMother,
                 const std::string& algType,
                 const std::string& extension,
                 const std::vector<std::string>&
                 containerList)
    : Object2MCLinker<OBJ2MCP>( myMother, algType, extension, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const Algorithm* myMother,
                 const std::string& algType,
                 const std::string& extension,
                 const std::string& container)
    : Object2MCLinker<OBJ2MCP>( myMother, algType, extension, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC(const GaudiTool* myMother)
    : Object2MCLinker<OBJ2MCP>( myMother )
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const GaudiTool* myMother,
                 const int method,
                 const std::vector<std::string>& containerList)
    : Object2MCLinker<OBJ2MCP>( myMother, method, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const GaudiTool* myMother,
                 const int method,
                 const std::string& container )
    : Object2MCLinker<OBJ2MCP>( myMother, method, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const GaudiTool* myMother,
                 const std::string& algType,
                 const std::string& extension,
                 const std::vector<std::string>&
                 containerList)
    : Object2MCLinker<OBJ2MCP>( myMother, algType, extension, containerList)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  Object2FromMC( const GaudiTool* myMother,
                 const std::string& algType,
                 const std::string& extension,
                 const std::string& container)
    : Object2MCLinker<OBJ2MCP>( myMother, algType, extension, container)
    , m_linkFromList()
    , m_linkFrom( m_linkFromList.end() ) {}

  virtual ~Object2FromMC() {} ///< Destructor

  typedef LinkedFrom<OBJ2MCP>  From;
  typedef typename std::vector<From>::iterator FromIterator;

  bool        isAssociated( const KeyedObject<int>* obj)
  {
    const OBJ2MCP* part = dynamic_cast<const OBJ2MCP*>(obj);
    if( NULL != part ) return NULL != firstMCP(part);
    const LHCb::MCParticle* mcPart = dynamic_cast<const LHCb::MCParticle*>(obj);
    if( NULL != mcPart ) return NULL != firstP(mcPart);
    return false;
  }

  OBJ2MCP*     firstP( const LHCb::MCParticle* mcPart, double& weight)
  {
    OBJ2MCP* part = firstP(mcPart);
    weight = weightP();
    return part;
  }

  OBJ2MCP*     firstP( const LHCb::MCParticle* mcPart)
  {
    if( (NULL == this->m_myGaudiAlg &&
         NULL == this->m_myGaudiTool) ||
        "" == this->m_linkerAlgType ||
        0 == this->m_containerList.size() ) return NULL;

    createFromLinks();
    for( FromIterator fr = m_linkFromList.begin();
         m_linkFromList.end() != fr; fr++) {
      OBJ2MCP* part = fr->first( mcPart );
      if( NULL != part ) {
        // Remember which table and which MCParticle we are dealing with...
        m_linkFrom = fr;
        m_linkFromMCP = mcPart;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    m_linkFromMCP = NULL;
    return NULL;
  }

  OBJ2MCP*     nextP(double& weight)
  {
    OBJ2MCP* part = nextP();
    if( NULL != part) {
      weight = weightP();
    } else {
      weight = 0.;
    }
    return part;
  }

  OBJ2MCP*     nextP()
  {
    // If there was not a first() called before, stop there
    if( NULL == m_linkFromMCP ) return NULL;
    FromIterator fr = m_linkFrom;
    if( m_linkFromList.end() != fr ) {
      OBJ2MCP* part = fr->next();
      if( NULL != part ) {
        return part;
      }
    } else {
      return NULL;
    }


    while( m_linkFromList.end() != ++fr ) {
      // Get the first Particle associated to the MCP we were dealing with
      OBJ2MCP* part = fr->first( m_linkFromMCP );
      if( NULL != part ) {
        m_linkFrom = fr;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    m_linkFromMCP = NULL;
    return NULL;
  }

  double      weightP()
  {
    return m_linkFromList.end() != m_linkFrom ? m_linkFrom->weight() : 0.;
  }

  int         associatedP( const KeyedObject<int>* obj)
  {
    int n = 0;
    for( OBJ2MCP* part = firstP(obj);
         NULL != part;
         part = nextP(), n++) { }
    
    return n;
  }

protected:

private:
  std::vector<From>        m_linkFromList;
  FromIterator             m_linkFrom;
  const LHCb::MCParticle*  m_linkFromMCP;

  void        createFromLinks()
  {
    m_linkFromList.clear();
    for( std::vector<std::string>::const_iterator contIt =
           this->m_containerList.begin();
         this->m_containerList.end() != contIt; contIt++) {
      const std::string name =
        *contIt + this->m_extension;
      From test(  this->m_evtSvc, NULL, name);
      if( test.notFound() ) {
        this->createLinks( *contIt );
        test = From(  this->m_evtSvc, NULL, name);
      }
      m_linkFromList.push_back( test );
    }
    m_linkFrom = m_linkFromList.end();
  }

};

#include "Particle2MCLinker.icpp"

/** Linker type for associations between ProtoParticles and MCParticles
 *
 *  @author Philippe Charpentier
 *  @date   2004-04-29
 */
typedef Object2FromMC<LHCb::ProtoParticle> ProtoParticle2MCLinker;

/** Linker type for associations between Particles and MCParticles
 * 
 *  @author Philippe Charpentier
 *  @date   2004-04-29
 */
typedef Object2FromMC<>                    Particle2MCLinker;

#endif // DAVINCIASSOCIATORS_PARTICLE2MCLINKER_H
