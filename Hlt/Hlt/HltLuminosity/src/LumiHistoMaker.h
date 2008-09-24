// $Id: LumiHistoMaker.h,v 1.5 2008-09-24 18:52:18 graven Exp $
#ifndef LUMIHISTOMAKER_H 
#define LUMIHISTOMAKER_H 1

// Include files
#include "Event/RecVertex.h"
#include "HltBase/HltTypes.h"
#include "HltBase/HltBaseAlg.h"
#include "Event/HltLumiSummary.h"
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
class LumiHistoMaker : public HltBaseAlg {
public:

  // Standard constructor
  LumiHistoMaker( const std::string& name, ISvcLocator* pSvcLocator );

  // Standard destructor
  virtual ~LumiHistoMaker( ); 


  // initialize algorithm
  virtual StatusCode initialize();

  // finalize algorithm
  virtual StatusCode finalize  ();

protected:

  // initialize the counters
  virtual void initCounters();


private:  

  
  // driver of the execute()
  StatusCode execute();

  //protected:


  // force decision to the value of decision
  void setDecision(bool decision);
    

    
private:

  // period to update filling of histogram
  int m_histogramUpdatePeriod;


protected:

  Hlt::Counter m_counterEntries;               // counter with all the entries of the algorithm
  Hlt::Counter m_counterHistoInputs;           // counter with all the events with fine inputs

  std::string m_DataName;                      // input location of summary data
  LHCb::HltLumiSummary* m_HltLumiSummary;      // summary data
  std::vector<std::string> m_Variables;        // list of variables to look at
  std::vector<int> m_MaxBins;                  // number of bins in the histos
  std::vector<int> m_Thresholds;               // thresholds to apply
  int m_MaxBin;                                // default maximum bin number
  int m_Threshold;                             // default threshold

  std::vector<Hlt::Histo*> m_Histos;           // the straight histogram of counts
  std::vector<Hlt::Histo*> m_ThresholdHistos;  // two-bin histogram after threshold
  std::vector<std::string> m_names;            // list of named counters to look at
  std::vector<int> m_keys;                     // key value in the summary bank
  
};
#endif // LUMIHISTOMAKER_H
