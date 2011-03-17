// $Id:
// ============================================================================
// Include files 
// ============================================================================
// boost
// ============================================================================
#include <boost/foreach.hpp>
// ============================================================================
// GaudiKernel
// ============================================================================
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/SmartDataPtr.h>
// ============================================================================
// Event 
// ============================================================================
#include <Event/VertexBase.h>
#include <Event/RecVertex.h>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysSinks.h"
#include "LoKi/Services.h"
#include "LoKi/AlgUtils.h"
// ============================================================================
/** @file 
 *  Implementation file for various sinks
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date 2011-03-17
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SinkTES::SinkTES 
( const std::string&            path , 
  IDataProviderSvc*             svc  )
   : LoKi::Vertices::SinkTES::_Sink () 
   , m_path    ( path ) 
   , m_dataSvc ( svc  ) 
{
   // get GaudiAlgorithm 
   GaudiAlgorithm*  alg = LoKi::AlgUtils::getGaudiAlg( *this, true ) ;
   Assert ( 0 != alg    , "GaudiAlgorithm points to NULL!" ) ;
   // get IAlgorithm 
   IAlgorithm*     ialg = LoKi::AlgUtils::getAlg( *this ) ;
   Assert ( alg == ialg ,
            "GaudiAlgorithm is not *my* IAlgorithm" ) ;
   //
   m_alg = alg;
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Vertices::SinkTES::SinkTES 
( const LoKi::Vertices::SinkTES& right ) 
   : LoKi::AuxFunBase               ( right ) 
   , LoKi::Vertices::SinkTES::_Sink ( right ) 
   , m_path    ( right.m_path    ) 
   , m_dataSvc ( right.m_dataSvc ) 
{
   // get GaudiAlgorithm 
   GaudiAlgorithm*  alg = LoKi::AlgUtils::getGaudiAlg( *this, true ) ;
   Assert ( 0 != alg    , "GaudiAlgorithm points to NULL!" ) ;
   // get IAlgorithm 
   IAlgorithm*     ialg = LoKi::AlgUtils::getAlg( *this ) ;
   Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;
   //
   m_alg = alg;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::SinkTES::~SinkTES(){}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Vertices::SinkTES::result_type 
LoKi::Vertices::SinkTES::operator()
   ( LoKi::Vertices::SinkTES::argument a ) const 
{
   //
   Assert ( 0 != alg() , "Invalid setup" ) ;
   //
   Assert ( !m_path.empty() , "No TES location is specified!" ) ;
   //
   if ( !m_dataSvc ) {
      const LoKi::Services& svcs = LoKi::Services::instance() ;
      m_dataSvc = svcs.evtSvc();
      Assert ( m_dataSvc.validPointer(),
               "Could not locate valid IDataProviderSvc" ) ;
   }  
   //
   LHCb::RecVertices* out = new LHCb::RecVertices();
   //
   StatusCode sc = m_dataSvc->registerObject( "/Event/" + m_path, out );
   Assert ( sc.isSuccess(), "Could not register vertices in the TES" );
   //
   BOOST_FOREACH( const LHCb::VertexBase* vx, a ) {
      const LHCb::RecVertex* tmp = dynamic_cast< const LHCb::RecVertex* >( vx );
      Assert( 0 != tmp, "VertexBase is not a RecVertex" );
      LHCb::RecVertex* rvx = tmp->clone();
      out->insert( rvx );
   }
   //
   return a;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Vertices::SinkTES::fillStream ( std::ostream& o ) const 
{
   return o << "RV_SINKTES('" << m_path << "')";
}
// ============================================================================
// The END 
// ============================================================================
