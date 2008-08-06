// $Id: HltVertexReportsWriter.h,v 1.1 2008-08-06 14:51:54 tskwarni Exp $
#ifndef HLTVERTEXREPORTSWRITER_H 
#define HLTVERTEXREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsWriter HltvertexReportsWriter.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsWriter : public HltBaseAlg {
public:

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };



  /// Standard constructor
  HltVertexReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float floatFromInt(unsigned int i);
  static unsigned int doubleToInt(double d);

protected:

private:


  // ----------------------- data members 

  /// location of input
  StringProperty m_inputHltVertexReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  


};

#endif // HLTVERTEXREPORTSWRITER_H
