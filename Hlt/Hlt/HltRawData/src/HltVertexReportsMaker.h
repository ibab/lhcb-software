// $Id: HltVertexReportsMaker.h,v 1.3 2009-02-24 13:50:27 graven Exp $
#ifndef HLTVERTEXREPORTSMAKER_H 
#define HLTVERTEXREPORTSMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"

#include "Event/HltVertexReports.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsMaker HltvertexReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsMaker : public HltBaseAlg {
public:

  /// Standard constructor
  HltVertexReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // ----------------------- data members 

  /// location of output Hlt Summary
  StringProperty m_outputHltVertexReportsLocation;

  /// location of output Hlt Summary
  StringArrayProperty m_vertexSelections;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  


};

#endif // HLTVERTEXREPORTSMAKER_H
