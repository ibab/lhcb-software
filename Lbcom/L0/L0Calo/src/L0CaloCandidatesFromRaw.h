// $Id: L0CaloCandidatesFromRaw.h,v 1.3 2006-11-07 10:25:41 ocallot Exp $
#ifndef L0CALOCANDIDATESFROMRAW_H 
#define L0CALOCANDIDATESFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "L0CaloCandidatesFromRawBank.h"

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
  std::string    m_extension;    ///< Adedd to default name, allows comparisons
  L0CaloCandidatesFromRawBank* m_convertTool;
};
#endif // L0CALOCANDIDATESFROMRAW_H
