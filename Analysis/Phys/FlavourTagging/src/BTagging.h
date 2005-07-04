// $Id: BTagging.h,v 1.13 2005-07-04 08:20:05 pkoppenb Exp $
#ifndef USER_BTAGGING_H 
#define USER_BTAGGING_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "FlavourTagging/IBTaggingTool.h"

/** @class BTagging BTagging.h 
 *
 *  Algorithm to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class BTagging : public DVAlgorithm {

public: 
  /// Standard constructor
  BTagging( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTagging( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  IBTaggingTool* m_tag;
  IDataProviderSvc* m_eventSvc;
  bool m_WriteToTES;
  std::string m_TagLocation, m_PartsLocation;

};

//=======================================================================//
#endif // USER_BTAGGING_H
