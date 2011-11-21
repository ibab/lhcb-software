// $Id: $
#ifndef FASTVERTEX_H 
#define FASTVERTEX_H 1

// Include files
#include "TrackForPV.h"

/** @class FastVertex FastVertex.h
 *  Simple object to compute a working vertex
 *
 *  @author Olivier Callot
 *  @date   2011-11-15
 */
class FastVertex {
public: 
  /// Standard constructor
  FastVertex( std::vector<TrackForPV*>:: iterator itT1, 
              std::vector<TrackForPV*>:: iterator itT2 ); 

  virtual ~FastVertex( ); ///< Destructor

  void addTrack( TrackForPV* track );
  void removeTrack( TrackForPV* track );
  void removeTrack( std::vector<TrackForPV*>::iterator it );
  void removeWorsts( double maxChi2 );

  std::vector<TrackForPV*>& tracks() { return m_tracks; }

  Gaudi::XYZPoint& vertex() { return m_vertex; }

  unsigned int nTracks() { return m_tracks.size(); }
  unsigned int nBack()   { return m_nBack; }

  Gaudi::SymMatrix3x3 cov ( );
  
protected:
  void solve();

private:
  std::vector<TrackForPV*> m_tracks;
  int m_nBack;
  double m_s0;
  double m_u0;
  double m_sx;
  double m_uy;
  double m_stx;
  double m_uty;
  double m_sxTx;
  double m_uyTy;
  double m_stx2;
  double m_uty2;

  Gaudi::XYZPoint m_vertex;
};
#endif // FASTVERTEX_H
