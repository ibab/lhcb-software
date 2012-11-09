// $Id: RichG4ReconFlatMirr.h,v 1.5 2009-10-19 15:48:26 silviam Exp $
#ifndef GAUSSRICH_RICHG4RECONFLATMIRR_H
#define GAUSSRICH_RICHG4RECONFLATMIRR_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/ISolid.h"

/** @class RichG4ReconFlatMirr RichG4ReconFlatMirr.h RichAnalysis/RichG4ReconFlatMirr.h
 *
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-11
 */
class RichG4ReconFlatMirr {

public:

  /// Standard constructor
  RichG4ReconFlatMirr( );

  virtual ~RichG4ReconFlatMirr( ); ///< Destructor
  void setRich1FlatMirrorParam( );
  void setRich2FlatMirrorParam( );

  Gaudi::XYZPoint FlatMirrorReflect(const Gaudi::XYZPoint & HitCoordQw ,
                               int FlatMirrorType);

  Gaudi::XYZPoint FlatMirrorIntersection( const Gaudi::XYZPoint & aGlobalPoint1,
                                          const Gaudi::XYZPoint & aGlobalPoint2,
                                          int aRichDetNum, int aFlatMirrorNum) ;
  
 Gaudi::XYZPoint ConvertToGlobal(const Gaudi::XYZPoint & aLocalPoint,
                                int aRichDetNum, int aMirrorNum );
  
Gaudi::XYZPoint ConvertToLocal(const Gaudi::XYZPoint & aGlobalPoint, 
                               int aRichDetNum, int aMirrorNum ) ;
  
  const SolidSphere* getCurMirrorSolid (int RichDetNum, int aMirrorNum ) ;
  
  DetectorElement* getMirrorDetElem (int RichDetNum, int aMirrorNum );

  Gaudi::XYZPoint   FlatMirrorCoC( int  RichDetNum, int aMirrorNum );

protected:

private:

  int m_RichNumFlatMirrorOrientationParam;
  int m_RichNumFlatMirrorLocationTypes;
  std::vector<std::vector<double> > m_RichFlatMirrorNominalOrientation;
  int m_RichNumSecMirrorCoCParam;
  int m_Rich1NumSecMirror;
  int m_Rich2NumSecMirror;

  std::vector<std::vector<double> >m_RichSecMirrCoCRad;
  std::string m_Rich1Mirror2CommonPath;
  std::string m_Rich2Mirror2CommonPath;
  std::vector<std::string> m_Rich1Mirror2IndPathString;
  std::vector<std::string> m_Rich2Mirror2IndPathString;
  
};
#endif // GAUSSRICH_RICHG4RECONFLATMIRR_H
