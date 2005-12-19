// $Id: TTDumpGeom.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _TTDumpGeom_H
#define _TTDumpGeom_H

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

class DeTTDetector;

/** @class TTDumpGeom TTDumpGeom.h
 *
 *  Class for printing out geometry
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class TTDumpGeom : public GaudiAlgorithm {

public:
 
  /// constructer
  TTDumpGeom(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~TTDumpGeom();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();


private:

  DeTTDetector* m_tracker;
 
};

#endif // _TTDumpGeom_H













