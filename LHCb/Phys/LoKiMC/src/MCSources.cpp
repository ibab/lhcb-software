// $Id: MCSources.cpp,v 1.2 2010-06-05 20:15:58 ibelyaev Exp $
// ============================================================================
// Include files 
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
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path       ( path      ) 
  , m_dataSvc    ( svc       ) 
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( true ) )
  , m_use_finder ( false     ) 
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const std::string&           path , 
  const LoKi::MCTypes::MCCuts& cuts ,
  IDataProviderSvc*            svc  ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path       ( path      ) 
  , m_dataSvc    ( svc       ) 
  , m_cut        ( cuts      )
  , m_use_finder ( false     ) 
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() ) 
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
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const std::string&  path       , 
  const std::string&  descriptor , 
  IDataProviderSvc*   svc        ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path       ( path       ) 
  , m_dataSvc    ( svc        ) 
  , m_cut        ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant ( false ) )
  , m_use_finder ( true       ) 
  , m_finder     ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() ) 
{
  //
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to locate tool service" ) ; 
  //
  std::string factory = "LoKi::MCDecay" ;
  //
  Decays::IMCDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( factory , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + factory + "'" , sc ) ; 
  Assert ( 0 != tool       , "Decays::IMCDecay* points to NULL" ) ; 
  //
  m_finder = tool -> tree ( descriptor ) ;
  //
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  Assert ( !(!m_finder) , "the tree is invalid:  '" + descriptor + "'" ) ;
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
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               , 
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::MCParticle::Container> events ( m_dataSvc , m_path ) ;
  //
  const LHCb::MCParticle::Container* parts = events ;
  Assert ( 0 != parts , "No valid data is found at location '"+m_path+"'") ;
  //
  //
  LHCb::MCParticle::ConstVector result ;
  //
  if ( m_use_finder ) 
  {
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
                         result          ) ;
  }
  else 
  {
    // use cuts! 
    LoKi::select      ( parts->begin ()  , 
                        parts->end   ()  , 
                        std::back_inserter ( result ) , m_cut ) ;  
  }
  //
  return result ;
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
LoKi::MCVertices::SourceTES::SourceTES 
( const std::string&           path , 
  IDataProviderSvc*            svc  )
  : LoKi::MCVertices::SourceTES::_Source () 
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
  : LoKi::MCVertices::SourceTES::_Source () 
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
    Assert ( m_dataSvc.validPointer ( )               , 
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::MCVertex::Container> events ( m_dataSvc , m_path ) ;
  //
  const LHCb::MCVertex::Container* parts = events ;
  Assert ( 0 != parts , "No valid data is found at location '"+m_path+"'") ;
  //
  //
  LHCb::MCVertex::ConstVector result ;
  //
  // use cuts! 
  LoKi::select      ( parts->begin ()  , 
                      parts->end   ()  , 
                      std::back_inserter ( result ) , m_cut ) ;
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
