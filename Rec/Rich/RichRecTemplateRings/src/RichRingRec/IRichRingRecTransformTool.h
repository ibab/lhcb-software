// $Id: IRichRingRecTransformTool.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef RICHRINGREC_IRICHRINGRECTRANSFORMTOOL_H
#define RICHRINGREC_IRICHRINGRECTRANSFORMTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      static const InterfaceID IID_IRichRingRecTransformTool ( "IRichRingRecTransformTool", 1, 0 );

      /** @class IRichRingRecTransformTool IRichRingRecTransformTool.h RichRingRec/IRichRingRecTransformTool.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-12
       */
      class IRichRingRecTransformTool : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichRingRecTransformTool; }

        virtual double CartToPolRad(double x, double y, double aXc, double aYc )=0;
        virtual double CartToPolPhi(double x, double y, double aXc, double aYc )=0;
        virtual double CartToPolRadFromTHNum(int aHitNum, int aTrackNum , int rad)=0;
        virtual double CartToPolPhiFromTHNum(int aHitNum, int aTrackNum , int rad)=0;
        virtual double XDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich)=0;
        virtual double YDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich)=0;
        virtual int RichDetNumFromRadiator(int rad) =0;
        virtual int RichDetNumFromHitZ(double GlobalZHit)=0;
        virtual int SearchIndexInVect(VD aVect, double aVal)=0;
        virtual VD GetCorrelation( VD aTar, VD aTemp)=0;
        virtual VI FindPeakInRP(VVD aPolVec, int rad)=0;
        virtual VI getTrackIndexLimits( int irad) =0;
        virtual VD TrackExpectedRadii(int itk, int irad)=0;
        virtual double TrackExpectedRadiusValue(int itk, int irad, int PartType)=0;
        virtual double TrackExpectedRadiusFromTruePidCode(int itk, int irad, int aTruePidCode)=0;
        virtual double getTrackMassFromRadius(int itk, int irad, double aRadius )=0;
        virtual bool CheckBGThreshold( double aMass, int itk, int irad)=0;
        virtual  double getBGFromRadius(int irad, double aRadius)=0;
        virtual  double getRadiusFromBG(int irad, double aBG)=0;
        virtual  double  MaxRadiusFit(int irad) =0;



      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHRINGRECTRANSFORMTOOL_H
