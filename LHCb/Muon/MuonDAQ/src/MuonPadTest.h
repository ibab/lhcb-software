#ifndef MUONPADTEST_H 
#define MUONPADTEST_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "MuonDAQ/IMuonRawBuffer.h"


/** @class MuonPadTest MuonPadTest.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-03-19
 */
class MuonPadTest : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonPadTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonPadTest( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
IMuonRawBuffer* m_MuonBuffer;

};
#endif // MUONPADTEST_H
