#ifndef MUONTRACKRECMUONTRACK_H 
#define MUONTRACKRECMUONTRACK_H 1

// Include files
#include <GaudiKernel/StatusCode.h>
#include <GaudiKernel/Point4DTypes.h>
#include "MuonInterfaces/MuonHit.h"
#include <vector>
#include <iostream>
#include <map>
/** @class MuonTrack MuonTrack.h
 *  
 *  A MuonTrack is an object containing a list of MuonHits belonging to a muon 
 *  track. The hits are stored in a std::map<int, MuonHit> where the int key is
 *  the ID (i.e. progressive number) of the inserted MuonHit.
 *
 *  @author Giovanni Passaleva / Giacomo Graziani
 *  @date   2007-08-21
 */
#ifdef _WIN32
    #define EXTERN extern __declspec(dllimport) 
#else
    #define EXTERN extern
#endif

#ifndef MUONTRACKRECNMSPC
namespace MuonTrackRec {
  EXTERN double muspeed;
  EXTERN double Zref;
  EXTERN bool PhysTiming;
  EXTERN bool IsCosmic;
  EXTERN bool IsPhysics;
}
#endif  // MUONTRACKRECNMSPC
#undef EXTERN

class MuonTrack {
public: 
  /// Standard constructor
  MuonTrack();
  ~MuonTrack( ); ///< Destructor

  /// public member functions
  /// insert a hit in the track
  void insert( const int id, const MuonHit* xyz );
  /// store the best hit candidates in each station
  void setBestCandidate( std::vector<const MuonHit*> bcand );
  /// retrieve the array of best candidates. Best candidates 
  /// are sorted by station by construction
  std::vector<const MuonHit*> bestCandidate() const { return m_bestcand; };
  /// set the clone flag
  void setClone() { m_isClone = true; }
  /// get the Clone flag
  bool isClone() const { return m_isClone; }
  /// return a vector with hits
  std::vector< const MuonHit* > getHits() const;
  /// get the track span i.e. the max length in station units
  int getSpan() const;
  /// get the number of stations giving a hit to this track
  int getFiringStations() const;
  // linear chi2 fit. Track is fitted separately in XZ, YZ, TZ (assuming speed of light)
  StatusCode linFit();
  // add XTalk tracks
  StatusCode AddXTalk(const std::vector< MuonHit* >* trackhits, float cut=1.5, int m_skipStation=-1);
  // linear fit to get particle speed
  StatusCode speedFit();
  inline double chi2x() const {return m_chi2x;}  /// chi2/dof XZ
  inline double chi2y() const {return m_chi2y;}  /// chi2/dof YZ
  // slope XZ
  inline double sx() const {return m_sx;} 
  // intercept XZ
  inline double bx() const {return m_bx;} 
  // slope YZ
  inline double sy() const {return m_sy;} 
  // direction (+1 forward or -1 backward)
  double sz() const ;
  // intercept YZ
  inline double by() const {return m_by;}
  // track t0
  inline double t0() const {return m_t0;}
  // estimated error on time
  inline double sigmat() const {return m_sigmat;}
  // fitted speed (in c units)
  inline double speed()  {
    if(! m_speedFitted) speedFit();
    return m_speed;}
  // error on fitted speed (in c units)
  inline double sigmaspeed() const {return m_sigmaspeed;}

  // errors on the above parameters 
  inline double errsx() const {return m_errsx;} 
  inline double errbx() const {return m_errbx;} 
  inline double covbsx() const {return m_covbsx;}
  inline double errsy() const {return m_errsy;} 
  inline double errby() const {return m_errby;} 
  inline double covbsy() const {return m_covbsy;}
  inline double errt0() const {return m_errt0;}
  

  // polar angles wrt beam axis (taking into account track direction)
  inline double theta() const {return acos(sz()/sqrt(m_sx*m_sx+m_sy*m_sy+1));}
  inline double phi() const {return atan2(m_sy,m_sx);}
  // polar angles wrt vertical axis (pointing down)
  inline double thetaVert() const {return acos(-(m_sy/sz())/sqrt(m_sx*m_sx+m_sy*m_sy+1));}
  inline double phiVert() const {return atan2(m_sx,sz());}

  // track extrapolation
  void extrap(double Z, 
              Gaudi::XYZTPoint& Pos, 
              Gaudi::XYZTPoint* PosErr = NULL);
  // residuals
  Gaudi::XYZTPoint& residuals(MuonHit& hit);
  // corrected time of i-th hit (i from 0)
  double correctedTime(const MuonHit& hit) const;
  /// cluster size (total, and in the x/y views) associated to a given hit 
  /// (only if first hit of a given station, to avoid double counting)
  int clsize(MuonHit* hit, int& xsize, int& ysize);
  /// track momentum variables; setters
  inline void setP( double p ){ m_trackP = p; }
  inline void setPt( double pt ){ m_trackPt = pt; }
  inline void setqOverP( double qp ){ m_trackqOverP = qp; }
  inline void setMomentum(  Gaudi::XYZVector mom ){ m_trackMomentum = mom; }
  /// track momentum variables; getters
  inline double p() const { return m_trackP; }
  inline double pt() const { return m_trackPt; }
  inline double qOverP() const { return m_trackqOverP; }
  inline Gaudi::XYZVector momentum() const{ return m_trackMomentum; }


private:

  double correctTOF(double rawT,
                    double X,
                    double Y,
                    double Z) const;
  double correctTime(double rawT,
                     double X,
                     double Y,
                     double Z) const;
  typedef std::map< int, MuonHit* >::value_type MuonTkVtype;
  std::map< int, MuonHit* > m_muonTrack;
  std::vector<const MuonHit*> m_bestcand;
  bool m_isClone;
  double m_chi2x;
  double m_chi2y;
  double m_sx;
  double m_bx;
  double m_sy;
  double m_by;
  double m_errsx;
  double m_errbx;
  double m_covbsx;
  double m_errsy;
  double m_errby;
  double m_covbsy;
  double m_t0;
  double m_errt0;
  double m_sigmat;
  double m_speed;
  double m_sigmaspeed;
  bool m_speedFitted;
  Gaudi::XYZTPoint m_point;
  // track momentum tracks
  double m_trackP;
  double m_trackPt;
  double m_trackqOverP;
  Gaudi::XYZVector m_trackMomentum;
  // 
};


#endif // MUONTRACKRECMUONTRACK_H
