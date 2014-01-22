// $Id$
// ============================================================================
// $URL$
// ============================================================================
#ifndef LOKI_HLT1FUNCTIONS_H 
#define LOKI_HLT1FUNCTIONS_H 1
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
#include "LoKi/Interface.h"
#include "LoKi/Hlt1.h"
// ============================================================================
/** @file LoKi/Hlt1Functions.h
 *  Collection of specific functions for Hl1 implementation
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
 *  @date   2009-03-30
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Tracks 
  {
    // ========================================================================
    /** @class MuonDeltaP 
     *  Simple class, LoKi-image of MuonDeltaPTool by Leandro de Paula.
     *  @see LoKi::Cuts::TrDPMU
     *  @see LoKi::Cuts::TrDPMU_
     *  @see HltDeltaPTool 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    class MuonDeltaP : public LoKi::TrackTypes::TrCuts
    {
    public:
      // ======================================================================
      /// constructor from delta-cut and list of p-cuts
      MuonDeltaP ( double                     delta , 
                   std::vector<double>        cuts  ) ;
      /// constructor from delta-cut 
      MuonDeltaP ( double                     delta ) ;
      /// constructor from list of p-cuts 
      MuonDeltaP ( std::vector<double>        cuts  ) ;
      /// default constructor 
      MuonDeltaP () ;
      /// MANDATORY: virtual destructor
      virtual ~MuonDeltaP() {}
      /// MANDATOTY: clone method ("virtual constructor")
      virtual  MuonDeltaP* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printput 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      /// =====================================================================
    private:
      // ======================================================================
      /// the cut on deltaP
      double m_deltaP ;                                    // the cut on deltaP
      /// the list of p-cuts for various regions 
      std::vector<double> m_pCuts ;   // the list of p-cuts for various regions 
      // ======================================================================
    };
    // ========================================================================
    /** @class Hlt1TrackMinIp
     *  simple function to get the mininal value  of impact parameter of track
     *  with respect to primart vertices 
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2010-12-05
     */
    class Hlt1TrackMinIp : public LoKi::TrackTypes::TrFunc 
    {
    public :
      // ======================================================================
      /// constructor from vertex selection 
      Hlt1TrackMinIp 
      ( const Hlt::TSelection<LHCb::RecVertex>* selection  ) ;
      /// constructor from vertex selection name 
      Hlt1TrackMinIp
      ( std::string                             selection  ) ;
      /// constructor from vertex selection and cuts 
      Hlt1TrackMinIp 
      ( const Hlt::TSelection<LHCb::RecVertex>*            selection  ,
        const LoKi::Functor<const LHCb::VertexBase*,bool>& cuts       ) ;
      /// constructor from vertex selection name and cuts 
      Hlt1TrackMinIp 
      ( std::string                                        selection  ,
        const LoKi::Functor<const LHCb::VertexBase*,bool>& cuts       ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Hlt1TrackMinIp() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hlt1TrackMinIp* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // =====================================================================
    private:
      // =====================================================================
      /// the default constructor is disabled 
      Hlt1TrackMinIp  () ;              // the default constructor is disabled 
      // =====================================================================
    protected:
      // =====================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::RecVertex>* selection () const 
      { return m_vertices.selection() ; }
      // =====================================================================
    protected:
      // =====================================================================
      /** the pseudo-source of vertices
       *  @attention! we'll not use it as the actual source!!
       */
      LoKi::Hlt1::VxSelection                                m_vertices      ;
      LoKi::FunctorFromFunctor<const LHCb::VertexBase*,bool> m_vxcut         ;
      bool                                                   m_vxcut_trivial ;
      // =====================================================================
    } ;
    // ========================================================================
    /** @class Hlt1TrackMinIpChi2 
     *  simple function to get the mininal value  of impact parameter chi2 
     *  with respect to primart vertices 
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2010-12-05
     */
    class Hlt1TrackMinIpChi2 : public  Hlt1TrackMinIp
    {
    public :
      // ======================================================================
      /// constructor from vertex selection 
      Hlt1TrackMinIpChi2
      ( const Hlt::TSelection<LHCb::RecVertex>*            selection  ) ;
      /// constructor from vertex selection name 
      Hlt1TrackMinIpChi2
      ( std::string                                        selection  ) ;
      /// constructor from vertex selection and cuts 
      Hlt1TrackMinIpChi2 
      ( const Hlt::TSelection<LHCb::RecVertex>*            selection  ,
        const LoKi::Functor<const LHCb::VertexBase*,bool>& cuts       ) ;
      /// constructor from vertex selection name and cuts 
      Hlt1TrackMinIpChi2 
      ( std::string                                        selection  ,
        const LoKi::Functor<const LHCb::VertexBase*,bool>& cuts       ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Hlt1TrackMinIpChi2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hlt1TrackMinIpChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // =====================================================================
    private:
      // =====================================================================
      /// the default constructor is disabled 
      Hlt1TrackMinIpChi2  () ;          // the default constructor is disabled 
      // =====================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef TrDPMU_ 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU
     *  @see LoKi::Cuts::TrDPMUON
     *  @see LoKi::Cuts::TrDSELTAPMU
     *  @see LoKi::Cuts::TrDSELTAPMUON
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    typedef LoKi::Tracks::MuonDeltaP                                  TrDPMU_ ;
    // ========================================================================
    /** @var TrDPMU 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                     TrDPMU ;
    // ========================================================================
    /** @var TrDPMUON 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                   TrDPMUON ;
    // ========================================================================
    /** @var TrDELTAPMU 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                 TrDELTAPMU ;
    // ========================================================================
    /** @var TrDELTAPMUON
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                               TrDELTAPMUON ;
    // ========================================================================
    /** @typedef  Tr_HLTMIP
     *  Simple functor to cut on miminal IP with respoect to HLT-vertices 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-05
     */
    typedef LoKi::Tracks::Hlt1TrackMinIp                         Tr_HLTMIP    ;
    // ========================================================================
    /** @typedef  Tr_HLTMIPCHI2
     *  Simple functor to cut on miminal chi2(IP) with respect to HLT-vertices 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-05
     */
    typedef LoKi::Tracks::Hlt1TrackMinIpChi2                    Tr_HLTMIPCHI2 ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_HLT1FUNCTIONS_H
// ============================================================================

