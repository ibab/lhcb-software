// $Id: HltVertexReportsDecoder.h,v 1.3 2010-06-14 13:38:00 tskwarni Exp $
#ifndef HLTVERTEXREPORTSDECODER_H 
#define HLTVERTEXREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "HltRawBankDecoderBase.h"



/** @class HltvertexReportsDecoder HltvertexReportsDecoder.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsDecoder : public HltRawBankDecoderBase {
public:

  enum HeaderIDs { kVersionNumber=2 };

  /// Standard constructor
  HltVertexReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );
  ~HltVertexReportsDecoder( ) override = default; ///< Destructor
  StatusCode execute   () override;    ///< Algorithm execution

private:
  // ----------------------- data members 
  /// location of output
  StringProperty m_outputHltVertexReportsLocation;

};

#endif // HLTVERTEXREPORTSDECODER_H
