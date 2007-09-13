// $Id: TrackListPrinter.h,v 1.1 2007-09-13 08:35:07 wouter Exp $
#ifndef _TrackListPrinter_H_
#define _TrackListPrinter_H_

/** @class TrackListPrinter TrackListPrinter.h
 *
 *  Copy a container of tracks. By default do not copy tracks that failed the fit
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>


class TrackListPrinter: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackListPrinter(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackListPrinter();

  virtual StatusCode execute();

private:
  std::string m_inputLocation;
};

#endif
