// $Id: OTLayer.cpp,v 1.5 2002-10-14 15:44:08 jvantilb Exp $

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// OTDet
#include "OTDet/OTLayer.h"

/** @file OTLayer.cpp
 *
 *  Implementation of OTLayer class
 *   Original class name: OTDetectionLayer
 *
 *  @author M. Witek
 *  @date   11-11-2000
 */

OTLayer::OTLayer(int iLayer, int iStation, double zLayer, 
                 double xCen,  double yCen, 
                 double xSide, double ySide,
                 double xOut,  double yOut, 
                 int nStandStraw, double pitchStraw, 
                 double stereoAngle ) : 
      m_layerID(iLayer), 
      m_stationID(iStation), 
      m_zOfLayer(zLayer), 
      m_pitchStraw(pitchStraw),  
      m_cellRadius(2.5*mm),
      m_stereoAngle(stereoAngle*degree)
{
  m_sinAngle = sin(stereoAngle*degree);  
  m_cosAngle = cos(stereoAngle*degree);  

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
  m_halfNumModule = 1 + 2 + 2 + 2 * nMod3;  // always odd number  

  m_halfNumStraw  = new    int[2*m_halfNumModule+1];
  m_xFirstStrawA  = new double[2*m_halfNumModule+1];
  m_xModuleCenter = new double[2*m_halfNumModule+1];
  m_yModuleCenter = new double[2*m_halfNumModule+1];
  m_xModuleSize   = new double[2*m_halfNumModule+1];
  m_yModuleSize   = new double[2*m_halfNumModule+1];

  // fill arrays ( modules are counted from 1 to 2*m_halfNumModule )
  m_halfNumStraw[0]  = 0;
  m_xFirstStrawA[0]  = 0.0;
  m_xModuleCenter[0] = 0.0;  
  m_yModuleCenter[0] = 0.0;
  m_xModuleSize[0]   = 0.0;
  m_yModuleSize[0]   = 0.0;

  // central modules top and bottom
  int iCenBot = m_halfNumModule/2 + 1; 
  int iCenTop = iCenBot+m_halfNumModule;

  m_halfNumStraw[iCenTop] = nStraw1;
  m_halfNumStraw[iCenBot] = nStraw1;

  m_xModuleSize[iCenTop]  = pitchMod1; 
  m_xModuleSize[iCenBot]  = pitchMod1 ; 

  m_yModuleSize[iCenTop]  = yOut-yCen; 
  m_yModuleSize[iCenBot]  = yOut-yCen; 

  m_xModuleCenter[iCenTop] = -m_sinAngle * (yCen+yOut)/2.;
  m_xModuleCenter[iCenBot] = -m_xModuleCenter[iCenTop];

  m_yModuleCenter[iCenTop] = m_cosAngle * (yCen+yOut)/2.;
  m_yModuleCenter[iCenBot] = -m_yModuleCenter[iCenTop];


  // wing modules
  int iRightUp   = iCenTop + 1;
  int iRightDown = iCenBot + 1;
  int iLeftUp    = iCenTop - 1;
  int iLeftDown  = iCenBot - 1;

  m_halfNumStraw[iRightUp  ] = nStraw2;
  m_halfNumStraw[iRightDown] = nStraw2;
  m_halfNumStraw[iLeftUp   ] = nStraw2;
  m_halfNumStraw[iLeftDown ] = nStraw2;

  m_xModuleSize[iRightUp  ]  = pitchMod2; 
  m_xModuleSize[iRightDown]  = pitchMod2; 
  m_xModuleSize[iLeftUp   ]  = pitchMod2; 
  m_xModuleSize[iLeftDown ]  = pitchMod2; 

  m_yModuleSize[iRightUp  ]  = yOut-ySide; 
  m_yModuleSize[iRightDown]  = yOut-ySide; 
  m_yModuleSize[iLeftUp   ]  = yOut-ySide; 
  m_yModuleSize[iLeftDown ]  = yOut-ySide;
 
  xshift = -m_sinAngle * (ySide+yOut)/2.;
  xcenter = pitchMod1/2. + pitchMod2/2.;
  m_xModuleCenter[iRightUp  ] =  xcenter + xshift;
  m_xModuleCenter[iRightDown] =  xcenter - xshift;
  m_xModuleCenter[iLeftUp   ] = -xcenter + xshift;
  m_xModuleCenter[iLeftDown ] = -xcenter - xshift;

  ycenter = m_cosAngle * (ySide+yOut)/2.;
  m_yModuleCenter[iRightUp  ] =  ycenter;
  m_yModuleCenter[iRightDown] = -ycenter;
  m_yModuleCenter[iLeftUp   ] =  ycenter;
  m_yModuleCenter[iLeftDown]  = -ycenter;

  // standard modules
  int iMod;
  for (iMod=1; iMod <= nMod3; iMod++) {

    iRightUp   = iCenTop + 1 + iMod;
    iRightDown = iCenBot + 1 + iMod;
    iLeftUp    = iCenTop - 1 - iMod;
    iLeftDown  = iCenBot - 1 - iMod;
  
    m_halfNumStraw[iRightUp  ] = nStraw3;
    m_halfNumStraw[iRightDown] = nStraw3;
    m_halfNumStraw[iLeftUp   ] = nStraw3;
    m_halfNumStraw[iLeftDown ] = nStraw3;
  
    m_xModuleSize[iRightUp  ]  = pitchMod3; 
    m_xModuleSize[iRightDown]  = pitchMod3; 
    m_xModuleSize[iLeftUp   ]  = pitchMod3; 
    m_xModuleSize[iLeftDown ]  = pitchMod3; 
  
    m_yModuleSize[iRightUp  ]  = yOut; 
    m_yModuleSize[iRightDown]  = yOut; 
    m_yModuleSize[iLeftUp   ]  = yOut; 
    m_yModuleSize[iLeftDown ]  = yOut;
   
    xshift = -m_sinAngle * yOut/2.;
    xcenter = pitchMod1/2.+pitchMod2 + (iMod-0.5)*pitchMod3;
    m_xModuleCenter[iRightUp  ] =  xcenter + xshift;
    m_xModuleCenter[iRightDown] =  xcenter - xshift;
    m_xModuleCenter[iLeftUp   ] = -xcenter + xshift;
    m_xModuleCenter[iLeftDown ] = -xcenter - xshift;
  
    ycenter = m_cosAngle * yOut/2.;
    m_yModuleCenter[iRightUp  ] =  ycenter;
    m_yModuleCenter[iRightDown] = -ycenter;
    m_yModuleCenter[iLeftUp   ] =  ycenter;
    m_yModuleCenter[iLeftDown]  = -ycenter;
  
 }

  // end modules
  iRightUp   = iCenTop + 1 + nMod3 +1;
  iRightDown = iCenBot + 1 + nMod3 +1; 
  iLeftUp    = iCenTop - 1 - nMod3 -1;
  iLeftDown  = iCenBot - 1 - nMod3 -1;

  m_halfNumStraw[iRightUp  ] = nStraw4;
  m_halfNumStraw[iRightDown] = nStraw4;
  m_halfNumStraw[iLeftUp   ] = nStraw4;
  m_halfNumStraw[iLeftDown ] = nStraw4;

  m_xModuleSize[iRightUp  ]  = pitchMod4; 
  m_xModuleSize[iRightDown]  = pitchMod4; 
  m_xModuleSize[iLeftUp   ]  = pitchMod4; 
  m_xModuleSize[iLeftDown ]  = pitchMod4; 

  m_yModuleSize[iRightUp  ]  = yOut; 
  m_yModuleSize[iRightDown]  = yOut; 
  m_yModuleSize[iLeftUp   ]  = yOut; 
  m_yModuleSize[iLeftDown ]  = yOut;
 
  xshift = -m_sinAngle * yOut/2.;
  xcenter = pitchMod1/2. + pitchMod2 + nMod3*pitchMod3 + pitchMod4/2.;
  m_xModuleCenter[iRightUp  ] =  xcenter + xshift;
  m_xModuleCenter[iRightDown] =  xcenter - xshift;
  m_xModuleCenter[iLeftUp   ] = -xcenter + xshift;
  m_xModuleCenter[iLeftDown ] = -xcenter - xshift;

  ycenter = m_cosAngle * yOut/2.;
  m_yModuleCenter[iRightUp  ] =  ycenter;
  m_yModuleCenter[iRightDown] = -ycenter;
  m_yModuleCenter[iLeftUp   ] =  ycenter;
  m_yModuleCenter[iLeftDown]  = -ycenter;

  
  for (iMod=1; iMod<=2*m_halfNumModule; iMod++) {
     m_xFirstStrawA[iMod] = -(((double) m_halfNumStraw[iMod])/2. + 0.25)*
                                                           m_pitchStraw;
     m_xModuleSize[iMod]*=m_cosAngle;
  }

}


