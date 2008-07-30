#ifndef IHLTDATASVC_H
#define IHLTDATASVC_H
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "HltBase/stringKey.h"
#include "HltBase/HltSelection.h"
#include "HltTypes.h"
#include <vector>
#include <boost/function.hpp>
class IAlgorithm;

class IHltDataSvc : virtual public INamedInterface {

public:

    static const InterfaceID& interfaceID();
    virtual ~IHltDataSvc();


    //@TODO: have two interfaces: one to only 'query' the structure, the other to
    //       get access to everything, the latter inheriting from the former...
    //@TODO: extend the query capabilities: who uses what as input, who produced which output..
    //@TODO: move the {register,retrieve}{,T}selection overhere...
    virtual StatusCode addSelection(Hlt::Selection* sel,IAlgorithm* parent,bool originatesFromTES=false) = 0;
    virtual bool hasSelection(const stringKey& id) const = 0;
    //@TODO: make typesafe, i.e. allow getting of derived classes 
    virtual Hlt::Selection& selection(const stringKey& id,IAlgorithm* parent) =0;
    virtual std::vector<stringKey> selectionKeys() = 0;

    virtual Hlt::Configuration& config() = 0;

};
#endif
