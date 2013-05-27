#ifndef STDECODINGBASEALG_H 
#define STDECODINGBASEALG_H 1

#include "Kernel/STAlgBase.h"
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


class STTell1Board;

class STDecodingBaseAlg : public ST::AlgBase {

public:

  /// Standard constructor
  STDecodingBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STDecodingBaseAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
   

protected:


 LHCb::RawBank::BankType bankType() const;

 bool forceVersion() const;  

 void createSummaryBlock(LHCb::RawEvent* rawEvt, const unsigned int& nclus, const unsigned int& pcn, 
                         const bool pcnsync, const unsigned int bytes,  const std::vector<unsigned int>& bankList,
                         const std::vector<unsigned int>& missing, 
                         const LHCb::STSummary::RecoveredInfo& recoveredBanks ) const;

 unsigned int pcnVote(const std::vector<LHCb::RawBank* >& banks) const;

 bool checkDataIntegrity(STDecoder& decoder, const STTell1Board* aBoard, 
                         const unsigned int bankSize, const STDAQ::version& bankVersion) const;

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

 /** check the spill is read out **/
 bool validSpill() const;

 /** return spill offset */
 LHCb::STCluster::Spill spill() const;

 unsigned int byteSize(const std::vector<LHCb::RawBank*>& banks) const;
 
 bool m_skipErrors;
 std::string m_bankTypeString;

  /// Location in the transient store of the RawEvent object.
  /// @warning Obsolete: use m_rawEventLocations
  std::string m_rawEventLocation;

  /// List of locations in the transient store to search the RawEvent object.
  std::vector<std::string> m_rawEventLocations;

 int m_forcedVersion;
 bool m_checkValidSpill;
 
private:

  StatusCode decodeErrors() const;


  std::string toSpill(const std::string& location) const;
  LHCb::STCluster::Spill spillOffset(const std::string& spill) const;
  LHCb::STCluster::Spill m_spillOffset;



  std::string m_errorLocation;
  std::string m_errorBankString;
  std::string m_pedestalBankString;
  std::string m_fullBankString;
  LHCb::RawBank::BankType m_errorType; 
  LHCb::RawBank::BankType m_pedestalType; 
  LHCb::RawBank::BankType m_fullType; 
 

  std::string m_summaryLocation;
  LHCb::RawBank::BankType m_bankType;

 
  bool m_recoverMode;

};



inline LHCb::RawBank::BankType STDecodingBaseAlg::bankType() const {
  return m_bankType;
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
    if (errorInfo->linkInfo(beetle, port,pcn) == LHCb::STTELL1Error::kNone){
      ok = true;
    }
  }
  return ok ; 
}

inline LHCb::STCluster::Spill STDecodingBaseAlg::spill() const{
  return m_spillOffset;
}


#endif // STDECODINGBASEALG_H 
