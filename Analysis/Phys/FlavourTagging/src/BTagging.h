// $Id: BTagging.h,v 1.17 2006-10-24 10:16:44 jpalac Exp $
#ifndef USER_BTAGGING_H 
#define USER_BTAGGING_H 1

#include "Kernel/DVAlgorithm.h"
#include "Kernel/IBTaggingTool.h"

/** @class BTagging BTagging.h 
 *
 *  Algorithm to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   02/10/2006
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
