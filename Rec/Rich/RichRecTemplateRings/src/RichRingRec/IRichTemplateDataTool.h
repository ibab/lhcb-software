// $Id: IRichTemplateDataTool.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef RICHRINGREC_IRICHTEMPLATEDATATOOL_H
#define RICHRINGREC_IRICHTEMPLATEDATATOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "RichRingRec/RichDataArrayDefinitions.h"

static const InterfaceID IID_IRichTemplateDataTool ( "IRichTemplateDataTool", 1, 0 );

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class IRichTemplateDataTool IRichTemplateDataTool.h RichRingRec/IRichTemplateDataTool.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-06
       */
      class IRichTemplateDataTool : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichTemplateDataTool; }

        virtual StatusCode init() =0;

        virtual StatusCode ConfigureTemplateForRadiator()=0;
        virtual double RHitTemplateValue(int i, int irad)=0;
        virtual double ThHitTemplateValue(int i, int irad)=0;
        virtual int NumHitsInTemplateValue(int irad )=0;


      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHTEMPLATEDATATOOL_H
