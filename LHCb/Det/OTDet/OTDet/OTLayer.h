// $Id: OTLayer.h,v 1.1.1.1 2002-05-26 22:36:34 jvantilb Exp $
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

  /// get unique layer ID
  virtual int    getAbsLayerID() const;

  /// layer Id relative to station number
  virtual int    getLayerID() const ;

  /// station ID
  virtual int getStationID() const;

  /// z of layer
  virtual double getZ() const;

  /// returns u of straw iStraw in module iModule of this layer
  double getU(const int iStraw, const int iModule) const;

  /// returns z of the straw
  double getZofStraw(const int iStraw, const int iModule) const;  

  /// returns (x,y,z) of the center of straw
  HepPoint3D getXYZofStrawCenter(const int iStraw, const int iModule) const;

  /// returns stereo angle of the layer
  double getStereoAngle() const;

  /// radius of straw
  double getCellRadius() const; 

  int nStrawsInModule(const int iModule) const;
  
  // basic routine to calculate hits for MC hit
  bool calculateHits( HepPoint3D entryPoint,
                      HepPoint3D exitPoint,
                      std::vector<OTChannelID>& channels,
                      std::vector<double>& driftDistances);

  /// returns distance to electronics along the wire
  double distanceAlongWire(const int iModule, 
                           const double xHit, const double yHit) const;

  /// wire halfLength
  double wireHalfLength(const int iModule) const; 

  /// neigbouring straw (right)
  void nextRightStraw(int iHitStraw,int iHitModule,
                      int& iRightStraw,int& iRightModule) const;

  /// neigbouring straw (left)
  void nextLeftStraw(int iHitStraw,int iHitModule,
                    int& iLeftStraw,int& iLeftModule) const;

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


  int     m_LayerID;         // index of layer in this station

  int     m_StationID;       // index of station the layer belongs to;

  double  m_ZOfLayer;        // z of the center of layer

  int     m_HalfNumModule;   // half a number of modules 
                            // ( = nr of top modules = nr of bottom modules) 

  int*    m_HalfNumStraw;    // array of nr of straws in one monolayer

  double* m_XFirstStrawA ;   // array of x-positions of first straw in module
                            // relative to the center of module

  double* m_XModuleCenter;   // array of x-coordinates of module centers

  double* m_YModuleCenter;   // array of y-coordinates of module centers

  double* m_XModuleSize;     // array of x-sizes of module centers

  double* m_YModuleSize;     // array of y-sizes of module centers

  double  m_PitchStraw;      // distance between straws

  double  m_CellRadius;      // straw radius

  double  m_StereoAngle;     // stereo angle

  double  m_SinAngle;        // sin of the stereo angle

  double  m_CosAngle;        // cos of the stereo angle


};

#endif // _OTLayer_H

