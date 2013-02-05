// $Id: PatMatchTool.h,v 1.6 2009-02-09 14:33:10 albrecht Exp $
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

	ITrackMomentumEstimate* m_fastMomentumTool;

	IAddTTClusterTool*   m_addTTClusterTool;

	double m_zMagnet;
	double m_zMagnetTx2;
	double m_zMagnetDsl2;
	double m_zMatchY;
	double m_dxTol;
	double m_dxTolSlope;
	double m_dyTol;
	double m_dyTolSlope;
	double m_magnetBend;
	double m_momentumScale;
	double m_maxChi2;
  double m_minMomentum;
  double m_minPt;
  

};

#endif // PATMATCHTOOL_H

// vim: sw=4:tw=78:ft=cpp
