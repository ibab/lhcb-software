// $Id$
// ============================================================================
#ifndef LOKI_VXMAKERTOOL_H 
#define LOKI_VXMAKERTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Hlt1.h"
#include "LoKi/OldTypes.h"
// ============================================================================
/** @file 
 *  Implementation file for classes from the file LoKi/Hlt1Wrappers.h
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-31
 * 
 *  @todo VxCreator: check if we really need this code 
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class VxCreator LoKi/VxMakerTool.h
     *  Helper class for creation of vertices 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-13
     */
    class VxCreator : public virtual LoKi::AuxFunBase 
    {
    public:
      // ======================================================================
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TrCuts&  cuts    , 
                  const LoKi::Types::TTrCuts& cuts2tr ,
                  const LoKi::Types::RVCuts&  cust4rv ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TTrCuts& cuts2tr ,
                  const LoKi::Types::RVCuts&  cust4rv ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TrCuts&  cuts    , 
                  const LoKi::Types::RVCuts&  cust4rv ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TrCuts&  cuts    , 
                  const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TrCuts&  cuts    ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from the cuts 
      VxCreator ( const LoKi::Types::RVCuts&  cust4rv ) ;
      /// default contructor, no cuts 
      VxCreator () ;
      /// MANDATORY: virtual destructor 
      virtual ~VxCreator() {}
      // ======================================================================
      /// nice printout 
      // ======================================================================
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    protected:
      // ======================================================================
      /** make the vertices
       *  @param tracks1 the first set of tracks 
       *  @param tracks2 the second set of tracks 
       *  @param vertices (cumulative) container of created vertices   
       *  @return number of created vertices 
       */
      size_t make 
      ( const LHCb::Track::ConstVector& tracks1 , 
        const LHCb::Track::ConstVector& tracks2 , 
        std::vector<LHCb::RecVertex*>& vertices  ) const ;
      // ======================================================================
      /// alkl cuts are trivial?
      bool trivial() const 
      { return m_cut_trivial && m_cut2tr_trivial && m_cut4rv_trivial ; }
      // ======================================================================
    private:
      // ======================================================================
      const LoKi::Types::TrCuts&  cuts    () const { return m_cut    ; }
      const LoKi::Types::TTrCuts& cuts2tr () const { return m_cut2tr ; }
      const LoKi::Types::RVCuts&  cuts4rv () const { return m_cut4rv ; }
      // ======================================================================
    private:
      // ======================================================================
      /// cuts to be applied for each track 
      LoKi::Types::TrCut               m_cut     ;
      bool                             m_cut_trivial ;        //       trivial?      
      /// cuts to be applied for two track-combination 
      LoKi::Types::TTrCut              m_cut2tr  ;            // cut on 2-track 
      bool                             m_cut2tr_trivial ;     //       trivial?
      /// cuts to be applied for the recontructed vertex 
      LoKi::Types::RVCut               m_cut4rv  ;            //      cut on RV 
      bool                             m_cut4rv_trivial ;     //       trivial?
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VXMAKERTOOL_H
// ============================================================================
