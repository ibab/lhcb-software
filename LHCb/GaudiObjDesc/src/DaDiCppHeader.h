// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiCppHeader.h,v 1.1.1.1 2001-10-03 16:39:17 mato Exp $
#ifndef DADICPPHEADER_H 
#define DADICPPHEADER_H 1

// Include files
#include "DaDiFrontEnd.h"
#include "DaDiTools.h"
#include "DaDiPackage.h"

/** @class DaDiCppHeader DaDiCppHeader.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   13/06/2001
 */

class DDBEcpp
{
public:
  DDBEcpp() {};
  ~DDBEcpp() {};
  
  static void printCppHeader(DaDiPackage* gddPackage, char* envXmlDB, char* envOut, bool additionalImports);

};

#endif // DADICPPHEADER_H
