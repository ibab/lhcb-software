// $Id$
// ============================================================================
#ifndef LOKI_TRACKTOOL_H 
#define LOKI_TRACKTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// TrackEvent 
// ============================================================================
#include  "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include  "LoKi/Functor.h"
// ============================================================================
/** @file  LoKi/TrackTool.h
 *  
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-02-07
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class TrackTool TrackTool.h LoKi/TrackTool.h
     *
     *  Helper class to keep some track filter 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2011-02-07
     */
    // ========================================================================
    class GAUDI_API TrackTool 
    {
    public:
      // ======================================================================
      /// the constructor form the cut
      TrackTool ( const LoKi::Functor<const LHCb::Track*,bool>& cut ) ;
      /// default constructor 
      TrackTool () ;
      /// virtual destructor 
      virtual ~TrackTool () ;
      // ======================================================================
    public:
      // ======================================================================
      /// accept track ?
      bool acceptTrack ( const LHCb::Track* track ) const 
      {
        return
          0 == track ? false : 
          m_has_cut  ? m_cut.fun ( track ) : true ;
      }
      // ======================================================================
      /// accept track ?
      bool operator() ( const LHCb::Track* track ) const 
      { return acceptTrack ( track ) ; }
      // ======================================================================      
    protected:
      // ======================================================================
      /// has nontrivial filter ?
      bool hasFilter   () const { return m_has_cut ; }
      /// get the filter 
      const LoKi::Functor<const LHCb::Track*,bool>& filter() const 
      { return m_cut.func () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// has  non-trivial track filter ?
      bool                                               m_has_cut ;
      ///      non-trivial track filter 
      LoKi::FunctorFromFunctor<const LHCb::Track*,bool>  m_cut     ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRACKTOOL_H
// ============================================================================
