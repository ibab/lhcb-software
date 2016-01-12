// $Id$
// ============================================================================
#ifndef LOKI_TRACKS_H 
#define LOKI_TRACKS_H 1
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
// ============================================================================
// Track Interfaces 
// ============================================================================
#include "TrackInterfaces/ITrackSelector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ExtraInfo.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Interface.h"
#include "LoKi/BeamSpot.h"
// ============================================================================
#include "KalmanFilter/FastVertex.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::Tracks Tracks.h LoKi/Tracks.h
   *  Namespace with few basic "track"-functors 
   *  @see LHCb::Track
   *
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
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
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-06-08
   */
  namespace Tracks
  {
    // ========================================================================
    /** @class Key
     *  simple evaluator of the "key"
     *  @see LoKi::Cuts::TrKEY
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API Key : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Key() { }
      /// MANDATORY: virtual destructor  
      virtual ~Key () ;
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Key* clone() const ;
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const  ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class InTES
     *  Simple predicate which check is teh obejct registered in TES
     *  @see LoKi::Cuts::TrINTES
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API InTES : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      InTES() { }
      /// MANDATORY: virtual destructor  
      virtual ~InTES () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  InTES* clone() const { return new InTES(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrINTES" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Charge
     *  simple evaluator of the charge of the track 
     *  @see LoKi::Cuts::TrQ
     *  @author Vanya BELYAEV ibelyaev@physiocs.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API Charge : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Charge() { }
      /// MANDATORY: virtual destructor  
      virtual ~Charge () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Charge* clone() const { return new Charge(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrQ" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TransverseMomentum 
     *  simple evaluator of transverse momentum of the track
     *  @see LoKi::Cuts::TrPT
     *  @author Vanya BELYAEV ibelyaev@physiocs.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API TransverseMomentum 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      TransverseMomentum() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~TransverseMomentum() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  TransverseMomentum* clone() const 
      { return new TransverseMomentum(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPT" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Momentum 
     *  @see LoKi::Cuts::TrP 
     *  simple evaluator of momentum of the track
     *  @author Vanya BELYAEV ibelyaev@physiocs.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API Momentum : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Momentum() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~Momentum() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Momentum* clone() const { return new Momentum(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Phi
     *  @see LoKi::Cuts::TrPHI
     *  @see LHCb::Track::phi
     *  simple evaluator of phi angle for the track 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2015-01-30
     */  
    class GAUDI_API Phi : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Phi() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~Phi() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Phi* clone() const { return new Phi(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPHI" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Eta
     *  @see LoKi::Cuts::TrETA
     *  @see LHCb::Track::pseudoRapidity
     *  simple evaluator of pseudorapidity for the track 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2015-01-30
     */  
    class GAUDI_API Eta : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Eta () { }
      /// MANDATORY: virtual destrcutor  
      virtual ~Eta () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Eta* clone() const { return new Eta(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrETA" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumX 
     *  @see LoKi::Cuts::TrPX 
     *  simple evaluator of momentum of the track
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date   2011-03-18
     */  
    class GAUDI_API MomentumX : public LoKi::Tracks::Momentum
    {
    public:
      // ======================================================================
      /// Default Constructor
      MomentumX() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~MomentumX () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MomentumX* clone() const { return new MomentumX(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPX" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumY
     *  @see LoKi::Cuts::TrPY
     *  simple evaluator of momentum of the track
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date   2011-03-18
     */  
    class GAUDI_API MomentumY : public LoKi::Tracks::Momentum
    {
    public:
      // ======================================================================
      /// Default Constructor
      MomentumY() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~MomentumY () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MomentumY* clone() const { return new MomentumY(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPY" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumZ
     *  @see LoKi::Cuts::TrPZ
     *  simple evaluator of momentum of the track
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date   2011-03-18
     */  
    class GAUDI_API MomentumZ : public LoKi::Tracks::Momentum
    {
    public:
      // ======================================================================
      /// Default Constructor
      MomentumZ() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~MomentumZ () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MomentumZ* clone() const { return new MomentumZ(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPZ" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CheckFlag 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Cuts::TrBACKWARD
     *  @see LoKi::Cuts::TrINVALID
     *  @see LoKi::Cuts::TrCLONE
     *  @see LoKi::Cuts::TrUSED
     *  @see LoKi::Cuts::TrIPSELECTED
     *  @see LoKi::Cuts::TrPIDSELECTED
     *  @see LoKi::Cuts::TrSELECTED
     *  @see LoKi::Cuts::TrL0CANDIDATE
     *  @simple predicate to check the flag 
     *  @author Vanya BELYAEV ibelyaev@physiocs.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API CheckFlag 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor form the flag ; 
      CheckFlag ( LHCb::Track::Flags flag ) ;
      /// MANDATORY: virtual destrcutor  
      virtual ~CheckFlag() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  CheckFlag* clone() const { return new CheckFlag (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled
      CheckFlag() ; ///< no default contructor
      // ======================================================================
    private:
      // ======================================================================
      // the flag to be checked:
      LHCb::Track::Flags m_flag ; ///< the flag to be checked:
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class Selector
     *  Simple class GAUDI_API to use "track-selector"
     *  @see ITrackSelector
     *  @see LoKi:Cuts::TrSELECTOR
     *  @author Vanya BELYAEV ibelayev@physics.syr.edu
     *  @date 2007-06-10
     */
    class GAUDI_API Selector 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate
    {
    public :
      // ======================================================================
      /// constructor form the tool 
      Selector ( const ITrackSelector*  tool ) ;
      /// constructor form the tool 
      Selector ( const LoKi::Interface<ITrackSelector>& tool ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Selector() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Selector* clone() const { return new Selector(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// conversion operator to the tool
      operator const LoKi::Interface<ITrackSelector>&() const { return m_tool ;}    
      const LoKi::Interface<ITrackSelector>& selector() const { return m_tool ;}    
      // ======================================================================
      /// the only one essential method 
      bool eval ( const LHCb::Track* t ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// the default constructor is protected
      Selector () ;                     // the default constructor is protected
      /// set new selector tool 
      void setSelector ( const ITrackSelector* selector ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// the tool itself 
      mutable LoKi::Interface<ITrackSelector> m_tool ;            // the tool itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Filter
     *  Simple class GAUDI_API to use "track-selector"
     *  @see ITrackSelector
     *  @see LoKi:Cuts::TrFILTER
     *  @author Vanya BELYAEV ibelayev@physics.syr.edu
     *  @date 2007-06-10
     */
    class GAUDI_API Filter : public Selector 
    {
    public :
      // ======================================================================
      /// constructor form the tool name 
      Filter ( const std::string& nick ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Filter () ;
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Filter* clone() const { return new Filter(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Filter () ;                        // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// set new selector tool 
      void getSelector () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the tool typename
      std::string m_nick ;                                 // the tool typename
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::Track::hasInfo
     *  function
     *
     *  It relies on the method LoKi::Info::hasInfo
     *
     *  @see LHCb::Track
     *  @see LoKi::Cuts::TrHASINFO 
     *  @see LoKi::ExtraInfo::CheckInfo
     *  @see LoKi::ExtraInfo::hasInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API HasInfo : public LoKi::ExtraInfo::CheckInfo<const LHCb::Track*>
    {
    public:
      // ======================================================================
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// destructor 
      virtual ~HasInfo(){} ;
      /// clone method (mandatory!)
      virtual HasInfo* clone() const { return new HasInfo(*this) ; }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      HasInfo();
      // ======================================================================
    };
    // ========================================================================    
    /** @class Info
     *  Trivial function which evaluates LHCb::Track::info
     *  
     *  It relies on the method LoKi::ExtraInfo::info 
     *
     *  @see LHCb::Track
     *  @see LoKi::Cuts::TrINFO 
     *  @see LoKi::ExtraInfo::GeInfo
     *  @see LoKi::ExtraInfo::info
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API Info : public LoKi::ExtraInfo::GetInfo<const LHCb::Track*>
    {
    public:
      // ======================================================================
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for missing key/invalid object 
       */
      Info ( const int    key , const double def ) ;
      /// destructor 
      virtual ~Info(){};
      /// clone method (mandatory!)
      virtual Info* clone() const { return new Info(*this); }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      Info();
      // ======================================================================
    };
    // ========================================================================    
    /** @class SmartInfo
     *  Trivial function which:
     *    - checks the presence of informnation in LHCb::Track::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  
     *  @see LHCb::Track
     *  @see LoKi::Cuts::TrSINFO 
     *  @see LoKi::ExtraInfo::GetSmartInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::ExtraInfo::hasInfo
     *  @see LoKi::ExtraInfo::setInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API SmartInfo 
      : public LoKi::ExtraInfo::GetSmartInfo<const LHCb::Track*>
    {
    public:
      // ======================================================================
      /** constructor from fuction, key and update-flag
       *  @param index the key in LHCb::Track::extraInfo table 
       *  @param fun functionto be evaluated for missing keys 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const int                                                index          , 
        const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun            , 
        const bool                                               update = false ) ;
      /// destructor 
      virtual ~SmartInfo(){};
      /// clone method (mandatory!)
      virtual  SmartInfo* clone() const { return new SmartInfo(*this); }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      SmartInfo();
      // ======================================================================
    };
    // ========================================================================
    /** @class Chi2
     *  simple evaluator of the LHcb::Track::chi2
     *  @see LoKi::Cuts::TrCHI2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API Chi2 : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Chi2() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~Chi2 () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Chi2* clone() const { return new Chi2(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrCHI2" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Chi2PerDoF
     *  simple evaluator of the LHcb::Track::chi2PerDoF
     *  @see LoKi::Cuts::TrCHI2PDOF
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2007-06-08
     */  
    class GAUDI_API Chi2PerDoF : public Chi2
    {
    public:
      // ======================================================================
      /// Default Constructor
      Chi2PerDoF() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~Chi2PerDoF () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Chi2PerDoF* clone() const { return new Chi2PerDoF(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrCHI2PDOF" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ProbChi2
     *  simple evaluator of the LHCb::Track::probChi2
     *  @see LoKi::Cuts::TrPROBCHI2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API ProbChi2 : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      ProbChi2() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~ProbChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  ProbChi2* clone() const { return new ProbChi2(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrPROBCHI2" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GhostProb
     *  simple evaluator of the LHCb::Track::ghostProbability
     *  @see LoKi::Cuts::TrGHOSTPROB
     *  @author Sascha Stahl sascha.stahl@cern.ch
     *  @date   2016-01-12
     */  
    class GAUDI_API GhostProb : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      GhostProb() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~GhostProb () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  GhostProb* clone() const { return new GhostProb(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrGHOSTPROB" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasStateAt 
     *  Simple predicate which evaluates LHCb::Track::HasStateAt
     *  @see LoKi::Cuts::TrHASSTATE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API HasStateAt : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor
      HasStateAt ( const LHCb::State::Location& loc ) ;
      /// MANDATORY: virtual destructor 
      virtual ~HasStateAt() {}  ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasStateAt* clone() const { return new HasStateAt(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrHASSTATE(" << loc () << ")" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the location 
      int loc () const { return m_loc ; }
      // ======================================================================
    private:
      // ======================================================================
      // defautl constructor is disabled 
      HasStateAt() ; ///< no default constructor 
      // ======================================================================
    private:
      // ======================================================================
      /// the location 
      LHCb::State::Location m_loc ;                             // the location 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class IsOnTrack
     *  Simple predicate which evaluates LHCb::Track::isOnTrack
     *  @see LoKi::Cuts::TrISONTRACK
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API IsOnTrack : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate
    {
    public:
      // ======================================================================
      IsOnTrack ( const LHCb::LHCbID& id ) ;
      /// MANDATORY: virtual destructor 
      virtual ~IsOnTrack () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsOnTrack* clone() const { return new IsOnTrack(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrIsOnTRACK(LHCb.LHCbID(" << m_id.lhcbID() << "))" ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      IsOnTrack() ;                                    // no default consructor 
      // ======================================================================
    private:
      // ======================================================================
      LHCb::LHCbID m_id ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Type
     *  simple evaluator of the LHCb::Track::type
     *  @see LHCb::Track::type 
     *  @see LHCb::Track::Types
     *  @see LoKi::Cuts::TrTYPE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */  
    class GAUDI_API Type : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Type() { }
      /// MANDATORY: virtual destructor  
      virtual ~Type() {} 
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  Type* clone() const { return new Type(*this) ; }
      /// mandatory: the only one essential method 
      virtual result_type operator() ( argument t ) const ; 
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrTYPE" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class StateZ 
     *  check Z-position for the given state 
     *  @see LoKi::Cuts::TrSTATEZ 
     *  @see LoKi::Cuts::TrFIRSTHITZ
     *  @see LHCb::State::Location
     *  @author Vanya Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    class GAUDI_API StateZ 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// constructor with the state indicator 
      StateZ 
      ( const LHCb::State::Location location = LHCb::State::FirstMeasurement ) ;
      /// constructor with the state indicator & bad value  
      StateZ ( const LHCb::State::Location location , 
               const double       bad      ) ;
      /// MANDATORY: virtual destructor 
      virtual ~StateZ() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  StateZ* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the string representation of the state 
      const std::string& state() const ; // the string representation 
      // ======================================================================
    private:
      // ======================================================================
      /// the state itself 
      LHCb::State::Location m_state ;                       // the state itself 
      /// the bad value 
      double       m_bad   ;                                //    the bad value
      /// the state 
      mutable std::string m__state ;                        //        the state 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Cov2 
     *  Get the element of covarinace element for the track state 
     *  @see Tr_COV2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    class GAUDI_API Cov2
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from indices
      Cov2 ( const unsigned short i , 
             const unsigned short j ) ;
      /// constructor from indices & state location 
      Cov2 ( const LHCb::State::Location location , 
             const unsigned short        i        , 
             const unsigned short        j        ) ;
      /// constructor from the indices and Z-position: 
      Cov2 ( const double                z        , 
             const unsigned short        i        , 
             const unsigned short        j        ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Cov2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Cov2* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Cov2 () ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      enum _Case {
        _First    , 
        _Location ,
        _Z     
      } ;
      // ======================================================================
    private:
      // ======================================================================
      _Case                 m_case ;
      double                m_z    ;
      LHCb::State::Location m_loc  ;
      unsigned  short       m_i    ;
      unsigned  short       m_j    ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class NVeloMissed
     *  @see Hlt::MissedVeloHits 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date 2011-01-28
     */
    class GAUDI_API NVeloMissed 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      NVeloMissed() { }
      /// MANDATORY: virtual destructor 
      virtual ~NVeloMissed() ;
      /// MANDATORY: clone method (virtual constructor) 
      virtual  NVeloMissed* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrNVELOMISS" ; }
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class NTHits
     *  Count number of effective T-hits : 2x#IT + #OT
     *  @see LoKi::Cuts::TrNTHITS
     *  @see LoKi::Cuts::TrTNORMIDC
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date 2011-02-02
     */
    class GAUDI_API NTHits
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      NTHits() { }
      /// MANDATORY: virtual destructor 
      virtual ~NTHits () ;
      /// MANDATORY: clone method (virtual constructor) 
      virtual  NTHits* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class HasT 
     *  Check if track is of a type that goes thro T stations
     *  @see LHCb::Track::hasT
     *  @see LoKi::Cuts::TrHAST
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-03-18
     */
    class GAUDI_API HasT 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Predicate 
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasT() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~HasT() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  HasT* clone() const { return new HasT ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasVelo
     *  Check if track is of a type that goes thro Velo
     *  @see LHCb::Track::hasVelo
     *  @see LoKi::Cuts::TrHASVELO
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-03-18
     */
    class GAUDI_API HasVelo : public LoKi::Tracks::HasT 
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasVelo() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~HasVelo() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  HasVelo* clone() const { return new HasVelo ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasTT
     *  Check if track is of a type that goes thro TT
     *  @see LHCb::Track::hasTT
     *  @see LoKi::Cuts::TrHASTT
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-03-18
     */
    class GAUDI_API HasTT : public LoKi::Tracks::HasT 
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasTT() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~HasTT () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  HasTT* clone() const { return new HasTT ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /**
     *  @class FastDOCAToBeamLine
     *  use TTrDOCA and BEAMSPOT to evaluate the closest distance of a track to
     *  the beam line, similar to BEAMSPOTRHO
     *  @see LoKi::BeamSpot
     *  @see LoKi::Cuts::VX_BEAMSPOTRHO
     *  @author Pieter David pieter.david@cern.ch
     *  @date 2012-02-24
     */
    class GAUDI_API FastDOCAToBeamLine
      : LoKi::BeamSpot
      , public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // =====================================================================
      /// Constructor from resolver bound
      FastDOCAToBeamLine ( const double       bound    ) ;
      /// Constructor from resolved bound and condition name
      FastDOCAToBeamLine ( const double       bound    ,
                           const std::string& condname ) ;
      /// MANDATORY: virtual destructor
      virtual ~FastDOCAToBeamLine() {}
      /// update the condition
      StatusCode     updateCondition ();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FastDOCAToBeamLine* clone() const
      { return new FastDOCAToBeamLine( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "Tr_FASTDOCATOBEAMLINE"; }
    private:
      /// beamline
      LoKi::FastVertex::Line m_beamLine;
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKS_H
// ============================================================================
