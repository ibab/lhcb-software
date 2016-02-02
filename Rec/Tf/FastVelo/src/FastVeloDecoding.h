#ifndef FASTVELODECODING_H 
#define FASTVELODECODING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "VeloDet/DeVelo.h"
#include "Event/ProcStatus.h"

/** @class FastVeloDecoding FastVeloDecoding.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2012-10-15
 */
class FastVeloDecoding : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastVeloDecoding( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastVeloDecoding( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum AlgStatusType{
    OK = 0,
    BadTELL1IDMapping = 1,
    UnsupportedBufferVersion = 2,
    CorruptVeloBuffer = 3,
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
  std::string  m_outputLocation;
  bool         m_ignoreErrors;
  bool         m_compareResult;
  DeVelo*      m_velo;
  unsigned int m_maxVeloClusters;
  unsigned int m_errorCount;  ///< number of errors to print
  std::string  m_compareLocation;
  std::vector<std::string> m_rawEventLocations;
  
};
#endif // FASTVELODECODING_H
