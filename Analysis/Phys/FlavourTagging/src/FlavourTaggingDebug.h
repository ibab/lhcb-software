// $Id: FlavourTaggingDebug.h,v 1.1 2002-09-20 12:09:05 odie Exp $
#ifndef FLAVOURTAGGINGDEBUG_H 
#define FLAVOURTAGGINGDEBUG_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCAsct.h"

class IDebugTool;

/** @class FlavourTaggingDebug FlavourTaggingDebug.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   2002-09-12
 */
class FlavourTaggingDebug : public DVAlgorithm {
public:
  /// Standard constructor
  FlavourTaggingDebug( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FlavourTaggingDebug( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  Particle2MCAsct::IAsct* m_pAsctLinks; ///< Pointer to associator using links
  IDebugTool *m_debug;
};
#endif // FLAVOURTAGGINGDEBUG_H
