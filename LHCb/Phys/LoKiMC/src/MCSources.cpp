// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCSources.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/MCExtract.h"
#include "LoKi/Services.h"
#include "LoKi/MCParticles1.h"
#include "LoKi/Trees.h"
#include "LoKi/Algs.h"
// ============================================================================
/** @file
 *  Implementation file for various sources
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @date 2006-12-07
 */
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&           path ,
  IDataProviderSvc*            svc  )
  : LoKi::AuxFunBase ( std::tie ( path ) )
  , LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( true ) )
  , m_use_finder ( false     )
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
  , m_decay      ()
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&           path ,
  const LoKi::MCTypes::MCCuts& cuts ,
  IDataProviderSvc*            svc  )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) )
  , LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( cuts      )
  , m_use_finder ( false     )
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
  , m_decay      ()
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&           path ,
  const Decays::iNode&         node ,
  IDataProviderSvc*            svc  )
  : LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( LoKi::MCParticles::DecNode ( node ) )
  , m_use_finder ( false     )
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
  , m_decay      ()
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&              path   ,
  const Decays::IMCDecay::Finder& finder ,
  IDataProviderSvc*               svc  )
  : LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( false ) )
  , m_use_finder ( true      )
  , m_finder     ( finder    )
  , m_decay      ()
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&              path  ,
  const Decays::IMCDecay::iTree&  tree  ,
  IDataProviderSvc*               svc   )
  : LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( false ) )
  , m_use_finder ( true      )
  , m_finder     ( tree      )
  , m_decay      ()
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES
( const std::string&  path       ,
  const std::string&  descriptor ,
  IDataProviderSvc*   svc        )
  : LoKi::AuxFunBase ( std::tie ( path , descriptor ) )
  , LoKi::MCParticles::SourceTES::_Source ()
  , m_path       ( path       )
  , m_dataSvc    ( svc        )
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( false ) )
  , m_use_finder ( true       )
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
  , m_decay      ( descriptor )
{
  //
  if ( gaudi() ) { buildFinder () ; }
}
// ============================================================================
void LoKi::MCParticles::SourceTES::buildFinder () const
{
  //
  if ( !use_finder() || m_finder.valid() ) { return ; }
  //
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to locate tool service" ) ;
  //
  std::string factory = "LoKi::MCDecay" ;
  //
  Decays::IMCDecay* tool = nullptr ;
  StatusCode sc = toolSvc -> retrieveTool ( factory , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + factory + "'" , sc ) ;
  Assert ( nullptr != tool       , "Decays::IMCDecay* points to NULL" ) ;
  //
  m_finder = tool -> tree ( m_decay) ;
  //
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore
  //
  Assert ( !(!m_finder) , "the tree is invalid:  '" + m_decay + "'" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::SourceTES::~SourceTES(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::SourceTES*
LoKi::MCParticles::SourceTES::clone() const
{ return new LoKi::MCParticles::SourceTES ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::MCParticles::SourceTES::result_type
LoKi::MCParticles::SourceTES::operator() () const
{
  //
  LHCb::MCParticle::ConstVector result ;
  //
  get ( m_path , result ) ;
  //
  return result ;
}
// ============================================================================
// get the data from TES
// ============================================================================
std::size_t LoKi::MCParticles::SourceTES::get
( const std::string&             location ,
  LHCb::MCParticle::ConstVector& output   ) const
{
  //
  if ( !m_dataSvc )
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc,
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::MCParticle::Container> events ( m_dataSvc , location ) ;
  //
  const LHCb::MCParticle::Container* parts = events ;
  Assert ( nullptr != parts , "No valid data is found at location '"+location+"'") ;
  //
  if ( m_use_finder )
  {
    if ( !m_finder && !m_decay.empty() ) { buildFinder() ; }
    if ( !m_finder )
    {
      const LoKi::Services& svcs = LoKi::Services::instance() ;
      StatusCode sc = m_finder.validate ( svcs.ppSvc() ) ;
      Assert ( sc.isSuccess () , "Unable to validate the DecayFinder" ) ;
    }
    //
    Assert   ( !(!m_finder)    , "Unable to validate the DecayFinder" ) ;
    // use finder!!
    m_finder.findDecay ( parts->begin () ,
                         parts->end   () ,
                         output          ) ;
  }
  else
  {
    // use cuts!
    std::copy_if( parts->begin ()  ,
                  parts->end   ()  ,
                  std::back_inserter ( output ) , std::cref(m_cut) ) ;
  }
  //
  return output.size() ;
}
// ============================================================================
// get the data from TES
// ============================================================================
std::size_t LoKi::MCParticles::SourceTES::count
( const std::string&             location  ) const
{
  //
  if ( !m_dataSvc )
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc,
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::MCParticle::Container> events ( m_dataSvc , location ) ;
  //
  const LHCb::MCParticle::Container* parts = events ;
  Assert ( nullptr != parts , "No valid data is found at location '"+location+"'") ;
  //
  if ( m_use_finder )
  {
    LHCb::MCParticle::ConstVector output ;
    if ( !m_finder && !m_decay.empty() ) { buildFinder() ; }
    if ( !m_finder )
    {
      const LoKi::Services& svcs = LoKi::Services::instance() ;
      StatusCode sc = m_finder.validate ( svcs.ppSvc() ) ;
      Assert ( sc.isSuccess () , "Unable to validate the DecayFinder" ) ;
    }
    //
    Assert   ( !(!m_finder)    , "Unable to validate the DecayFinder" ) ;
    // use finder!!
    m_finder.findDecay ( parts->begin () ,
                         parts->end   () ,
                         output          ) ;

    return output.size() ;
  }
  //
  // use cuts!
  //
  return LoKi::Algs::count_if ( parts->begin ()  ,
                                parts->end   ()  , m_cut ) ;
  //
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream&
LoKi::MCParticles::SourceTES::fillStream ( std::ostream& o ) const
{
  o << " MCSOURCE( '" << m_path << "' , " ;
  if  ( m_use_finder ) { o << m_finder ; }
  else                 { o << m_cut    ; }
  //
  return o << " ) " ;
}



// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::TESCounter::TESCounter
( const std::string&           path ,
  const LoKi::MCTypes::MCCuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) )
  , LoKi::Functor<void,double> ()
  , m_source ( path , cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::TESCounter::TESCounter
( const std::string&           path ,
  const Decays::iNode&         node )
  : LoKi::Functor<void,double> ()
  , m_source ( path , node )
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::TESCounter::TESCounter
( const std::string&              path ,
  const Decays::IMCDecay::Finder& finder )
  : LoKi::Functor<void,double> ()
  , m_source ( path , finder )
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::TESCounter::TESCounter
( const std::string&              path ,
  const Decays::IMCDecay::iTree&  finder )
  : LoKi::Functor<void,double> ()
  , m_source ( path , finder )
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCParticles::TESCounter::TESCounter
( const std::string&              path ,
  const std::string&              finder )
  : LoKi::AuxFunBase ( std::tie ( path , finder ) )
  , LoKi::Functor<void,double> ()
  , m_source ( path , finder )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::TESCounter::~TESCounter (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::TESCounter*
LoKi::MCParticles::TESCounter::clone() const
{ return new LoKi::MCParticles::TESCounter ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::MCParticles::TESCounter::result_type
LoKi::MCParticles::TESCounter::operator() ( /* argument */ ) const
{ return m_source.count ( m_source.path() ) ; }
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream&
LoKi::MCParticles::TESCounter::fillStream ( std::ostream& o ) const
{
  o << " MCNUM( '" << m_source.path() << "' , " ;
  if  ( m_source.use_finder() ) { o << m_source.finder() ; }
  else                          { o << m_source.cut   () ; }
  //
  return o << " ) " ;
}


// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCVertices::SourceTES::SourceTES
( const std::string&           path ,
  IDataProviderSvc*            svc  )
  : LoKi::AuxFunBase ( std::tie ( path ) )
  , LoKi::MCVertices::SourceTES::_Source ()
  , m_path       ( path      )
  , m_dataSvc    ( svc       )
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCVertex*>::BooleanConstant ( true ) )
{}
// ============================================================================
// constructor from the service, TES location and cuts
// ============================================================================
LoKi::MCVertices::SourceTES::SourceTES
( const std::string&            path ,
  const LoKi::MCTypes::MCVCuts& cuts ,
  IDataProviderSvc*             svc  )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) )
  , LoKi::MCVertices::SourceTES::_Source ()
  , m_path       ( path )
  , m_dataSvc    ( svc  )
  , m_cut        ( cuts )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCVertices::SourceTES::~SourceTES(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCVertices::SourceTES*
LoKi::MCVertices::SourceTES::clone() const
{ return new LoKi::MCVertices::SourceTES ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::MCVertices::SourceTES::result_type
LoKi::MCVertices::SourceTES::operator() () const
{
  if ( !m_dataSvc )
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc,
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::MCVertex::Container> events ( m_dataSvc , m_path ) ;
  //
  const LHCb::MCVertex::Container* parts = events ;
  Assert ( parts , "No valid data is found at location '"+m_path+"'") ;
  //
  LHCb::MCVertex::ConstVector result ;
  // use cuts!
  std::copy_if      ( parts->begin ()  ,
                      parts->end   ()  ,
                      std::back_inserter ( result ) , std::cref(m_cut) ) ;
  //
  return result ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream&
LoKi::MCVertices::SourceTES::fillStream ( std::ostream& o ) const
{ return o << " MCVSOURCE( '" << m_path << "', " << m_cut << " ) " ; }

// ============================================================================
// The END
// ============================================================================
