// $Id: OTMuonCosmicsMatching.h,v 1.1 2008-10-31 20:57:53 janos Exp $
#ifndef OTMUONCOSMICSMATCHING_H 
#define OTMUONCOSMICSMATCHING_H 1

// Include files
// from STD
#include <string>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from MuonNNet
#include "MuonNNet/IMuonNNetRec.h"

// forward declarations
class MuonTrack;
namespace LHCb {
 class State;
 class Track;
}

/** @class OTMuonCosmicsMatching OTMuonCosmicsMatching.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2008-10-28
 */

class OTMuonCosmicsMatching : public GaudiHistoAlg {

public:
  /// Standard constructor
  OTMuonCosmicsMatching( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTMuonCosmicsMatching( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef std::pair< Gaudi::Vector4, Gaudi::SymMatrix4x4 > MyState;

  StatusCode extrapolate( MyState& state, double oldZ, double newZ ) const;
  double matchChi2(const LHCb::Track& match, const MuonTrack& to, double atZ ) const;
  Gaudi::Matrix4x4 jacobian( double z ) const;

private:
  
  IMuonNNetRec*       m_muonRecTool     ;
  ITrackExtrapolator* m_extrapolator    ;
  std::string         m_tracksToMatchLoc;
  std::string         m_nameExtrapolator;
  double              m_matchAtZ;

};

inline Gaudi::Matrix4x4 OTMuonCosmicsMatching::jacobian( double z ) const {
  
  /// Linear extrapolation of state = ( x, y, tx, ty )
  /// x' = x + z*tx
  /// y' = y + z*ty
  /// tx = tx
  /// ty = ty
  /// Jacobian is also the transform
  Gaudi::Matrix4x4 jacobian;
  jacobian( 0, 0 ) = 1.0;
  jacobian( 0, 2 ) = z;
  jacobian( 1, 1 ) = 1.0;
  jacobian( 1, 3 ) = z;
  jacobian( 2, 2 ) = 1;
  jacobian( 3, 3 ) = 1;

  return jacobian;
}

#endif // OTMUONCOSMICSMATCHING_H
