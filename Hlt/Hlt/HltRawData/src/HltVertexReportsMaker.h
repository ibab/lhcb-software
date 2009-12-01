// $Id: HltVertexReportsMaker.h,v 1.5 2009-12-01 22:53:41 tskwarni Exp $
#ifndef HLTVERTEXREPORTSMAKER_H 
#define HLTVERTEXREPORTSMAKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltVertexReports.h"
#include "Kernel/IANNSvc.h"
#include "HltBase/IHltDataSvc.h"
#include "Kernel/IOnOffline.h"


/** @class HltvertexReportsMaker HltvertexReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsMaker : public GaudiAlgorithm {
public:

  /// Standard constructor
  HltVertexReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // ----------------------- data members 

  /// location of output Hlt Summary
  StringProperty m_outputHltVertexReportsLocation;

  /// location of output Hlt Summary
  StringArrayProperty m_vertexSelections;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  
  IHltDataSvc* m_hltDataSvc;  
  IOnOffline* m_onOfflineTool;

  std::vector< std::string > m_selectionNames;
  std::string m_tesPath;  

};

#endif // HLTVERTEXREPORTSMAKER_H
