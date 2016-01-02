#ifndef INCLUDE_TRACKHITCOLLECTOR_H
#define INCLUDE_TRACKHITCOLLECTOR_H 1

#include <vector>
#include <utility>
#include <memory>
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "TrackInterfaces/ITrackHitCollector.h"

namespace LHCb {
    class Track;
    class TrackTraj;
    class Trajectory;
    class State;
}

namespace Tf {
    template<class Hit> class TStationHitManager;
    template<class Hit> class TTStationHitManager;
    class DefaultVeloRHitManager;
    class DefaultVeloPhiHitManager;
    template<class A, class B, int C> class DefaultVeloHitManager;
    class LineHit;
    class VeloRHit;
    class VeloPhiHit;
}

class PatForwardHit;
class PatTTHit;
class DeVeloRType;
class DeVeloPhiType;
class ITrajPoca;

/** @class TrackHitCollector TrackHitCollector.h
 *
 * collect Hits around a track 
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2010-01-27
 *
 * current limitation: does not collect Muon hits (yet)
 */
class TrackHitCollector : public GaudiTool, virtual public ITrackHitCollector
{
    public:
	typedef Tf::TStationHitManager<PatForwardHit> HitManT;
	typedef Tf::TTStationHitManager<PatTTHit> HitManTT;
	typedef Tf::DefaultVeloPhiHitManager HitManVeloPhi;
	typedef Tf::DefaultVeloRHitManager HitManVeloR;

    public:

	/// Standard Constructor
	TrackHitCollector(const std::string& type,
		const std::string& name,
		const IInterface* parent);

	virtual ~TrackHitCollector(); ///< Destructor

	virtual StatusCode initialize(); ///< Tool initialization

	/// collect hits around a track
	/** collect hits around a track
	 *
	 * @param tr	track around which hits are to be collected
	 * @param ids	vector to hold collected hits with residuals
	 * @param collectVelo collect hits in Velo
	 * @param collectTT collect hits in TT
	 * @param collectIT collect hits in IT
	 * @param collectOT collect hits in OT
	 * @param collectMuon collect hits in Muon
	 * @return StatusCode::SUCCESS or StatusCode::FAILURE
	 */
	virtual StatusCode execute(
		const LHCb::Track& tr, std::vector<IDWithResidual>& ids,
		bool collectVelo = true, bool collectTT = true,
		bool collectIT = true, bool collectOT = true,
		bool collectMuon = true);

    protected:


    private:
	// hit managers
	HitManT* m_hitManagerT;
	HitManTT* m_hitManagerTT;
	HitManVeloR* m_hitManagerVeloR;
	HitManVeloPhi* m_hitManagerVeloPhi;
	// poca tool for Velo hits
	ITrajPoca* m_trajPoca;

	double m_tolX; ///< initial tolerance when getting rough hit list
	double m_tolY; ///< initial tolerance when getting rough hit list
	double m_tolR; ///< initial tolerance when getting rough hit list
	double m_tolPhi; ///< initial tolerance when getting rough hit list
	double m_nSigma; ///< initial track pos. uncertainty scale for rough hit list
	double m_maxDistOT; ///< max. final dist. hit-track in OT
	double m_maxDistIT; ///< max. final dist. hit-track in IT
	double m_maxDistTT; ///< max. final dist. hit-track in TT
	double m_maxDistVelo; ///< max. final dist. hit-track in Velo
	
	/// collect line hits as used in IT, TT and OT
	template<typename HITMANAGER,
	    unsigned regMin, unsigned regMax,
	    unsigned staMin, unsigned staMax,
	    unsigned layMin, unsigned layMax>
	void collectLineHits(HITMANAGER* hitman,
		const LHCb::TrackTraj& ttraj,
		std::vector<IDWithResidual>& ids) const;

	/// compute residual between track and hit, return true if small enough
	bool computeResidAndErr(
		const PatForwardHit* hit, const LHCb::TrackTraj& ttraj,
		const LHCb::State& st, double& resid, double& err) const;
	/// compute residual between track and hit, return true if small enough
	bool computeResidAndErr(
		const PatTTHit* hit, const LHCb::TrackTraj& ttraj,
		const LHCb::State& st, double& resid, double& err) const;

	/// compute quick and dirty doca of track and hit in T/TT
	void docaInTandTT(const LHCb::TrackTraj& ttraj, const Tf::LineHit& lhit,
		const double refy, const double refz, Gaudi::XYZVector& doca) const;

	/// compute residual between track and hit, return true if small enough
	bool computeResidAndError(
		const Tf::VeloRHit* hit,
		const LHCb::Trajectory& htraj,
		const LHCb::TrackTraj& ttraj, const Gaudi::XYZPoint& ptrack,
		const DeVeloRType* sensor,
		const double trerr2, const double trerrphi2,
		double& resid, double& err) const;
	/// compute residual between track and hit, return true if small enough
	bool computeResidAndError(
		const Tf::VeloPhiHit* hit,
		const LHCb::Trajectory& htraj,
		const LHCb::TrackTraj& ttraj, const Gaudi::XYZPoint& ptrack,
		const DeVeloPhiType* sensor,
		const double trerr2, const double trerrphi2,
		double& resid, double& err) const;

	/// select correct coordinate range (halfbox frame)
	std::pair<double, double> selectVeloCoordinates(
		const DeVeloRType* sensor, const int iZone, const double r,
		const double rmin, const double rmax,
		const double phimin, const double phimax) const;
	/// select correct coordinate range (halfbox frame)
	std::pair<double, double> selectVeloCoordinates(
		const DeVeloPhiType* sensor, const int iZone, const double r,
		const double rmin, const double rmax,
		const double phimin, const double phimax) const;

	/// check if two ranges in phi overlap
	bool phiRangesOverlap(const std::pair<double, double>& r1,
		const std::pair<double, double>& r2) const;

	/// collect Velo hits (R/Phi depending on HITMANAGER)
	template<typename HITMANAGER>
	void collectVeloHits(HITMANAGER* hitman,
		const LHCb::TrackTraj& ttraj,
		std::vector<IDWithResidual>& ids) const;

	/// update list with proper unbiased residuals (if available)
	void updateWithProperResiduals(
		std::vector<IDWithResidual>& ids,
		const LHCb::Track& tr) const;
};
#endif // INCLUDE_TRACKHITCOLLECTOR_H

// vim: sw=4:tw=78:ft=cpp
