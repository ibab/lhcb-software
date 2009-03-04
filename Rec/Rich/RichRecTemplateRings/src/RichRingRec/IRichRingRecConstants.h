// $Id: IRichRingRecConstants.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IRICHRINGRECCONSTANTS_H 
#define RICHRINGREC_IRICHRINGRECCONSTANTS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichRingRecConstants ( "IRichRingRecConstants", 1, 0 );

/** @class IRichRingRecConstants IRichRingRecConstants.h RichRingRec/IRichRingRecConstants.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-06
 */
class IRichRingRecConstants : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichRingRecConstants; }
  virtual  double Rich1Rich2ZLimit()=0;
  virtual  int  maxNumRich() =0;
  virtual  int  maxNumRadiator()=0;
  virtual double  TemplateRadiusZoomFactorValue(int rad)=0;
  virtual double  TargetRadiusZoomFactorValue(int rad)=0;
  virtual double r2gHeavyPartMomThreshold()=0;
  virtual double r1gHeavyPartMomThreshold()=0;
    
  virtual double MinTheta()=0;
  virtual double MaxTheta()=0;
  virtual int NumW()=0;  
  virtual int NumR()=0;
  virtual double ThrCorrVectValue(int irad ) =0;
  virtual double  MaxNominalRadiusForRadiatorValue(int irad)=0;
  virtual double RadiusSigmaValue( int irad )=0;
  virtual int RichNumParticleSpecies() =0; 
  virtual double  RichParticleTrueMassValue(int isp )=0;
  virtual double    RadiusTightSigmaValue( int irad )=0;
  virtual int RichParticleTypeFromTruePidCodeValue(int acode )=0;
  virtual double  RichParticleCherenkovThreshold(int irad, int isp )=0;
  virtual double RichHeavyMassBegin()=0;
  virtual double MaxBGLim()=0;
  virtual double  MaxMomTrack(int irad)=0;
  virtual double  MinMomTrack(int irad)=0;
  virtual double  meanRadiusDiff(int irad)=0;
  virtual double  sigmaRadiusDiff(int irad)=0;
  virtual int estimatePidFromMassAlone( double amass)=0;
  
  

protected:

private:

};
#endif // RICHRINGREC_IRICHRINGRECCONSTANTS_H
