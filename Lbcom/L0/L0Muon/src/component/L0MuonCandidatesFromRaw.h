// $Id: L0MuonCandidatesFromRaw.h,v 1.1 2006-10-20 10:37:29 jucogan Exp $
#ifndef L0MUONCANDIDATESFROMRAW_H 
#define L0MUONCANDIDATESFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
// //#include "GaudiKernel/Algorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/MuonTileID.h"

#include "Event/L0MuonCandidate.h"
#include "Event/RawEvent.h"

/** @class L0MuonCandidatesFromRaw L0MuonCandidatesFromRaw.h
 *  
 *
 *  @author 
 *  @date
 */
class L0MuonCandidatesFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0MuonCandidatesFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  ~L0MuonCandidatesFromRaw( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  // Return the MID of the pads corresponding to the given addresses and offsets.
  std::vector<LHCb::MuonTileID> address2Pads(int quarter, int board, int pu, int addM3, int offM2, int offM1); 
    
  std::vector<double> m_ptParameters;  // Pt calculation parameters

  // Counters for final report
  int m_totNumCand; // Tot number of candidates found
  int m_totTrigEvt; // Tot number of events with at least a candidate
  int m_totRawSize; // Total size of the raw buffer
  int m_totEvent ;  // Tot number of events processed
  double m_averagePt ; // Average Pt of the candidates    
  double m_averageCh ; // Average charge of the candidates 
  
};
#endif // L0MUONCANDIDATESFROMRAW_H
