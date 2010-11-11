#ifndef STRIPPINGREPORT_H 
#define STRIPPINGREPORT_H 1

// Include files
// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IIncidentListener.h"

/** @class StrippingReport StrippingReport.h
 *  
 *
 *  @author Anton Poluektov
 *  @date   2010-03-15
 */

class IAlgManager;
class IIncidentSvc;

/// Report structure
class ReportStat {

  public: 
  
    std::string name;      /// Selection name
    int decisions;         /// Number of selected events 
    int candidates;        /// Number of candidates
    double avgtime;        /// Average time per event
    int errors;            /// Number of errors produced by the line 
    int incidents;         /// Number of incidents produced by the line (usually, too many candidates per event)
    int slow_events;       /// Number of slow events
}; 


class StrippingReport : public GaudiTupleAlg, 
                        virtual public IIncidentListener {
public: 
  /// Standard constructor
  StrippingReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StrippingReport( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  virtual void handle ( const Incident& ) ;
  
protected:

private:

  void report(bool onlyPositive);
  void reportLatex(bool onlyPositive);

  std::string m_hdrLocation;     ///< Location of the stripping HltDecReport
  std::vector < std::string > m_selections;  ///< List of selections to report
  bool m_onlyPositive;           ///< If true, show only selections with positive decision
  bool m_everyEvent;             ///< If true, show selection statistics for every event
  int m_reportFreq;              ///< Frequency of reports during run processing 
  bool m_printNonResponding;     ///< If true, show non-responding (with zero events selected) lines 
  bool m_printHot;               ///< If true, show hot (with rate above threshold) lines
  double m_hotThreshold;         ///< Rate threshold for hot lines
  bool m_normalizeByGoodEvents;  ///< If true, use good event number (after pre-filter) to calculate rates
  bool m_latex;                  ///< false for TWiki, true for LaTeX style
  
  std::vector < ReportStat > m_stat;
  
  IAlgManager* m_algMgr;                 ///< Pointer to algorithm manager
  IChronoStatSvc* m_chronoSvc;           ///< Chrono service
  IIncidentSvc* m_incSvc;                ///< Incident service
  
  long m_event;                   ///< Event count
  long m_goodEvent;               ///< Good event count (after pre-filter)
  
};

#endif // STRIPPINGREPORT_H
