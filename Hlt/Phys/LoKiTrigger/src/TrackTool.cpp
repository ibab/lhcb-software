// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTool.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Tracks::TrackTool
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
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @date 2011-02-07 
 *  @author Vanya  BELYAEV Ivan.Belyaev@cern.ch
 *
 *                    $Revision:$
 *  Last Modification $Date:$ 
 *                 by $Author:$
 */
// ============================================================================
// the constructor from the cut
// ============================================================================
LoKi::Hlt1::TrackTool::TrackTool
( const LoKi::Functor<const LHCb::Track*,bool>& cut ) 
  : m_has_cut ( true ) 
  , m_cut     ( cut  )
{}
// ============================================================================
// default constructor 
// ============================================================================
LoKi::Hlt1::TrackTool::TrackTool () 
  : m_has_cut ( true ) 
  , m_cut     ( LoKi::Constant<const LHCb::Track*,bool>( true ) )
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hlt1::TrackTool::~TrackTool (){}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

