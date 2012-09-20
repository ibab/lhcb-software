// $Id: $
#ifndef VPHIT_H 
#define VPHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "Event/VPLiteCluster.h"

/** @class VPHit VPHit.h
 *  
 *
 *  @author Wenbin Qian
 *  @date   2011-02-16
 */
class VPHit {
public: 
  /// Standard constructor
  VPHit():
	m_x(0.),m_y(0.),m_z(0.),m_dx(0.),m_dy(0.),m_used(0),m_sensor(-1){};

  VPHit( float x, float y, float z, float dx, float dy, int sensor){
    m_x = x;
    m_y = y;
    m_z = z;
    m_dx = dx;
    m_dy = dy;
    m_used = 0;
    m_sensor = sensor;
}; 

  VPHit( double x, double y, double z, double dx, double dy, int sensor){
    m_x = float(x);
    m_y = float(y);
    m_z = float(z);
    m_dx = float(dx);
    m_dy = float(dy);
    m_used = 0; 
    m_sensor = sensor;
}; 

  void setLHCbID (LHCb::LHCbID id){
	 m_lhcbid = id;
}
  LHCb::LHCbID lhcbID() const { return m_lhcbid;}
  float x()  const { return m_x; }
  float y()  const { return m_y; }
  float z()  const { return m_z; }
  float dx() const { return m_dx; }
  float dy() const { return m_dy; }
  float r() {return sqrt(m_x*m_x + m_y*m_y);}
  void addused() {m_used++;}
  void removeused() {m_used--;}
  int  getused() const {return m_used;}
  int sensor() const{return m_sensor;}
  virtual ~VPHit( ) {}; ///< Destructor
protected:

private:
  float m_x;
  float m_y;
  float m_z;
  float m_dx;
  float m_dy;
  int m_used;
  int m_sensor;
  LHCb::LHCbID m_lhcbid;
};

typedef std::vector<VPHit*> VPHits;
#endif // VPHIT_H
