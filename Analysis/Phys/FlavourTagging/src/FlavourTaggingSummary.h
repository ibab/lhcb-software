// $Id: FlavourTaggingSummary.h,v 1.1 2003-03-27 09:54:14 odie Exp $
#ifndef FLAVOURTAGGINGSUMMARY_H 
#define FLAVOURTAGGINGSUMMARY_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"

/** @class FlavourTaggingSummary FlavourTaggingSummary.h
 *  Print a one line summary of all the tags.
 *
 *  @author Olivier Dormond
 *  @date   2003-03-27
 */
class FlavourTaggingSummary : public DVAlgorithm {
public:
  /// Standard constructor
  FlavourTaggingSummary( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FlavourTaggingSummary( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Vector of locations of the tags to monitor
  std::vector<std::string> m_tagsLocations;
};
#endif // FLAVOURTAGGINGSUMMARY_H
