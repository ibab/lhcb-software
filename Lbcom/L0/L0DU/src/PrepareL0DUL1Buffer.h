// $Id: PrepareL0DUL1Buffer.h,v 1.1 2004-01-26 14:28:45 odescham Exp $
#ifndef PREPAREL0DUL1BUFFER_H 
#define PREPAREL0DUL1BUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0Muon.h"
  

/** @class PrepareL0DUL1Buffer PrepareL0DUL1Buffer.h
 *  L0DU -> L1Buffer
 *
 *  @author Olivier Deschamps
 *  @date   2004-01-13
 */
class PrepareL0DUL1Buffer : public Algorithm {
public:
  /// Standard constructor
  PrepareL0DUL1Buffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareL0DUL1Buffer( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_nameOfInputL0MuonCandidate;
  std::string m_nameOfInputPileUpVeto;
  /// Setting the candidate bitcode bits
  void setWord(l1_int& word, int value, int shift, int mask);
};
#endif // PREPAREL0DUL1BUFFER_H
