// $Id: STDumpGeom.h,v 1.1 2006-02-20 16:44:32 mneedham Exp $
#ifndef _STDumpGeom_H
#define _STDumpGeom_H

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

class DeSTDetector;

/** @class STDumpGeom STDumpGeom.h
 *
 *  Class for printing out geometry
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class STDumpGeom : public GaudiAlgorithm {

public:
 
  /// constructer
  STDumpGeom(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STDumpGeom();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();


private:

  std::string m_detType;
  DeSTDetector* m_tracker;
 
};

#endif // _STDumpGeom_H













