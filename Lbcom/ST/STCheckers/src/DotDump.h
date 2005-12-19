// $Id: DotDump.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _dotDump_H
#define _dotDump_H

#include <string>
#include <vector>


#include "GaudiAlg/GaudiAlgorithm.h"

class DetectorElement;

/** @class dotDump dotDump.h
 *
 *  Class for printing out geometry
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class DotDump : public GaudiAlgorithm {

public:
 
  /// constructer
  DotDump(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~DotDump();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();


private:
 
  std::string m_path;

  void children(std::string path, DetectorElement* parent);
  std::vector<std::string> m_dStrings;
  void addToDot(const std::string aString);


};

#endif // _dotDump_H













