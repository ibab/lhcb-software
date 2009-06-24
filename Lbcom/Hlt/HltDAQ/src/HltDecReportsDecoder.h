// $Id: HltDecReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTDECREPORTSDECODER_H 
#define HLTDECREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltDecReport.h"

#include "Kernel/IANNSvc.h"


/** @class HltDecReportsDecoder HltDecReportsDecoder.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltDecReports from Raw Data and create container on TES
 *
 */
class HltDecReportsDecoder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltDecReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=1 
  };

  template <typename HDRConverter,typename I > 
  void
  decodeHDR(I i, I end,  
            LHCb::HltDecReports& output,
            const std::vector<IANNSvc::minor_value_type>& NameToIntMap ) const ;

  /// location of input H
  StringProperty m_outputHltDecReportsLocation;

  /// location of input
  StringProperty m_inputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;

};
#endif // HLTDECREPORTSDECODER_H
