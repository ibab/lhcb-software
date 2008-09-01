// $Id: STDecodingBaseAlg.h,v 1.12 2008-09-01 08:52:29 mneedham Exp $
#ifndef STDECODINGBASEALG_H 
#define STDECODINGBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RawBank.h"
#include "Kernel/STDAQDefinitions.h"

#include "Event/STSummary.h"
#include "Event/STCluster.h"

#include <vector>
#include <string>

/** @class STDecodingBaseAlg STDecodingBaseAlg.h
 *  
 *  Algorithm to create STClusters from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2004-01-07
 */

#include "Event/RawEvent.h"

#include "Kernel/STClusterWord.h"
#include "Kernel/STDecoder.h"
#include "Event/STTELL1BoardErrorBank.h"

#include <string>

class ISTReadoutTool;
class STTell1Board;

class STDecodingBaseAlg : public GaudiAlgorithm {

public:

  /// Standard constructor
  STDecodingBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STDecodingBaseAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
   
  virtual StatusCode finalize();

protected:

 ISTReadoutTool* readoutTool() const;

 LHCb::RawBank::BankType bankType() const;

 std::string detType() const;

 bool forceVersion() const;  

 void createSummaryBlock(const unsigned int& nclus, const unsigned int& pcn, 
                         const bool pcnsync, const std::vector<unsigned int>& bankList,
                         const std::vector<unsigned int>& missing, 
                         const LHCb::STSummary::RecoveredInfo& recoveredBanks ) const;

 unsigned int pcnVote(const std::vector<LHCb::RawBank* >& banks) const;

 bool checkDataIntegrity(STDecoder& decoder, const STTell1Board* aBoard, 
                         const unsigned int bankSize, const int version) const;

 /** list of boards missing in action */
 std::vector<unsigned int> missingInAction(const std::vector<LHCb::RawBank*>& banks) const; 

 /** decode the error banks */ 
 LHCb::STTELL1BoardErrorBanks* getErrorBanks() const; 

 /** recover mode **/
 bool recoverMode() const;

 /** find the error bank **/
 LHCb::STTELL1BoardErrorBank* findErrorBank(const unsigned int& source) const;

 /** can be recovered recover **/
 bool canBeRecovered(const LHCb::STTELL1BoardErrorBank* bank, 
                     const STClusterWord& word, 
                     const unsigned int pcn) const;

 bool m_skipErrors;
 std::string m_bankTypeString;
 std::string m_rawEventLocation;
 int m_forcedVersion;
 
private:

  StatusCode decodeErrors() const;

  std::string m_detType;

  std::string m_errorLocation;
  std::string m_errorBankString;
  LHCb::RawBank::BankType m_errorType; 

  std::string m_summaryLocation;
  LHCb::RawBank::BankType m_bankType;

  std::string m_readoutToolName;
  ISTReadoutTool* m_readoutTool;
 
  bool m_recoverMode;

};

inline ISTReadoutTool* STDecodingBaseAlg::readoutTool() const{
  return m_readoutTool;
}

inline LHCb::RawBank::BankType STDecodingBaseAlg::bankType() const {
  return m_bankType;
}

inline std::string STDecodingBaseAlg::detType() const{
  return m_detType;
}

inline bool STDecodingBaseAlg::forceVersion() const{
  return m_forcedVersion >= 0;
}

inline bool STDecodingBaseAlg::recoverMode() const{
  return m_recoverMode;
}

inline LHCb::STTELL1BoardErrorBank* STDecodingBaseAlg::findErrorBank(const unsigned int& source) const{
  LHCb::STTELL1BoardErrorBanks* errorBanks = getErrorBanks();
  return(errorBanks->object(source));
}

#include "Kernel/STClusterWord.h"
#include "Kernel/PPRepresentation.h"
#include "Kernel/StripRepresentation.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/LHCbConstants.h"
#include "Event/STTELL1Error.h"

inline bool STDecodingBaseAlg::canBeRecovered(const LHCb::STTELL1BoardErrorBank* bank, 
                                              const STClusterWord& word,
                                              const unsigned int pcn) const{

  STDAQ::PPRepresentation ppRep = STDAQ::PPRepresentation(STDAQ::StripRepresentation(word.channelID()));
  unsigned int pp, beetle, port, strip;
  ppRep.decompose(pp, beetle, port, strip); // split up the word 
  const LHCb::STTELL1Error* errorInfo = bank->ppErrorInfo(pp);
  bool ok = false;
  if (errorInfo != 0 ){
     if (errorInfo->badLink(beetle, port,pcn) == true){
      std::cout << " pcn " << pcn << "bad link " << word.channelID() << " pp " << pp << " " << beetle << " port " << port  << " chip " << errorInfo->ChipAddr() <<std::endl;  
     }
     else { 
       ok = true;
     }

  }
  return ok ; 
}



#endif // STDECODINGBASEALG_H 
