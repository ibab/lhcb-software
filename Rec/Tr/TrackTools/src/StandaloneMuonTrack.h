// $Id: StandaloneMuonTrack.h,v 1.2 2010-02-12 07:49:12 graven Exp $
#ifndef STANDALONEMUONTRACK_H 
#define STANDALONEMUONTRACK_H 1

// Include files
#include "StandaloneMuonPoint.h"

/** @class StandaloneMuonTrack StandaloneMuonTrack.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-10-08
 *
 * Removed from Hlt/HltMuon and ported to Tr/TrackTools
 *
 *  @author Paul Seyfert
 *  @date   2011-03-03
 */
class StandaloneMuonTrack {
public: 
  /// Standard constructor
  StandaloneMuonTrack( ){m_clone=0;}; 

  virtual ~StandaloneMuonTrack( ){}; ///< Destructor
  void setPoint(unsigned int station, StandaloneMuonPoint point)
  {m_points[station]=point;  
  };
  
  StandaloneMuonPoint  point(unsigned int station) const
  {return m_points[station];  
  };
  
  void setClone(){m_clone=1;};
  bool clone() const { return m_clone>0; }

double slopeX(int stationFirst,int stationSecond, 
	double zFirst, double zSecond) const {
return 
(m_points[stationFirst].x()-m_points[stationSecond].x())/(zFirst-zSecond);
};
double slopeY(int stationFirst,int stationSecond, 
        double zFirst, double zSecond) const {
return 
(m_points[stationFirst].y()-m_points[stationSecond].y())/(zFirst-zSecond);
};


private:
  StandaloneMuonPoint m_points[5];
  unsigned int m_clone;  
};
#endif // STANDALONEMUONTRACK_H
