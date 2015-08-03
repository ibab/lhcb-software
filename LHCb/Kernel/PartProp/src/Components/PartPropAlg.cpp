// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Algorithm.h" 
// ============================================================================
// Kenrel/PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"
// ============================================================================
// Boots 
// ============================================================================
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  namespace Example 
  {
    // ========================================================================
    /** @class PartPropAlg 
     *  Simple algorithm to illustrate the major properties of new Particle 
     *  Property Service 
     *  @see LHCb::ParticlePropertySvc 
     *  @see LHCb::IParticlePropertySvc 
     *  @see LHCb::ParticleProperty
     *  @see LHCb::ParticleProperties 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-01 
     */
    class PartPropAlg : public Algorithm
    {
      // ======================================================================
      /// friend factory for algorithm instantiation
      friend class AlgFactory<LHCb::Example::PartPropAlg> ;  //     the factory 
      // ======================================================================
    public :
      // ======================================================================
      /// standard algorithm initialization 
      virtual StatusCode  initialize () 
      {
        StatusCode sc = Algorithm::initialize () ;   //     initialize the base 
        if ( sc.isFailure() ) { return sc ; }
        // locate particle property service 
        ppSvc() ;
        return StatusCode::SUCCESS ;
      }
      /// standard algorithm finalization 
      virtual StatusCode  finalize   () 
      {
        // release the aquired service 
        if ( 0 != m_ppSvc ) { m_ppSvc->release() ; m_ppSvc = 0 ; }
        return Algorithm::finalize () ;            //        finalize the base 
      }
      /// algorithm execution 
      virtual StatusCode  execute    () ;
      // ======================================================================
    protected:
      // ======================================================================
      /** standard constructor 
       *  @param name algorithm instance name 
       *  @param pSvc pointer to the service locator 
       */
      PartPropAlg 
      ( const std::string& name ,      //               algorithm instance name 
        ISvcLocator*       pSvc )      //        pointer to the service locator 
        : Algorithm ( name , pSvc ) 
        , m_ppSvc ( 0 )
      {}
      /// virtual destructor 
      virtual ~PartPropAlg() {}        //                    virtual destructor
      // ======================================================================
    protected:
      // ======================================================================
      /// locate the new particle property service 
      const LHCb::IParticlePropertySvc* ppSvc() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PartPropAlg () ;                 //   the default constructor is disabled 
      /// the copy constructor is disabled 
      PartPropAlg ( const PartPropAlg& ) ; //      copy constructor is disabled 
      /// assignement operator is disbaled 
      PartPropAlg& operator=( const PartPropAlg& ) ; //          no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to new particle property service 
      mutable LHCb::IParticlePropertySvc* m_ppSvc ;    // new part.property.svc 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LHCb::Example 
  // ==========================================================================
} // end of namespace LHCb 
// ============================================================================
// locate the new particle property service 
// ============================================================================
const LHCb::IParticlePropertySvc* LHCb::Example::PartPropAlg::ppSvc() const 
{
  if ( 0 == m_ppSvc ) 
  {
    StatusCode sc = service ( "LHCb::ParticlePropertySvc", m_ppSvc , true );
    if ( sc.isFailure() ) 
    {
      throw GaudiException
        ("Service [LHCb::ParticlePropertySvc] not found", 
         name(), sc ) ;
    }
  }
  // ==========================================================================
  if ( 0 == m_ppSvc ) 
  {
    throw GaudiException
      ("Service [LHCb::ParticlePropertySvc] is invalid " , 
       name() , StatusCode::FAILURE ) ;
  }
  // ==========================================================================
  return m_ppSvc ;
}
// ============================================================================
// execute the algorithm
// ============================================================================
StatusCode LHCb::Example::PartPropAlg::execute()
{
  // get the service 
  const LHCb::IParticlePropertySvc* svc = ppSvc () ;
  
  if ( 0 == svc ) { return StatusCode::FAILURE ; }

  // avoid long names 
  typedef LHCb::IParticlePropertySvc::ParticleProperties PPs ;
  using namespace boost::lambda ;
  
  MsgStream log ( msgSvc () , name() ) ;

  // ==========================================================================
  { // get all invalid 
    PPs invalid ;
    svc->get
      (
       // functor : lepton
       !bind ( &LHCb::ParticleID::isValid , *_1 ) ,
       // output
       std::back_inserter( invalid ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Invalid = " << invalid.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( invalid , log , svc ) ;
    log << endmsg ;
  }  
  // ==========================================================================
  { // get all not from quarks 
    PPs noquarks ;
    svc->get
      (
       // functor : has no quarks 
       !bind ( &LHCb::ParticleID::hasQuarks , *_1 ) ,
       // output
       std::back_inserter( noquarks ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Has no quarks = " << noquarks.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( noquarks , log , svc ) ;
    log << endmsg ;
  }  
  // ==========================================================================
  { // get all 'fundamental'
    PPs fundamental  ;
    svc->get
      (
       // functor : fundamental
       0   <  bind ( &LHCb::ParticleID::fundamentalID , *_1 ) &&
       100 >= bind ( &LHCb::ParticleID::fundamentalID , *_1 )   ,
       // output
       std::back_inserter( fundamental ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Fundamental (0,100] = " << fundamental.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( fundamental , log , svc ) ;
    log << endmsg ;
  }  
  // ==========================================================================
  { // get all leptons 
    PPs leptons ;
    svc->get
      (
       // functor : lepton
       bind ( &LHCb::ParticleID::isLepton , *_1 ) ,
       // output
       std::back_inserter( leptons ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Leptons = " << leptons.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( leptons , log , svc ) ;
    log << endmsg ;
  }
  // ==========================================================================
  { // get all long-lived (>1um)
    PPs longlived ;
    svc -> get 
      ( 
       // functor : ctau>1um 
       bind ( &LHCb::ParticleProperty::ctau , _1 ) > 
       1 * Gaudi::Units::micrometer ,
       // output
       std::back_inserter( longlived ) ) ;    
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Long-lived(>1mu) = " << longlived.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( longlived , log , svc ) ;
    log << endmsg ;
  }
  // =========================================================================
  { // get all nuclea
    PPs nuclea ;
    svc->get
      (
       // functor : nucleus 
       bind ( &LHCb::ParticleID::isNucleus , *_1 ) ,
       // output
       std::back_inserter( nuclea ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO << " # Nuclea  = " << nuclea.size() << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( nuclea , log , svc ) ;
    log << endmsg ;
  }
  // ==========================================================================
  { // get all beauty baryons 
    PPs  bbaryons;
    svc->get
      (
       // functor : beauty & baryon 
       bind ( &LHCb::ParticleID::hasBottom , *_1 ) && 
       bind ( &LHCb::ParticleID::isBaryon  , *_1 ) ,
       // output
       std::back_inserter( bbaryons ) ) ;
    // print as the table 
    // header ? 
    log << MSG::INFO 
        << " # Beauty Baryons  = " << bbaryons.size() 
        << std::endl ;
    // content 
    LHCb::ParticleProperties::printAsTable ( bbaryons , log , svc ) ;
    log << endmsg ;
  }
  // ==========================================================================
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb::Example,PartPropAlg)
// ============================================================================
// The END 
// ============================================================================

