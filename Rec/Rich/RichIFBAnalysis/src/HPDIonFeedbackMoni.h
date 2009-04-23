// $Id: HPDIonFeedbackMoni.h,v 1.1.1.1 2009-04-23 14:47:20 jonrob Exp $
#ifndef RICHIFBANALYSIS_HPDIonFeedbackMoni_H
#define RICHIFBANALYSIS_HPDIonFeedbackMoni_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichPixelClusteringTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/RichHistoAlgBase.h"

namespace Rich
{
  namespace Mon
  {

    //-----------------------------------------------------------------------------
    /** @class HPDIonFeedbackMoni HPDIonFeedbackMoni.h
     *
     *  Monitors the Ion Feedback in the RICH HPDs
     *
     *  @author Ross Young
     *  @author Nicholas Styles
     */
    //-----------------------------------------------------------------------------

    class HPDIonFeedbackMoni : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      HPDIonFeedbackMoni( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~HPDIonFeedbackMoni( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution

    private:

      //std::vector<IFBrate> m_IFBvect;
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;
      const Rich::ISmartIDTool * m_idTool;
      const Rich::DAQ::IPixelClusteringTool * m_clusterTool;

      int m_ionFeedbackCut;      // Define what IFB means: cluster of size this or larger = IFB
      int m_ionFeedbackCutALICE; // Define what IFB means: cluster of size this or larger = IFB (ALICE MODE)

    };
  }
}

#endif // RICHIFBANALYSIS_HPDIonFeedbackMoni_H
