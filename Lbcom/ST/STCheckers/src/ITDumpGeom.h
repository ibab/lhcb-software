// $Id: ITDumpGeom.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _ITDumpGeom_H
#define _ITDumpGeom_H


#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

class DeITDetector;

/** @class ITDumpGeom ITDumpGeom.h
 *
 *  Class for printing out geometry
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class ITDumpGeom : public GaudiAlgorithm {

public:
 
  /// constructer
  ITDumpGeom(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~ITDumpGeom();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();



private:

  DeITDetector* m_tracker;
 
};

#endif // _ITDumpGeom_H













