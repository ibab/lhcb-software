// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES12_H 
#define LOKI_PARTICLES12_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LHCb { class ProtoParticle ; }
namespace LHCb { class Track         ; }
namespace LHCb { class MuonPID       ; }
// ============================================================================
/** @file
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
 *   with the campain of Dr.O.Callot et al.: 
 *    ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *     
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-23 
 *
 *                   $Revision$
 *  LastModification $Date$
 *                by $Author$ 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {   
    // ========================================================================
    /** @class ProtoHasInfo
     *  The trivial predicate whcii evaluated for true 
     *  if the protoparticle "hasInfo".
     *  
     *  @see LHCb::Particle
     *  @see LHCb::Particle::proto
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::hasInfo
     *
     *  @see LoKi::Cuts::PPHASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class GAUDI_API ProtoHasInfo 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor form the index in ProtoParticle::ExtraInfo
      ProtoHasInfo ( int index ) ;
      /// copy constructor 
      ProtoHasInfo ( const ProtoHasInfo& right ) ;
      /// MANDAROTY: virtual destructor 
      virtual ~ProtoHasInfo() {}
      /// clone method (mandatory!)
      virtual  ProtoHasInfo* clone() const { return new ProtoHasInfo(*this) ; }    
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // default contructor is disabled 
      ProtoHasInfo() ;
      // ======================================================================
    private :
      // ======================================================================
      /// index in ProtoParticle::ExtraInfo 
      int m_info ;                        // index in ProtoParticle::ExtraInfo
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class ProtoInfo
     *  Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particle::proto
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::info
     *  @see LoKi::Cuts::PPINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API ProtoInfo
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       *  @param bad bad value to be retured for invalid particle 
       */
      ProtoInfo 
      ( const int    key , 
        const double def , 
        const double bad ) ;
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for info 
       */
      ProtoInfo 
      ( const int    key , 
        const double def ) ;
      /// copy constructor 
      ProtoInfo ( const ProtoInfo& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProtoInfo() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoInfo* clone() const { return new ProtoInfo(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      ProtoInfo();
      // ======================================================================
    private:
      // ======================================================================
      /// index in ProtoParticle::ExtraInfo
      int    m_key ; // index in ProtoParticle::ExtraInfo
      /// value to be returned for invalid particle and protoparticle 
      double m_def ;
      /// default value for missing infomration 
      double m_bad ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class ProtoHasRichPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::richPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::richPID 
     *  @see LHCb::RichPID 
     *  @see LoKi::Cuts::PPHASRICH 
     *  @see LoKi::Cuts::HASRICH 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API ProtoHasRichPID
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      ProtoHasRichPID() { }
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasRichPID() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasRichPID* clone() const 
      { return new ProtoHasRichPID(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class ProtoHasMuonPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::muonPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::muonPID 
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::PPHASMUON
     *  @see LoKi::Cuts::HASMUON
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API ProtoHasMuonPID 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      ProtoHasMuonPID() { }
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasMuonPID() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasMuonPID* clone() const 
      { return new ProtoHasMuonPID(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class ProtoHasCaloHypos
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::calo() 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::calo
     *  @see LHCb::CaloHypo
     *  @see LoKi::Cuts::PPHASCALO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API ProtoHasCaloHypos 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      ProtoHasCaloHypos() { }
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasCaloHypos() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasCaloHypos* clone() const 
      { return new ProtoHasCaloHypos(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class IsMuon
     *  The trivial predicate whith returns LHCb::MuonPID::isMuon
     *
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::ISMUON
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-02-25
     */
    class GAUDI_API IsMuon
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsMuon() { }
      /// MANDATORY: virtual destructor 
      virtual ~IsMuon() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsMuon* clone() const { return new IsMuon(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// "extract" muonPID form the particle 
      const LHCb::MuonPID* muonPID ( const LHCb::Particle* p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// helper object to decode muon-PID-status 
      mutable LHCb::MuonPID m_pid ;  // helper object to decode muon-PID-status 
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class IsMuonLoose
     *  The trivial predicate whith returns LHCb::MuonPID::isMuonLoose
     *
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::ISMUONLOOSE
     *  @see LoKi::Cuts::ISLOOSEMUON
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-26
     */
    class GAUDI_API IsMuonLoose : public IsMuon 
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsMuonLoose() { }
      /// MANDATORY: virtual destructor 
      virtual ~IsMuonLoose() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsMuonLoose* clone() const { return new IsMuonLoose(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class IsMuonTight
     *  The trivial predicate whith returns LHCb::MuonPID::isMuonTight
     *
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::ISMUONTIGHT
     *  @see LoKi::Cuts::ISTIGHTMUON
     *
     *  @author Ricardo VAZQUEZ GOMEZ rvazquez@cern.ch
     *  @date 2015-07-16
     */
    class GAUDI_API IsMuonTight : public IsMuon
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsMuonTight() { }
      /// MANDATORY: virtual destructor 
      virtual ~IsMuonTight() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsMuonTight* clone() const { return new IsMuonTight(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class InMuonAcceptance
     *  Simple functor that checks if the particle is in muon acceptance
     *  @thanks Gaia Lanfranchi for kind help!
     *  @see LHcb::MuonPID::InAcceptance 
     *  @see LoKi::Cuts::INMUON
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-08
     */
    class GAUDI_API InMuonAcceptance : public IsMuonLoose 
    {
    public:
      // ======================================================================
      /// Default Constructor
      InMuonAcceptance() { }
      /// MANDATORY: virtual destructor 
      virtual ~InMuonAcceptance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  InMuonAcceptance* clone () const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TrackHasInfo
     *  The trivial predicate whcii evaluated for true 
     *  if the tarck "hasInfo".
     *  
     *  @see LHCb::Particle
     *
     *  @see LoKi::Cuts::THASINFO 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-02-11 
     */
    class GAUDI_API TrackHasInfo 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the index in Track::ExtraInfo
      TrackHasInfo ( int index ) ;
      /// MANDAROTY: virtual destructor 
      virtual ~TrackHasInfo() {}
      /// clone method (mandatory!)
      virtual  TrackHasInfo* clone() const { return new TrackHasInfo(*this) ; }    
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // default contructor is disabled 
      TrackHasInfo() ;
      // ======================================================================
    private :
      // ======================================================================
      /// index in Track::ExtraInfo 
      int m_info ; // index in Track::ExtraInfo
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TrackInfo
     *  Trivial function which evaluates LHCb::Track::info
     *  
     *  It relies on the method LHCb::Track::info
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particle::proto
     *  @see LHCb::Track
     *  @see LHCb::Track::info
     *  @see LoKi::Cuts::TINFO 
     *  @see LoKi::Cuts::TRINFO 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-02-11
     */
    class GAUDI_API TrackInfo
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       *  @param bad bad value to be retured for invalid particle 
       */
      TrackInfo 
      ( const int    key , 
        const double def , 
        const double bad ) ;
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for info 
       */
      TrackInfo 
      ( const int    key , 
        const double def ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrackInfo() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrackInfo* clone() const { return new TrackInfo(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackInfo();  // the default constructor is disabled 
      // ======================================================================
    private:
      // =======================================================================
      /// index in Track::ExtraInfo
      int    m_key ; // index in ProtoParticle::ExtraInfo
      /// value to be returned for invalid particle and protoparticle 
      double m_def ; // value to be returned for invalid objects
      /// default value for missing information
      double m_bad ;  // default value for missing information
      // ======================================================================
    } ;
    // ========================================================================
    /** @class NShared 
     *  Get number of shared muon hits 
     *  @see LoKi::Cuts::NSHAREDMU 
     *  @see LHCb::MuonPID::nShared 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-10
     */
    class NShared
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      NShared() { }
      /// MANDATORY: virtual destructor 
      virtual ~NShared () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NShared* clone() const ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printtout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
     };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================
