// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include  "GaudiKernel/Bootstrap.h"
#include  "GaudiKernel/ISvcLocator.h"
#include  "GaudiKernel/IAlgorithm.h"
#include  "GaudiKernel/IAlgManager.h"
#include  "GaudiKernel/SmartIF.h"
#include  "GaudiKernel/ToStream.h"
#include  "GaudiKernel/TypeNameString.h"
#include  "GaudiKernel/IJobOptionsSvc.h"
// ============================================================================
// GaudiALg
// ============================================================================
#include  "GaudiAlg/GaudiAlgorithm.h"
#include  "GaudiAlg/GetAlgs.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/AlgFunctors.h"
#include "LoKi/Report.h"
#include "LoKi/Services.h"
#include "LoKi/Assert.h"
// ============================================================================
#ifdef __INTEL_COMPILER       // Disable ICC remark
#pragma warning(disable:2259) // non-pointer conversion may lose significant bits
#pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#endif
// ============================================================================
/** @file
 *  Implementation file for functions from namespace LoKi::Algorithms 
 *  @see LoKi::Algorithms 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-010-14 
 */ 
// ============================================================================
namespace 
{
  // ==========================================================================
  //  const IAlgManager* const s_ALGMANAGER = 0 ;
  const IAlgorithm*  const s_ALGORITHM  = 0 ;
  // ==========================================================================
  /// get the algorithm manager
  inline SmartIF<IAlgManager> getAlgManager ( LoKi::ILoKiSvc* loki )
  {
    if ( 0 == loki ) { loki = LoKi::Services::instance().lokiSvc () ; }
    SmartIF<IAlgManager> alg ( loki ) ;
    if ( !alg ) 
    { LoKi::Report::Error 
        ( "AlgFunctors::getAlgManager: Unable to locate IAlgManager" ) ; }
    return alg ;
  }
  // =========================================================================
  // get the algorithm 
  // =========================================================================
  inline LoKi::Interface<IAlgorithm> getAlgorithm 
  ( const std::string&    name , 
    SmartIF<IAlgManager>  iam  )
  {

    if ( !iam ) 
    {
      LoKi::Report::Error 
        ( "AlgFunctors::getAlgorithm: IAlgManager* points to NULL" ) ; 
      return LoKi::Interface<IAlgorithm>()  ; 
    }

    
    {
      // if algorith, is already exist - just get it! 
      IAlgorithm* iialg = 0 ;
      iialg = iam->algorithm ( name , false ) ;
      if ( nullptr != iialg ) 
      { return LoKi::Interface<IAlgorithm>( iialg ) ; }
    }
    
    ///////// start of code copied from GaudiSequencer...

    /** @todo       AlgFunctors.cpp : This stupid code MUST BE REMOVED ASAP!
     *              we definitely can;t live with it!
     *  
     *  @warning    AlgFunctors.cpp : This stupid code MUST BE REMOVED ASAP!
     *              we definitely can;t live with it!
     *
     *  @attention  AlgFunctors.cpp : This stupid code MUST BE REMOVED ASAP!
     *              we definitely can;t live with it!
     */
    
    // get job options svc -- todo: move this one level up, not done 
    // on this branch to retain binary compatiblity...
    LoKi::ILoKiSvc *ls = LoKi::Services::instance().lokiSvc ();
    SmartIF<IAlgContextSvc> cntx ( ls ) ;
    LoKi::Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    GaudiAlgorithm *parent = Gaudi::Utils::getGaudiAlg ( cntx ) ;
    LoKi::Assert ( parent!=nullptr , "parent not a GaudiAlg!" ) ;
    IJobOptionsSvc* jos = parent->svc<IJobOptionsSvc>( "JobOptionsSvc" );

    /////////////////////////////
    /////////////////////////////
    const Gaudi::Utils::TypeNameString typeName(name);
    const std::string &theName = typeName.name();
    const std::string &theType = typeName.type();

    bool addedContext = false;
    bool addedRootInTES = false;
    // do not create it now
    SmartIF<IAlgorithm> myIAlg = iam->algorithm( typeName , false); 

    if ( !myIAlg.isValid() ) {
      //== Set the Context if not in the jobOptions list
      if ( ""  != parent->context() ||
           ""  != parent->rootInTES() ) {
        bool foundContext = false;
        bool foundRootInTES = false;
        const std::vector<const Property*>* properties = 
          jos->getProperties( theName );
        if ( properties ) {
          // Iterate over the list to set the options
          for (const auto & property : *properties)   {
            const StringProperty* sp =
              dynamic_cast<const StringProperty*>(property);
            if ( sp )    {
              if ( "Context" == property->name() ) foundContext = true;
              if ( "RootInTES" == property->name() ) foundRootInTES = true;
            }
          }
        }
        if ( !foundContext && "" != parent->context() ) {
          StringProperty contextProperty( "Context", parent->context() );
          jos->addPropertyToCatalogue( theName, contextProperty ).ignore();
          addedContext = true;
        }
        if ( !foundRootInTES && "" != parent->rootInTES() ) {
          StringProperty rootInTESProperty( "RootInTES", parent->rootInTES() );
          jos->addPropertyToCatalogue( theName, rootInTESProperty ).ignore();
          addedRootInTES = true;
        }
      }

      Algorithm *myAlg = nullptr;
      StatusCode result = parent->createSubAlgorithm( theType, theName, myAlg );
      // (MCl) this should prevent bug #35199... even if I didn't manage to
      // reproduce it with a simple test.
      if (result.isSuccess()) myIAlg = myAlg;
    } else {
      Algorithm *myAlg = dynamic_cast<Algorithm*>(myIAlg.get());
      if (myAlg) {
        parent->subAlgorithms()->push_back(myAlg);
        // when the algorithm is not created, 
        // the ref count is short by one, so we have to fix it.
        myAlg->addRef();
      }
    }
    

    //== Remove the property, in case this is not a GaudiAlgorithm...
    if ( addedContext ) {
      jos->removePropertyFromCatalogue( theName, "Context" ).ignore();
      addedContext = false;
    }
    if ( addedRootInTES ) {
      jos->removePropertyFromCatalogue( theName, "RootInTES" ).ignore();
      addedRootInTES = false;
    }
    ///////// end of code copied from GaudiSequencer...

    IAlgorithm* _a =  myIAlg;
    if ( !_a->isInitialized() ) 
    {
      StatusCode sc = _a->sysInitialize() ;
      if ( sc.isFailure() ) 
      {
        LoKi::Report::Error 
          ( "AlgFunctors::getAlgorithm: Failure to initialize '" + name + "'" , sc ) ;
        return LoKi::Interface<IAlgorithm>()  ; 
      } 
    }
    //
    
    if ( Gaudi::StateMachine::RUNNING != _a -> FSMState() &&
         Gaudi::StateMachine::RUNNING == LoKi::Services::instance().lokiSvc()->FSMState() )
    { 
      // start it! 
      StatusCode sc = _a->sysStart() ;
      if ( sc.isFailure() ) 
      {
        LoKi::Report::Error 
          ( "AlgFunctors::getAlgorithm: Failure to start '" + name + "'" , sc ) ;
        return LoKi::Interface<IAlgorithm>()  ; 
      } 
    }
    //
    return LoKi::Interface<IAlgorithm>( _a )  ; 
  }
  // ===========================================================================
  // get the algorithm 
  inline LoKi::Interface<IAlgorithm> getAlgorithm 
  ( const std::string&    name , LoKi::ILoKiSvc* loki )  
  {
    SmartIF<IAlgManager> iam = getAlgManager  ( loki ) ;
    if ( !iam ) { return nullptr ; }  
    return getAlgorithm ( name , iam ) ;
  }
  // ==========================================================================
  // filter passed ?
  struct FilterPassed : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    { return 0 == ia ? false : ia -> filterPassed () ; }
  } ;
  // ========================================================================
  // is enabled ?
  struct IsEnabled : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    { return 0 == ia ? false : ia -> isEnabled () ; }
  } ;
  // ==========================================================================
  // is executed ?
  struct IsExecuted : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    { return 0 == ia ? false : ia -> isExecuted () ; }
  } ;
  // ==========================================================================
}
// ============================================================================
// get the algorithm 
// ============================================================================
const LoKi::Interface<IAlgorithm>& 
LoKi::Algorithms::Passed::getAlgorithm 
( const std::string& name ) const  // get the algorithm 
{
  const IAlgorithm* a = ::getAlgorithm ( name , lokiSvc () ) ;
  Assert ( 0 != a , "Unable to locate algorithm '" + name + "'" ) ;
  m_algorithm = a ;
  return algorithm () ;
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Passed::Passed 
( const std::string& name )
  : LoKi::AuxFunBase ( std::tie ( name ) ) 
  , LoKi::BasicFunctors<void>::Predicate () 
  , m_name       ( name        ) 
  , m_algorithm  ( s_ALGORITHM ) 
{
  if ( gaudi() ) { this->getAlgorithm ( algName() ) ; }
}
// ============================================================================
LoKi::Algorithms::Passed::~Passed () 
{
  if ( m_algorithm && !gaudi() ) 
  {
    // Warning("IAlgorithm: manual reset!") ;
    m_algorithm.reset() ;
  }  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Passed::result_type 
LoKi::Algorithms::Passed::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  //
  FilterPassed fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Passed::fillStream( std::ostream& s ) const 
{ return s << "ALG_PASSED(" << Gaudi::Utils::toString( algName() ) << ")" ; }
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Enabled::Enabled ( const std::string& name ) : Passed ( name ){}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Enabled::result_type 
LoKi::Algorithms::Enabled::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  //
  IsEnabled fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Enabled::fillStream( std::ostream& s ) const 
{ return s << "ALG_ENABLED(" << Gaudi::Utils::toString( algName() ) << ")" ; }

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Executed::Executed 
( const std::string& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) ) 
  , Passed ( name ){}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Executed::result_type 
LoKi::Algorithms::Executed::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  // 
  IsExecuted fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Executed::fillStream( std::ostream& s ) const 
{ return s << "ALG_EXECUTED(" << Gaudi::Utils::toString( algName() ) << ")" ; }


// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Run::Run ( const std::string& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) ) 
  , Passed ( name ){}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Run::result_type 
LoKi::Algorithms::Run::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  // 
  IsEnabled    fun1 ;
  IsExecuted   fun2 ;
  FilterPassed fun3 ;
  //
  if ( !fun1 ( algorithm() ) ) 
  {
    Warning("Algorithm '" + algName() + "' is disabled, return false " );
    return false ;                                                  // RETURN
  } 
  //
  if ( !fun2 ( algorithm() ) ) 
  {
    StatusCode sc = algorithm()->sysExecute() ;  // EXECUTE IT!!!
    if ( sc.isFailure() ) 
    {
      Error("Error from algorithm '" + algName() + "' sysExecute", sc );
      return false ;                                                // RETURN 
    } 
  }
  //
  // finally: 
  return fun3 ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Run::fillStream( std::ostream& s ) const 
{ return s << "ALG_RUN(" << Gaudi::Utils::toString( algName() ) << ")" ; }


// ============================================================================
// get the algorithm 
// ============================================================================
LoKi::Interface<IAlgorithm>
LoKi::Algorithms::AnyPassed::getAlgorithm 
( const std::string& name ) const  // get the algorithm 
{
  LoKi::Interface<IAlgorithm> a = ::getAlgorithm ( name , lokiSvc () ) ;
  Assert ( a.validPointer() , "Unable to locate algorithm '" + name + "'" ) ;
  return a ;
}
// ============================================================================
// get the algorithms
// ============================================================================
void LoKi::Algorithms::AnyPassed::getAlgorithms () const  // get the algorithm 
{
  // unsigned int i = 0 ;
  for ( std::vector<std::string>::const_iterator iname = 
          m_names.begin() ; m_names.end() != iname ; ++iname ) 
  {
    const IAlgorithm* a = this->getAlgorithm ( *iname ) ;
    m_algorithms.push_back ( a ) ;
    // Warning( " AQUICRE alg: " + a->name() 
    // + Gaudi::Utils::toCpp ( ++i ) + "/"
    // + Gaudi::Utils::toCpp ( m_names.size() ) ) ;
  }
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 )
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) ) 
  , LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  //
  if ( gaudi() ) { getAlgorithms() ; }
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 ,
  const std::string& name3 )
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) ) 
  , LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
  //
  if ( gaudi() ) { getAlgorithms() ; }
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 )
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) ) 
  , LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
  m_names.push_back ( name4 ) ;
  //
  if ( gaudi() ) { getAlgorithms() ; }
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::vector<std::string>& names ) 
  : LoKi::AuxFunBase ( std::tie ( names ) ) 
  , LoKi::BasicFunctors<void>::Predicate () 
  , m_names     ( names ) 
  , m_algorithms ( ) 
{
  if ( gaudi() ) { getAlgorithms() ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Algorithms::AnyPassed::~AnyPassed()
{
  for ( Algorithms::iterator ia = m_algorithms.begin() ; m_algorithms.end() != ia ; ++ia ) 
  {
    if ( ( *ia )  && !gaudi() ) 
    {
      // Warning("IAlgorithm: manual reset!") ;
      ia->reset() ;
    }
  }   
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyPassed::result_type 
LoKi::Algorithms::AnyPassed::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end() != std::find_if ( begin () , end   () , FilterPassed() ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::AnyPassed::print 
( const std::string& name , std::ostream& s ) const 
{ 
  if      ( 4 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 2 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 3 ) ) << ")" ;   // RETURN 
  }
  else if ( 3 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 2 ) ) << ")" ;   // RETURN 
  }
  else if ( 2 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << ")" ;   // RETURN 
  }
  return s << name << "(" << Gaudi::Utils::toString( algNames() ) << ")" ; 
}



// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyEnabled::AnyEnabled 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyEnabled::AnyEnabled
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyEnabled::AnyEnabled 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyEnabled::AnyEnabled
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::AnyPassed ( name ) 
{}

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllEnabled::AllEnabled 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllEnabled::AllEnabled
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllEnabled::AllEnabled 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllEnabled::AllEnabled
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::AnyPassed ( name ) 
{}

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyExecuted::AnyExecuted
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyExecuted::AnyExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyExecuted::AnyExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyExecuted::AnyExecuted
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::AnyPassed ( name ) 
{}

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllExecuted::AllExecuted
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllExecuted::AllExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllExecuted::AllExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllExecuted::AllExecuted
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::AnyPassed ( name ) 
{}


// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AllPassed::AllPassed 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::AllPassed::AllPassed 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::AllPassed::AllPassed
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::AnyPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::AllPassed::AllPassed 
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::AnyPassed ( name ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllPassed::result_type 
LoKi::Algorithms::AllPassed::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1 ( FilterPassed() ) ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyEnabled::result_type 
LoKi::Algorithms::AnyEnabled::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end() != std::find_if ( begin () , end () , IsEnabled () ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllEnabled::result_type 
LoKi::Algorithms::AllEnabled::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1( IsEnabled () ) ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyExecuted::result_type 
LoKi::Algorithms::AnyExecuted::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end () != std::find_if ( begin () , end () , IsExecuted () ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllExecuted::result_type 
LoKi::Algorithms::AllExecuted::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1 ( IsExecuted () ) ) ;
}
// ============================================================================


// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) ) 
  , LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) ) 
  , LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 , name3 ) {}
