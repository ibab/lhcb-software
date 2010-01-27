#ifndef TRACKINTERFACES_ITRACKHITCOLLECTOR_H
#define TRACKINTERFACES_ITRACKHITCOLLECTOR_H 1

// Include files
// -------------

#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward decls
namespace LHCb {
    class LHCbID;
    class Track;
}

static const InterfaceID IID_ITrackHitCollector ( "ITrackHitCollector", 1, 0 );

/// interface for tools collecting hits around a track
/** @class ITrackHitCollector ITrackHitCollector.h
 *  
 *  interface for tools collecting hits around a track
 *
 *  @author Manuel Tobias Schiller
 *  @date   2010-01-27
 */
class ITrackHitCollector : virtual public IAlgTool {
    public:
	// Return the interface ID
	static const InterfaceID& interfaceID() { return IID_ITrackHitCollector; }

	/// collect hits around a track
	/** collect hits around a track
	 *
	 * @param tr    track around which hits are to be collected
	 * @param ids   vector to hold collected LHCbIDs
	 * @param collectVelo collect hits in Velo
	 * @param collectTT collect hits in TT
	 * @param collectIT collect hits in IT
	 * @param collectOT collect hits in OT
	 * @param collectMuon collect hits in Muon
	 * @return StatusCode::SUCCESS or StatusCode::FAILURE
	 */
	virtual StatusCode execute(
		const LHCb::Track& tr, std::vector<LHCb::LHCbID>& ids,
		bool collectVelo = true, bool collectTT = true,
		bool collectIT = true, bool collectOT = true,
		bool collectMuon = true) = 0;

	/// collect hits around a track
	/** collect hits around a track
	 *
	 * @param tr track around which hits are to be collected
	 * @param ids vector to hold collected LHCbIDs
	 * @param resids vector to hold residuals to collected LHCbIDs
	 * @param collectVelo collect hits in Velo
	 * @param collectTT collect hits in TT
	 * @param collectIT collect hits in IT
	 * @param collectOT collect hits in OT
	 * @param collectMuon collect hits in Muon
	 * @return StatusCode::SUCCESS or StatusCode::FAILURE
	 */
	virtual StatusCode execute(
		const LHCb::Track& tr, std::vector<LHCb::LHCbID>& ids,
		std::vector<double>& resids,
		bool collectVelo = true, bool collectTT = true,
		bool collectIT = true, bool collectOT = true,
		bool collectMuon = true) = 0;


	/// collect hits around a track
	/** collect hits around a track
	 *
	 * @param tr    track around which hits are to be collected
	 * @param ids   vector to hold collected LHCbIDs
	 * @param resids vector to hold residuals to collected LHCbIDs
	 * @param errests vector to hold error estimates for residuals
	 * @param collectVelo collect hits in Velo
	 * @param collectTT collect hits in TT
	 * @param collectIT collect hits in IT
	 * @param collectOT collect hits in OT
	 * @param collectMuon collect hits in Muon
	 * @return StatusCode::SUCCESS or StatusCode::FAILURE
	 */
	virtual StatusCode execute(
		const LHCb::Track& tr, std::vector<LHCb::LHCbID>& ids,
		std::vector<double>& resids, std::vector<double>& errests,
		bool collectVelo = true, bool collectTT = true,
		bool collectIT = true, bool collectOT = true,
		bool collectMuon = true) = 0;
};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_ITRACKHITCOLLECTOR_H
