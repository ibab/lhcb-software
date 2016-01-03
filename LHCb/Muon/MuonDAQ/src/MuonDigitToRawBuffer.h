#ifndef MUONDIGITTORAWBUFFER_H 
#define MUONDIGITTORAWBUFFER_H 1

// Include files
// from STL
#include <string>

#include "Kernel/MuonTileID.h"
#include "MuonDet/DeMuonDetector.h"
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class MuonDigitToRawBuffer MuonDigitToRawBuffer.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-19
 */
class MuonDigitToRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonDigitToRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDigitToRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  StatusCode ProcessDC06(); 
  StatusCode ProcessV1();
  StatusCode ProcessDigitDC06();  
  StatusCode ProcessDigitV1();  
  StatusCode ProcessPads();
  

protected:

private: 
  DeMuonDetector* m_muonDet;

  std::vector<unsigned int> m_digitsInODE[MuonDAQHelper_maxODENumber];
  std::vector<unsigned int> m_digitsInL1[MuonDAQHelper_maxTell1Number];
  unsigned int firedInODE[MuonDAQHelper_maxODENumber];
  unsigned int firedInPP[MuonDAQHelper_maxTell1Number*4];


  std::vector<unsigned int> m_padInL1[MuonDAQHelper_maxTell1Number];  


  long m_TotL1Board;
  unsigned int m_M1Tell1;

  unsigned int m_vtype;
  
};
#endif // MUONDIGITTORAWBUFFER_H
