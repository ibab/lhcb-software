// $Id: CompareTrack.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef COMPARETRACK_H 
#define COMPARETRACK_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"
#include "Event/PackedTrack.h"

/** @class CompareTrack CompareTrack.h
 *  Compare two containers of Tracks
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class CompareTrack : public GaudiAlgorithm
{

public: 

  /// Standard constructor
  CompareTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareTrack( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_testName;

};

#endif // COMPARETRACK_H
