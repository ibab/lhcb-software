// $Id: PackTrack.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef PACKTRACK_H 
#define PACKTRACK_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/PackedTrack.h"

/** @class PackTrack PackTrack.h
 *
 *  Pack a track container
 *
 *  @author Olivier Callot
 *  @date   2008-11-12
 */
class PackTrack : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  PackTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTrack( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution
  
private:

  std::string m_inputName;  ///< Input location
  std::string m_outputName; ///< Output location
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing
  bool m_enableCheck;       ///< Flag to turn on automatic unpacking and checking of the output post-packing

};

#endif // PACKTRACK_H
