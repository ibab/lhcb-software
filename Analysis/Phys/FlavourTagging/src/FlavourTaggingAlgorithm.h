// $Id: FlavourTaggingAlgorithm.h,v 1.1.1.1 2002-05-23 23:25:50 gcorti Exp $
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
  int n_B_events;
  int n_B;
  int n_b_tags;
  int n_bbar_tags;

  std::string m_pDesktop_name;
  std::string m_taggingTool_name;
  std::string m_tags_location;
  std::string m_particles_location;
  std::string m_primVertices_location;

  IPhysDesktop *m_pDesktop;        ///< Reference to desktop tool
  IFlavourTaggingTool *m_taggingTool;
};
#endif // FLAVOURTAGGINGALGORITHM_H
