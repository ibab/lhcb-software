// $Id: DaDiFrontEnd.h,v 1.6 2003-04-30 12:04:18 mato Exp $
#ifndef DADI_FRONTEND_H
#define DADI_FRONTEND_H 1

#include "DaDiTools.h"
#include "DaDiPackage.h"

#include "xercesc/dom/DOMNode.hpp"

#include <iostream>
#include <list>

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

  static void DaDiFrontEnd(DaDiPackage*, char* filename);
  //static void parseImport(DOM_Node node, DaDiPackage* gddPackage);
  //static void parseImport(DOM_Node node, DaDiClass* gddClass);
  //static void parsePackage(DOM_Node node, DaDiPackage* gddPackage);
  //static void parseClass(DOM_Node node, DaDiClass* gddClass);
  //static void parseBaseClass(DOM_Node node, DaDiClass* gddClass);
  //static void parseEnum(DOM_Node node, DaDiClass* gddClass);
  //static void parseMethod(DOM_Node node, DaDiClass* gddClass);
  //static void parseConstructor(DOM_Node node, DaDiClass* gddClass);
  //static void parseDestructor(DOM_Node node, DaDiClass* gddClass);
  //static void parseAttribute(DOM_Node node, DaDiClass* gddClass);
  //static void parseRelation(DOM_Node node, DaDiClass* gddClass);

};



#endif // DADI_FRONTEND_H
