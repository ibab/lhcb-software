// $Id: OTLayer.cpp,v 1.1.1.1 2002-05-26 22:36:33 jvantilb Exp $
//
// This File contains the definition of the OTLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Witek
//   Created: 11-11-2000
//   Original class name: OTDetectionLayer

#include <algorithm> 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"

// LHCbKernel
#include "Kernel/OTChannelID.h"

// OTSicbGeom
#include "OTDet/OTLayer.h"


OTLayer::OTLayer(int iLayer, int iStation, double zLayer, 
                                   double xCen,  double yCen, 
                                   double xSide, double ySide,
                                   double xOut,  double yOut, 
                                   int nStandStraw, double pitchStraw, 
                                   double stereoAngle ) : 
      m_LayerID(iLayer), 
      m_StationID(iStation), 
      m_ZOfLayer(zLayer), 
      m_PitchStraw(pitchStraw),  
      m_CellRadius(2.5*mm),
      m_StereoAngle(stereoAngle*degree)
{
  m_SinAngle = sin(stereoAngle*degree);  
  m_CosAngle = cos(stereoAngle*degree);  

  // calculate number of modules, width and pitch of various module types

  int    nStraw1   = (int) (2.*xCen/pitchStraw-0.5);
  double pitchMod1 = (nStraw1+1.0)*pitchStraw;

  int    nStraw2   = (int) ((xSide - xCen)/pitchStraw-0.5);
  double pitchMod2 = (nStraw2+1.0)*pitchStraw;

  int    nStraw3 = nStandStraw;
  double pitchMod3 = (nStraw3+1.0)*pitchStraw;

  double xStart = (pitchMod1/2.) + pitchMod2;   
  double xStop  = xOut;

  int    nMod3 = (int) ( (xStop - xStart) / pitchMod3 );

  double xEnd = pitchMod1/2. + pitchMod2 + nMod3*pitchMod3;

  int    nStraw4   = (int) ((xOut - xEnd)/pitchStraw-0.5);
  double pitchMod4 = (nStraw4+1.0)*pitchStraw;

  double xshift,xcenter,ycenter;

  // half nr of modules = 
  // 1 central + 2 wing + 2 end + 2* n_standard 
  m_HalfNumModule = 1 + 2 + 2 + 2 * nMod3;  // always odd number  

  m_HalfNumStraw  = new    int[2*m_HalfNumModule+1];
  m_XFirstStrawA  = new double[2*m_HalfNumModule+1];
  m_XModuleCenter = new double[2*m_HalfNumModule+1];
  m_YModuleCenter = new double[2*m_HalfNumModule+1];
  m_XModuleSize   = new double[2*m_HalfNumModule+1];
  m_YModuleSize   = new double[2*m_HalfNumModule+1];

  // fill arrays ( modules are counted from 1 to 2*m_HalfNumModule )
  m_HalfNumStraw[0]  = 0;
  m_XFirstStrawA[0]  = 0.0;
  m_XModuleCenter[0] = 0.0;  
  m_YModuleCenter[0] = 0.0;
  m_XModuleSize[0]   = 0.0;
  m_YModuleSize[0]   = 0.0;

  // central modules top and bottom
  int iCenBot = m_HalfNumModule/2 + 1; 
  int iCenTop = iCenBot+m_HalfNumModule;

  m_HalfNumStraw[iCenTop] = nStraw1;
  m_HalfNumStraw[iCenBot] = nStraw1;

  m_XModuleSize[iCenTop]  = pitchMod1; 
  m_XModuleSize[iCenBot]  = pitchMod1 ; 

  m_YModuleSize[iCenTop]  = yOut-yCen; 
  m_YModuleSize[iCenBot]  = yOut-yCen; 

  m_XModuleCenter[iCenTop] = -m_SinAngle * (yCen+yOut)/2.;
  m_XModuleCenter[iCenBot] = -m_XModuleCenter[iCenTop];

  m_YModuleCenter[iCenTop] = m_CosAngle * (yCen+yOut)/2.;
  m_YModuleCenter[iCenBot] = -m_YModuleCenter[iCenTop];


  // wing modules
  int iRightUp   = iCenTop + 1;
  int iRightDown = iCenBot + 1;
  int iLeftUp    = iCenTop - 1;
  int iLeftDown  = iCenBot - 1;

  m_HalfNumStraw[iRightUp  ] = nStraw2;
  m_HalfNumStraw[iRightDown] = nStraw2;
  m_HalfNumStraw[iLeftUp   ] = nStraw2;
  m_HalfNumStraw[iLeftDown ] = nStraw2;

  m_XModuleSize[iRightUp  ]  = pitchMod2; 
  m_XModuleSize[iRightDown]  = pitchMod2; 
  m_XModuleSize[iLeftUp   ]  = pitchMod2; 
  m_XModuleSize[iLeftDown ]  = pitchMod2; 

  m_YModuleSize[iRightUp  ]  = yOut-ySide; 
  m_YModuleSize[iRightDown]  = yOut-ySide; 
  m_YModuleSize[iLeftUp   ]  = yOut-ySide; 
  m_YModuleSize[iLeftDown ]  = yOut-ySide;
 
  xshift = -m_SinAngle * (ySide+yOut)/2.;
  xcenter = pitchMod1/2. + pitchMod2/2.;
  m_XModuleCenter[iRightUp  ] =  xcenter + xshift;
  m_XModuleCenter[iRightDown] =  xcenter - xshift;
  m_XModuleCenter[iLeftUp   ] = -xcenter + xshift;
  m_XModuleCenter[iLeftDown ] = -xcenter - xshift;

  ycenter = m_CosAngle * (ySide+yOut)/2.;
  m_YModuleCenter[iRightUp  ] =  ycenter;
  m_YModuleCenter[iRightDown] = -ycenter;
  m_YModuleCenter[iLeftUp   ] =  ycenter;
  m_YModuleCenter[iLeftDown]  = -ycenter;

  // standard modules
  int iMod;
  for (iMod=1; iMod <= nMod3; iMod++) {

    iRightUp   = iCenTop + 1 + iMod;
    iRightDown = iCenBot + 1 + iMod;
    iLeftUp    = iCenTop - 1 - iMod;
    iLeftDown  = iCenBot - 1 - iMod;
  
    m_HalfNumStraw[iRightUp  ] = nStraw3;
    m_HalfNumStraw[iRightDown] = nStraw3;
    m_HalfNumStraw[iLeftUp   ] = nStraw3;
    m_HalfNumStraw[iLeftDown ] = nStraw3;
  
    m_XModuleSize[iRightUp  ]  = pitchMod3; 
    m_XModuleSize[iRightDown]  = pitchMod3; 
    m_XModuleSize[iLeftUp   ]  = pitchMod3; 
    m_XModuleSize[iLeftDown ]  = pitchMod3; 
  
    m_YModuleSize[iRightUp  ]  = yOut; 
    m_YModuleSize[iRightDown]  = yOut; 
    m_YModuleSize[iLeftUp   ]  = yOut; 
    m_YModuleSize[iLeftDown ]  = yOut;
   
    xshift = -m_SinAngle * yOut/2.;
    xcenter = pitchMod1/2.+pitchMod2 + (iMod-0.5)*pitchMod3;
    m_XModuleCenter[iRightUp  ] =  xcenter + xshift;
    m_XModuleCenter[iRightDown] =  xcenter - xshift;
    m_XModuleCenter[iLeftUp   ] = -xcenter + xshift;
    m_XModuleCenter[iLeftDown ] = -xcenter - xshift;
  
    ycenter = m_CosAngle * yOut/2.;
    m_YModuleCenter[iRightUp  ] =  ycenter;
    m_YModuleCenter[iRightDown] = -ycenter;
    m_YModuleCenter[iLeftUp   ] =  ycenter;
    m_YModuleCenter[iLeftDown]  = -ycenter;
  
 }

  // end modules
  iRightUp   = iCenTop + 1 + nMod3 +1;
  iRightDown = iCenBot + 1 + nMod3 +1; 
  iLeftUp    = iCenTop - 1 - nMod3 -1;
  iLeftDown  = iCenBot - 1 - nMod3 -1;

  m_HalfNumStraw[iRightUp  ] = nStraw4;
  m_HalfNumStraw[iRightDown] = nStraw4;
  m_HalfNumStraw[iLeftUp   ] = nStraw4;
  m_HalfNumStraw[iLeftDown ] = nStraw4;

  m_XModuleSize[iRightUp  ]  = pitchMod4; 
  m_XModuleSize[iRightDown]  = pitchMod4; 
  m_XModuleSize[iLeftUp   ]  = pitchMod4; 
  m_XModuleSize[iLeftDown ]  = pitchMod4; 

  m_YModuleSize[iRightUp  ]  = yOut; 
  m_YModuleSize[iRightDown]  = yOut; 
  m_YModuleSize[iLeftUp   ]  = yOut; 
  m_YModuleSize[iLeftDown ]  = yOut;
 
  xshift = -m_SinAngle * yOut/2.;
  xcenter = pitchMod1/2. + pitchMod2 + nMod3*pitchMod3 + pitchMod4/2.;
  m_XModuleCenter[iRightUp  ] =  xcenter + xshift;
  m_XModuleCenter[iRightDown] =  xcenter - xshift;
  m_XModuleCenter[iLeftUp   ] = -xcenter + xshift;
  m_XModuleCenter[iLeftDown ] = -xcenter - xshift;

  ycenter = m_CosAngle * yOut/2.;
  m_YModuleCenter[iRightUp  ] =  ycenter;
  m_YModuleCenter[iRightDown] = -ycenter;
  m_YModuleCenter[iLeftUp   ] =  ycenter;
  m_YModuleCenter[iLeftDown]  = -ycenter;

  
  for (iMod=1; iMod<=2*m_HalfNumModule; iMod++) {
     m_XFirstStrawA[iMod] = -(((double) m_HalfNumStraw[iMod])/2. + 0.25)*
                                                           m_PitchStraw;
     m_XModuleSize[iMod]*=m_CosAngle;
  }

}


