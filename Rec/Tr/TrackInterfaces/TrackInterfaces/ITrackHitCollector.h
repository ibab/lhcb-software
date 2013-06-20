#ifndef TRACKINTERFACES_ITRACKHITCOLLECTOR_H
#define TRACKINTERFACES_ITRACKHITCOLLECTOR_H 1

// Include files
// -------------

#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/LHCbID.h"

// forward decls
namespace LHCb {
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

	/// an LHCbID along with residual and error
	struct IDWithResidual {
	    double m_res;
	    double m_reserr;
	    // m_id is last to avoid gaps in memory layout
	    LHCb::LHCbID m_id;
	    /// constructor
	    IDWithResidual(LHCb::LHCbID id, double res, double reserr) :
		m_res(res), m_reserr(reserr), m_id(id)
	    { }
	};

	/// collect hits around a track
	/** collect hits around a track
	 *
	 * @param tr    track around which hits are to be collected
	 * @param ids   vector of IDWithResidual holding collected hits
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
		bool collectMuon = true) = 0;
};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_ITRACKHITCOLLECTOR_H
