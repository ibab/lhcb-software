// $Id: CkvG4ReconFlatMirr.h,v 1.5 2009-10-19 15:48:26 silviam Exp $
#ifndef GAUSSCHERENKOV_CKVG4RECONFLATMIRR_H
#define GAUSSCHERENKOV_CKVG4RECONFLATMIRR_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/ISolid.h"

/** @class CkvG4ReconFlatMirr CkvG4ReconFlatMirr.h RichAnalysis/CkvG4ReconFlatMirr.h
 *
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-11
 */
class CkvG4ReconFlatMirr {

public:

  /// Standard constructor
  CkvG4ReconFlatMirr( );

  virtual ~CkvG4ReconFlatMirr( ); ///< Destructor
  void setRich1FlatMirrorParam( );
  void setRich2FlatMirrorParam( );

  Gaudi::XYZPoint FlatMirrorReflect(const Gaudi::XYZPoint & HitCoordQw ,
                                    int FlatMirrorType);

  double DistFromFlatMirror( const Gaudi::XYZPoint & HitCoordQw ,
                             int FlatMirrorType);
  
  double AngleOnFlatMirror(const Gaudi::XYZPoint& HitCoordQw,
                           const Gaudi::XYZPoint& PtOnFM,
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
  std::vector<std::vector<double> > RichFlatMirrorNominalOrientation() 
  {
    return m_RichFlatMirrorNominalOrientation;
  }
  

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

  bool m_Rich1HorizFlag;
  
};
#endif // GAUSSCHERENKOV_CKVG4RECONFLATMIRR_H
