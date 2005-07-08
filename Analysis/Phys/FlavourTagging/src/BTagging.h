// $Id: BTagging.h,v 1.16 2005-07-08 11:32:06 pkoppenb Exp $
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
#include "Kernel/IBTaggingTool.h"

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
  //bool m_WriteToTES; ///< write FlavourTag to TES
  std::string m_TagLocation; ///< Location of tags

};

//=======================================================================//
#endif // USER_BTAGGING_H
