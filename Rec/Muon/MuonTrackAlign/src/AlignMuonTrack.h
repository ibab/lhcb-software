// $Id: AlignMuonTrack.h,v 1.3 2010-03-24 13:24:37 rlambert Exp $
#ifndef ALIGNMUONTRACK_H 
#define ALIGNMUONTRACK_H 1

// Include files
#include "AlignMuonPoint.h"

/** @class AlignMuonTrack AlignMuonTrack.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-10-08
 */
class AlignMuonTrack {
public: 
  /// Standard constructor
  AlignMuonTrack( ){m_clone=0;}; 
  
  virtual ~AlignMuonTrack( ){}; ///< Destructor
  /*
  void setPoint(unsigned int station, AlignMuonPoint point)
  {m_points[station]=point;  
  };*/
  void setPoint(unsigned int station, std::vector<AlignMuonPoint> point)
  {
    for(std::vector<AlignMuonPoint>::const_iterator iV = point.begin(); iV != point.end(); iV++) 
      m_points[station].push_back(*iV);  
  };  // Estensione per un vettore di punti
  /*
  AlignMuonPoint  point(unsigned int station)
  {return m_points[station];  
  };*/
  std::vector<AlignMuonPoint>  points(unsigned int station)
  {return m_points[station];  
  };
  AlignMuonPoint  point(unsigned int station)
  {return m_points[station][0];  
  };
  
  void setClone(){m_clone=1;};
  bool clone(){if(m_clone>0)return true;
	else return false;};
  
  float slopeX(int stationFirst,int stationSecond,float zFirst, float zSecond){
    return 
      float((m_points[stationFirst][0].x()-m_points[stationSecond][0].x())/(zFirst-zSecond));
  };
  float slopeY(int stationFirst,int stationSecond, 
               float zFirst, float zSecond){
    return 
      float((m_points[stationFirst][0].y()-m_points[stationSecond][0].y())/(zFirst-zSecond));
  };
  
  float slopeX(int stationFirst,int stationSecond){
    return 
      float((m_points[stationFirst][0].x()-m_points[stationSecond][0].x())/(m_points[stationFirst][0].z()-m_points[stationSecond][0].z()));
  };
  float slopeY(int stationFirst,int stationSecond){
    return 
      float((m_points[stationFirst][0].y()-m_points[stationSecond][0].y())/(m_points[stationFirst][0].z()-m_points[stationSecond][0].z()));
  };
  
  
protected:
  
private:
  std::vector<AlignMuonPoint> m_points[5];
  
  unsigned int m_clone;  
};
#endif // ALIGNMUONTRACK_H
