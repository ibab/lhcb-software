#ifndef EVTMODELREGEXTRAS_H_
#define EVTMODELREGEXTRAS_H_

#include <memory>
#include "EvtGenModels/EvtModelReg.hh"
/** 
 * Provides a list of external decay models to
 * use with EvtGen.
*/
typedef std::list<EvtDecayBase*> EvtModelList;
class EvtModelRegExtras
{
public:
	static std::auto_ptr<const EvtModelList> getModels();
};

#endif /*EVTMODELREGEXTRAS_H_*/
