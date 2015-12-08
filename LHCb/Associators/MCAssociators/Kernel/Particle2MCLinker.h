// $Id: Particle2MCLinker.h,v 1.8 2009/01/19 18:20:58 jpalac Exp $
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

// DaVinciMCKernel
#include "Kernel/Particle2MCMethod.h"
#include "Kernel/MCAssociation.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

/** @class Object2MCLinker Particle2MCLinker.h Kernel/Particle2MCLinker.h
    *
    *  Class providing association functionality to MCParticles
    * 
    *  @author Philippe Charpentier
    *  @date   2004-04-29
    */

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
      , m_myGaudiTool(nullptr)
      , m_evtSvc( myMother->evtSvc())
      , m_msgSvc( myMother->msgSvc())
      , m_svcLoc( myMother->svcLoc())
      , m_name( myMother->name())
      , m_context (m_myGaudiAlg->context() )
      , m_extension(Particle2MCMethod::extension[method])
      , m_linkerAlgType(Particle2MCMethod::algType[method])
      , m_linkerAlg(nullptr)
      , m_containerList(containerList)
      , m_linkTo( m_evtSvc,nullptr,"")
    , m_linkerTable( m_evtSvc,nullptr,"") { }
  
  Object2MCLinker( const Algorithm* myMother,
                   const int method,
                   const std::string& container)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(nullptr)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const Algorithm* myMother,
                   const std::string& algType,
                   const std::string& extension,
                   const std::vector<std::string>& containerList)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(nullptr)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(nullptr)
    , m_containerList(containerList)
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const Algorithm* myMother,
                   const std::string& algType,
                   const std::string& extension,
                   const std::string& container)
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(nullptr)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())      
    , m_context (m_myGaudiAlg->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const Algorithm* myMother )
    : m_myGaudiAlg(dynamic_cast<const GaudiAlgorithm*>(myMother))
    , m_myGaudiTool(nullptr)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiAlg->context() )
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>())
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  // Now constructors from tools
  Object2MCLinker( const GaudiTool* myMother,
                   const int method,
                   const std::vector<std::string>& containerList)
    : m_myGaudiAlg(nullptr)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(nullptr)
    , m_containerList(containerList)
    , m_linkTo( m_evtSvc,nullptr,"")
    , m_linkerTable( m_evtSvc,nullptr,"") {}

  Object2MCLinker( const GaudiTool* myMother,
                   const int method,
                   const std::string& container)
    : m_myGaudiAlg(nullptr)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension(Particle2MCMethod::extension[method])
    , m_linkerAlgType(Particle2MCMethod::algType[method])
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const GaudiTool* myMother,
                   const std::string& algType,
                   const std::string& extension,
                   const std::vector<std::string>&
                   containerList)
    : m_myGaudiAlg(nullptr)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(nullptr)
    , m_containerList(containerList)
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const GaudiTool* myMother,
                   const std::string& algType,
                   const std::string& extension,
                   const std::string& container)
    : m_myGaudiAlg(nullptr)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension(extension)
    , m_linkerAlgType(algType)
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>(1,container))
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

  Object2MCLinker( const GaudiTool* myMother )
    : m_myGaudiAlg(nullptr)
    , m_myGaudiTool(myMother)
    , m_evtSvc( myMother->evtSvc())
    , m_msgSvc( myMother->msgSvc())
    , m_svcLoc( myMother->svcLoc())
    , m_name( myMother->name())
    , m_context (m_myGaudiTool->context() )
    , m_extension("")
    , m_linkerAlgType("")
    , m_linkerAlg(nullptr)
    , m_containerList(std::vector<std::string>())
    , m_linkTo(m_evtSvc,nullptr,"")
    , m_linkerTable(m_evtSvc,nullptr,"") {}

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
    weight = ( mcPart ? m_linkTo.weight() : 0. );
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
    return ( obj->parent() && obj->parent()->registry() ) ? 
                  obj->parent()->registry()->identifier() :
                  std::string{} ;
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
    if( part ) return nullptr != firstMCP(part);
    const LHCb::MCParticle* mcPart = dynamic_cast<const LHCb::MCParticle*>(obj);
    if( mcPart ) return nullptr != firstP(mcPart);
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
    if( (!this->m_myGaudiAlg &&
         !this->m_myGaudiTool) ||
         this->m_linkerAlgType.empty() ||
        0 == this->m_containerList.size() ) return nullptr;

    createFromLinks();
    for( FromIterator fr = m_linkFromList.begin();
         m_linkFromList.end() != fr; fr++) {
      OBJ2MCP* part = fr->first( mcPart );
      if( part ) {
        // Remember which table and which MCParticle we are dealing with...
        m_linkFrom = fr;
        m_linkFromMCP = mcPart;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    m_linkFromMCP = nullptr;
    return nullptr;
  }

  OBJ2MCP*     nextP(double& weight)
  {
    OBJ2MCP* part = nextP();
    if( part) {
      weight = weightP();
    } else {
      weight = 0.;
    }
    return part;
  }

  OBJ2MCP*     nextP()
  {
    // If there was not a first() called before, stop there
    if( !m_linkFromMCP ) return nullptr;
    FromIterator fr = m_linkFrom;
    if( m_linkFromList.end() == fr ) return nullptr;
    OBJ2MCP* part = fr->next();
    if( part ) return part;

    while( m_linkFromList.end() != ++fr ) {
      // Get the first Particle associated to the MCP we were dealing with
      OBJ2MCP* part = fr->first( m_linkFromMCP );
      if( part ) {
        m_linkFrom = fr;
        return part;
      }
    }
    m_linkFrom = m_linkFromList.end();
    m_linkFromMCP = nullptr;
    return nullptr;
  }

  double      weightP()
  {
    return m_linkFromList.end() != m_linkFrom ? m_linkFrom->weight() : 0.;
  }

  int         associatedP( const KeyedObject<int>* obj)
  {
    int n = 0;
    for( OBJ2MCP* part = firstP(obj);
         part;
         part = nextP(), n++) { }
    
    return n;
  }

private:
  std::vector<From>        m_linkFromList;
  FromIterator             m_linkFrom;
  const LHCb::MCParticle*  m_linkFromMCP;

  void        createFromLinks()
  {
    m_linkFromList.clear();
    for( const auto& cont : this->m_containerList ) {
      const std::string name = cont + this->m_extension;
      From test(  this->m_evtSvc, nullptr, name);
      if( test.notFound() ) {
        this->createLinks( cont );
        test = From(  this->m_evtSvc, nullptr, name);
      }
      m_linkFromList.emplace_back( std::move(test) );
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