OTLayer::~OTLayer()
{
  // destructer
  delete[] m_HalfNumStraw;
  delete[] m_XFirstStrawA;
  delete[] m_XModuleCenter;
  delete[] m_YModuleCenter;
  delete[] m_XModuleSize;
  delete[] m_YModuleSize;
}

int OTLayer::getAbsLayerID() const {

  return (100*m_StationID)+m_LayerID;
}

int OTLayer::getLayerID() const {

  return m_LayerID;
}

int OTLayer::getStationID() const {

  return m_StationID;
}

double OTLayer::getZ() const {
 
  return m_ZOfLayer;
}

double OTLayer::getStereoAngle() const{
  return m_StereoAngle;
}

double OTLayer::getCellRadius() const {
  return m_CellRadius;
}

int OTLayer::nStrawsInModule(const int iModule) const {
 
  int nStraw = 0;
  if (iModule<(2*m_HalfNumModule+1)){
    nStraw = 2*m_HalfNumStraw[iModule];
  }
  return nStraw;
}


bool OTLayer::calculateHits( HepPoint3D entryPoint,
                                      HepPoint3D exitPoint,
                                      std::vector<OTChannelID>& channels,
                                      std::vector<double>& driftDistances) {

  double x1,y1,z1;
  double x2,y2,z2;
  double u1,v1;
  double u2,v2;
  double uLow,uHigh;
  double distXY;
  int iHitModuleA,iHitModuleB,iHitStrawA,iHitStrawB,iDummy; 

  x1=entryPoint[0];
  y1=entryPoint[1];
  z1=entryPoint[2];

  x2=exitPoint[0];
  y2=exitPoint[1];
  z2=exitPoint[2];

  distXY=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

  // ensure correct z ordering
  if (entryPoint.z()> exitPoint.z()) {
    std::swap(x1,x2);
    std::swap(y1,y2);
    std::swap(z1,z2);
  }

  xy2uv(x1, y1, u1, v1);
  xy2uv(x2, y2, u2, v2);

  if(u1<u2) {
    uLow  = u1;
    uHigh = u2;
  }
  else {
    uLow  = u2;
    uHigh = u1;
  }

  // check if input is resonable
  if( fabs(z1-z2)<0.5 && distXY<5. ) {
    return false;
  }
  if( fabs((z1+z2)/2.-m_ZOfLayer)>2.0*m_CellRadius ) {
    return false;
  }


  if (fabs(z1-z2)>0.5*mm) {

    double dUdZ=(u2-u1)/(z2-z1);  
  
    double uAtMonoA = u1 + dUdZ*(m_ZOfLayer-0.5*m_PitchStraw-z1);  
    double uAtMonoB = u1 + dUdZ*(m_ZOfLayer+0.5*m_PitchStraw-z1);
  
    // calculate which module was hit

    double uMiddle=(u1+u2)/2.;
    double vMiddle=(v1+v2)/2.;
    hitModuleAndStraw(uMiddle,vMiddle,iHitModuleA,iHitStrawA,iDummy);
    hitModuleAndStraw(uMiddle,vMiddle,iHitModuleB,iDummy,iHitStrawB);

    double cosTrack=1.0/sqrt(1.0+dUdZ*dUdZ);
  
    double distTmp;
    int iRightStraw,iRightModule;
    int iLeftStraw,iLeftModule;
    double uStraw;
    int iModule;
    int iStraw;
    int iHiStr, iHiMod;
    bool betweenU;
  
    // calculate how many straws was hit left or right
    int nSideStraws = (int) (fabs(dUdZ)/2. + 1.0);
    if (nSideStraws > 100) {
      nSideStraws = 100;
    }
  
    // store hits for part A
    if ( iHitModuleA > 0 ) {
      if ((iHitStrawA >= 1) && (iHitStrawA <= m_HalfNumStraw[iHitModuleA])) {
  
  	double uHitStrawA = getU(iHitStrawA,iHitModuleA);
      	distTmp = (uAtMonoA - uHitStrawA) * cosTrack;
     
  	betweenU = ( uHitStrawA > uLow-m_PitchStraw ) && 
  		   ( uHitStrawA < uHigh+m_PitchStraw);
  
  	if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	  iStraw=iHitStrawA;
  	  iModule=iHitModuleA;

          OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                            iModule, iStraw);
          channels.push_back(tmpChan);
          driftDistances.push_back(distTmp);
  	}
    
  	// check adjacent straws (right) 
    
  	iHiStr = iHitStrawA;
  	iHiMod = iHitModuleA;
    
  	for (int iRStraw = 1; iRStraw < nSideStraws + 1; iRStraw++) { 
  	  nextRightStraw( iHiStr, iHiMod, iRightStraw, iRightModule);
  	  if (iRightModule < 0 ) { break; }
  	  uStraw=getU(iRightStraw,iRightModule);
  	  distTmp = (uAtMonoA - uStraw) * cosTrack;
    
  	  iHiStr = iRightStraw;
  	  iHiMod = iRightModule;
    
  	  betweenU = ( uStraw > uLow-m_PitchStraw ) && 
  		     ( uStraw < uHigh+m_PitchStraw);
  
  	  if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	    iStraw=iRightStraw;
  	    iModule=iRightModule;
            OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                              iModule, iStraw);
            channels.push_back(tmpChan);
            driftDistances.push_back(distTmp);
  	  }
  	}
  	// check adjacent straws (left) 
    
  	iHiStr = iHitStrawA;
  	iHiMod = iHitModuleA;
    
  	for (int iLStraw = 1; iLStraw < nSideStraws + 1; iLStraw++) { 
  	  nextLeftStraw( iHiStr, iHiMod, iLeftStraw, iLeftModule);
  	  if (iLeftModule < 0 ) { break; }
  	  uStraw=getU(iLeftStraw,iLeftModule);
  	  distTmp = (uAtMonoA - uStraw) * cosTrack; 
    
  	  iHiStr = iLeftStraw;
  	  iHiMod = iLeftModule;
    
  	  betweenU = ( uStraw > uLow-m_PitchStraw ) && 
  		     ( uStraw < uHigh+m_PitchStraw);
  	  if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	    iStraw=iLeftStraw;
  	    iModule=iLeftModule;
            OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                              iModule, iStraw);
            channels.push_back(tmpChan);
            driftDistances.push_back(distTmp);
  	  }
  	}
      }
    } // layer A    
      
    // store hits for part B
    if ( iHitModuleB > 0 ) {
      if ((iHitStrawB >= m_HalfNumStraw[iHitModuleB]+1) && 
  	  (iHitStrawB <= 2*m_HalfNumStraw[iHitModuleB])) {
    
  	double uHitStrawB = getU(iHitStrawB,iHitModuleB);
  
  	distTmp = (uAtMonoB - uHitStrawB) * cosTrack;
    
  	betweenU = ( uHitStrawB > uLow-m_PitchStraw ) && 
  		   ( uHitStrawB < uHigh+m_PitchStraw);
  	if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	  iStraw=iHitStrawB;
  	  iModule=iHitModuleB;
          OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                            iModule, iStraw);
          channels.push_back(tmpChan);
          driftDistances.push_back(distTmp);
  	}
    
  	// check adjacent straws taking care about module change 
    
  	iHiStr = iHitStrawB;
  	iHiMod = iHitModuleB;
    
  	for (int iRStraw = 1; iRStraw < nSideStraws + 1; iRStraw++) { 
  	  nextRightStraw( iHiStr, iHiMod, iRightStraw, iRightModule);
  	  if (iRightModule < 0 ) { break; }
  	  uStraw=getU(iRightStraw,iRightModule); 
  	  distTmp = (uAtMonoB - uStraw) * cosTrack; 
    
  	  iHiStr = iRightStraw;
  	  iHiMod = iRightModule;
    
  	  betweenU = ( uStraw > uLow-m_PitchStraw ) && 
  		     ( uStraw < uHigh+m_PitchStraw);
  	  if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	    iStraw=iRightStraw;
  	    iModule=iRightModule;
            OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                              iModule, iStraw);
            channels.push_back(tmpChan);
            driftDistances.push_back(distTmp);
  	  }
  	}
    
  	iHiStr = iHitStrawB;
  	iHiMod = iHitModuleB;
    
  	for (int iLStraw = 1; iLStraw < nSideStraws + 1; iLStraw++) { 
  	  nextLeftStraw( iHiStr, iHiMod, iLeftStraw, iLeftModule);
  	  if (iLeftModule < 0 ) { break; }
  	  uStraw=getU(iLeftStraw,iLeftModule);
  	  distTmp = (uAtMonoB - uStraw) * cosTrack;
    
  	  iHiStr = iLeftStraw;
  	  iHiMod = iLeftModule;
    
  	  betweenU = ( uStraw > uLow-m_PitchStraw ) && 
  		     ( uStraw < uHigh+m_PitchStraw);
  
  	  if ( (fabs(distTmp) < m_CellRadius) && betweenU ) {
  	    iStraw=iLeftStraw;
  	    iModule=iLeftModule;
            OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                              iModule, iStraw);
            channels.push_back(tmpChan);
            driftDistances.push_back(distTmp);
  	  }
  	} // loop straws
      }    
    } // part B
  } // dz > 0.5 mm
  else {
  // dz < 0.5 mm , treatment of curling tracks

    double z3Circ,zCirc,uCirc,rCirc;

    //    z3Circ = m_ZOfLayer;
    double zfrac,zint;
    // zfrac is between 0 and 1. 2.7839542167 means nothing.
    // if distance xy entry-exit is small generate z3 close
    // to the z1 and z2 ( z1 is close to z2)
    zfrac = modf( (fabs(u1)+fabs(v1))/2.7839542167, &zint);
    if (distXY > 2.0 *m_PitchStraw ) {
      z3Circ = m_ZOfLayer + 2.0*(zfrac-0.5)*m_PitchStraw;
    }
    else {
      if ( z1-m_ZOfLayer < 0 ) {
        z3Circ = m_ZOfLayer - zfrac*m_PitchStraw; 
      } 
      else {
        z3Circ = m_ZOfLayer + zfrac*m_PitchStraw; 
      }
    }

    sCircle(z1, u1, z2, u2, z3Circ, zCirc, uCirc, rCirc);

    // monolayer A
    int iStrawA;
    int iModuleA;
    hitModuleAndStraw(uLow,v1,iModuleA,iStrawA,iDummy);
    double zStrawA = getZofStraw(iStrawA,iModuleA);
    double uStep = uLow;
    int iCount = 0;

    while ( (uStep < uHigh) && (iCount < 100) && (iModuleA > 0) ) {
      iCount++;
      uStep = getU(iStrawA,iModuleA);
      double distCirc = 
         sqrt((zCirc-zStrawA)*(zCirc-zStrawA) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int iStraw  = iStrawA;
      int iModule = iModuleA;

      double ambTmp = - (uStep-(u1+u2)/2.) * (distCirc-rCirc);
      
      if ( distTmp < m_CellRadius ) {
        if (ambTmp < 0.0 ) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                          iModule, iStraw);
        channels.push_back(tmpChan);
        driftDistances.push_back(distTmp);
      }
      nextRightStraw(iStraw,iModule,iStrawA,iModuleA);
    } // while

    // monolayer B
    int iStrawB;
    int iModuleB;
    hitModuleAndStraw(uLow,v1,iModuleB,iDummy,iStrawB);
    double zStrawB = getZofStraw(iStrawB,iModuleB);
    uStep = uLow;
    iCount = 0;
   
    while ( (uStep < uHigh) && (iCount < 100) && (iModuleB > 0) ) {
      iCount++;
      uStep = getU(iStrawB,iModuleB);
      //double vStep = v1+((v2-v1)/(u2-u1))*(uStep-u1);
      double distCirc = 
        sqrt((zCirc-zStrawB)*(zCirc-zStrawB) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int iModule = iModuleB;
      int iStraw  = iStrawB;

      double ambTmp = - (uStep-(u1+u2)/2.) * (distCirc-rCirc);
      if ( distTmp < m_CellRadius ) {
        if (ambTmp < 0.0) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID(m_StationID, m_LayerID, 
                                          iModule, iStraw);
        channels.push_back(tmpChan);
        driftDistances.push_back(distTmp);
      }
      nextRightStraw(iStraw,iModule,iStrawB,iModuleB);
    } //while


  } // else
  
  return true;

}


