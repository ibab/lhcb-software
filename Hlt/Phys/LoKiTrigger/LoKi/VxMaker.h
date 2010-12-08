// $Id$
// ============================================================================
#ifndef LOKI_VXMAKER_H 
#define LOKI_VXMAKER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
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
#include "LoKi/HelperTool.h"
#include "LoKi/VxMakerConf.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
/** @file  LoKi/VxMaker.h
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
 *  @date   2010-10-28
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
    /** @class VxMakerBase LoKi/VxMaker.h
     *  Helper class to implement vertex making 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    class GAUDI_API VxMakerBase : public LoKi::Hlt1::HelperTool
    {
    public:      
      // ======================================================================
      ///  constructor from the vertex making parameters 
      VxMakerBase ( const LoKi::Hlt1::VxMakerConf& conf ) ;
      /// virtual destructor 
      virtual ~VxMakerBase() ;                            // virtual destructor
      // ======================================================================
    protected:
      // ======================================================================
      /** make the vertex out of two tracks 
       *  @param track1 (INPUT) the first  track 
       *  @param track1 (INPUT) the second track 
       *  @param apply  (INPUT) apply the track cuts 
       *  @return the vertex (properly registered) 
       */
      const LHCb::RecVertex* makeVertex 
      ( const LHCb::Track* track1 , 
        const LHCb::Track* track2 ,
        const bool         apply  ) const ;
      /** make the vertex out of two tracks 
       *  @param track1 (INPUT) the first  track 
       *  @param track1 (INPUT) the second track 
       *  @param apply  (INPUT) apply the track cuts 
       *  @return the vertex (properly registered) 
       */
      const LHCb::RecVertex* makeVertex 
      ( const LHCb::Track* track1 , 
        const LHCb::Track* track2 ,
        double&            doca   , 
        double&            chi2   , 
        const bool         apply  ) const ;
      /** check the consistency of two tracks with "vertex"  
       *  @param track1 (INPUT) the first  track 
       *  @param track1 (INPUT) the second track 
       *  @param apply  (INPUT) apply the track cuts 
       *  @return the vertex (properly registered) 
       */
      bool checkVertex 
      ( const LHCb::Track* track1 , 
        const LHCb::Track* track2 , 
        const bool         apply  ) const ;
      // ======================================================================
    private:      
      // ======================================================================
      /// the default constructor is disabled 
      VxMakerBase () ;                   // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the configuration
      const    LoKi::Hlt1::VxMakerConf& config () const { return m_conf     ; }
      /// cast 
      operator const LoKi::Hlt1::VxMakerConf&  () const { return config ()  ; }
      // ======================================================================
    public: // delegate functions 
      // ======================================================================
      // apply cuts 
      // ======================================================================
      /// apply cut for the first track 
      bool track1cut  ( const LHCb::Track*      t ) const 
      { return m_conf.track1cut ( t ) ; }
      /// apply cut for the first track 
      bool track2cut  ( const LHCb::Track*      t ) const 
      { return m_conf.track2cut ( t ) ; }
      /// apply cut for the vertex 
      bool vxcut      ( const LHCb::VertexBase* v ) const
      { return m_conf.vxcut     ( v ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double docamax () const { return m_conf.docamax () ; }
      double chi2max () const { return m_conf.chi2max () ; }        
      // ======================================================================
    private:
      // ======================================================================
      /// the actual condifguration 
      LoKi::Hlt1::VxMakerConf m_conf ;             // the actual condifguration 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VxMaker 
     *  the simple "vertex-maker" from di-tracks 
     *  @see LoKi::Cuts::TC_VXMAKE2
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     */
    class GAUDI_API VxMaker
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe 
      , public LoKi::Hlt1::VxMakerBase 
    {
    public:
      // ======================================================================
      /// constructor 
      VxMaker 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// MANTATOY: virtual destructor 
      virtual ~VxMaker() ;      
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VxMaker* clone() const ; 
      /// MANDATORY: the only essential method
      virtual  result_type   operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual  std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      VxMaker () ;                       // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// the output selection 
      const std::string& output () const { return m_sink.output() ; }
      // ======================================================================
    protected :
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::Sink               m_sink    ;  //                      'sink'
      // ======================================================================
    };
    // ========================================================================
    /** @class VxMaker2 
     *  the simple "vertex-maker" from two track selections 
     *  @see LoKi::Cuts::TC_VXMAKE2
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     */
    class GAUDI_API VxMaker2 : public LoKi::Hlt1::VxMaker 
    {
    private:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      VxMaker2 
      ( const std::string&             output  ,   // output selection name/key 
        const Source&                  tracks2 ,   // tracks to be matched with 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// constructor 
      VxMaker2 
      ( const std::string&             output  ,   // output selection name/key 
        const std::string&             tracks2 ,   // tracks to be matched with 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// MANTATOY: virtual destructor 
      virtual ~VxMaker2 () ;      
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VxMaker2* clone() const ; 
      /// MANDATORY: the only essential method
      virtual  result_type   operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual  std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      VxMaker2 () ;                       // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// the second source 
      const Source&      source () const { return m_source2.func() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// souce of tracks to be matched 
      LoKi::Assignable<Source>::Type m_source2 ; //            source of tracks 
      // ======================================================================
    };
    // ========================================================================
    /** @class VxMaker2 
     *  the simple "vertex-maker" as source 
     *  @see LoKi::Cuts::TC_VXMAKE3
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2010-12-04 
     */
    class GAUDI_API VxMaker3
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Source 
    {
    private:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      VxMaker3 
      ( const std::string&             output  ,   // output selection name/key 
        const Source&                  tracks1 ,   //       the first selection 
        const Source&                  tracks2 ,   //       the first selection 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// MANTATORY: virtual destructor 
      virtual ~VxMaker3 () ;      
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VxMaker3* clone() const ; 
      /// MANDATORY: the only essential method
      virtual  result_type    operator() ( /* argument a */ ) const ;
      /// OPTIONAL: nice printout 
      virtual  std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// the first source 
      const Source& source1 () const { return m_source1.func   () ; }
      /// the second source 
      const Source& source2 () const { return m_vxmaker.source () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      VxMaker3 () ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// souce of tracks to be matched 
      LoKi::Assignable<Source>::Type  m_source1 ; //           source of tracks 
      /// the actual vertex maker 
      LoKi::Hlt1::VxMaker2            m_vxmaker ;
      // ======================================================================
    };
    // ========================================================================
    /** @class DiTrackMaker 
     *  the simple "di-tracks-maker" from two separate selections 
     *  @see LoKi::Cuts::TC_DITRACKS
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     */
    class GAUDI_API DiTrackMaker
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe 
      , public LoKi::Hlt1::VxMakerBase 
    {
    private:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      DiTrackMaker
      ( const std::string&             output  ,   // output selection name/key 
        const Source&                  tracks2 ,   // tracks to be matched with 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// constructor 
      DiTrackMaker 
      ( const std::string&             output  ,   // output selection name/key 
        const std::string&             tracks2 ,   // tracks to be matched with 
        const LoKi::Hlt1::VxMakerConf& config  ) ; //        tool configuration 
      /// MANTATOY: virtual destructor 
      virtual ~DiTrackMaker() ;      
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DiTrackMaker* clone() const ; 
      /// MANDATORY: the only essential method
      virtual  result_type   operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual  std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DiTrackMaker () ;                  // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// the second source 
      const Source&      source () const { return m_source2.func() ; }
      // ======================================================================
      /// the output selection 
      const std::string& output () const { return m_sink.output() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// souce of tracks to be matched 
      LoKi::Assignable<Source>::Type m_source2 ; //            source of tracks 
      // ======================================================================
    protected :
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::Sink               m_sink    ;  //                      'sink'
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef TC_VXMAKE 
     *  vertex maker from di-tracks 
     *  
     *  @see LHCb::Track
     *  @see LHCb::RecVertex 
     *  @see Hlt::MultiTrack
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */
    typedef LoKi::Hlt1::VxMaker                                     TC_VXMAKE ;
    // ========================================================================
    /** @typedef TC_VXMAKE2
     *  vertex maker from two track selections
     *  
     *  @see LHCb::Track
     *  @see LHCb::RecVertex 
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */
    typedef LoKi::Hlt1::VxMaker2                                   TC_VXMAKE2 ;
    // ========================================================================
    /** @typedef TC_DITRACKS
     *  di-track maker from two separate track selections 
     *  
     *  @see LHCb::Track
     *  @see LHCb::RecVertex 
     *  @see Hlt::MultiTrack
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */
    typedef LoKi::Hlt1::DiTrackMaker                              TC_DITRACKS ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VXMAKER_H
// ============================================================================
