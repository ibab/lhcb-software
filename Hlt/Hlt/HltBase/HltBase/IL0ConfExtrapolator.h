// $Id: IL0ConfExtrapolator.h,v 1.5 2010-01-07 14:02:53 graven Exp $
#ifndef IL0CONFEXTRAPOLATOR_H
#define IL0CONFEXTRAPOLATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "HltBase/FwdHypothesis.h"
#include "HltBase/ParabolaHypothesis.h"

static const InterfaceID IID_IL0ConfExtrapolator( "IL0ConfExtrapolator", 1, 0 );

namespace LHCb {
class Track;
}
namespace GAUDI {
class XYZPoint;
}

/** @class IL0ConfExtrapolator IL0ConfExtrapolator.h
 *
 *  Interface for tool which does the extrapolation from the seed
 *  to the T-stations for muon, hadron and electron L0-T-confirmation.
 *
 *  Provides as well the parameterization to obtain the track hypothesis
 *  (i.e. the search window in which T-hits are decoded and PR is performed)
 *  for the TF::TStationHitManager.
 *  For L0 confirmation, the size of the search window is saved in the error
 *  matrix of the track (prepared by the XXXSeedTool), for velo-calo confirmation,
 *  the window size is saved here.
 *
 *  @author Johannes Albrecht
 *  @date   2008-01-18
 */
class IL0ConfExtrapolator : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_IL0ConfExtrapolator; }

    // provides a third order polynomial for T-decoding
    // parametrization by Olivier Callot
    virtual FwdHypothesis getFwdHypothesis( const LHCb::Track &veloTrack,
                                            const LHCb::Track &caloTrack,
                                            double nSigma ) const = 0;

    // provides a second order polynomial for T-decoding
    virtual ParabolaHypothesis getParabolaHypothesis( const LHCb::Track &seedTrack,
                                                      double nSigma ) const = 0;

    virtual ParabolaHypothesis getParabolaHypothesis( const LHCb::State &aState,
                                                      double nSigma ) const = 0;

    virtual ParabolaHypothesis getParabolaHypothesis( const LHCb::State &aState,
                                                      double nSigmaX,
                                                      double nSigmaY ) const = 0;

    // Extrapolates the seed track to the end of T3
    virtual void muon2T( const LHCb::Track &muonTrack,
                         LHCb::State &stateAtT ) const = 0;

    virtual void ecal2T( const LHCb::Track &ecalTrack, LHCb::State &statePosAtT,
                         LHCb::State &stateNegAtT ) const = 0;

    virtual void hcal2T( const LHCb::Track &ecalTrack, LHCb::State &statePosAtT,
                         LHCb::State &stateNegAtT ) const = 0;

    // provides the Calo region for a given position
    // 0: ECal IP, 1: ECal MP, 2: ECal OP
    // 3: HCal IP, 4: HCal OP
    virtual int getCaloRegion( double stateX, double stateY,
                               double stateZ ) const = 0;

    virtual double getBScale() = 0;
};
#endif // IL0CONFEXTRAPOLATOR_H