void OTLayer::xy2uv(const double x, const double y, 
                             double& u, double& v) const
{
  u =  x*m_CosAngle + y*m_SinAngle;
  v = -x*m_SinAngle + y*m_CosAngle;
}

void OTLayer::uv2xy(const double u, const double v, 
                             double& x, double& y) const
{
  x =  u*m_CosAngle - v*m_SinAngle;
  y =  u*m_SinAngle + v*m_CosAngle;
}

void OTLayer::nextRightStraw(int iHitStraw,int iHitModule,
                                      int& iRightStraw,int& iRightModule) const {

  if( (iHitModule == m_HalfNumModule || iHitModule == 2*m_HalfNumModule ) &&
      (iHitStraw  == m_HalfNumStraw[iHitModule] || 
                                   iHitStraw == 2*m_HalfNumStraw[iHitModule])) {
    iRightModule = -9999;
    iRightStraw  = -9999;
  }
  else if( iHitStraw == m_HalfNumStraw[iHitModule] ) {
    iRightModule = iHitModule + 1 ;
    iRightStraw  = 1;
  }
  else if( iHitStraw == 2*m_HalfNumStraw[iHitModule] ) {
    iRightModule = iHitModule + 1 ;
    iRightStraw  = m_HalfNumStraw[iRightModule]+1;
  }
  else {
    iRightStraw  = iHitStraw + 1;
    iRightModule = iHitModule;
  }
}

