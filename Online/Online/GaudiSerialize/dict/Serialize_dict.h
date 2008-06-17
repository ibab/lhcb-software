#include <cstdlib>
#include <vector>
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"

// Add here addition include files for the dictionary generation

struct __Instantiations 
{
std::vector<std::string> p1;
std::vector<DataObject*> p2;
std::vector<std::pair<std::string,DataObject*> > p3;
std::pair<std::string,DataObject*> p4;
//IRegistry* p6;
//LinkManager* p7;
};
