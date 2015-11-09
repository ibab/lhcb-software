// $Id: HltSelReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTSELREPORTSDECODER_H 
#define HLTSELREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "HltRawBankDecoderBase.h"

#include "HltDAQ/IReportConvert.h"
#include "HltDAQ/ReportConvertTool.h"

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
  /// Standard constructor
  HltSelReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode execute   () override;    ///< Algorithm execution

private:
  enum HeaderIDs { kVersionNumber=7 };
  /// location of output
  StringProperty m_outputHltSelReportsLocation;
  /// for converting objects in to summaries
  IReportConvert* m_conv;
};

#endif // HLTSELREPORTSDECODER_H
