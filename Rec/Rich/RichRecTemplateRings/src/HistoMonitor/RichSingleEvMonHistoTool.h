// $Id: RichSingleEvMonHistoTool.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H
#define HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H 1

// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichSingleEvMonHistoTool.h"            // Interface

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichSingleEvMonHistoTool RichSingleEvMonHistoTool.h HistoMonitor/RichSingleEvMonHistoTool.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-18
       */
      class RichSingleEvMonHistoTool : public RichRingRecToolBase,
                                       virtual public IRichSingleEvMonHistoTool {
      public:
        /// Standard constructor
        RichSingleEvMonHistoTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);

        virtual ~RichSingleEvMonHistoTool( ); ///< Destructor

        void PlotHitsAndTracksInEv(int irad);
        void PlotTargetAndTemplate();
        void PlotInvCorr( VVD aRPInv);


      private:

        AIDA::IHistogram2D* m_HistoInvCorrSingleTrack;
        AIDA::IHistogram1D* m_TestHistoMonitor;

      };

    }
  }
}

#endif // HISTOMONITOR_RICHSINGLEEVMONHISTOTOOL_H
