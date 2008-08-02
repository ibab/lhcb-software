// $Id: HltSelReportsReader.h,v 1.1.1.1 2008-08-02 16:40:07 tskwarni Exp $
#ifndef HLTSELREPORTSREADER_H 
#define HLTSELREPORTSREADER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "HltBase/HltBaseAlg.h"

#include "Kernel/IANNSvc.h"


/** @class HltSelReportsReader HltSelReportsReader.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltSelReports from Raw Data and create containers on TES
 *
 */
class HltSelReportsReader : public GaudiAlgorithm {
//class HltSelReportsReader :  public HltBaseAlg {
public: 

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };

  /// Standard constructor
  HltSelReportsReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelReportsReader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float floatFromInt(unsigned int i);

protected:

  /// location of output
  StringProperty m_outputHltSelReportsLocation;

  /// location of input
  StringProperty m_inputRawEventLocation;


  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;

private:



};


#endif // HLTSELREPORTSREADER_H
