// $Id$
// ============================================================================
#ifndef LOKI_VXMAKERCONF_H 
#define LOKI_VXMAKERCONF_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/VertexBase.h"
// ============================================================================
// LoKi 
// ============================================================================
#include  "LoKi/Functor.h"
// ============================================================================
/** @file  LoKi/VxMakerConf.h
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-11-12
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
    /** @class VxMakerConf VxMakerConf.h LoKi/VxMakerConf.h
     *  
     *
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-04
     */
    class GAUDI_API VxMakerConf 
    {
    public:
      // =====================================================================
      ///  constructor from the vertex making parameters 
      VxMakerConf
      ( const double docamax , 
        const double chi2max ) ;
      ///  Full constructor form the vertex making parameters & cuts 
      VxMakerConf
      ( const double docamax , 
        const double chi2max , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c1 , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c2 , 
        const LoKi::Functor<const LHCb::VertexBase*,bool>& c3 ) ;
      ///  Constructor form the vertex making parameters & cuts 
      VxMakerConf
      ( const double docamax , 
        const double chi2max , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c1 , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c2 ) ;
      ///  Constructor form the vertex making parameters & cuts 
      VxMakerConf
      ( const double docamax , 
        const double chi2max , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c1 , 
        const LoKi::Functor<const LHCb::VertexBase*,bool>& c3 ) ;
      ///  Constructor form the vertex making parameters & cuts 
      VxMakerConf
      ( const double docamax , 
        const double chi2max , 
        const LoKi::Functor<const LHCb::Track*     ,bool>& c1 ) ;
      ///  Constructor form the vertex making parameters & cuts 
      VxMakerConf
      ( const double docamax , 
        const double chi2max , 
        const LoKi::Functor<const LHCb::VertexBase*,bool>& c1 ) ;     
      /// virtual destructor 
      virtual ~VxMakerConf () ;                           // virtual destructor
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor id disabled 
      VxMakerConf() ;                    // the default constructor id disabled 
      // ======================================================================
    public: // the cuts 
      // ======================================================================
      /// cuts for the first track 
      const LoKi::Functor<const LHCb::Track*,bool>& track1cut  () const 
      { return m_track1cut ; }
      /// cuts for the first track 
      const LoKi::Functor<const LHCb::Track*,bool>& track2cut  () const 
      { return m_track2cut ; }
      /// cuts for vertex 
      const LoKi::Functor<const LHCb::VertexBase*,bool>& vxcut () const 
      { return m_vxcut     ; }
      // ======================================================================
    public: // apply the cuts 
      // ======================================================================
      /// apply cut for the first track 
      bool track1cut  ( const LHCb::Track*      t ) const 
      { return  0 != t && ( m_track1cut_trivial || m_track1cut .fun ( t ) ) ; }
      /// apply cut for the first track 
      bool track2cut  ( const LHCb::Track*      t ) const 
      { return  0 != t && ( m_track2cut_trivial || m_track2cut .fun ( t ) ) ; }
      /// apply cut for the vertex 
      bool vxcut      ( const LHCb::VertexBase* v ) const
      { return  0 != v && ( m_vxcut_trivial     || m_vxcut     .fun ( v ) ) ; }
      // ======================================================================
    public: // trivial cuts ? 
      // ======================================================================
      bool track1cut_trivial () const { return m_track1cut_trivial ; }
      bool track2cut_trivial () const { return m_track2cut_trivial ; }
      bool     vxcut_trivial () const { return     m_vxcut_trivial ; }
      // ======================================================================
    public:
      // ======================================================================
      double docamax () const { return m_docamax ; }
      double chi2max () const { return m_chi2max ; }        
      // ======================================================================
    public:
      // ======================================================================     
      /// printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const ;
      std::string toString() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the cut for the first track
      LoKi::FunctorFromFunctor<const LHCb::Track*     , bool>  m_track1cut ;
      /// the cut for the second track
      LoKi::FunctorFromFunctor<const LHCb::Track*     , bool>  m_track2cut ;
      /// the cut for the vertex 
      LoKi::FunctorFromFunctor<const LHCb::VertexBase*, bool>  m_vxcut     ;
      // ======================================================================
      // trivial cuts ? 
      // ======================================================================
      bool m_track1cut_trivial ;
      bool m_track2cut_trivial ;
      bool m_vxcut_trivial     ;
      // ======================================================================
      // vertex-making criteria 
      // ======================================================================
      /// the DOCA-cut
      double m_docamax ; // the DOCA-cut
      /// CHI2-cut
      double m_chi2max ; // CHI2-cut
      // ======================================================================
    };
    // ========================================================================
    /// output operator to ostream  
    inline 
    std::ostream& operator<<( std::ostream&                  s , 
                              const LoKi::Hlt1::VxMakerConf& o ) 
    { return o.fillStream ( s ) ; }
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VXMAKERCONF_H
// ============================================================================

