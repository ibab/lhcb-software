// $Id: RichG4CkvRecon.h,v 1.1 2004-02-04 13:52:59 seaso Exp $
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

class RichG4CkvRecon {
public:

    /// Standard constructor
  RichG4CkvRecon( ); 

  virtual ~RichG4CkvRecon( ); ///< Destructor


  void SetCurrentLocalHitCoord( double xhit, 
                                double yhit, double zhit);
  void SetCurrentEmissPt( double xEmiss, double yEmiss, 
                          double zEmiss);
  void SetCurrentTkMom( double xMom, double yMom,
                        double zMom);
  void SetCurrentRichDetNum(int aRichDetNum) ;
  
  void SetCurrentHpdNum( int aHpdNum);
  
  void SetCurrentFlatMirrorType(int aType);
  
  void SetCurrentRichSector(int aSectorNum);

  void SetcurDetPoint (HepPoint3D aDetPoint );
  
  void SetcurReflPt (  HepPoint3D aReflPt );
  
  HepPoint3D ReconPhCoordFromLocalCoord( HepPoint3D aLocalHitCoord);
  HepPoint3D ReconReflectionPointOnSPhMirror(HepPoint3D aDetectionPoint,
            HepPoint3D aEmissionPoint );

  HepPoint3D ReconReflectionPointOnSPhMirrorStdInput();
  std::vector<std::complex<double> > SolveQuartic (double denom,  
                                                  double a[4]);
  
  HepPoint3D GetSiHitCoordFromPixelNum(int aPx, int aPy);
  HepPoint3D getPhotAgelExitZ( double ex, double ey, double ez, 
         RichG4Hit* bHit);
  
    
  int  CurrentFlatMirrorType() ;
  int  NumRichDet() 
  { return m_NumRichDet;
  
  }
  int   CurrentRichSector() 
  {
    return m_CurrentRichSector;
  }
  
  std::vector<int> NumHpdRich() 
  {
    return  m_NumHpdRich;
  }

  double CherenkovThetaFromReflPt(HepPoint3D aReflPoint, 
        HepPoint3D aEmisPt );
  RichG4ReconHpd* getRichG4ReconHpd() 
  {
    return m_RichG4ReconHpd;
  }

  void SetChTrackPreStepPosition( double xprepos,
                                  double yprepos,
                                  double zprepos);
  
  HepPoint3D  ChTrackPreStepPosition() 
  {
    return m_ChTrackPreStepPosition;
    
  }
  void SetChTrackPostStepPosition( double xpostpos,
                                  double ypostpos,
                                   double zpostpos);
  
 
  HepPoint3D  ChTrackPostStepPosition() 
  {
    return m_ChTrackPostStepPosition;
    
  }
     
  double CherenkovThetaInAerogel(HepPoint3D aReflPoint,
         HepPoint3D aEmisPt );
  
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
