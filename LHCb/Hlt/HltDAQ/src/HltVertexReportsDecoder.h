// $Id: HltVertexReportsDecoder.h,v 1.3 2010-06-14 13:38:00 tskwarni Exp $
#ifndef HLTVERTEXREPORTSDECODER_H 
#define HLTVERTEXREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsDecoder HltvertexReportsDecoder.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsDecoder : public GaudiAlgorithm {
public:

  enum HeaderIDs { kVersionNumber=2 };


  /// Standard constructor
  HltVertexReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  // ----------------------- data members 

  /// location of output
  StringProperty m_outputHltVertexReportsLocation;

  /// location of input
  std::string m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  

  /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
  UnsignedIntegerProperty m_sourceID;

};

#endif // HLTVERTEXREPORTSDECODER_H
