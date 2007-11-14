// $Id: HltPrepareVertices.h,v 1.2 2007-11-14 13:57:03 hernando Exp $
#ifndef HLTPREPAREVERTICES_H 
#define HLTPREPAREVERTICES_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltPrepareVertices : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_TESInputVerticesName;

};
#endif // HLTHADALLEYPRETRIGGER_H
