// $Id$
// ============================================================================
#ifndef LOKI_VXMAKER_H 
#define LOKI_VXMAKER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Hlt1.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class VxMaker VxMaker.h LoKi/VxMaker.h
     *  Simple "source" of reconstructed vertices - vertex maker...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-11
     */
    class VxMaker : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Source
    {
    private:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<LHCb::Track*>::Source TrSource ;
      // ======================================================================
    public:
      // ======================================================================
      // constructor from two sources and the key(string) 
      // ======================================================================
      /// constructor from two track sources 
      VxMaker ( const TrSource&             tr1     , 
                const TrSource&             tr2     , 
                const std::string&          key     ) ;
      /// constructor from two track sources 
      VxMaker ( const TrSource&             tr1     , 
                const TrSource&             tr2     , 
                const std::string&          key     ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      /// constructor from two track sources 
      VxMaker ( const TrSource&             tr1     , 
                const TrSource&             tr2     , 
                const std::string&          key     ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from two track sources 
      VxMaker ( const TrSource&             tr1     , 
                const TrSource&             tr2     , 
                const std::string&          key     ,
                const LoKi::Types::RVCuts&  cuts4rv ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from two track sources 
      VxMaker ( const TrSource&             tr1     , 
                const TrSource&             tr2     , 
                const std::string&          key     , 
                const LoKi::Types::TTrCuts& cuts2tr ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      // ======================================================================
      // constructor from one source and the key 
      // ======================================================================
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const Gaudi::StringKey&     key     ) ;
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const Gaudi::StringKey&     key     ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const Gaudi::StringKey&     key     ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const Gaudi::StringKey&     key     , 
                const LoKi::Types::RVCuts&  cuts4rv ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const Gaudi::StringKey&     key     , 
                const LoKi::Types::TTrCuts& cuts2tr ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      // ======================================================================
      // constructor from one source and the key(string) 
      // ======================================================================
      /// constructor from one track source
      VxMaker ( const TrSource&             trk     , 
                const std::string&          key     ) ;
      /// constructor from one track source
      VxMaker ( const TrSource&             trk     , 
                const std::string&          key     ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      /// constructor from one track source
      VxMaker ( const TrSource&             trk     , 
                const std::string&          key     ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from one track source
      VxMaker ( const TrSource&             trk     , 
                const std::string&          key     ,
                const LoKi::Types::RVCuts&  cuts4rv ,
                const LoKi::Types::TTrCuts& cuts2tr ) ;
      /// constructor from one track source 
      VxMaker ( const TrSource&             trk     , 
                const std::string&          key     , 
                const LoKi::Types::TTrCuts& cuts2tr ,
                const LoKi::Types::RVCuts&  cuts4rv ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~VxMaker() {}                   // MANDATORY: virtual destructor 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VxMaker* clone() const { return new VxMaker(*this); }
      /// MANDATORY: the only essential method ("source!")
      virtual result_type operator() ()  const ;                   // "source"! 
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      /// =====================================================================
      /// initialize the object 
      void init() const ;                              // initialize the object 
      // ======================================================================
    private:
      /// =====================================================================
      /// the first  source of tracks   
      LoKi::Assignable<TrSource>::Type m_track1  ;          // the first source   
      /// the second source of tracks   
      LoKi::Assignable<TrSource>::Type m_track2  ;         // the second source   
      /// the flag to identify "the same sources" 
      bool                             m_same    ;         // the same sources?
      // ======================================================================
      /// cuts to be applies for two track-combination 
      LoKi::Types::TTrCut              m_cut2tr  ;            // cut on 2-track 
      bool                             m_cut2tr_trivial ;     //       trivial?
      /// cuts to be applies for the recontructed vertex 
      LoKi::Types::RVCut               m_cut4rv  ;            //      cut on RV 
      bool                             m_cut4rv_trivial ;     //       trivial?
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::RvRegister           m_sink    ;                     // 'sink'
      // ======================================================================
    private:
      // ======================================================================
      /// the algorithm 
      mutable GaudiAlgorithm* m_alg ;                         // the algorithm 
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namesppace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VXMAKER_H
// ============================================================================
