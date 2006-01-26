// $Id: L0CaloCandidatesFromRaw.h,v 1.2 2006-01-26 16:52:13 ocallot Exp $
#ifndef L0CALOCANDIDATESFROMRAW_H 
#define L0CALOCANDIDATESFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "CaloDet/DeCalorimeter.h"

#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"

/** @class L0CaloCandidatesFromRaw L0CaloCandidatesFromRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-12-15
 */
class L0CaloCandidatesFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0CaloCandidatesFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0CaloCandidatesFromRaw( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  DeCalorimeter* m_ecal;         ///< ECAL detector element
  DeCalorimeter* m_hcal;         ///< HCAL detector element
  std::string    m_extension;    ///< Adedd to default name, allows comparisons

  double         m_etScale;      ///< Conversion int -> Et.
};
#endif // L0CALOCANDIDATESFROMRAW_H
