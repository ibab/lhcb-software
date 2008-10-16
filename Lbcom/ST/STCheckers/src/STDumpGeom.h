// $Id: STDumpGeom.h,v 1.3 2008-10-16 13:10:34 mneedham Exp $
#ifndef STDumpGeom_H
#define STDumpGeom_H 1

#include "Kernel/STAlgBase.h"

class DetectorElement;

/** @class STDumpGeom STDumpGeom.h
 *
 *  Class for printing out the full geometry tree
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class STDumpGeom : public ST::AlgBase {

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
  bool m_fullDetail;       ///< Print out the details of each DetectorElement
};

#endif // STDumpGeom_H
