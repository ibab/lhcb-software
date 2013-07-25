// $Id: HltVertexReportsWriter.h,v 1.2 2009-11-26 13:06:07 tskwarni Exp $
#ifndef HLTVERTEXREPORTSWRITER_H 
#define HLTVERTEXREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsWriter HltvertexReportsWriter.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsWriter : public GaudiAlgorithm {
public:



  /// Standard constructor
  HltVertexReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=1 
  };



  // ----------------------- data members 

  /// location of input
  StringProperty m_inputHltVertexReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  


};

#endif // HLTVERTEXREPORTSWRITER_H
