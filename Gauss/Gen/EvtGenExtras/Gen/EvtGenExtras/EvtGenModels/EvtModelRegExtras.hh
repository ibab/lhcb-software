#ifndef EVTMODELREGEXTRAS_H_
#define EVTMODELREGEXTRAS_H_

#include <memory>
#include "EvtGenModels/EvtModelReg.hh"

class EvtModelRegExtras
{
public:
	static std::auto_ptr<const EvtModelList> getModels();
};

#endif /*EVTMODELREGEXTRAS_H_*/
