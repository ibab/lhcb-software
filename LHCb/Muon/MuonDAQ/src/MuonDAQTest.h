#ifndef MUONDAQTEST_H 
#define MUONDAQTEST_H 1

// Include files

#include "MuonDAQ/IMuonRawBuffer.h"
#include "Event/MuonDigit.h"


// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
/** @class MuonDAQTest MuonDAQTest.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-10-19
 */
class MuonDAQTest : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonDAQTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDAQTest( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:


IMuonRawBuffer* m_MuonBuffer;
};
#endif // MUONDAQTEST_H
