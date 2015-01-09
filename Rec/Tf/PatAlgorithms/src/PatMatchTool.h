#ifndef PATMATCHTOOL_H
#define PATMATCHTOOL_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"

#include "TrackInterfaces/IMatchTool.h"            // Interface
#include "TrackInterfaces/ITrackMatch.h"            // Interface for Hlt



// forward decls
class ITrackMomentumEstimate;
class IAddTTClusterTool;


/** @class PatMatchTool PatMatchTool.h
 *
 * Match Velo and Seed tracks
 * The parameters used to calculate the chi2 can be derived using the "MatchFitParams" algorithm.
 *
 * Parameters:
 * 
 * - maxMatchChi2: Cut value for matching chi2. This is the main classifier that influences efficiency / ghost rate.
 * - zMagnet, zMagnetTx2, zMagnetDsl, zMagnetDsl2, zMagnetSeedX: Parameters (coefficients) to calculate the z-position for the matching in the x-coordinate
 * - zMatchY, MagnetBendDsl, MagnetBendDslY: Parameters (coefficients) for the matching in the y-coordinate
 * - dxTol, dxTolSlope, dyTol, dyTolSlope: Tolerances for the chi2 calculation.
 * - MinPt: Minimum pT for a track to be accepted
 * - MinMomentum: Mimimum momentum for a track to be accepted.
 * - FastMomentumToolName: Name of the tool that calculates the momentum.
 * - AddTTClusters: Add TT hits to this track (when possible)?
 * - AddTTClusterName: Name of the tool that adds TT hits.
 * - writeNNVariables: Write out LHCb::Track::NCandCommonHits?
 * - RejectVeloUsed: Use a velo track only once?
 * - RejectSeedUsed: Use a seed track only once?
 *
 * @author Michel De Cian
 * @date 2015-01-09
 *  change parametrisation for z(x) and bending in y.
 *
 * @author Manuel Schiller
 * @date 2012-01-31
 *	code cleanup, make sure all states are there for fit in HLT
 *
 * @author Johannes Albrecht
 * @date 2008-04-25
 * 	reorganize match as tool
 *
 * @author Olivier Callot
 * @date   2007-02-07
 *	initial implementation
 */

class PatMatchTool : public GaudiTool,
                     virtual public ITrackMatch,
                     virtual public IMatchTool
{
public:
	/// Standard constructor
	PatMatchTool(const std::string& type, const std::string& name,
               const IInterface* parent);
	/// Destructor
	virtual ~PatMatchTool();
	/// Algorithm initialization
	virtual StatusCode initialize();
	/// Algorithm finalization
	virtual StatusCode finalize();
  
	/// create match tracks from containers of velo and seed tracks
	virtual StatusCode match(const LHCb::Tracks& velos,
                           const LHCb::Tracks& seeds,
                           LHCb::Tracks& matchs);
  
	/// create a match track from a velo and a seed track
	virtual StatusCode matchSingle(const LHCb::Track& velo,
                                 const LHCb::Track& seed,
                                 LHCb::Track& output ,
                                 double& chi2);
  
	/// Function for matching in Hlt
	StatusCode match(const LHCb::Track& veloTrack,
                   const LHCb::Track& tTrack,
                   LHCb::Track& matchedTrack,
                   double& quality, double& quality2);
  
public:
  
	/** @class MatchCandidate PatMatch.h
	 *
	 * Match candidate for Tf::PatMatch algorithm
	 *
	 * @author Manuel Schiller
	 * @date 2012-01-31
	 * 	code cleanups
	 *
	 * @author Olivier Callot
	 * @date   2007-02-07
	 * 	initial implementation
	 */
	class MatchCandidate
	{
  public:
		MatchCandidate(const LHCb::Track* vTr,
                   const LHCb::Track* sTr, double dist) :
      m_vTr(vTr), m_sTr(sTr), m_dist(dist)
    { }
    
		~MatchCandidate() { }
    
		const LHCb::Track* vTr() const { return m_vTr; }
		const LHCb::Track* sTr() const { return m_sTr; }
		double dist() const { return m_dist; }
    
		struct lowerByChi2 {
      bool operator()(const MatchCandidate& first,
                      const MatchCandidate& second) const
      { return first.dist() < second.dist(); }
		};
    
  private:
		const LHCb::Track* m_vTr;
		const LHCb::Track* m_sTr;
		double m_dist;
	};
  
private:
	/// calculate matching chi^2
	double getChi2Match(const LHCb::Track& velo,
                      const LHCb::Track& seed) const;
  
	/// merge velo and seed segment to output track
	void makeTrack(const LHCb::Track& velo,
                 const LHCb::Track& seed, LHCb::Track& output,
                 double chi2) const;
  
	// use tool for momentum parametrisation
	std::string m_fastMomentumToolName;
	std::string m_addTTClusterName;
  
	bool m_addTT;
	bool m_writeNNVariables;
  bool m_rejectVeloUsed;
  bool m_rejectSeedUsed;
  
	ITrackMomentumEstimate* m_fastMomentumTool;
  
	IAddTTClusterTool*   m_addTTClusterTool;
  
	double m_zMagnet;
	double m_zMagnetTx2;
  double m_zMagnetDsl;
	double m_zMagnetDsl2;
  double m_zMagnetSeedX;
	double m_zMatchY;
  double m_magnetBendDsl;
  double m_magnetBendDslY;
	double m_dxTol;
	double m_dxTolSlope;
	double m_dyTol;
	double m_dyTolSlope;
  double m_maxChi2;
  double m_minMomentum;
  double m_minPt;
  
};

#endif // PATMATCHTOOL_H

// vim: sw=4:tw=78:ft=cpp
