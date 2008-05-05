#ifndef MONOBJECTCREATOR_H
#define MONOBJECTCREATOR_H 1

#include "Gaucho/MonObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

//  Author: jotalo, 28/04/2008

class MonObjectCreator {
private:

public:
  MonObjectCreator();
  virtual ~MonObjectCreator();
  static MonObject* createMonObject(const std::string &monObjectTypeName, IMessageSvc*  msgSvc, const std::string &source);
  static int monObjectSize(MonObject* monObject);
};

#endif // MONOBJECTCREATOR_H

