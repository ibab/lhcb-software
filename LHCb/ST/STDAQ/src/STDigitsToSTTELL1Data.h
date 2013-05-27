// $Id: STDigitsToSTTELL1Data.h,v 1.1 2008-10-17 15:13:24 mneedham Exp $
#ifndef STDIGITSTOSTTELL1DATA_H 
#define STDIGITSTOSTTELL1DATA_H 1

#include "Kernel/STAlgBase.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/STTELL1Data.h"
#include "Event/STDigit.h"

#include <vector>
#include <string>

/** @class RawBankToSTProcFull RawBankToSTProcFull.h
 *  
 *  Algorithm to create STTELL1Data (type ProcFull) from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2007-10-10
 */

class ISTReadoutTool;
class DeSTDetector;

class STDigitsToSTTELL1Data: public ST::AlgBase {
    
public:

  /// Standard constructor
  STDigitsToSTTELL1Data( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STDigitsToSTTELL1Data(); ///< Destructor


  virtual StatusCode execute();    ///< Algorithm execution

private:

  StatusCode createTell1Data(const LHCb::STDigits* digits, 
                             LHCb::STTELL1Datas* outCont) const;


  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // STDigitsToSTTELL1Data_H 
