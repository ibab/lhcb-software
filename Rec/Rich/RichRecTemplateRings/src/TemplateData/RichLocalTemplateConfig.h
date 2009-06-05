// $Id: RichLocalTemplateConfig.h,v 1.2 2009-06-05 17:21:34 jonrob Exp $
#ifndef TEMPLATEDATA_RICHLOCALTEMPLATECONFIG_H
#define TEMPLATEDATA_RICHLOCALTEMPLATECONFIG_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichLocalTemplateConfig.h"            // Interface

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichLocalTemplateConfig RichLocalTemplateConfig.h TemplateData/RichLocalTemplateConfig.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-14
       */
      class RichLocalTemplateConfig : public RichRingRecToolBase,
                                      virtual public IRichLocalTemplateConfig {
      public:
        /// Standard constructor
        RichLocalTemplateConfig( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

        virtual ~RichLocalTemplateConfig( ); ///< Destructor

        void resetTemplateHitLists( int rad );

        StatusCode ConfigureTemplateHitsForFF(int rad);

        VD RpnTemplate(){return m_RpnTemplate;}

        VD PpnTemplate(){return m_PpnTemplate;}


        StatusCode ScaleTemplateHits( VI aVec, int itk , int irad );



      protected:

      private:

        VD m_RpnTemplate;
        VD m_PpnTemplate;


        VD m_RHitScaledTemplate;
        //  VD m_ThHitScaledTemplate;
        VD m_RpnScaledTemplate;
        VD m_PpnScaledTemplate;


      };

    }
  }
}

#endif // TEMPLATEDATA_RICHLOCALTEMPLATECONFIG_H
