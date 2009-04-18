// $Id: HltVertexReportsReader.h,v 1.2 2009-04-18 18:52:37 graven Exp $
#ifndef HLTVERTEXREPORTSREADER_H 
#define HLTVERTEXREPORTSREADER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsReader HltvertexReportsReader.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsReader : public GaudiAlgorithm {
public:

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };


  /// Standard constructor
  HltVertexReportsReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsReader( ); ///< Destructor

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

#endif // HLTVERTEXREPORTSREADER_H