// ============================================================================
// constructor from the algorithm name  
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) ) 
  , LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 , name3 , name4 ) {}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) ) 
  , LoKi::BasicFunctors<void>::Function ()
  , m_fun  ( name ) {}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumPassed::result_type 
LoKi::Algorithms::NumPassed::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , FilterPassed() ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumEnabled::result_type 
LoKi::Algorithms::NumEnabled::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , IsEnabled() ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumExecuted::result_type 
LoKi::Algorithms::NumExecuted::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , IsExecuted () ) ;
}



// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumEnabled::NumEnabled 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumEnabled::NumEnabled 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumEnabled::NumEnabled 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumEnabled::NumEnabled 
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::NumPassed ( name ) 
{}

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumExecuted::NumExecuted
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumExecuted::NumExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumExecuted::NumExecuted
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2 , name3 , name4 ) )
  , LoKi::Algorithms::NumPassed ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name      
// ============================================================================
LoKi::Algorithms::NumExecuted::NumExecuted
( const std::vector<std::string>& name ) 
  : LoKi::AuxFunBase ( std::tie ( name ) )
  , LoKi::Algorithms::NumPassed ( name ) 
{}


// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::RunAll::RunAll 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2  ) ) 
  , LoKi::Algorithms::AllExecuted ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::RunAll::RunAll 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2  , name3 ) ) 
  , LoKi::Algorithms::AllExecuted ( name1 , name2 , name3 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::RunAll::RunAll 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::AuxFunBase ( std::tie ( name1 , name2  , name3 , name4 ) ) 
  , LoKi::Algorithms::AllExecuted ( name1 , name2 , name3 , name4 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::RunAll::RunAll 
( const std::vector<std::string>& names ) 
  : LoKi::AuxFunBase ( std::tie ( names ) ) 
  , LoKi::Algorithms::AllExecuted ( names ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::RunAll::result_type 
LoKi::Algorithms::RunAll::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  IsEnabled    fun1 ;
  IsExecuted   fun2 ;
  FilterPassed fun3 ;
  //
  const Algorithms& algs = algorithms() ;
  for ( Algorithms::const_iterator ialg = algs.begin() ; 
        algs.end() != ialg ; ++ialg ) 
  {
    //
    IAlgorithm* alg = *ialg ;
    if ( 0 == alg ) 
    {
      Warning("Invaild algotithm!, return false " );
      return false ;                                                  // RETURN
    }
    //
    if ( !fun1 ( alg ) ) 
    {
      Warning("Algorithm '" + alg->name() + "' is disabled, return false " );
      return false ;                                                  // RETURN
    } 
    //
    if ( !fun2 ( alg ) ) 
    {
      StatusCode sc = alg->sysExecute() ;  // EXECUTE IT!!!
      if ( sc.isFailure() ) 
      {
        Error("Error from algorithm '" + alg->name() + "' sysExecute", sc );
        return false ;                                                // RETURN 
      } 
    }
    //
    if ( !fun3 ( alg ) ) { return false ; }                           // RETURN 
  }
  //
  return true ;
}
// ============================================================================
// The END 
// ============================================================================
