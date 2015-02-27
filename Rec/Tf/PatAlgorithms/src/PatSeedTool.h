// $Id: PatSeedTool.h,v 1.6 2009-11-26 18:00:48 mschille Exp $
#ifndef PATSEEDTOOL_H
#define PATSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatSeedTrack.h"

#include "TfKernel/HitExtension.h"


static const InterfaceID IID_PatSeedTool ( "PatSeedTool", 1, 0 );


/** @class PatSeedTool PatSeedTool.h
 *  Tool to fit the PatSeeding tracks
 *
 *  @author Olivier Callot
 *  @date   2006-10-23 Initial version
 *  @date   2007-08-20 Update for a-team framework 
 *  @date   2008-06-04 protect against tracks with too few hits to solve
 *          for fit parameters (i.e. protect against solving a linear system
 *          represented by a singular matrix)
 *  @date   2008-07-11 protect against singular matrix, 2nd part
 *  @date   2010-09-02 simultaneous fit to x and stereo hits, stub refit for
 *          PatSeeding, OT ambiguities from pitch residuals
 */

class PatSeedTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatSeedTool; }

    /// Standard constructor
    PatSeedTool( const std::string& type,
	const std::string& name,
	const IInterface* parent);

    virtual ~PatSeedTool( ); ///< Destructor

    /** fit a track
     * @param track PatSeedTrack to fit
     * @param maxChi2 outlier removal starts at maxChi2
     * @param minPlanes track must have at least minPlanes layers
     * @param xOnly ignore stereo part if true
     * @param forceDebug force printing debugging information
     * @return false if fit failed to satisfy criteria given by arguments, true otherwise
     */
    bool fitTrack( PatSeedTrack& track, double maxChi2, unsigned minPlanes,
	bool xOnly, bool forceDebug ) const;

    /** refit a stub (four IT hits in same station)
     *
     * simulaneous fit for ty, x0, tx at the track's referenze z0
     * the track is kept pointing to y = 0 at z = 0, its curvature is inferred
     * from the x intercept of a straight line extrapolation to z = 0
     *
     * @param track PatSeedTrack to refit
     * @param arrow conversion constant from x intercept at z=0 to curvature
     * @return false on failure, true otherwise
     */
    bool refitStub(PatSeedTrack& track, double arrow) const;

  protected:

    /// helper for debugging output
    inline void printTCoord( MsgStream& msg,
	const PatSeedTrack& track, const PatFwdHit* hit ) const;
  private:
    bool m_ambigFromPitchResiduals; ///< OT ambiguities from pitch residuals
    bool m_ambigFromLargestDrift; ///< OT ambig. from largest drift time in each station

    /// @brief class to wrap an iterator pair for use as a range
    template<class IT> struct iter_pair_range : public std::pair<IT, IT> {
      iter_pair_range(IT begin, IT end) : std::pair<IT, IT>(begin, end) { }
      IT begin() const { return std::pair<IT, IT>::first; }
      IT end() const { return std::pair<IT, IT>::second; }
    };
    /// @brief construct a range from an iterator pair
    template <class IT>
    static iter_pair_range<IT> as_range(IT begin, IT end)
    { return iter_pair_range<IT>(begin, end); }
    /// workspace (temporary hit collection)
    typedef std::array<PatFwdHit*, 32> workspace;
    /// resolve ambiguities from pitch residuals
    template <class Range>
    void resAmbFromPitchRes(PatSeedTrack& tr, Range hits) const;
    /// resolve ambiguities from largest drift time in each station
    template <class Range>
    bool resAmbFromLargestDrift( PatSeedTrack& track, Range othits, bool forceDebug ) const;

    /// remove hits while max chi^2 per hit too large
    template <class FIT, bool xOnly>
    bool removeHitsWhileChi2TooLarge(
	PatSeedTrack& track, const double maxChi2,
	const unsigned minPlanes, bool debug) const;

    /// little helper class to set ambiguities in a pair of hits
    template <bool sameSign>
    struct HitPairAmbSetter {
      /// set ambiguity of h1 to amb, h2 according to sameSign
      template <class HIT>
      static inline void set(HIT& h1, HIT& h2, int amb, bool setFirst)
      { if (setFirst) h1->setRlAmb(amb); h2->setRlAmb(sameSign ? amb : -amb); }
    };
};

#endif // PATSEEDTOOL_H
// vim:shiftwidth=2:tw=78
