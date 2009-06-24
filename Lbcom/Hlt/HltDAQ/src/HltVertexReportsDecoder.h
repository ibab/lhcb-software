// $Id: HltVertexReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
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

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };


  /// Standard constructor
  HltVertexReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float doubleFromInt(unsigned int i);

protected:

private:


  // ----------------------- data members 

  /// location of output
  StringProperty m_outputHltVertexReportsLocation;

  /// location of input
  StringProperty m_inputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  


};

#endif // HLTVERTEXREPORTSDECODER_H
