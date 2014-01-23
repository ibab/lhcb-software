// $Id: LumiHistoMaker.h,v 1.8 2008-12-30 21:23:13 graven Exp $
#ifndef LUMIHISTOMAKER_H
#define LUMIHISTOMAKER_H 1

// Include files
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "HltBase/HltBaseAlg.h"

/** @class LumiHistoMaker
 *
 *
 *  functionality: Fill Histos with info from HltLumiSummary
 *
 *
 *  options:
 *
 *
 *  @author Antonio Perez-Calero
 *  @date   2008-07-24
 */
class LumiHistoMaker : public HltBaseAlg
{
  public:
    // Standard constructor
    LumiHistoMaker( const std::string& name, ISvcLocator* pSvcLocator );

    // Standard destructor
    virtual ~LumiHistoMaker();

    // initialize algorithm
    virtual StatusCode initialize();

    // driver of the execute()
    StatusCode execute();

  private:
    // force decision to the value of decision
    void setDecision( bool decision );

  private:
    // period to update filling of histogram
    int m_histogramUpdatePeriod;

    std::string m_DataName;                 // input location of summary data
    LHCb::HltLumiSummary* m_HltLumiSummary; // summary data
    std::vector<std::string> m_Variables;   // list of variables to look at
    std::vector<int> m_MaxBins;             // number of bins in the histos
    std::vector<int> m_Thresholds;          // thresholds to apply
    int m_MaxBin;                           // default maximum bin number
    int m_Threshold;                        // default threshold

    std::vector<AIDA::IHistogram1D*> m_Histos; // the straight histogram of counts
    std::vector<AIDA::IHistogram1D*> m_ThresholdHistos; // two-bin histogram after
                                                        // threshold
    std::vector<std::string> m_names; // list of named counters to look at
    std::vector<int> m_keys;          // key value in the summary bank
};
#endif // LUMIHISTOMAKER_H
