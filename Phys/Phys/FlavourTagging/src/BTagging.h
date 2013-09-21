// $Id: BTagging.h,v 1.19 2009-11-25 08:42:27 jpalac Exp $
#ifndef USER_BTAGGING_H
#define USER_BTAGGING_H 1

#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IBTaggingTool.h"

/** @class BTagging BTagging.h
 *
 *  Algorithm to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   02/10/2006
 */

class BTagging : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  BTagging( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BTagging( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  /// Run the tagging on the given location
  void performTagging(const std::string & location);

private:

  std::string m_TagLocation; ///< Location of tags

};

//=======================================================================//
#endif // USER_BTAGGING_H