void OTLayer::nextLeftStraw(int iHitStraw,int iHitModule,
                                     int& iLeftStraw,int& iLeftModule) const {

  if( (iHitModule == 1 || iHitModule == m_HalfNumModule+1 ) &&
      (iHitStraw  == 1 || iHitStraw == m_HalfNumStraw[iHitModule]+1)) {
    iLeftModule = -9998;
    iLeftStraw  = -9998;
  }
  else if( iHitStraw == 1 ) {
    iLeftModule = iHitModule - 1 ;
    iLeftStraw  = m_HalfNumStraw[iLeftModule];
  }
  else if( iHitStraw == m_HalfNumStraw[iHitModule]+1 ) {
    iLeftModule = iHitModule - 1 ;
    iLeftStraw  = 2*m_HalfNumStraw[iLeftModule];
  }
  else {
    iLeftStraw  = iHitStraw - 1;
    iLeftModule = iHitModule;
  }
}

double OTLayer::getU(const int iStraw, const int iModule) const{

  int locStraw = iStraw;
  double uFirst=m_XFirstStrawA[iModule];

  if (iStraw > m_HalfNumStraw[iModule]) {
    uFirst += 0.5*m_PitchStraw;
    locStraw-=m_HalfNumStraw[iModule];
  }

  double uCen,vCen;
  this->xy2uv(m_XModuleCenter[iModule],m_YModuleCenter[iModule],uCen,vCen);

  return ( uCen + uFirst + (locStraw-1)*m_PitchStraw);

}

