// $Id$
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculator.h"
// ============================================================================
/** @file 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-04-12
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class TrgDistanceCalculator
   *
   *  It is the fast "trigger-oriented" implementation of the basic math, 
   *  needed for the real implementation 
   *  on the abstract interface IDistanceCalculator 
   *  Essentially it relies on many nice functions, 
   *  coded by Juan & Matt  
   *  
   *  @see IDistanceCalculator 
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
   *  @date   2008-03-05
   */
  class TrgDistanceCalculator : public LoKi::DistanceCalculator
  {
    // ========================================================================
    // the friend factory for instantiation 
    friend class ToolFactory<LoKi::TrgDistanceCalculator> ;
    // ========================================================================
  protected: 
    // ========================================================================
    /** Standard constructor
     *  @param type tool type(?)
     *  @param name tool instance name 
     *  @param parent the pointer to the parent
     */
    TrgDistanceCalculator
    ( const std::string& type   ,    // tool type (?)
      const std::string& name   ,    // tool instance name 
      const IInterface*  parent )   // the parent 
      : LoKi::DistanceCalculator(  type , name , parent ) 
    {
      //
      StatusCode sc = setProperty ( "MaxPrints"           , 0    ) ;
      Assert ( sc.isSuccess () ,
               "Unable (re)set property ``MaxPrints''"         , sc ) ;
      sc            = setProperty ( "MaxIterations"       , 0    ) ;
      Assert ( sc.isSuccess () , 
               "Unable (re)set property ``MaxIterations''"     , sc ) ;
      sc            = setProperty ( "UseFastAlgorithms"   , true ) ;
      Assert ( sc.isSuccess () , 
               "Unable (re)set property ``UseFastAlgorithms''" , sc ) ;
      sc            = setProperty ( "Transporter"         , 
                                    "DaVinci::ParticleTransporter:PUBLIC" ) ;
      Assert ( sc.isSuccess () , 
               "Unable (re)set property ``Transporter''"  , sc ) ;
      sc            = setProperty ( "TrackExtrapolator"   , 
                                    "TrackParabolicExtrapolator:PUBLIC"   ) ;
      Assert ( sc.isSuccess () , 
               "Unable (re)set property ``TrackExtrapolator''"  , sc ) ;
    }
    // virtual and protected desctrustor 
    virtual ~TrgDistanceCalculator (){}  // Destructor
    // ========================================================================
  }; //                                end of class LoKi::TrgDistanceCalculator 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,TrgDistanceCalculator)
// ============================================================================
// The END 
// ============================================================================
