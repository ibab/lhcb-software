// $Id: IRichRingRecToolBase.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef IRICHRINGRECTOOLBASE_H
#define IRICHRINGRECTOOLBASE_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Rich
#include "RichRecBase/RichRecHistoToolBase.h"

//#include "RichRecBase/IRichPhotonPredictor.h"

//local

#include "RichRingRec/IRichTargetDataTool.h"
#include "RichRingRec/IRichTemplateDataTool.h"
#include "RichRingRec/IRichRingRecConstants.h"
#include "RichRingRec/IRichRingRecTransformTool.h"
#include "RichRingRec/IRichRingReconParam.h"
#include "RichRingRec/IRichLocalTargetConfig.h"
#include "RichRingRec/IRichLocalTemplateConfig.h"
#include "RichRingRec/IRichFFPlan.h"
#include "RichRingRec/IRichRingRecResult.h"
#include "RichRingRec/IRichSingleEvMonHistoTool.h"
#include "RichRingRec/IRichMCTruthData.h"
#include "RichRingRec/IRichRingRecMass.h"
//#include "RichRingRec/IRichRingRecStorePidTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {


      static const InterfaceID IID_IRichRingRecToolBase ( "IRichRingRecToolBase", 1, 0 );

      /** @class IRichRingRecToolBase IRichRingRecToolBase.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-17
       */
      class IRichRingRecToolBase : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichRingRecToolBase; }

        virtual IRichTargetDataTool* tgD() =0;
        virtual IRichRingRecConstants* RConst()=0;
        virtual IRichTemplateDataTool* tmD() =0;
        virtual IRichRingRecTransformTool*  Tfm()=0;
        virtual IRichRingReconParam* RParam()=0;
        virtual IRichLocalTargetConfig* RLocTgC()=0;
        virtual IRichLocalTemplateConfig* RLocTDC()=0;
        virtual IRichFFPlan* RFFP() =0;
        virtual IRichRingRecResult* RRslt()=0;
        virtual IRichSingleEvMonHistoTool* HSingleEv()=0;
        virtual IRichMCTruthData* MCT()=0;
        virtual IRichRingRecMass* RMass() =0;
        //  virtual IRichRingRecStorePidTool*  RPid()=0;



        //  virtual  IRichPhotonPredictor* RPhotonPredict()=0;



      protected:

      private:



      };

    }
  }
}

#endif // IRICHRINGRECTOOLBASE_H
