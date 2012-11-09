// $Id: RichG4CkvRecon.h,v 1.6 2008-05-30 13:43:23 gcorti Exp $
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

#include "GaussRICH/RichG4SvcLocator.h"

#include <CLHEP/Vector/ThreeVector.h>

#include "RichG4ReconTransformHpd.h"
#include "GaussRICH/RichG4TransformPhDet.h"
#include "RichG4ReconHpd.h"
#include "GaussRICH/RichG4ReconFlatMirr.h"
#include "GaussRICH/RichG4Hit.h"
#include <complex>
#include "GaussRICH/RichSolveQuarticEqn.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

class RichG4CkvRecon {
public:

  /// Standard constructor
  RichG4CkvRecon( );

  virtual ~RichG4CkvRecon( ); ///< Destructor


  void SetCurrentLocalHitCoord( const double xhit,
                                const double yhit, const double zhit)
  {
    m_curLocalHitCoord =  Gaudi::XYZPoint(xhit,yhit,zhit);
  }

  void SetCurrentEmissPt( const double xEmiss, const double yEmiss,
                          const double zEmiss)
  {
    m_curEmisPt= Gaudi::XYZPoint(xEmiss,yEmiss, zEmiss);
  }

  void SetCurrentTkMom( const double xMom, const double yMom,
                        const double zMom)
  {
    m_curTkMom=  Gaudi::XYZVector( xMom, yMom,zMom);
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

  void SetcurDetPoint (const Gaudi::XYZPoint & aDetPoint )
  {
    m_curDetPoint = aDetPoint;
  }

  void SetcurReflPt ( const Gaudi::XYZPoint & aReflPt )
  {
    m_curReflPt = aReflPt;
  }

  Gaudi::XYZPoint ReconPhCoordFromLocalCoord( const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr );
  Gaudi::XYZPoint ReconPhDetPlaneCoordFromLocalCoord( const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr );

  Gaudi::XYZPoint ReconReflectionPointOnSPhMirror(const Gaudi::XYZPoint & aDetectionPoint,
                                             const Gaudi::XYZPoint & aEmissionPoint , const Gaudi::XYZPoint & aQwPoint,
                                             int aRichDetNum, int aFlatMirrNum  );

  Gaudi::XYZPoint ReconReflectionPointOnSPhMirrorStdInput();
  //  void SolveQuartic ( std::vector<std::complex<double> > & z,
  //                    double denom,
  //                    double a[4] );

  void SolveQuartic ( gsl_complex  z[4],
                      double denom,
                      double a[4] );

  Gaudi::XYZPoint GetSiHitCoordFromPixelNum(int aPx, int aPy);
  Gaudi::XYZPoint getPhotAgelExitZ( double ex, double ey, double ez,
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

  double CherenkovThetaFromReflPt(const Gaudi::XYZPoint & aReflPoint,
                                  const Gaudi::XYZPoint & aEmisPt );

  RichG4ReconHpd* getRichG4ReconHpd()
  {
    return m_RichG4ReconHpd;
  }

  void SetChTrackPreStepPosition( double xprepos,
                                  double yprepos,
                                  double zprepos);

  const Gaudi::XYZPoint & ChTrackPreStepPosition() const
  {
    return m_ChTrackPreStepPosition;
  }

  void SetChTrackPostStepPosition( double xpostpos,
                                   double ypostpos,
                                   double zpostpos);

  const Gaudi::XYZPoint  & ChTrackPostStepPosition() const
  {
    return m_ChTrackPostStepPosition;
  }

  double CherenkovThetaInAerogel(const Gaudi::XYZPoint & aReflPoint,
                                 const Gaudi::XYZPoint & aEmisPt,
                                 const int tilenum, const int subtilenum  );

  RichG4ReconFlatMirr* getCurReconFlatMirr() {
    return m_CurReconFlatMirr;}
  void setCurReconFlatMirr( RichG4ReconFlatMirr* aCurReconFlatMirr) {
    m_CurReconFlatMirr = aCurReconFlatMirr;}
  RichG4TransformPhDet* getCurPhDetTrans(int aSect) 
  {
    return m_PhDetTransforms[aSect];
  }

  Gaudi::XYZPoint   ApplyQwRefrCorrAndGetGlobalPos(const Gaudi::XYZPoint & aPhCathPoint );
  
protected:

private:


  int m_NumRichDet;
  std::vector<int> m_NumHpdRich;

  std::vector<std::vector<RichG4ReconTransformHpd*> > m_HpdTransforms;

  std::vector<RichG4TransformPhDet*>  m_PhDetTransforms;
  
  std::vector<std::vector<double> > m_SphMirrCC;
  std::vector<double> m_SphMirrRad;
  RichG4ReconHpd* m_RichG4ReconHpd;


  Gaudi::XYZPoint m_curLocalHitCoord;
  Gaudi::XYZPoint m_curEmisPt;
  Gaudi::XYZVector m_curTkMom;
  Gaudi::XYZVector m_curGlobalHitPhCath;

  Gaudi::XYZPoint m_curDetPoint;
  Gaudi::XYZPoint m_curReflPt;


  int m_CurrentRichDetNum;
  int m_CurrentHpdNum;
  int m_CurrentFlatMirrorType;
  int m_CurrentRichSector;

  double m_HpdSiDetThickness;
  double  m_HpdSiPixelXSize;
  double  m_HpdSiPixelYSize;
  int  m_HpdSiNumPixelX;
  int m_HpdSiNumPixelY;
  Gaudi::XYZPoint m_ChTrackPreStepPosition;
  Gaudi::XYZPoint  m_ChTrackPostStepPosition;

  double m_c4f10nominalrefrativeindex;
  double m_agelnominalrefractiveindex;
  RichG4ReconFlatMirr* m_CurReconFlatMirr;
  std::vector<double> m_AgelNominalTileRefIndex;
  std::vector<std::vector<double> >m_AgelNominalSubTileRefIndex;
  double m_AgelMeanNominalTileRefIndex;
  
};

#endif // RICHANALYSIS_RICHG4CKVRECON_H
