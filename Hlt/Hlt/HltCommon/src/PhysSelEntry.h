// $Id: PhysSelEntry.h,v 1.1 2007-02-01 18:35:55 hernando Exp $
#ifndef PHYSSELENTRY_H 
#define PHYSSELENTRY_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/SelResult.h"
#include "HltBase/HltAlgorithm.h"

/** @class PhysSelEntry PhysSelEntry.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class PhysSelEntry : public HltAlgorithm {
public: 
  /// Standard constructor
  PhysSelEntry( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PhysSelEntry( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_physSelLocation;

  StringArrayProperty m_physSelNames;

protected:

  LHCb::SelResults* m_physSels;

  std::vector<std::string> m_decays;

  HltHisto m_histoPhysSel;

protected:
  

};
#endif // L0ENTRY_H
