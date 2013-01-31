// $Id: $
#ifndef GENERATORS_APPLYPHOTOS_H
#define GENERATORS_APPLYPHOTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class ApplyPhotos ApplyPhotos.h component/ApplyPhotos.h
 *  Class to apply photos on a given particle
 *
 *  @author Patrick Robbe
 *  @date   2011-05-27
 */
class ApplyPhotos : public GaudiAlgorithm {
public:
  /// Standard constructor
  ApplyPhotos( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ApplyPhotos( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::string  m_hepMCEventLocation ;    ///< Input TES for HepMC events

  std::vector< int > m_pdgIdList    ;    ///< list of the particle ID to study
  std::set   < int > m_pdgIds       ;    ///< ordered list of PDG Ids
};
#endif // GENERATORS_APPLYPHOTOS_H
