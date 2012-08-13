// $Id: FilterOnLumiSummary.h,v 1.1 2010-02-12 16:56:42 panmanj Exp $
#ifndef FILTERONLUMISUMMARY_H 
#define FILTERONLUMISUMMARY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// event model
#include "Event/HltLumiSummary.h"

/** @class FilterOnLumiSummary FilterOnLumiSummary.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2010-01-29
 */
class FilterOnLumiSummary : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterOnLumiSummary( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterOnLumiSummary( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_DataName;             // input location of summary data
  std::string m_ValueName;            // value required 
  std::string m_CounterName;          // counter looked at
  int m_Counter;                      // int value required   
  int m_Value;			      // int counter looked at

private:

};
#endif // FILTERONLUMISUMMARY_H
