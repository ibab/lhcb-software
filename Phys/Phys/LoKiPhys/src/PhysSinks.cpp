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
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date 2011-03-17
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SinkTES::SinkTES 
( const std::string&            path )
  : LoKi::AuxFunBase ( std::tie ( path ) ) 
  , LoKi::Vertices::SinkTES::_Sink () 
  , m_path    ( path  ) 
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
  Assert ( 0 != alg()      , "Invalid setup" ) ;
  //
  Assert ( !m_path.empty() , "No TES location is specified!" ) ;
  //
  LHCb::RecVertices* out = new LHCb::RecVertices();
  //
  alg() -> put ( out , m_path ) ;
  //
  for ( LHCb::VertexBase::ConstVector::const_iterator iv = 
          a.begin() ; a.end() != iv ; ++iv ) 
  {
    //
    const LHCb::VertexBase* vx = *iv ;
    if ( 0 == vx ) { continue ; }                                // CONTINUE 
    //
    const LHCb::RecVertex* rv = dynamic_cast<const LHCb::RecVertex*> ( vx ) ;
    //
    Assert ( 0 != rv , "VertexBase is not a RecVertex" );        // ASSERT 
    //
    out -> insert ( rv -> clone () ) ;
  }
  //
  return a;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Vertices::SinkTES::fillStream ( std::ostream& o ) const 
{ return o << "RV_SINKTES('" << m_path << "')" ; }
// ============================================================================
// The END 
// ============================================================================
