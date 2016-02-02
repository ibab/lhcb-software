#ifndef FASTSTDECODING_H 
#define FASTSTDECODING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "STDet/DeSTDetector.h"
#include "Kernel/ISTReadoutTool.h"
#include "Event/ProcStatus.h"

/** @class FastSTDecoding FastSTDecoding.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2012-10-15
 */
class FastSTDecoding : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastSTDecoding( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastSTDecoding( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum AlgStatusType{
    OK = 0,
    BadTELL1IDMapping = 1,
    UnsupportedBufferVersion = 2,
    CorruptSTBuffer = 3,
    TooManyClusters = 4,
    HeaderErrorBit = 5,
    Other = 99
  };

protected:
  void failEvent( const std::string &ErrorText,
                  const std::string &ProcText,
                  AlgStatusType status,
                  bool procAborted );

private:
  std::string   m_outputLocation;
  bool          m_ignoreErrors;
  bool          m_compareResult;
  DeSTDetector* m_stDet;
  ISTReadoutTool* m_readoutTool;
  std::string   m_detectorName;
  LHCb::RawBank::BankType m_bankType;
  std::string   m_compareLocation;
  unsigned int  m_errorCount;  ///< number of errors to print
  std::vector<std::string> m_rawEventLocations;
  

};
#endif // FASTSTDECODING_H
