// $Id: RichRingRecConstants.h,v 1.3 2009-06-15 09:08:28 seaso Exp $
#ifndef RICHRINGRECCONSTANTS_H
#define RICHRINGRECCONSTANTS_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingRecConstants.h"            // Interface

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingRecConstants RichRingRecConstants.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-06
       */
      class RichRingRecConstants :  public RichRingRecToolBase ,
                                    virtual public IRichRingRecConstants {
      public:
        /// Standard constructor
        RichRingRecConstants( const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

        virtual ~RichRingRecConstants( ); ///< Destructor

        void InitRichRingRecConstantsParam();

        double Rich1Rich2ZLimit()  {  return m_Rich1Rich2ZLimit;}
        int maxNumRich()  {return m_maxNumRich;}
        int maxNumRadiator(){    return m_maxNumRadiator;}
        double  TemplateRadiusZoomFactorValue(int rad){  return m_TemplateRadiusZoomFactor[rad];}
        double  TargetRadiusZoomFactorValue(int rad){  return m_TargetRadiusZoomFactor[rad];}

        double   r2gHeavyPartMomThreshold(){  return  m_r2gHeavyPartMomThreshold;}
        double   r1gHeavyPartMomThreshold(){  return  m_r1gHeavyPartMomThreshold;}


        double MinTheta(){  return  m_MinTheta;}

        double MaxTheta()  {  return m_MaxTheta;}

        int NumW()  {  return  m_NumW;}

        int NumR()  {  return  m_NumR;}

        double ThrCorrVectValue(int irad )
        {  return m_ThrCorrVect[irad];}
        double MaxNominalRadiusForRadiatorValue(int irad)  {  return m_MaxNominalRadiusForRadiator[irad];}
        double RadiusSigmaValue( int irad )  {  return m_RadiusSigma [irad];}
        double RadiusTightSigmaValue( int irad )  {  return m_RadiusTightSigma [irad];}

        void setParticleTrueMass();
        void setParticleCherenkovThreshold ();
        void setParticleIDParam();

        int  RichNumParticleSpecies(){    return m_RichNumParticleSpecies;}
        double RichParticleTrueMassValue(int isp );
        int RichParticleTypeFromTruePidCodeValue(int acode );

        double RichParticleCherenkovThreshold(int irad, int isp )
        { return m_RichParticleCherenkovThreshold [irad] [isp];}

        double RichLightMassBegin()
        {  return m_RichLightMassBegin;}

        double RichLightMassEnd()
        {  return m_RichLightMassEnd;}

        double  RichHeavyMassBegin()
        {  return m_RichHeavyMassBegin;}

        double  RichHeavyMassEnd()
        {  return m_RichHeavyMassEnd;}

        double MaxBGLim()
        {  return m_MaxBGLim;}

        double MinMomTrack(int irad) {return  m_MinMomTrack[irad];}
        double MaxMomTrack(int irad) {return  m_MaxMomTrack[irad];}
        double meanRadiusDiff(int irad) {return m_meanRadiusDiff[irad];}
        double sigmaRadiusDiff(int irad) {return m_sigmaRadiusDiff[irad];}
        int estimatePidFromMassAlone( double amass);

        double radiusScaleFactorToRadiansSingle(int aRadiator) 
        { double aScale = ( (aRadiator > -1)  && (aRadiator < 3) ) ? m_radiusScaleFactorToRadians[aRadiator] : 0;
          return aScale;}



      protected:

      private:

        double m_Rich1Rich2ZLimit;
        int m_maxNumRich;
        int m_maxNumRadiator;
        VD  m_TemplateRadiusZoomFactor;
        VD  m_TargetRadiusZoomFactor;

        double m_r2gHeavyPartMomThreshold;
        double m_r1gHeavyPartMomThreshold;


        double m_MinTheta;
        double m_MaxTheta;
        int m_NumW;  // num bins in phi
        int m_NumR;  // num bins in log R

        VD  m_ThrCorrVect;  // nominal thresholds for peak finding.


        VD m_MaxNominalRadiusForRadiator;

        VD m_RadiusSigma;

        VD m_RadiusTightSigma;


        VD m_RichParticleTrueMass; // increasing order of mass
        // 0=electron,  1=muon, 2=pion,
        // 3=Kaon, 4=Proton

        VI m_RichParticleTrueCode; // abs value in the increasing
        // order of mass as above.


        VVD m_RichParticleCherenkovThreshold; // thresholds for each particle in
        // in each radiator.

        VD m_MinMomTrack;  // minimum momentum of the track in each radiator for PID
        VD m_MaxMomTrack;  // maximum momentum of the track in each radiator for PID

        double m_RichParticleCkvThrTolerence;


        double m_RichLightMassBegin;
        double m_RichLightMassEnd;
        double  m_RichHeavyMassBegin;
        double  m_RichHeavyMassEnd;
        double m_MaxBGLim;


        int m_RichNumParticleSpecies;

        VD m_meanRadiusDiff;  // mean of the radius diff in each radiator
        VD m_sigmaRadiusDiff; // sigma of the radius diff in each radiator

        VD m_radiusSatRingTypical; // radius of typical saturated rings
        VD m_angleSatRingTypical; // cherenkov angle of typical saturated rings
        VD m_radiusScaleFactorToRadians; // conversion from Cherenkov radius to angle
 

      };

    }
  }
}

#endif // RICHRINGRECCONSTANTS_H
