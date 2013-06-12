// $Id: $
/**
 * @file TaggingHelpers.h
 *
 * various tagging helper routines
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-01-08
 */
#ifndef TAGGING_HELPERS_H
#define TAGGING_HELPERS_H 1

namespace TaggingHelpers {
    /** @brief return the difference between two angles
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-08
     *
     * @param phi1	first angle in radians
     * @param phi2	second angle in radians
     *
     * @return		difference between angles in radians
     */
    double dphi(const double phi1, const double phi2);

    /** @brief return the difference in angle between two vectors in the plane
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-08
     *
     * @param x1	x component of first vector in the plane
     * @param y1	y component of first vector in the plane
     * @param x2	x component of second vector in the plane
     * @param y2	y component of second vector in the plane
     *
     * @return		difference in angles in radians, or NaN if at least
     * 			one of the vectors has zero length
     */
    double dphi(const double x1, const double y1,
	    const double x2, const double y2);

    /** @brief enum to define the different outcomes of isSameTrack
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-09
     *
     * order degree of particle "sameness" by increasing "strictness classes",
     * so that comparisons are meaningful:
     *
     * @code
     * SameTrackStatus status = ...;
     * if (status) {
     *     // do something if tracks are considered the same in the broadest
     *     // possible sense
     * }
     * if (status > CloneHitContent) {
     *     // if we are sure that it's at least the same underlying track, we
     *     // do something else...
     * }
     * @endcode
     */
    typedef enum {
	DifferentParticles = 0,	//< different particles
	ConvertedGamma,		//< looks like converted gamma
	MattTxTyQp,		//< same slope in Velo (criteria due to M. Needham)
	MattTxTy,		//< same slope in Velo (criteria due to M. Needham)
	CloneTrackParams,	//< same particle based on match in p,eta,phi
	CloneHitContent,	//< same particle based on track hit content
	SameTrack,		//< based on same track
	SameProtoParticle,	//< based on same protoparticle
	SameParticle,		//< same particle
	nStates			//< comes last and gives the number of "states"
    } SameTrackStatus;

    /** @brief convert enum isSameTrack to string
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-09
     *
     * @param status	value for which to return string representation
     *
     * @return	string representation
     */
    const std::string& toString(const SameTrackStatus status);

    /** @brief check if two particles are based on the same track
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-09
     *
     * This routine checks if two tracks are the same or different by checking
     * these criteria in the order given above:
     * - p1 and p2 are the same particle (based on pointer comparison)
     * - p1 and p2 are based on the same protoparticle (pointer comparison)
     * - p1 and p2 are based on the same track (pointer comparison)
     * - the tracks for p1 and p2 share more than a certain fraction of hits
     *   (in Velo and T stations seperately)
     * - p1 and p2 fall into the same @f$(dp,d\eta,d\phi)@f bin (for pairs of
     *   charged tracks, we check the difference in curvature d(q/p) instead)
     * - p1 and p2 essentially have the same slopes in the Velo
     * - p1 and p2 appear to come from a gamma conversion (gamma -> ee / µµ)
     *
     * @param p1	particle 1
     * @param p2	particle 2
     * @param dEta	tolerance in @f$\eta@f$
     * @param dPhi	tolerance in @f$\phi@f$
     * @param sigmaP	tolerance in @f$p@f$ in terms of momentum or curvature
     * 			uncertainty
     * @param dppErr	if negative, use momentum uncertainty of particles,
     * 			else use dppErr as the relative momentum uncertainty
     * @param minSharedHitFraction
     * 			when comparing tracks based on hit content, require
     * 			at least this fraction of shared hits (if the track
     * 			has either a Velo or a T station segment, this
     * 			threshold has to apply for both the track seen as a
     * 			whole and the Velo- and T station segment considered
     * 			on its own) to consider the two tracks identical;
     * 			this part of the comparison can be switched off by
     * 			putting minSharedHitFraction greater than 1 or smaller
     * 			than 0
     * @param cosConvertedGammaAngle
     * 			try to find conversions by looking for low invariant
     * 			mass pairs of oppositely charged tracks (either
     * 			assuming the electron or muon mass), and require the
     * 			cosine of the angle between the two tracks larger than
     * 			this
     *
     * @return	SameTrackStatus which enumerates the possibilities of
     *		different underlying tracks and the different ideas of
     *		"they are based on the same track"
     */
    SameTrackStatus isSameTrack(
	    const LHCb::Particle& p1, const LHCb::Particle& p2,
	    const double dEta = 0.1, const double dPhi = 0.1,
	    const double sigmaP = 3., const double dppErr = -1.,
	    const double minSharedHitFraction = 0.7,
	    const double cosConvertedGammaAngle = 0.999999);
}

#endif // TAGGING_HELPERS_H

// vim: tw=78:sw=4:ft=cpp
