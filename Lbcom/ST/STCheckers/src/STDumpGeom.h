// $Id: STDumpGeom.h,v 1.2 2006-12-21 17:54:48 jvantilb Exp $
#ifndef STDumpGeom_H
#define STDumpGeom_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

class DeSTDetector;

/** @class STDumpGeom STDumpGeom.h
 *
 *  Class for printing out the full geometry tree
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class STDumpGeom : public GaudiAlgorithm {

public:
 
  /// constructer
  STDumpGeom(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~STDumpGeom();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  /// Recursively print out the tree of the child DetectorElements
  void children(std::string indent, DetectorElement* parent);

  // job options
  std::string m_detType;   ///< Switch between TT and IT
  bool m_fullDetail;       ///< Print out the details of each DetectorElement
};

#endif // STDumpGeom_H
