// $Id: OTLayer.h,v 1.3 2002-06-06 08:53:38 jvantilb Exp $
#ifndef _OTLayer_H
#define _OTLayer_H 1

// This File contains the declaration of the OTLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Witek
//   Created: 11-11-2000
//   Original class name: OTDetectionLayer


#include <list>
#include "Kernel/OTChannelID.h"
#include "CLHEP/Geometry/Point3D.h"


class OTLayer {

public:

  /// constructor
  OTLayer(int iLayer, int iStationID, double zLayer, 
          double xCen,  double yCen, 
          double xSide, double ySide,
          double xOut,  double yOut, 
          int nStandStraw, double pitchStraw, 
          double stereoAngle );

  /// destructor
  virtual ~OTLayer();

  // basic routine to calculate hits for MC hit
  bool calculateHits( HepPoint3D entryPoint,
                      HepPoint3D exitPoint,
                      std::vector<OTChannelID>& channels,
                      std::vector<double>& driftDistances);

  /// returns (x,y,z) of the center of straw
  HepPoint3D centerOfStraw(const int iStraw, const int iModule) const;

  /// returns distance to electronics along the wire
  double distanceAlongWire(const int iModule, 
                           const double xHit, 
                           const double yHit) const;

  /// wire halfLength
  double wireHalfLength(const int iModule) const; 

  /// neigbouring straw (right)
  void nextRightStraw(int iHitStraw,int iHitModule,
                      int& iRightStraw,int& iRightModule) const;

  /// neigbouring straw (left)
  void nextLeftStraw(int iHitStraw,int iHitModule,
                    int& iLeftStraw,int& iLeftModule) const;

  /// returns the layer ID in the station
  unsigned int layerID()      const { return m_layerID ; }

  /// returns the station ID
  unsigned int stationID()    const { return m_stationID ; }

  /// Number of modules in this layer
  unsigned int nbModules()    const { return 2 * m_halfNumModule; }

  /// z of layer
  double z()                  const { return m_zOfLayer ;  }
  
  /// stereo angle
  double stereoAngle()        const { return m_stereoAngle ;  }

  /// returns u of straw iStraw in module iModule of this layer
  double uOfStraw(const int iStraw, const int iModule) const;

  /// returns z of the straw
  double zOfStraw(const int iStraw, const int iModule) const;  

  /// number of straw in that module
  int nStrawsInModule(const int iModule) const;

  /// cell radius
  double cellRadius()         const { return m_cellRadius ;  }

  /// Check if module is bottom module
  bool bottomModule(const int iModule) const;

  /// Check if module is top module
  bool topModule(const int iModule) const;

  /// Check if straw is in monolayer A
  bool monoLayerA(const int iModule, const int iStraw) const;
  
  /// Check if straw is in monolayer B
  bool monoLayerB(const int iModule, const int iStraw) const;

private:

  /// (x,y) -> (u,v) transformation
  void xy2uv(const double x, const double y, 
             double& u, double& v) const;

  /// (u,v) -> (x,y) transformation
  void uv2xy(const double u, const double v, 
             double& x, double& y) const;

  /// calculates of zc,uc of the center and radius 
  ///rc of the circle (special assumptions)

  void sCircle(const double z1, const double u1, const double z2, 
               const double u2, const double dz,
               double& zc, double& uc, double& rc);

  void hitModuleAndStraw( double u, double v,
                          int& hitMod, int& hitStrA, int& hitStrB) const;


  int     m_layerID;         // index of layer in this station

  int     m_stationID;       // index of station the layer belongs to;

  double  m_zOfLayer;        // z of the center of layer

  int     m_halfNumModule;   // half a number of modules 
                            // ( = nr of top modules = nr of bottom modules) 

  int*    m_halfNumStraw;    // array of nr of straws in one monolayer

  double* m_xFirstStrawA ;   // array of x-positions of first straw in module
                            // relative to the center of module

  double* m_xModuleCenter;   // array of x-coordinates of module centers

  double* m_yModuleCenter;   // array of y-coordinates of module centers

  double* m_xModuleSize;     // array of x-sizes of module centers

  double* m_yModuleSize;     // array of y-sizes of module centers

  double  m_pitchStraw;      // distance between straws

  double  m_cellRadius;      // straw radius

  double  m_stereoAngle;     // stereo angle

  double  m_sinAngle;        // sin of the stereo angle

  double  m_cosAngle;        // cos of the stereo angle


};

#endif // _OTLayer_H