double OTLayer::getZofStraw(const int iStraw, const int iModule) const {

  if (( iStraw < 1 ) || ( iStraw > 2*m_HalfNumStraw[iModule] )) {
    return  -99999.; 
  }

  if ( iStraw <= m_HalfNumStraw[iModule] ) {
    return (m_ZOfLayer-0.5*m_PitchStraw);
  }
  else {
    return (m_ZOfLayer+0.5*m_PitchStraw);
  }

}

HepPoint3D OTLayer::getXYZofStrawCenter(const int iStraw, const int iModule) const{

  double x,y,z,u,v,uCen,vCen;

  HepPoint3D tmpPoint(0.,0.,0.);

  z = this->getZofStraw(iStraw,iModule);

  xy2uv(m_XModuleCenter[iModule],m_YModuleCenter[iModule],uCen,vCen);

  u = this->getU(iStraw,iModule);
  v = vCen;

  uv2xy(u,v,x,y);

  tmpPoint[0]=x;  
  tmpPoint[1]=y;  
  tmpPoint[2]=z;  

  return tmpPoint;
}

double OTLayer::distanceAlongWire(const int iModule, 
                                           const double xHit, 
                                           const double yHit) const{ 
  
  // distance from x,y to end of wire
  // add check if xHit,yHit indeed inside iModule?

  double uHit,vHit,uCen,vCen;
  xy2uv(xHit,yHit,uHit,vHit);
  xy2uv(m_XModuleCenter[iModule],m_YModuleCenter[iModule],uCen,vCen);

  double dist;
  if ( iModule <= m_HalfNumModule ) {
    // Bottom Module
    dist = vHit - vCen + (0.5*m_YModuleSize[iModule]);
  } else {
    // Top Module
    dist = vCen - vHit + (0.5*m_YModuleSize[iModule]);
  }

  if (dist < 0.0 ) {
    dist = 0.0;
  }
  return dist;
}

