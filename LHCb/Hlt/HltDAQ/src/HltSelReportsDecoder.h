// $Id: HltSelReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTSELREPORTSDECODER_H 
#define HLTSELREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "HltRawBankDecoderBase.h"

/** @class HltSelReportsDecoder HltSelReportsDecoder.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltSelReports from Raw Data and create containers on TES
 *
 */

class HltSelReportsDecoder : public HltRawBankDecoderBase {
public: 

  enum HeaderIDs { kVersionNumber=2 };

  /// Standard constructor
  HltSelReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );
  ~HltSelReportsDecoder( ) override = default; ///< Destructor

  StatusCode execute   () override;    ///< Algorithm execution

private:

  /// location of output
  StringProperty m_outputHltSelReportsLocation;

  /// location of HltDecReports (will not be used unless ANNSvc not working) 
  StringProperty m_HltDecReportsLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;


};

#endif // HLTSELREPORTSDECODER_H
