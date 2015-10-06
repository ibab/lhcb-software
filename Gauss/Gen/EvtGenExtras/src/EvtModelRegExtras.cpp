#include "EvtGenModels/EvtModelRegExtras.hh"

#include "EvtGenModels/EvtBToKStarllDurham07.hh"
#include "EvtGenModels/EvtbTosllMS.hh"
#include "EvtGenModels/Evtbs2llGammaMNT.hh"
#include "EvtGenModels/Evtbs2llGammaISRFSR.hh"
#include "EvtGenModels/EvtDTohhhh.hh"
#include "EvtGenModels/EvtbsToLLLL.hh"
#include "EvtGenModels/EvtbTosllMSExt.hh"
#include "EvtGenModels/EvtbsToLLLLHyperCP.hh"
#include "EvtGenModels/EvtB2MuMuMuNu.hh"
#include "EvtGenModels/EvtFlatSqDalitz.hh"
#include "EvtGenModels/EvtBcVHad.hh"
#include "EvtGenModels/EvtPhspFlatLifetime.hh"
#include "EvtGenModels/EvtLb2plnuLCSR.hh"
#include "EvtGenModels/EvtLb2plnuLQCD.hh"
#include "EvtGenModels/EvtLb2Baryonlnu.hh"

//we use auto_ptr here to show explicit transfer of ownership
std::auto_ptr<const EvtModelList> EvtModelRegExtras::getModels(){

	EvtModelList* models = new EvtModelList();
	models->push_back(new EvtBToKStarllDurham07);
        models -> push_back( new EvtbTosllMS() );
        models -> push_back( new Evtbs2llGammaMNT() );
        models -> push_back( new Evtbs2llGammaISRFSR() );
        models -> push_back( new EvtDTohhhh() );
        models -> push_back( new EvtbsToLLLL() );
        models -> push_back( new EvtbTosllMSExt() );
        models -> push_back( new EvtbsToLLLLHyperCP() );
        models -> push_back( new EvtB2MuMuMuNu() );        
	models -> push_back( new EvtFlatSqDalitz() );
	models -> push_back( new EvtBcVHad() );
	models -> push_back( new EvtPhspFlatLifetime() ) ;
	models -> push_back(new EvtLb2plnuLQCD);
	models -> push_back(new EvtLb2plnuLCSR);
	models -> push_back (new EvtLb2Baryonlnu);
	return std::auto_ptr<const EvtModelList>(models);
	
}

