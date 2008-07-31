// $Id: LumiCountVertices.h,v 1.1 2008-07-31 21:34:47 panmanj Exp $
#ifndef LUMICOUNTVERTICES_H 
#define LUMICOUNTVERTICES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
// from LHCB
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"


/** @class LumiCountVertices LumiCountVertices.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-07-21
 */
class LumiCountVertices : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiCountVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiCountVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_InputSelectionName;
  const LHCb::RecVertices* m_InputContainer;
  std::string m_CounterName;
  int m_Counter;

  std::string m_OutputContainerName;
  LHCb::HltLumiSummarys* m_HltLumiSummarys;

private:

};
#endif // LUMICOUNTVERTICES_H
