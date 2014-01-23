// $Id: LumiHisto2dSPD.h,v 1.1 2009-01-27 21:21:39 aperezca Exp $
#ifndef LUMIHISTO2DSPD_H
#define LUMIHISTO2DSPD_H 1

// Include files
// from Gaudi
#include "CaloDAQ/ICaloDataProvider.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// from CaloDet
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"
// from CaloDAQ
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"
// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/L0ProcessorData.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "CaloUtils/Calo2Dview.h"

//#include "GaudiAlg/GaudiAlgorithm.h"
//#include "GaudiAlg/GaudiHistoAlg.h"

//#include "CaloDAQ/ICaloDataProvider.h"
//#include "GaudiAlg/GaudiHistoID.h"

/** @class LumiHisto2dSPD LumiHisto2dSPD.h
 *
 *
 *  @author Antonio Maria Perez Calero Yzquierdo
 *  @date   2009-01-27
 */
class LumiHisto2dSPD : public Calo2Dview
{
  public:
    /// Standard constructor
    LumiHisto2dSPD( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~LumiHisto2dSPD(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization

  private:
    // AIDA::IHistogram2D* HistoSPD;
    // Calo2Dview

    // Calo Data Provider
    ICaloDataProvider* m_daq;
    // Names
    std::string m_readoutTool;
    std::string m_htitle;
};
#endif // LUMIHISTO2DSPD_H
