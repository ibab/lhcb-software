// $Id: $
#ifndef PVOFFLINERECALCULATE_H
#define PVOFFLINERECALCULATE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"


static const InterfaceID IID_PVOfflineRecalculate ( "PVOfflineRecalculate", 1, 0 );
/** @class PVOfflineRecalculate PVOfflineRecalculate.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2010-10-05
 */
class PVOfflineRecalculate : public GaudiTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PVOfflineRecalculate; }

  /// Standard constructor
  PVOfflineRecalculate( const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  // Initialization
  virtual StatusCode initialize();

  // Finalize
  virtual StatusCode finalize();

  virtual ~PVOfflineRecalculate( ); ///< Destructor

  StatusCode RecalculateVertex(const LHCb::RecVertex* pvin,
                         const std::vector<const LHCb::Track*>& tracks2remove,
                         LHCb::RecVertex& pvout);

  void print_stats();

private:

  bool m_updatePVTracks;
  // Extrapolators
  ITrackExtrapolator* m_linExtrapolator;   // Linear extrapolator
  ITrackExtrapolator* m_fullExtrapolator;  // Full extrapolator

  std::array<int,9> m_counter_count;

private:

  bool remove_track(const LHCb::RecVertex* vtx,
                    Gaudi::SymMatrix3x3& hess,
                    ROOT::Math::SVector<double,3>& d0vec,
                    double& dchi2,
                    const LHCb::Track* vtrack,
                    double wg);

  void printRat(const std::string& mes, int a, int b);

};
#endif // PVOFFLINERECALCULATE_H
