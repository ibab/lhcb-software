// $Id: RichG4CkvRecon.h,v 1.3 2005-05-09 12:25:36 seaso Exp $
#ifndef RICHANALYSIS_RICHG4CKVRECON_H
#define RICHANALYSIS_RICHG4CKVRECON_H 1

// Include files

/** @class RichG4CkvRecon RichG4CkvRecon.h RichAnalysis/RichG4CkvRecon.h
 *
 *
 *
 *  @author Sajan Easo
 *  @date   2003-09-08
 */

#include <vector>
#include <complex>

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "RichG4SvcLocator.h"

#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Plane3D.h>
#include <CLHEP/Geometry/Vector3D.h>
#include <CLHEP/Geometry/Transform3D.h>
#include <CLHEP/Vector/ThreeVector.h>
#include "RichG4ReconTransformHpd.h"
#include "RichG4ReconHpd.h"
#include "RichG4Hit.h"
#include <complex>
#include "RichSolveQuarticEqn.h"

class RichG4CkvRecon {
public:

  /// Standard constructor
  RichG4CkvRecon( );

  virtual ~RichG4CkvRecon( ); ///< Destructor


  void SetCurrentLocalHitCoord( const double xhit,
                                const double yhit, const double zhit)
  {
    m_curLocalHitCoord =  HepPoint3D(xhit,yhit,zhit);
  }

  void SetCurrentEmissPt( const double xEmiss, const double yEmiss,
                          const double zEmiss)
  {
    m_curEmisPt= HepPoint3D(xEmiss,yEmiss, zEmiss);
  }

  void SetCurrentTkMom( const double xMom, const double yMom,
                        const double zMom)
  {
    m_curTkMom=  HepVector3D( xMom, yMom,zMom);
  }

  void SetCurrentRichDetNum(const int aRichDetNum)
  {
    m_CurrentRichDetNum=aRichDetNum;
  }

  void SetCurrentHpdNum( const int aHpdNum)
  {
    m_CurrentHpdNum= aHpdNum;
  }

  void SetCurrentFlatMirrorType(const int aType)
  {
    m_CurrentFlatMirrorType = aType;
  }

  void SetCurrentRichSector(const int aSectorNum)
  {
    m_CurrentRichSector=  aSectorNum;
  }

  void SetcurDetPoint (const HepPoint3D & aDetPoint )
  {
    m_curDetPoint = aDetPoint;
  }

  void SetcurReflPt ( const HepPoint3D & aReflPt )
  {
    m_curReflPt = aReflPt;
  }

  HepPoint3D ReconPhCoordFromLocalCoord( const HepPoint3D & aLocalHitCoord);
  HepPoint3D ReconReflectionPointOnSPhMirror(const HepPoint3D & aDetectionPoint,
                                             const HepPoint3D & aEmissionPoint );

  HepPoint3D ReconReflectionPointOnSPhMirrorStdInput();
  //  void SolveQuartic ( std::vector<std::complex<double> > & z,
  //                    double denom,
  //                    double a[4] );

  void SolveQuartic ( gsl_complex  z[4],
                      double denom,
                      double a[4] );

  HepPoint3D GetSiHitCoordFromPixelNum(int aPx, int aPy);
  HepPoint3D getPhotAgelExitZ( double ex, double ey, double ez,
                               RichG4Hit* bHit);


  int  CurrentFlatMirrorType() const
  {
    return m_CurrentFlatMirrorType ;
  }

  int  NumRichDet() const
  { return m_NumRichDet;

  }
  int CurrentRichSector() const
  {
    return m_CurrentRichSector;
  }

  const std::vector<int> & NumHpdRich() const
  {
    return  m_NumHpdRich;
  }

  double CherenkovThetaFromReflPt(const HepPoint3D & aReflPoint,
                                  const HepPoint3D & aEmisPt );

  RichG4ReconHpd* getRichG4ReconHpd()
  {
    return m_RichG4ReconHpd;
  }

  void SetChTrackPreStepPosition( double xprepos,
                                  double yprepos,
                                  double zprepos);

  const HepPoint3D & ChTrackPreStepPosition() const
  {
    return m_ChTrackPreStepPosition;
  }

  void SetChTrackPostStepPosition( double xpostpos,
                                   double ypostpos,
                                   double zpostpos);

  const HepPoint3D  & ChTrackPostStepPosition() const
  {
    return m_ChTrackPostStepPosition;
  }

  double CherenkovThetaInAerogel(const HepPoint3D & aReflPoint,
                                 const HepPoint3D & aEmisPt );

protected:

private:


  int m_NumRichDet;
  std::vector<int> m_NumHpdRich;

  std::vector<std::vector<RichG4ReconTransformHpd*> > m_HpdTransforms;

  std::vector<std::vector<double> > m_SphMirrCC;
  std::vector<double> m_SphMirrRad;
  RichG4ReconHpd* m_RichG4ReconHpd;


  HepPoint3D m_curLocalHitCoord;
  HepPoint3D m_curEmisPt;
  HepVector3D m_curTkMom;
  HepVector3D m_curGlobalHitPhCath;

  HepPoint3D m_curDetPoint;
  HepPoint3D m_curReflPt;


  int m_CurrentRichDetNum;
  int m_CurrentHpdNum;
  int m_CurrentFlatMirrorType;
  int m_CurrentRichSector;

  double m_HpdSiDetThickness;
  double  m_HpdSiPixelXSize;
  double  m_HpdSiPixelYSize;
  int  m_HpdSiNumPixelX;
  int m_HpdSiNumPixelY;
  HepPoint3D m_ChTrackPreStepPosition;
  HepPoint3D m_ChTrackPostStepPosition;

  double m_c4f10nominalrefrativeindex;
  double m_agelnominalrefractiveindex;
  
};

#endif // RICHANALYSIS_RICHG4CKVRECON_H
