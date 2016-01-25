#ifndef _TrackCloneCleaner_H_
#define _TrackCloneCleaner_H_

#include "GaudiAlg/GaudiAlgorithm.h"
/** @class TrackCloneCleaner TrackCloneCleaner.h
 *
 *  Clean out clone tracks, using information from the Clone linker table
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */
class TrackCloneCleaner: public GaudiAlgorithm
{

public:

  // Constructors and destructor
  TrackCloneCleaner(const std::string& name,
                    ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrackCloneCleaner() override;

  /// Execute
  StatusCode execute() override;

private:

  std::string m_inputLocation;  ///< Locations of Tracks in TES
  std::string m_linkerLocation; ///< Location of Clone linker in TES
  double m_cloneCut;            ///< Clone cut value

};

#endif
