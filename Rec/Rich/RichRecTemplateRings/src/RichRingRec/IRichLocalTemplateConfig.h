// $Id: IRichLocalTemplateConfig.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H
#define RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H 1

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

      static const InterfaceID IID_IRichLocalTemplateConfig ( "IRichLocalTemplateConfig", 1, 0 );

      /** @class IRichLocalTemplateConfig IRichLocalTemplateConfig.h RichRingRec/IRichLocalTemplateConfig.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-14
       */
      class IRichLocalTemplateConfig : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichLocalTemplateConfig; }

        virtual StatusCode ConfigureTemplateHitsForFF(int rad)=0;

        virtual VD RpnTemplate()=0;
        virtual VD PpnTemplate()=0;
        virtual StatusCode ScaleTemplateHits(VI aVec, int itk , int irad )=0;



      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H
