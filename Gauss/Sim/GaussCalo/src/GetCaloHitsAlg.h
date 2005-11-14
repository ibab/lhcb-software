// $Id: GetCaloHitsAlg.h,v 1.1 2005-11-14 14:57:46 robbep Exp $
#ifndef GAUSSCALO_GETCALOHITSALG_H 
#define GAUSSCALO_GETCALOHITSALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGiGaSvc ;
class IGiGaKineCnvSvc ;

/** @class GetCaloHitsAlg GetCaloHitsAlg.h
 *  Conversion from G4 hits to MC CaloHits
 *
 *  @author Patrick Robbe
 *  @date   2005-11-14
 */
class GetCaloHitsAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  GetCaloHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetCaloHitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
/// Name of the GiGa service
  std::string m_gigaSvcName ;
  /// Name of the GiGaCnv service
  std::string m_kineSvcName ;
  /// Name of the output location of MCHits
  std::string m_hitsLocation ;
  /// Name of the G4 hits collection
  std::string m_colName ;

  /// Pointer to the GiGa service
  IGiGaSvc * m_gigaSvc ;
  /// Pointer to the GiGaKine service
  IGiGaKineCnvSvc * m_gigaKineCnvSvc ;
};
#endif // GAUSSCALO_GETCALOHITSALG_H
