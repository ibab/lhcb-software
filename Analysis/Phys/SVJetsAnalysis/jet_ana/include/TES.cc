#include "TES.h"



void TES::release_particle(Particle *p){
//if(p != 0 || p != NULL){ delete p;}// p = NULL;}
if(debug)cout << p << endl;
if(p != 0 || p != NULL){ 
	Particle &rp = *p;
	p = 0;
	if(debug)cout << &rp << endl;
	if(debug)cout << p << endl;
	delete &rp;
}
//p->~Particle(); p = NULL;}
//if(p == 0 || p == NULL){if(debug)cout << "Got'cha!" << endl;}
}

void TES::release_particle_vec(std::vector<Particle*>& vec){
	while(vec.size() >0){
		Particle &rp = *vec[0];
		if(debug)cout << &rp << endl;
		vec.erase(vec.begin());
		delete &rp;
	}
}

void TES::ClearVariables_noselection(){
	//Mandatory for the noselection TES to prevent memory leak

	if(debug)cout << "start of noselection clearing" << endl;

	while(stdjets.size() > 0){
		Particle &rp = *stdjets[0];
		stdjets.erase(stdjets.begin());
		if(debug)cout << "rp " << &rp << " " << (&rp == 0) << endl;
		delete &rp;
	}

	if(debug)cout << "stdmuons clearing" << endl;

	while(stdmuons.size() > 0){
		Particle &rp = *stdmuons[0];
		stdmuons.erase(stdmuons.begin());
		if(debug)cout << "rp " << &rp << " " << (&rp == 0) << endl;
		delete &rp;
	}

	if(debug)cout << "fatjets clearing" << endl;

	while(fatjets.size() > 0){
		Particle &rp = *fatjets[0];
		fatjets.erase(fatjets.begin());
		if(debug)cout << "rp " << &rp << " " << (&rp == 0) << endl;
		delete &rp;
	}

}

void TES::ClearVariables()
{

if(debug)cout << "Clear Variabless" << endl;

	stdjets.clear();
	stdmuons.clear();
	fatjets.clear();

}

void TES::DefVars(){

sort(stdjets.begin(),stdjets.end(),order_by_pt);
sort(stdmuons.begin(),stdmuons.end(),order_by_pt);
sort(fatjets.begin(),fatjets.end(),order_by_pt);

} // DefVars
