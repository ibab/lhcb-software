// $Id: $
// ============================================================================
#ifndef HLTBASE_ITRACK2CANDIDATEMATCH_H
#define HLTBASE_ITRACK2CANDIDATEMATCH_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb
{
class Track;
} // Event/TrackEvent
namespace Hlt
{
class Candidate;
} // Hlt/HtlBase
// ============================================================================
namespace Hlt
{
// ==========================================================================
/** @class ITrack2CandidateMatch HltBase/ITrack2CandidateMatch.h
 *
 *  The abstract interface for Track<-->Candidate match
 *
 *  @see LHCb::Track
 *  @see Hlt::Candidate
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-02-11
 */
class GAUDI_API ITrack2CandidateMatch : virtual public IAlgTool
{
  public:
    // ========================================================================
    /** match a track with candidate
     *  @see ITrackMatch
     *  @param track     (INPUT)  the input track
     *  @param candidate (INPUT)  the input candidate
     *  @param matched   (OUTPUT) the matched track
     *  @param quality   (OUTPUT) the matching quality
     *  @param quality2  (OUTPUT) the matching quality-2
     *  @return status code
     */
    virtual StatusCode match( const LHCb::Track& track,
                              const Hlt::Candidate& candidate, LHCb::Track& matched,
                              double& quality, double& quality2 ) const = 0;
    // ========================================================================
    /** match a track with candidate ("filter-mode")
     *  @param track     (INPUT)  the input track
     *  @param candidate (INPUT)  the input candidate
     *  @return true if track and candidate are "matched"
     */
    virtual bool match( const LHCb::Track* track, const Hlt::Candidate* candidate,
                        const double quality = 1.e+6,
                        const double quality2 = 1.e+6 ) const = 0;
    // ========================================================================
  public:
    // ========================================================================
    /// InterfaceID
    DeclareInterfaceID( ITrack2CandidateMatch, 1, 0 ); // InterfaceID
    // ========================================================================
  protected:
    // ========================================================================
    /// protected & virtual destructor
    virtual ~ITrack2CandidateMatch(); // protected & virtual destructior
    // ========================================================================
};
// ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // HLTBASE_ITRACK2CANDIDATEMATCH_H
// ============================================================================
