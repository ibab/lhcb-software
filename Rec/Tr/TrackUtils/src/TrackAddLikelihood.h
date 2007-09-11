// $Id: TrackAddLikelihood.h,v 1.1 2007-09-11 14:49:46 mneedham Exp $
#ifndef _TrackAddLikelihood_H_
#define _TrackAddLikelihood_H_

/** @class TrackAddLikelihood TrackAddLikelihood.h
 *
 *  Copy a container of tracks. By default do not copy tracks that failed the fit
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

class ITrackManipulator;

class TrackAddLikelihood: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackAddLikelihood(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackAddLikelihood();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  ITrackManipulator* m_likelihood;
  std::string  m_likelihoodToolName;
};

#endif
