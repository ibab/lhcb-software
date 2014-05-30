// $Id: HltRawDataMonitor.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTRAWDATAMONITOR_H 
#define HLTRAWDATAMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IANNSvc.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

/** @class HltRawDataMonitor HltRawDataMonitor.h
 *  
 *
 *  @author Chun-Min Jen
 *  @date   2008-10-07
 **/

class HltRawDataMonitor : public GaudiHistoAlg {
public:

  enum HeaderIDs { kVersionNumber=2 };
  
  /// Standard constructor
  HltRawDataMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRawDataMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float floatFromInt(unsigned int i);
  
protected:

  /// location of input
  std::string m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;


  /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
  UnsignedIntegerProperty m_sourceID;

  /// <0 never 0=at finalize >0 event frequency
  IntegerProperty                                                 m_diagnosticsFrequency;

private:

  typedef std::pair<std::pair<std::string, AIDA::IHistogram1D*>, double>      HltSortedSelName;
  class sort {
  public:
    bool   operator() (const HltSortedSelName & q1, const HltSortedSelName & q2) const;
  };

  void fillDiag( const char* trigName, double length );

  StatusCode fillRawBank();
  StatusCode fillHltSelRep();
  StatusCode selectionDiagnostics();

  int     nLength(const LHCb::HltObjectSummary& p);
  double  nMedian(const AIDA::IHistogram1D& q);
  double  nMedian2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q);
  double  nMean2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q);
  double  nRMS2(unsigned int NumOfEvt, const AIDA::IHistogram1D& q);


  // lots of different sorts of histograms are declared here
  // call/book histograms here
  AIDA::IHistogram1D*                                                         m_bankSize;
  AIDA::IHistogram1D*                                                         m_hitSize;
  AIDA::IHistogram1D*                                                         m_objtypSize;
  AIDA::IHistogram1D*                                                         m_substrSize;
  AIDA::IHistogram1D*                                                         m_stdinfoSize;
  AIDA::IHistogram1D*                                                         m_extinfoSize;

  std::vector<std::pair<std::string, AIDA::IHistogram1D*> >                   m_hltSelNameList;
  std::vector< HltSortedSelName >                                             m_hltRankedSelName;

  unsigned int                                                                m_event;

};
#endif // HLTRAWDATAMONITOR_H