void OTLayer::sCircle(const double z1, const double u1, const double z2, 
                               const double u2, const double z3c,
	                       double& zc, double& uc, double& rc) {
  
  double zw,uw;
  
  zw=(z1+z2)/2.;
  uw=(u2+u1)/2.;
  zc=0.5*(z3c*z3c-zw*zw-(u1-uw)*(u1-uw))/(z3c-zw);
  uc=uw;
  rc=fabs(zc-z3c);
}


double OTLayer::wireHalfLength(const int iModule) const{
  // half length of wire
  return (m_YModuleSize[iModule]/2.);
}


void OTLayer::hitModuleAndStraw( double u, double v,
                         int& hitMod, int& hitStrA, int& hitStrB) const {

  hitMod=-1;
  hitStrA=-9999;
  hitStrB=-9999;

  double uCen,vCen,uHalf,vHalf;

  for (int iMod=1; iMod<=2*m_HalfNumModule; iMod++) {

    xy2uv(m_XModuleCenter[iMod],m_YModuleCenter[iMod],uCen,vCen);

    uHalf=m_XModuleSize[iMod]/2.;
    vHalf=m_YModuleSize[iMod]/2.;

    if ( (u <= uCen+uHalf && u >= uCen-uHalf) &&
         (v <= vCen+vHalf && v >= vCen-vHalf) ) {
      hitMod=iMod;
      break;
    }
  }
  if ( hitMod < 1 ) {
    return;
  }

  double uFirstStrawA = m_XFirstStrawA[hitMod];
  double uFirstStrawB = uFirstStrawA + 0.5*m_PitchStraw;   

  hitStrA = (int) ((u-(uCen+uFirstStrawA))/m_PitchStraw+1.5);
  hitStrB = (int) ((u-(uCen+uFirstStrawB))/m_PitchStraw+1.5);

  if ( hitStrA < 1 ) { hitStrA = 1; }
  if ( hitStrB < 1 ) { hitStrB = 1; }
  if ( hitStrA > m_HalfNumStraw[hitMod]) { hitStrA = m_HalfNumStraw[hitMod]; }
  if ( hitStrB > m_HalfNumStraw[hitMod]) { hitStrB = m_HalfNumStraw[hitMod]; }
  hitStrB += m_HalfNumStraw[hitMod];
}    

