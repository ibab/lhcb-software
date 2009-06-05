// $Id: RichRingRecToolBase.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef BASE_RICHRINGRECTOOLBASE_H
#define BASE_RICHRINGRECTOOLBASE_H 1

// Include files
// from Gaudi

#include "RichRingRec/IRichRingRecToolBase.h"            // Interface
#include "RichRingRec/RichDataArrayDefinitions.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingRecToolBase RichRingRecToolBase.h base/RichRingRecToolBase.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-17
       */
      class RichRingRecToolBase : public RichRecHistoToolBase,
                                  virtual public IRichRingRecToolBase{
      public:
        /// Standard constructor
        RichRingRecToolBase( const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

        virtual ~RichRingRecToolBase( ); ///< Destructor
        virtual StatusCode initialize();    ///< Tool initialization
        virtual StatusCode finalize  ();    ///< Tool finalization


        IRichTargetDataTool* tgD() ;
        IRichRingRecConstants* RConst();
        IRichTemplateDataTool* tmD();
        IRichRingRecTransformTool* Tfm();
        IRichRingReconParam* RParam();
        IRichLocalTargetConfig* RLocTgC();
        IRichLocalTemplateConfig* RLocTDC();
        IRichFFPlan* RFFP();
        IRichRingRecResult* RRslt();
        IRichSingleEvMonHistoTool* HSingleEv();
        IRichMCTruthData* MCT();
        IRichRingRecMass* RMass();
        //  IRichRingRecStorePidTool* RPid();


        //  IRichPhotonPredictor* RPhotonPredict();

      protected:

      private:

        IRichRingRecConstants* m_RichRingRecConstants;
        IRichTargetDataTool* m_TargetDataTool;
        IRichTemplateDataTool* m_TemplateDataTool;
        IRichRingRecTransformTool* m_RichRingRecTransformTool;
        IRichRingReconParam* m_RichRingReconParam;
        IRichLocalTargetConfig* m_RichLocalTargetConfig;
        IRichLocalTemplateConfig* m_RichLocalTemplateConfig;
        IRichFFPlan* m_RichFFPlan;
        IRichRingRecResult* m_RichRingRecResult;
        IRichSingleEvMonHistoTool* m_RichSingleEvMonHistoTool;
        IRichMCTruthData* m_RichMCTruthData;
        IRichRingRecMass* m_RichRingRecMass;
        //  IRichRingRecStorePidTool* m_IRichRingRecStorePidTool;



        //  IRichPhotonPredictor* m_RichPhotonPredictor;

      };

    }
  }
}

#endif // BASE_RICHRINGRECTOOLBASE_H
