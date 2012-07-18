// $Id: FastL0DUFilter.h,v 1.1 2008-12-08 18:03:28 odescham Exp $
#ifndef FASTL0DUFILTER_H 
#define FASTL0DUFILTER_H 1

// Include files
// from Gaudi
#include "L0Base/L0FromRawBase.h" 
#include "Event/RawEvent.h"


/** @class FastL0DUFilter FastL0DUFilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-11-12
 */
class FastL0DUFilter : public L0FromRawBase {
public: 
  /// Standard constructor
  FastL0DUFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastL0DUFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_source;
  bool m_noMuonSup;
  long m_sumCut;
  long m_haCut;
  long m_elCut;
  long m_muCut;
  long m_spdCut;
  bool m_useDecInBank;
  unsigned long m_count;
  unsigned long m_sel;
  


};
#endif // FASTL0DUFILTER_H
