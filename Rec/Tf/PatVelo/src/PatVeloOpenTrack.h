// $Id: PatVeloOpenTrack.h,v 1.1 2007-11-09 07:49:07 ocallot Exp $
#ifndef PATVELOOPENTRACK_H 
#define PATVELOOPENTRACK_H 1

// Include files
#include "PatVeloHit.h"

/** @class PatVeloOpenTrack PatVeloOpenTrack.h
 *  Representation of a space track for Open tracking
 *
 *  @author Olivier Callot
 *  @date   2007-03-13
 */
class PatVeloOpenTrack {
public: 
  /// Standard constructor
  PatVeloOpenTrack( ); 
  
  PatVeloOpenTrack ( Tf::PatVeloRHit* cR0, Tf::PatVeloRHit* cR2, 
                     Tf::PatVeloPhiHit* cPhi0, Tf::PatVeloPhiHit* cPhi2 );
  
  virtual ~PatVeloOpenTrack( ); ///< Destructor

  typedef std::vector<Tf::PatVeloRHit*>   PatVeloRHits;
  typedef std::vector<Tf::PatVeloPhiHit*> PatVeloPhiHits;

  double xAtZ( double z ) const { return m_tx * z + m_x0; }

  double yAtZ( double z ) const { return m_ty * z + m_y0; }

  double rAtZ( double z ) const { return sqrt( xAtZ(z)*xAtZ(z) + yAtZ( z) * yAtZ( z ) ); }

  double phiAtZ( double z ) const { return atan2( yAtZ( z), xAtZ( z ) ); }

  void addRCoord( Tf::PatVeloRHit* c ) { m_rCoords.push_back( c ); update(); }

  void addPhiCoord( Tf::PatVeloPhiHit* c ) { m_phiCoords.push_back( c ); update(); }

  double tx() const { return m_tx; }
  double x0() const { return m_x0; }
  double ty() const { return m_ty; }
  double y0() const { return m_y0; }

  void setChi2( double chi2 )       { m_chi2 = chi2; }
  double chi2()               const { return m_chi2; }

  const PatVeloRHits* rCoords()     const { return &m_rCoords; }
  const PatVeloPhiHits* phiCoords() const { return &m_phiCoords; }

  unsigned int nCoords() const { return m_rCoords.size() + m_phiCoords.size(); }

  bool backward() const { return m_backward; }

  double firstZ() const { return m_firstZ; }

  double lastZ() const { return m_lastZ; }

  double errX2() const { return m_errX2; }

  double errY2() const { return m_errY2; }

  double errTx2() const { return m_errTx2; }

  double errTy2() const { return m_errTy2; }

  void finalFit();

  bool valid() const { return m_valid; }
  void setValid( bool valid ) { m_valid = valid; }

  int half() const { return m_half; }
protected:
  void update();
  
private:
  bool m_valid;
  int  m_half;
  PatVeloRHits   m_rCoords;
  PatVeloPhiHits m_phiCoords;

  double m_x0;
  double m_tx;
  double m_y0;
  double m_ty;
  
  double m_chi2;
  bool   m_backward;
  double m_firstZ;
  double m_lastZ;
  double m_errX2;
  double m_errY2;
  double m_errTx2;
  double m_errTy2;
};
#endif // PATVELOOPENTRACK_H
