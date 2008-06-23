#ifndef IHLTDATASVC_H
#define IHLTDATASVC_H
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "HltBase/stringKey.h"
#include "HltData.h"
#include "HltTypes.h"
#include <vector>
class IAlgorithm;

class IHltDataSvc : virtual public INamedInterface {

public:
    static const InterfaceID& interfaceID();
    virtual ~IHltDataSvc();

    virtual void resetData() = 0;

    //@TODO: need to add flag to indicate ownership: if _also_ in TES, we don't own it,
    //       and should not delete/clean the contents...
    //@TODO: add the possibility to have the datasvc put it (also) into the TES...
    //@TODO: have two interfaces: one to only 'query' the structure, the other to
    //       get access to everything, the latter inheriting from the former...
    //@TODO: extend the query capabilities: who uses what as input, who produced which output..
    //@TODO: move the {register,retrieve}{,T}selection overhere...
    virtual StatusCode addSelection(Hlt::Selection* sel,IAlgorithm* parent,bool useTES=false) = 0;
    virtual bool hasSelection(const stringKey& id) const = 0;
    virtual Hlt::Selection& selection(const stringKey& id,IAlgorithm* parent) =0;
    virtual std::vector<stringKey> selectionKeys() = 0;
    virtual void clean() =0;


    virtual Hlt::Configuration& config() = 0;

};
#endif
