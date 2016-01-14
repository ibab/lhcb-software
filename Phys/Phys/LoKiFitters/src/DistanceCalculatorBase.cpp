// $Id$
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculatorBase.h"
// ============================================================================
/** @file
 *  The implementation of the class LoKi::DistanceCalculator
 *
 *  This file is a part of
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement
 *    with the campain of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-05
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
/*  Standard constructor
 *  @param type tool type(?)
 *  @param name tool instance name
 *  @param parent the pointer to the parent
 */
// ============================================================================
LoKi::DistanceCalculatorBase::DistanceCalculatorBase
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name
  const IInterface*  parent ) // the parent
  : LoKi::ParticleClassificator ( type , name , parent )
    // The name of particle transporter tool
  , m_transporterName   ( "ParticleTransporter:PUBLIC" ) /// The name of particle transpoter tool
    // The transporter tool itself
  , m_transporter       ( 0 ) /// The transporter tool itself
    // The name of track state provider
  , m_stateProviderName ( "TrackStateProvider:PUBLIC" )
    // The state provider  tool itself
  , m_stateProvider     ( 0 ) /// The state provider
    // The name of track extrapolator
  , m_extrapolatorName  ( "TrackMasterExtrapolator:PUBLIC" )
    // The state provider  tool itself
  , m_extrapolator      ( 0 ) /// The extrapolator
{
  // ==========================================================================
  declareProperty
    ( "Transporter"        , m_transporterName      ,
      "The Particle Transporter  tool to be used" );
  declareProperty
    ( "StateProvider"      , m_stateProviderName  ,
      "The Track State Provider  tool to be used" );
  declareProperty
    ( "TrackExtrapolator"  , m_extrapolatorName   ,
      "The Track Extrapolator    tool to be used" );
  // ==========================================================================
}
// ============================================================================
// virtual and protected desctructor
// ============================================================================
LoKi::DistanceCalculatorBase::~DistanceCalculatorBase(){}
// ============================================================================
// initialize
// ============================================================================
StatusCode LoKi::DistanceCalculatorBase::initialize ()
{
  StatusCode sc = LoKi::ParticleClassificator::initialize () ;
  if ( sc.isFailure() ) { return sc ; }         // RETURN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// initialize
// ============================================================================
StatusCode LoKi::DistanceCalculatorBase::finalize ()
{
  m_transporter   = 0 ;
  m_stateProvider = 0 ;
  return LoKi::ParticleClassificator::finalize () ;
}
// ============================================================================
// The END
// ============================================================================
