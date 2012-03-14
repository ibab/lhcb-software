#ifndef LOKI_VELOHITPATTERNFUNCTIONS_H
#define LOKI_VELOHITPATTERNFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
/** @file LoKi/VeloHitPatternFunctions.h
 *  Collection of Velo hit-pattern functions (that are not just counting the
 *  LHCbIDs passing a certain test)
 *
 *  @author Pieter David pieter.david@cern.ch
 *  @date   2012-03-12
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Tracks
  {
    // ========================================================================
    /** @class MaxNumConsecutiveVeloSpacePoints
     *  Get the maximal number of consecutive Velo R-Phi space points on a
     *  track
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    class GAUDI_API MaxNumConsecutiveVeloSpacePoints
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~MaxNumConsecutiveVeloSpacePoints () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual MaxNumConsecutiveVeloSpacePoints* clone() const
      { return new MaxNumConsecutiveVeloSpacePoints(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s <<  "TrNCONSECVELOSPACEPOINTS" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class NumVeloSpacePoints
     *  Count the Velo R-Phi space points on a track
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    class GAUDI_API NumVeloSpacePoints
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~NumVeloSpacePoints () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumVeloSpacePoints* clone() const
      { return new NumVeloSpacePoints(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s <<  "TrNVELOSPACEPOINTS" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class NumVeloACOverlapHits
     *  Count the A/C side overlap hits on a track, after taking the OR of R
     *  and Phi hits.
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    class GAUDI_API NumVeloACOverlapHits
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~NumVeloACOverlapHits () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumVeloACOverlapHits* clone () const
      { return new NumVeloACOverlapHits(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "TrNVELOOVERLAPS" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class NumVeloACOverlapHitsR
     *  Count the A/C side overlap R hits on a track
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    class GAUDI_API NumVeloACOverlapHitsR
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~NumVeloACOverlapHitsR () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumVeloACOverlapHitsR* clone () const
      { return new NumVeloACOverlapHitsR(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "TrNVELOOVERLAPSR" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class NumVeloACOverlapHitsPhi
     *  Count the A/C side overlap Phi hits on a track
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    class GAUDI_API NumVeloACOverlapHitsPhi
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~NumVeloACOverlapHitsPhi () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumVeloACOverlapHitsPhi* clone () const
      { return new NumVeloACOverlapHitsPhi(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "TrNVELOOVERLAPSPHI" ; }
      // ======================================================================
    };
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef  TrNCONSECVELOSPACEPOINTS
     *  Get the maximal number of consecutive Velo R-Phi space points on a
     *  track
     *  @see LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints
     *  @see LoKi::Cuts::TrNVELOSPACEPOINTS
     *  @see LoKi::Tracks::NumVeloSpacePoints
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    typedef LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints TrNCONSECVELOSPACEPOINTS ;
    // ========================================================================
    /** @typedef  TrNVELOSPACEPOINTS
     *  Count the Velo R-Phi space points on a track
     *  @see LoKi::Tracks::NumVeloSpacePoints
     *  @see LoKi::Cuts::TrNCONSECVELOSPACEPOINTS
     *  @see LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    typedef LoKi::Tracks::NumVeloSpacePoints TrNVELOSPACEPOINTS ;
    // ========================================================================
    /** @typedef TrNVELOOVERLAPS
     *  Count the A/C side overlap hits on a track, after taking the OR of R
     *  and Phi hits.
     *  @see LoKi::Tracks::NumVeloACOverlapHits
     *  @see LoKi::Cuts::TrNVELOOVERLAPSR
     *  @see LoKi::Tracks::NumVeloACOverlapHitsR
     *  @see LoKi::Cuts::TrNVELOOVERLAPSPHI
     *  @see LoKi::Tracks::NumVeloACOverlapHitsPhi
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    typedef LoKi::Tracks::NumVeloACOverlapHits TrNVELOOVERLAPS ;
    // ========================================================================
    /** @typedef TrNVELOOVERLAPSR
     *  Count the A/C side overlap R hits on a track
     *  @see LoKi::Tracks::NumVeloACOverlapHitsR
     *  @see LoKi::Cuts::TrNVELOOVERLAPS
     *  @see LoKi::Tracks::NumVeloACOverlapHits
     *  @see LoKi::Cuts::TrNVELOOVERLAPSPHI
     *  @see LoKi::Tracks::NumVeloACOverlapHitsPhi
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    typedef LoKi::Tracks::NumVeloACOverlapHitsR TrNVELOOVERLAPSR ;
    // ========================================================================
    /** @typedef TrNVELOOVERLAPSPHI
     *  Count the A/C side overlap R hits on a track
     *  @see LoKi::Tracks::NumVeloACOverlapHitsPhi
     *  @see LoKi::Cuts::TrNVELOOVERLAPS
     *  @see LoKi::Tracks::NumVeloACOverlapHits
     *  @see LoKi::Cuts::TrNVELOOVERLAPSR
     *  @see LoKi::Tracks::NumVeloACOverlapHitsR
     *  @author Wouter Hulsbergen wouterh@nikhef.nl
     *  @author Pieter David      pieter.david@cern.ch
     *  @date 2012-03-12
     */
    typedef LoKi::Tracks::NumVeloACOverlapHitsPhi TrNVELOOVERLAPSPHI ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_VELOHITPATTERNFUNCTIONS_H