OTLayer::~OTLayer()
{
  // destructer
  delete[] m_halfNumStraw;
  delete[] m_xFirstStrawA;
  delete[] m_xModuleCenter;
  delete[] m_yModuleCenter;
  delete[] m_xModuleSize;
  delete[] m_yModuleSize;
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
  if( fabs((z1+z2)/2.-m_zOfLayer)>2.0*m_cellRadius ) {
    return false;
  }


  if (fabs(z1-z2)>0.5*mm) {

    double dUdZ=(u2-u1)/(z2-z1);  
  
    double uAtMonoA = u1 + dUdZ*(m_zOfLayer-0.5*m_pitchStraw-z1);  
    double uAtMonoB = u1 + dUdZ*(m_zOfLayer+0.5*m_pitchStraw-z1);
  
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
      if ((iHitStrawA >= 1) && (iHitStrawA <= m_halfNumStraw[iHitModuleA])) {
  
  	double uHitStrawA = uOfStraw ( iHitStrawA, iHitModuleA);
      	distTmp = (uAtMonoA - uHitStrawA) * cosTrack;
     
  	betweenU = ( uHitStrawA > uLow-m_pitchStraw ) && 
  		   ( uHitStrawA < uHigh+m_pitchStraw);
  
  	if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	  iStraw=iHitStrawA;
  	  iModule=iHitModuleA;

          OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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
  	  uStraw  = uOfStraw( iRightStraw, iRightModule);
  	  distTmp = (uAtMonoA - uStraw) * cosTrack;
    
  	  iHiStr = iRightStraw;
  	  iHiMod = iRightModule;
    
  	  betweenU = ( uStraw > uLow-m_pitchStraw ) && 
  		     ( uStraw < uHigh+m_pitchStraw);
  
  	  if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	    iStraw=iRightStraw;
  	    iModule=iRightModule;
            OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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
  	  uStraw  = uOfStraw( iLeftStraw, iLeftModule);
  	  distTmp = (uAtMonoA - uStraw) * cosTrack; 
    
  	  iHiStr = iLeftStraw;
  	  iHiMod = iLeftModule;
    
  	  betweenU = ( uStraw > uLow-m_pitchStraw ) && 
  		     ( uStraw < uHigh+m_pitchStraw);
  	  if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	    iStraw=iLeftStraw;
  	    iModule=iLeftModule;
            OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
                                              iModule, iStraw);
            channels.push_back(tmpChan);
            driftDistances.push_back(distTmp);
  	  }
  	}
      }
    } // layer A    
      
    // store hits for part B
    if ( iHitModuleB > 0 ) {
      if ((iHitStrawB >= m_halfNumStraw[iHitModuleB]+1) && 
  	  (iHitStrawB <= 2*m_halfNumStraw[iHitModuleB])) {
    
  	double uHitStrawB = uOfStraw(iHitStrawB,iHitModuleB);
  
  	distTmp = (uAtMonoB - uHitStrawB) * cosTrack;
    
  	betweenU = ( uHitStrawB > uLow-m_pitchStraw ) && 
  		   ( uHitStrawB < uHigh+m_pitchStraw);
  	if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	  iStraw=iHitStrawB;
  	  iModule=iHitModuleB;
          OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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
  	  uStraw= uOfStraw(iRightStraw,iRightModule); 
  	  distTmp = (uAtMonoB - uStraw) * cosTrack; 
    
  	  iHiStr = iRightStraw;
  	  iHiMod = iRightModule;
    
  	  betweenU = ( uStraw > uLow-m_pitchStraw ) && 
  		     ( uStraw < uHigh+m_pitchStraw);
  	  if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	    iStraw=iRightStraw;
  	    iModule=iRightModule;
            OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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
  	  uStraw= uOfStraw(iLeftStraw,iLeftModule);
  	  distTmp = (uAtMonoB - uStraw) * cosTrack;
    
  	  iHiStr = iLeftStraw;
  	  iHiMod = iLeftModule;
    
  	  betweenU = ( uStraw > uLow-m_pitchStraw ) && 
  		     ( uStraw < uHigh+m_pitchStraw);
  
  	  if ( (fabs(distTmp) < m_cellRadius) && betweenU ) {
  	    iStraw=iLeftStraw;
  	    iModule=iLeftModule;
            OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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

    //    z3Circ = m_zOfLayer;
    double zfrac,zint;
    // zfrac is between 0 and 1. 2.7839542167 means nothing.
    // if distance xy entry-exit is small generate z3 close
    // to the z1 and z2 ( z1 is close to z2)
    zfrac = modf( (fabs(u1)+fabs(v1))/2.7839542167, &zint);
    if (distXY > 2.0 *m_pitchStraw ) {
      z3Circ = m_zOfLayer + 2.0*(zfrac-0.5)*m_pitchStraw;
    }
    else {
      if ( z1-m_zOfLayer < 0 ) {
        z3Circ = m_zOfLayer - zfrac*m_pitchStraw; 
      } 
      else {
        z3Circ = m_zOfLayer + zfrac*m_pitchStraw; 
      }
    }

    sCircle(z1, u1, z2, u2, z3Circ, zCirc, uCirc, rCirc);

    // monolayer A
    int iStrawA;
    int iModuleA;
    hitModuleAndStraw(uLow,v1,iModuleA,iStrawA,iDummy);
    double zStrawA = zOfStraw(iStrawA,iModuleA);
    double uStep = uLow;
    int iCount = 0;

    while ( (uStep < uHigh) && (iCount < 100) && (iModuleA > 0) ) {
      iCount++;
      uStep =  uOfStraw(iStrawA,iModuleA);
      double distCirc = 
         sqrt((zCirc-zStrawA)*(zCirc-zStrawA) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int iStraw  = iStrawA;
      int iModule = iModuleA;

      double ambTmp = - (uStep-(u1+u2)/2.) * (distCirc-rCirc);
      
      if ( distTmp < m_cellRadius ) {
        if (ambTmp < 0.0 ) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
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
    double zStrawB = zOfStraw(iStrawB,iModuleB);
    uStep = uLow;
    iCount = 0;
   
    while ( (uStep < uHigh) && (iCount < 100) && (iModuleB > 0) ) {
      iCount++;
      uStep =  uOfStraw(iStrawB,iModuleB);
      //double vStep = v1+((v2-v1)/(u2-u1))*(uStep-u1);
      double distCirc = 
        sqrt((zCirc-zStrawB)*(zCirc-zStrawB) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int iModule = iModuleB;
      int iStraw  = iStrawB;

      double ambTmp = - (uStep-(u1+u2)/2.) * (distCirc-rCirc);
      if ( distTmp < m_cellRadius ) {
        if (ambTmp < 0.0) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
                                          iModule, iStraw);
        channels.push_back(tmpChan);
        driftDistances.push_back(distTmp);
      }
      nextRightStraw(iStraw,iModule,iStrawB,iModuleB);
    } //while


  } // else
  
  return true;

}

double OTLayer::distanceToWire(const OTChannelID aChannel, 
                               const HepPoint3D& aPoint, 
                               const double tx, const double ty) const
{
  // calculate the distance to the wire (assuming straight lines)
  double z = zOfStraw(aChannel);
  double x = aPoint.x() + tx*(z - aPoint.z())  ;
  double y = aPoint.y() + ty*(z - aPoint.z())  ;
  double tu   = tx * m_cosAngle + ty * m_sinAngle;
  double cosU = 1./sqrt( 1. + pow( tu, 2 ));
  double dist = ( x*m_cosAngle + y*m_sinAngle - uOfStraw(aChannel) ) * cosU;

  return dist;
}

void OTLayer::xy2uv(const double x, const double y, 
                    double& u, double& v) const
{
  u =  x*m_cosAngle + y*m_sinAngle;
  v = -x*m_sinAngle + y*m_cosAngle;
}

void OTLayer::uv2xy(const double u, const double v, 
                    double& x, double& y) const
{
  x =  u*m_cosAngle - v*m_sinAngle;
  y =  u*m_sinAngle + v*m_cosAngle;
}

void OTLayer::nextRightStraw( int iHitStraw,
                              int iHitModule,
                              int& iRightStraw,
                              int& iRightModule) const {

  if( (iHitModule == m_halfNumModule || iHitModule == 2*m_halfNumModule ) &&
      (iHitStraw  == m_halfNumStraw[iHitModule] || 
                                   iHitStraw == 2*m_halfNumStraw[iHitModule])) {
    iRightModule = -9999;
    iRightStraw  = -9999;
  }
  else if( iHitStraw == m_halfNumStraw[iHitModule] ) {
    iRightModule = iHitModule + 1 ;
    iRightStraw  = 1;
  }
  else if( iHitStraw == 2*m_halfNumStraw[iHitModule] ) {
    iRightModule = iHitModule + 1 ;
    iRightStraw  = m_halfNumStraw[iRightModule]+1;
  }
  else {
    iRightStraw  = iHitStraw + 1;
    iRightModule = iHitModule;
  }
}

void OTLayer::nextLeftStraw(int iHitStraw,int iHitModule,
                            int& iLeftStraw,int& iLeftModule) const {

  if( (iHitModule == 1 || iHitModule == m_halfNumModule+1 ) &&
      (iHitStraw  == 1 || iHitStraw == m_halfNumStraw[iHitModule]+1)) {
    iLeftModule = -9998;
    iLeftStraw  = -9998;
  }
  else if( iHitStraw == 1 ) {
    iLeftModule = iHitModule - 1 ;
    iLeftStraw  = m_halfNumStraw[iLeftModule];
  }
  else if( iHitStraw == m_halfNumStraw[iHitModule]+1 ) {
    iLeftModule = iHitModule - 1 ;
    iLeftStraw  = 2*m_halfNumStraw[iLeftModule];
  }
  else {
    iLeftStraw  = iHitStraw - 1;
    iLeftModule = iHitModule;
  }
}

double OTLayer::uOfStraw(const int iStraw, const int iModule) const{

  int locStraw = iStraw;
  double uFirst=m_xFirstStrawA[iModule];

  if (iStraw > m_halfNumStraw[iModule]) {
    uFirst += 0.5*m_pitchStraw;
    locStraw-=m_halfNumStraw[iModule];
  }

  double uCen,vCen;
  this->xy2uv(m_xModuleCenter[iModule],m_yModuleCenter[iModule],uCen,vCen);

  return ( uCen + uFirst + (locStraw-1)*m_pitchStraw);

}

double OTLayer::uOfStraw(const OTChannelID aChannel) const
{
  return uOfStraw(aChannel.straw(), aChannel.module());
}

double OTLayer::zOfStraw(const int iStraw, const int iModule) const {

  if (( iStraw < 1 ) || ( iStraw > 2*m_halfNumStraw[iModule] )) {
    return  -99999.; 
  }

  if ( iStraw <= m_halfNumStraw[iModule] ) {
    return (m_zOfLayer-0.5*m_pitchStraw);
  }
  else {
    return (m_zOfLayer+0.5*m_pitchStraw);
  }

}

double OTLayer::zOfStraw(const OTChannelID aChannel) const
{
  return zOfStraw(aChannel.straw(), aChannel.module());
}


HepPoint3D OTLayer::centerOfStraw( const int iStraw, 
                                   const int iModule) const{

  double x,y,z,u,v,uCen,vCen;

  HepPoint3D tmpPoint(0.,0.,0.);

  z = this->zOfStraw(iStraw,iModule);

  xy2uv(m_xModuleCenter[iModule],m_yModuleCenter[iModule],uCen,vCen);

  u = this->uOfStraw(iStraw,iModule);
  v = vCen;

  uv2xy(u,v,x,y);

  tmpPoint[0]=x;  
  tmpPoint[1]=y;  
  tmpPoint[2]=z;  

  return tmpPoint;
}


bool OTLayer::bottomModule(const int iModule) const 
{
  // check if module is top or bottom
  return (iModule <= m_halfNumModule);
}


bool OTLayer::topModule(const int iModule) const 
{
  // check if module is top or bottom
  return (iModule > m_halfNumModule);
}


bool OTLayer::monoLayerA(const int iModule, const int iStraw) const
{
  bool inLayerA = false;
  if ( iModule > 0 ) {
    if ((iStraw >= 1) && 
        (iStraw <= m_halfNumStraw[iModule])) inLayerA = true;
  }
  return inLayerA;  
}
      

bool OTLayer::monoLayerB(const int iModule, const int iStraw) const
{
  bool inLayerB = false;
  if ( iModule > 0 ) {
    if ((iStraw >= m_halfNumStraw[iModule]+1) && 
        (iStraw <= 2*m_halfNumStraw[iModule])) inLayerB = true;
  }
  return inLayerB;  
}


double OTLayer::distanceAlongWire(const int iModule,
                                  const double xHit, 
                                  const double yHit) const
{ 
  // distance from x,y to end of wire
  // add check if xHit,yHit indeed inside iModule?

  double uHit,vHit,uCen,vCen;
  xy2uv(xHit,yHit,uHit,vHit);
  xy2uv(m_xModuleCenter[iModule],m_yModuleCenter[iModule],uCen,vCen);

  double dist;
  if ( iModule <= m_halfNumModule ) {
    // Bottom Module
    dist = vHit - vCen + (0.5*m_yModuleSize[iModule]);
  } else {
    // Top Module
    dist = vCen - vHit + (0.5*m_yModuleSize[iModule]);
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


double OTLayer::wireHalfLength(const int iModule) const
{
  // half length of wire
  return (m_yModuleSize[iModule]/2.);
}


void OTLayer::hitModuleAndStraw( double u, double v,
                                 int& hitMod, int& hitStrA, 
                                 int& hitStrB) const 
{
  hitMod=-1;
  hitStrA=-9999;
  hitStrB=-9999;

  double uCen,vCen,uHalf,vHalf;

  for (int iMod=1; iMod<=2*m_halfNumModule; iMod++) {

    xy2uv(m_xModuleCenter[iMod],m_yModuleCenter[iMod],uCen,vCen);

    uHalf=m_xModuleSize[iMod]/2.;
    vHalf=m_yModuleSize[iMod]/2.;

    if ( (u <= uCen+uHalf && u >= uCen-uHalf) &&
         (v <= vCen+vHalf && v >= vCen-vHalf) ) {
      hitMod=iMod;
      break;
    }
  }
  if ( hitMod < 1 ) {
    return;
  }

  double uFirstStrawA = m_xFirstStrawA[hitMod];
  double uFirstStrawB = uFirstStrawA + 0.5*m_pitchStraw;   

  hitStrA = (int) ((u-(uCen+uFirstStrawA))/m_pitchStraw+1.5);
  hitStrB = (int) ((u-(uCen+uFirstStrawB))/m_pitchStraw+1.5);

  if ( hitStrA < 1 ) { hitStrA = 1; }
  if ( hitStrB < 1 ) { hitStrB = 1; }
  if ( hitStrA > m_halfNumStraw[hitMod]) { hitStrA = m_halfNumStraw[hitMod]; }
  if ( hitStrB > m_halfNumStraw[hitMod]) { hitStrB = m_halfNumStraw[hitMod]; }
  hitStrB += m_halfNumStraw[hitMod];
}    

