// $Id: OTLayer.h,v 1.7 2002-11-12 12:48:49 jvantilb Exp $
#ifndef OTDET_OTLAYER_H
#define OTDET_OTLAYER_H 1

// Kernel
#include "Kernel/OTChannelID.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

/** @class OTLayer OTLayer.h "OTDet/OTLayer.h"
 *
 *  This class contains the geometry parameters of a Outer Tracker 
 *  (double) layer. 
 *  Each OT station consists of 4 of these layers. This class has 
 *  the calculateHits-method which calculates the closest wires 
 *  (=channels) and the distances from an entry- and exit-point. 
 *  Another method, distanceAlongWire, calculates the distance 
 *  between the hit and the read-out. These and some other functions 
 *  are used by the OT digitization as well as the track reconstruction.
 *  Original class name: OTDetectionLayer
 *
 *  @author M. Witek
 *  @author Adapted to the Event Model by J. van Tilburg 
 *          jtilburg@nikhef.nl (06-06-2002)
 *  @date   11-11-2000  
 */

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

  /// basic routine to calculate hits for MC hit
  bool calculateHits( HepPoint3D entryPoint,
                      HepPoint3D exitPoint,
                      std::vector<OTChannelID>& channels,
                      std::vector<double>& driftDistances);

  /// calculate the distance to the wire (assuming straight lines)
  double distanceToWire(const OTChannelID aChannel, 
                        const HepPoint3D& aPoint, 
                        const double tx, const double ty) const;

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

  /// returns u of OTChannelID
  double uOfStraw(const OTChannelID aChannel) const;

  /// returns z of the straw
  double zOfStraw(const int iStraw, const int iModule) const;  

  /// returns z of OTChannelID
  double zOfStraw(const OTChannelID aChannel) const;

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

  /// get the next channelID
  OTChannelID nextChannel( OTChannelID aChannel) const;

private:

  /// (x,y) -> (u,v) transformation
  void xy2uv(const double x, const double y, 
             double& u, double& v) const;

  /// (u,v) -> (x,y) transformation
  void uv2xy(const double u, const double v, 
             double& x, double& y) const;

  /// calculates of zc,uc of the center and radius 
  ///  rc of the circle (special assumptions)
  void sCircle(const double z1, const double u1, const double z2, 
               const double u2, const double dz,
               double& zc, double& uc, double& rc);

  /// Calculate which module and straws are hit
  void hitModuleAndStraw( double u, double v,
                          int& hitMod, int& hitStrA, int& hitStrB) const;


  int     m_layerID;         ///< index of layer in this station
  int     m_stationID;       ///< index of station the layer belongs to;
  double  m_zOfLayer;        ///< z of the center of layer
  int     m_halfNumModule;   ///< half a number of modules 
                             ///< ( = nr of top modules = nr of bottom modules)
  int*    m_halfNumStraw;    ///< array of nr of straws in one monolayer
  double* m_xFirstStrawA ;   ///< array of x-positions of first straw in module
                             ///< relative to the center of module
  double* m_xModuleCenter;   ///< array of x-coordinates of module centers
  double* m_yModuleCenter;   ///< array of y-coordinates of module centers
  double* m_xModuleSize;     ///< array of x-sizes of module centers
  double* m_yModuleSize;     ///< array of y-sizes of module centers
  double  m_pitchStraw;      ///< distance between straws
  double  m_cellRadius;      ///< straw radius
  double  m_stereoAngle;     ///< stereo angle
  double  m_sinAngle;        ///< sin of the stereo angle
  double  m_cosAngle;        ///< cos of the stereo angle
  double m_inefficientRegion;///< size of inefficient region around y=0

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline int OTLayer::nStrawsInModule(const int iModule) const {
 
  int nStraw = 0;
  if (iModule<(2*m_halfNumModule+1)){
    nStraw = 2*m_halfNumStraw[iModule];
  }
  return nStraw;
}

inline OTChannelID OTLayer::nextChannel( OTChannelID aChannel) const 
{
  unsigned int iModule  = aChannel.module();
  unsigned int iLayer   = aChannel.layer();
  int iStraw = aChannel.straw();

  if (iStraw < this->nStrawsInModule(iModule)) {
    ++iStraw;
  }
  else if (iModule < this->nbModules()) {
    ++iModule;
    iStraw = 1;
  }
  else {
    ++iLayer;
    iStraw = 1;
    iModule = 1;
  }
  return OTChannelID(aChannel.station(), iLayer, iModule, iStraw);
}

#endif // OTDET_OTLAYER_H

