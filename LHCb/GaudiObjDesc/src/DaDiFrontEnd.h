// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiFrontEnd.h,v 1.1.1.1 2001-10-03 16:39:17 mato Exp $
#ifndef DADI_FRONTEND_H 
#define DADI_FRONTEND_H 1

#include <iostream>
#include <dom/DOM_Node.hpp>
#include <list>

#include "DaDiTools.h"
#include "DaDiPackage.h"

/** @class DDFE DaDiFrontEnd.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   29/06/2001
 */



class DDFE
{
public:

 DDFE() {};
 ~DDFE() {};

static DaDiPackage* DaDiFrontEnd(char* filename);
static void parseImport(DOM_Node node, DaDiPackage* gddPackage);
static void parseImport(DOM_Node node, DaDiClass* gddClass);
static void parsePackage(DOM_Node node, DaDiPackage* gddPackage);
static void parseClass(DOM_Node node, DaDiClass* gddClass);
};



#endif // DADI_FRONTEND_H
