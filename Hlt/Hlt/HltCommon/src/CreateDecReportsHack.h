// $Id: CreateDecReportsHack.h,v 1.1 2008-09-21 16:44:57 graven Exp $
#ifndef HLTCOMMON_HLTDECISION_H 
#define HLTCOMMON_HLTDECISION_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class CreateDecReportsHack CreateDecReportsHack.h
 *  
 *  functionality:
 *
 *  Options:
 *  @author Gerhard Raven
 *  @date   2008-09-02
 */
class CreateDecReportsHack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CreateDecReportsHack( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CreateDecReportsHack( ); ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_location;
};
#endif
