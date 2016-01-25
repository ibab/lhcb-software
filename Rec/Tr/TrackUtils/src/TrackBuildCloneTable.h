#ifndef _TrackBuildCloneTable_H_
#define _TrackBuildCloneTable_H_

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

#include "TrackInterfaces/ITrackExtrapolator.h"

/** @class TrackBuildCloneTable TrackBuildCloneTable.h
 *
 *  Creates a summary linker object container clone information,
 *  using a Kullbeck Liebler Distance
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

class TrackBuildCloneTable: public GaudiAlgorithm
{

public:

  /// Constructor
  TrackBuildCloneTable(const std::string& name,
                       ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrackBuildCloneTable() override;

  /// Initialisation
  StatusCode initialize() override;

  /// Execution
  StatusCode execute() override;

private:

  double m_klCut; ///< Maximum distance for the information to be stored in the linker
  std::string m_inputLocation;  ///< Input TES location for tracks
  std::string m_outputLocation; ///< Output TES location for the linker object

  std::vector<double> m_zStates = { 0.0 } ; ///< The z position(s) to use for the comparision
  double m_maxDz;  ///< max tolerance on finding a state close to the comparison z position

  ITrackExtrapolator* m_extrapolator = nullptr; ///< Pointer to the state extrapolator to use
  std::string m_extraType;            ///< The state extrapolator type

};

#endif
