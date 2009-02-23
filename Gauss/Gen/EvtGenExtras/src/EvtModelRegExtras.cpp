#include "EvtGenModels/EvtModelRegExtras.hh"

#include "EvtGenModels/EvtBToKStarllDurham07.hh"

//we use auto_ptr here to show explicit transfer of ownership
std::auto_ptr<const EvtModelList> EvtModelRegExtras::getModels(){
	
	EvtModelList* models = new EvtModelList();
	models->push_back(new EvtBToKStarllDurham07);
	return std::auto_ptr<const EvtModelList>(models);
	
}

