// $Id: FlavourTaggingAlgorithm.h,v 1.2 2002-09-03 08:22:07 odie Exp $
#ifndef FLAVOURTAGGINGALGORITHM_H 
#define FLAVOURTAGGINGALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

class IPhysDesktop;
class IFlavourTaggingTool;

/** @class FlavourTaggingAlgorithm FlavourTaggingAlgorithm.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   24/04/2002
 */
class FlavourTaggingAlgorithm : public Algorithm {
public:
  /// Standard constructor
  FlavourTaggingAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FlavourTaggingAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_n_B_events;
  int m_n_B;
  int m_n_b_tags;
  int m_n_bbar_tags;

  std::string m_pDesktop_name;
  std::string m_taggingTool_name;
  std::string m_tags_location;
  std::string m_primVertices_location;
  std::vector<std::string> m_hypothesis_locations;

  IPhysDesktop *m_pDesktop;        ///< Reference to desktop tool
  IFlavourTaggingTool *m_taggingTool;
};
#endif // FLAVOURTAGGINGALGORITHM_H
