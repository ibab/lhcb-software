// $Id$
// ============================================================================
#ifndef LOKI_HLTFUNCTIONS_H 
#define LOKI_HLTFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/TriggerTypes.h"
#include "LoKi/Interface.h"
#include "LoKi/HltBinders.h"
#include "LoKi/OldTypes.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/IBiFunctionTool.h"
// ============================================================================
/** @file
 *  Implementation for Hlt-specific track-(bi)functions 
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
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 *
 *  @todo HltFunctions.h check if we need all this stuff..
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Tracks
  {  
    // ========================================================================    
#ifdef __INTEL_COMPILER
#pragma warning(disable:654) // overloaded virtual function is only partially overridden
#pragma warning(push)
#endif
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
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function
    {
    public:
      /// Default Constructor
      DistanceOfClosestApproach() { }
      /// MANDATORY: virtual destrcutor  
      virtual ~DistanceOfClosestApproach () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DistanceOfClosestApproach* clone() const 
      { return new DistanceOfClosestApproach(*this) ; }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function::operator();
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TTrDOCA" ; }
    } ;
    // ========================================================================
    /** @class DeltaP
     *  Evaluator of "DeltaP"
     *
     *  The class and implementation comes from  HltBase/HltFunctions.h
     *
     *  The actual lines are copied from the package Hlt/HltBase
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Cuts::TrDP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */
    class DeltaP
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function
    {
    public:
      /// Default constructor
      DeltaP() { }
      /// MANDATORY: virtual destructor
      ~DeltaP() override = default;
      /// MANDATORY: clone method ("virtual constructor")
      DeltaP* clone() const override
      { return new DeltaP(*this); }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream( std::ostream& s ) const override
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
    class DeltaE
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// Default constructor
      DeltaE() { }
      /// MANDATORY: virtual destructor
      ~DeltaE() override = default;
      /// MANDATORY: clone method ("virtual constructor")
      DeltaE* clone() const override
      { return new DeltaE(*this); }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<const LHCb::Track*>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream( std::ostream& s ) const override
      { return s << "TrDE" ; }
      // ======================================================================
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
    class TrackMatch
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function
    {
    public:
      /// constructor from the tool
      TrackMatch ( ITrackBiFunctionTool* tool ) ;
      /// constructor from the tool
      TrackMatch ( const LoKi::Interface<ITrackBiFunctionTool>& tool ) ;
      /// MANDATORY: virtual destructor
      ~TrackMatch() override = default;
      /// MANDATORY: clone method ("virtual constructor")
      TrackMatch* clone() const override
      { return new TrackMatch(*this) ; }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream( std::ostream& s ) const override
      { return s << "TTrMATCH" ; }
    public:
      /// cast to the tool
      operator const LoKi::Interface<ITrackBiFunctionTool>& () const { return m_tool ;}
    private:
      // no default constructor
      TrackMatch() ; ///< no default constructor
    private:
      // the tool itself
      LoKi::Interface<ITrackBiFunctionTool> m_tool ; ///< the tool itself
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
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackVertexPair>::Function
    {
    public:
      /// Default constructor
      ImpactParameter() = default;
      /// MANDATORY: virtual destructor
      ~ImpactParameter() override = default;
      /// MANDATORY: clone method ("virtual constructor")
      ImpactParameter* clone() const override
      { return new ImpactParameter (*this) ; }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackVertexPair>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream( std::ostream& s ) const override
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
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function
    {
    public:
      /// Default constructor
      MatchIDsFraction() { }
      /// MANDATORY: virtual destructor
      ~MatchIDsFraction() override = default;
      /// MANDATORY: clone method ("virtual constructor")
      MatchIDsFraction* clone() const override
      { return new MatchIDsFraction (*this) ; }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream( std::ostream& s ) const override
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
      : public LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function
    {
    public:
      /// Default constructor
      DeltaAngle() { }
      /// MANDATORY: virtual destructor
      ~DeltaAngle () override = default;
      /// MANDATORY: clone method ("virtual constructor")
      DeltaAngle* clone() const override
      { return new DeltaAngle (*this) ; }
      /// MANDATORY: the only one essential method
      using LoKi::BasicFunctors<LoKi::TrackTypes::TrackPair>::Function::operator();
      result_type operator() ( argument a ) const override;
      /// OPTIONAL: the nice printout
      std::ostream& fillStream ( std::ostream& s ) const override
      { return s << "TTrDA" ; }
    } ;
    // ========================================================================
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_HLTFUNCTIONS_H
// ============================================================================
