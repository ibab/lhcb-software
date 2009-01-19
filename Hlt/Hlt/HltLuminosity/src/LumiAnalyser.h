// $Id: LumiAnalyser.h,v 1.7 2009-01-19 20:08:46 graven Exp $
#ifndef LUMIANALYSER_H 
#define LUMIANALYSER_H 1

// Include files
#include "GaudiKernel/IHistogramSvc.h"
#include "Event/HltSummary.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "HltBase/HltBaseAlg.h"
#include "HltBase/ANNSvc.h"

/** @class LumiAnalyser 
 *  
 *
 *  functionality: Collect Counters in the HltLumiSummary and calculate "R"
 *
 *  options:
 *
 *  @author Jaap Panman
 *  @date   2008-08-15
 */
class LumiAnalyser : public HltBaseAlg {
public:

  // Standard constructor
  LumiAnalyser( const std::string& name, ISvcLocator* pSvcLocator );

  // Standard destructor
  virtual ~LumiAnalyser( ); 

  // initialize algorithm
  virtual StatusCode initialize();

  // finalize algorithm
  virtual StatusCode finalize  ();

protected:
  // analyse histos
  virtual StatusCode accumulate  ();
  virtual StatusCode analyse  ();
  virtual void storeTrend  (std::string varname, double lumiValue) ;
  
  // set up store
  virtual void setupStore  ();

  // to set interval
  ulonglong gpsTime  ();
  bool gpsTimeInterval  ();

  // force decision to the value of decision
  void setDecision(bool decision);
    
private:  
  // driver of the execute()
  StatusCode execute();

private:
  // period to update filling of histogram
  int m_histogramUpdatePeriod;

protected:
  IHistogramSvc* m_histogramSvc;

  long m_call_counter;

  // counter with all the events with fine inputs
  IANNSvc* m_annSvc;
  std::string m_DataName;                        // input location of summary data
  bool m_rawHistos;                              // flag to store raw histos of R

  std::vector< std::string > m_Variables;        // list of variables to look at
  std::vector< std::string > m_Averages;         // list of averages to look at
  std::vector<int> m_Thresholds;                 // thresholds to apply
  int m_MaxBin;                                  // default maximum bin number
  int m_Threshold;                               // default threshold
  std::vector< std::string > m_BXTypes;          // list of bunch crossing types to look at
  std::vector< std::string > m_addBXTypes;       // list of bunch crossing types to be added
  std::vector< std::string > m_subtractBXTypes;  // list of bunch crossing types to be subtracted

  typedef std::pair< long, long > iiPair;
  typedef std::map< std::string, iiPair* > iiMap;    // simple map of pairs
  typedef std::map< std::string, iiMap* > iiStore;   // map of maps
  iiStore m_theStore;                                // main store
  iiStore m_prevStore;                               // previous values store
  std::map< std::string, double > m_resultMap;       // to keep results per variable

  typedef std::map< std::string, AIDA::IHistogram1D* > histoMap;  // simple map of histos
  typedef std::map< std::string, histoMap* > histoStore;          // map of maps
  histoStore m_histoStore;                                        // main store for raw histos
  histoMap m_trendMap;                                            // trends
  long m_trendSize;                                               // size of trend histos
  long m_trendInterval;                                           // interval for trending
  unsigned long m_gpsPrevious;                                    // to keep previous time when analysed

  bool m_publishToDIM;                               // to publish to DIM
  std::vector<IANNSvc::minor_value_type> m_items;    
  unsigned int m_size;
  double *m_means;
  double *m_thresholds;
  unsigned int *m_infoKeys;

};
#endif // LUMIANALYSER_H
