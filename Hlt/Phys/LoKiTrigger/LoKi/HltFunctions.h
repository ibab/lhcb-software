// $Id: HltFunctions.h,v 1.4 2007-08-19 13:42:59 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTFUNCTIONS_H 
#define LOKI_HLTFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/TrackTypes.h"
#include "LoKi/Interface.h"
#include "LoKi/HltBinders.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/ITrackMatch.h"
// ============================================================================
namespace LoKi 
{
  namespace Tracks
  {  
    // ========================================================================
    /** @class DistanceOfClosestApproach 
     *  Evaluate the distance of the closest approach between two tracks.
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::closestDistanceMod 
     *  @see LoKi::Cuts::TrDOCA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */
    class DistanceOfClosestApproach 
      : public LoKi::Function<LoKi::TrackTypes::TrackPair>
    {
    public:
      /// MANDATORY: virtual destrcutor  
      virtual ~DistanceOfClosestApproach () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DistanceOfClosestApproach* clone() const 
      { return new DistanceOfClosestApproach(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TTrDOCA" ; }
    } ;
    // ========================================================================
    /** @class DeltaP
     *  Evaluator of "DeltaP"
     *
     *  The class and implementation comes from  HltBase /HltFunctions.h
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Cuts::TrDP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class DeltaP : public LoKi::Function<LoKi::TrackTypes::TrackPair>
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~DeltaP() {} ;
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DeltaP* clone() const { return new DeltaP(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TTrDP" ; }      
    } ;
    // ========================================================================
    /** @class DeltaE
     *  Evaluator of "DeltaE"
     *
     *  The class and implementation comes from  HltBase/HltFunctions.h
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Cuts::TrDE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class DeltaE : public LoKi::Function<LHCb::Track>
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~DeltaE() {} ;
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DeltaE* clone() const { return new DeltaE(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrDE" ; }      
    } ;
    // ========================================================================
    /** @class TrackMatch
     *  "Match" twho tracks and return the "distance"
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see LoKi::Cuts::TTrMATCH
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class TrackMatch : public LoKi::Function<LoKi::TrackTypes::TrackPair>
    {
    public:
      /// constructor from the tool 
      TrackMatch ( ITrackMatch* tool ) ;
      /// constructor from the tool 
      TrackMatch ( const LoKi::Interface<ITrackMatch>& tool ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrackMatch() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual TrackMatch* clone() const { return new TrackMatch(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TTrMATCH" ; }      
    public:
      /// cast to the tool
      operator const LoKi::Interface<ITrackMatch>& () const { return m_tool ;}
    private:
      // no default constructor 
      TrackMatch() ; ///< no default constructor
    private:
      // the tool itself 
      LoKi::Interface<ITrackMatch> m_tool ; ///< the tool itself 
    } ;  
    // ========================================================================
    /** @class RadialIP
     *  Evaluator of "radial" impact parameter between track and vertex
     *  @attention track needs to be of Velo RZ type
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see HltUtils::rImpactParameter
     *  @see LoKi::Cuts::TrVRIP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class RadialIP : public LoKi::Function<LoKi::TrackTypes::TrackVertexPair>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~RadialIP() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  RadialIP* clone() const { return new RadialIP(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrVRIP" ; }      
    } ;
    // ========================================================================
    /** @class ImpactParameter 
     *  Evaluator of impact parameter between track and vertex
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see HltUtils::impactParameter
     *  @see LoKi::Cuts::TrVIP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class ImpactParameter 
      : public LoKi::Function<LoKi::TrackTypes::TrackVertexPair>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ImpactParameter() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  ImpactParameter* clone() const
      { return new ImpactParameter (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TrVIP" ; }      
    } ;
    // ========================================================================
    /** @class MatchIDsFraction 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::matchIDsFraction
     *  @see LoKi::Cuts::TTrMATCHIDS
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class MatchIDsFraction
      : public LoKi::Function<LoKi::TrackTypes::TrackPair>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~MatchIDsFraction() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MatchIDsFraction* clone() const 
      { return new MatchIDsFraction (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TTrMATCHIDS" ; }
    } ;  
    // ========================================================================
    /** @class DeltaAngle 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::deltaAngle
     *  @see LoKi::Cuts::TTrDA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class DeltaAngle 
      : public LoKi::Function<LoKi::TrackTypes::TrackPair>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~DeltaAngle ()  {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DeltaAngle* clone() const { return new DeltaAngle (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "TTrDA" ; }
    } ;    
  } // end of namespace LoKi::Tracks
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTFUNCTIONS_H
// ============================================================================
