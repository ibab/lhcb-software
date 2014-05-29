// $Id: HltDecReportsDecoder.h,v 1.2 2010-04-08 08:12:02 jpalac Exp $
#ifndef HLTDECREPORTSDECODER_H 
#define HLTDECREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "DAQKernel/DecoderAlgBase.h"
#include "Event/HltDecReport.h"

#include "Kernel/IANNSvc.h"
#include "Kernel/IIndexedANNSvc.h"


/** @class HltDecReportsDecoder HltDecReportsDecoder.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltDecReports from Raw Data and create container on TES
 *
 */
class HltDecReportsDecoder : public Decoder::AlgBase {
public: 
  /// Standard constructor
  HltDecReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  ~HltDecReportsDecoder( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution

private:
  enum HeaderIDs { kVersionNumber=2 };

  template <typename HDRConverter,typename I, typename Table > 
  int decodeHDR(I i, I end,  LHCb::HltDecReports& output, const Table& table) const ;

  /// location of output
  StringProperty m_outputHltDecReportsLocation;
  
  
  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;
  IIndexedANNSvc* m_TCKANNSvc;

  std::map<unsigned int, GaudiUtils::VectorMap<unsigned int, Gaudi::StringKey>> m_translationtables;
  
  /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
  UnsignedIntegerProperty m_sourceID;
};
#endif // HLTDECREPORTSDECODER_H
