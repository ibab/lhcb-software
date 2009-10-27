// $Id: BTagging.h,v 1.18 2009-10-27 11:08:35 jpalac Exp $
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
  inline const std::vector<std::string>& inputLocations() 
  {
    return m_inputLocations;
  }

  void performTagging(const std::string & location);

private:
  //bool m_WriteToTES; ///< write FlavourTag to TES
  std::string m_TagLocation; ///< Location of tags
  std::vector< std::string>  m_inputLocations; ///< InputLocaitons from parent DVAlgorithm. Temporary.
};

//=======================================================================//
#endif // USER_BTAGGING_H
