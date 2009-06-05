// $Id: IRichLocalTargetConfig.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef RICHRINGREC_IRICHLOCALTARGETCONFIG_H
#define RICHRINGREC_IRICHLOCALTARGETCONFIG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichLocalTargetConfig ( "IRichLocalTargetConfig", 1, 0 );

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class IRichLocalTargetConfig IRichLocalTargetConfig.h RichRingRec/IRichLocalTargetConfig.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-13
       */
      class IRichLocalTargetConfig : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichLocalTargetConfig; }
        virtual   StatusCode SelectHitsInFoi(int itk, int irad , int iRich )=0;
        virtual   void SetRadiatorParam(int irad)=0;
        virtual  StatusCode InitTargetConfigParam()=0;
        virtual  VD CartToPolNumFromRP(double R, double P)=0;
        virtual  StatusCode ConfigureTargetHitsInFoiForFF(int itk,int irad , int iRich ) =0;

        virtual   VD   RpnTarget() =0;
        virtual   VD   PpnTarget() =0;

        virtual double GetScaleFromLog(VI aVec )=0;

        virtual  StatusCode EstimateMeanRadiusFromRawRadius( int itk, int irad , int iRich )=0;


      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHLOCALTARGETCONFIG_H
