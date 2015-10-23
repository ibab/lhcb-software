//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtXLL.cc
//
// Description: {B,Lb}->{Kpi,KK,pK,pipi,...}ell ell, w/ optimized dihadron, cos_theta, spectra 
//
// Modification history:
//
//    Biplab Dey, 4/09/2015    Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenModels/EvtXLL.hh"

#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
using std::fstream;

EvtXLL::~EvtXLL() {}

std::string EvtXLL::getName(){

    return "XLL";

}

EvtDecayBase* EvtXLL::clone(){

  return new EvtXLL;

}


void EvtXLL::initProbMax(){

  setProbMax(_MY_MAX_PROB);

}


void EvtXLL::init(){

    // Channel number
    _chan = -1;
    if (getNArg() > 0) {_chan = getArg(0);}

    // Maximum number of phase space iterations
    _nLoop = 5000;
    if (getNArg() > 3) {_nLoop = (int) getArg(3);}

    for (int ii = 0; ii < getNDaug(); ii++) {
	_masses[ii] = EvtPDL::getMass(getDaug(ii));
    }

    double prod, prod_min, m, delta, p_b, p_h, m_mumu_min, m_mumu_max, m_mumu, delta_mumu;
    double avg_wt(0.), n_tot(0.);

    int nDaug = getNDaug();

    if (nDaug == 2) {

        // J/psi -> mu mu
        _MX_LO = 0.0;
	_MX_HI = 0.0;
	_MY_MAX_PROB = 1.0;

    } else if (nDaug == 3) {

         // J/psi hh topology
	_MX_LO = _masses[1] + _masses[2]; // just mh1 + mh2
	_MX_HI = EvtPDL::getMass(getParentId()) - _masses[0]; // m_Mom - m_J/psi

	if (getNArg() == 3){ 
	    _MX_LO = (double)getArg(1);
	    _MX_HI = (double)getArg(2);
	}

	// tweak things a little bit just to be safe
	_MX_LO -= 0.005;
	_MX_HI += 0.005;

	prod_min = 1e7;
	m = _MX_LO;
	delta = (_MX_HI - _MX_LO)/100.0;
	
	while (m < _MX_HI) { // calculate the minimum value of p_b * p_h

	    p_b = fn_p_Mm1m2(EvtPDL::getMass(getParentId()),_masses[0],m);
	    p_h = fn_p_Mm1m2(m,_masses[1],_masses[2]);
	    prod = p_b * p_h/fn_wt_dihadron(m,_chan);
	    
	    if (prod < prod_min && p_b > 0.01 && p_h > 0.001) {prod_min = prod;}
	    m += delta;
	    
	}
	
	_MY_MAX_PROB = 1.0;
	
	if (prod_min > 0) {_MY_MAX_PROB = 1.0/prod_min;}
	

    } else if (nDaug == 4) {

        // mumuhh topology
	_MX_LO = _masses[2] + _masses[3]; // just mh1 + mh2
	_MX_HI = EvtPDL::getMass(getParentId()) - _masses[0] - _masses[1]; // m_Mom - 2 * m_mu
	
	if (getNArg() == 3){ 
	    _MX_LO = (double)getArg(1);
	    _MX_HI = (double)getArg(2);
	} 
	
	// tweak things a little bit just to be safe
	_MX_LO -= 0.005;
	_MX_HI += 0.005;
	
	prod_min = 1e8;
	m = _MX_LO;
	delta = (_MX_HI - _MX_LO)/100.0;
	m_mumu_min = _masses[0] + _masses[1];
	m_mumu_max =  EvtPDL::getMass(getParentId()) - _MX_LO;
	m_mumu = m_mumu_min;
	delta_mumu = (m_mumu_max - m_mumu_min)/100.0;
	n_tot  = 0.0;
	avg_wt = 0.0;
	
	while (m < _MX_HI){ 
	    
	    m_mumu = m_mumu_min;
	    
	    while (m_mumu < m_mumu_max){
		
		p_b = fn_p_Mm1m2(EvtPDL::getMass(getParentId()),m_mumu,m);
		p_h = fn_p_Mm1m2(m,_masses[2],_masses[3]);
		prod = p_b * p_h/fn_wt_dihadron(m,_chan);
		
		if (prod > 0.0 && prod < prod_min && p_b > 0.01 && p_h > 0.001) {prod_min = prod;}
		
		m_mumu += delta_mumu;
		
		if (prod > 0.0 && p_b > 0.01 && p_h > 0.001){ 
		    avg_wt += 1.0/prod; 
		    n_tot += 1.0;
		}
		
	    }
	    
	    m += delta;
	    
	}
	
	avg_wt /= n_tot;
	
	_MY_MAX_PROB = 1.0;
	
	if (prod_min > 0.0) {_MY_MAX_PROB = 1.0/prod_min;}
	
    }
    
}


double EvtXLL::fn_p_Mm1m2(double M, double m1, double m2){ 

    // 1 -> 2 3 breakup momentum
    double MSq = M*M;
    double m12p = m1 + m2;
    double m12m = m1 - m2;
    double ret = (MSq - m12p*m12p) * (MSq - m12m*m12m);

    double result(0.0);
    
    if (ret > 0.0 && fabs(M) > 1e-10) {
	result = 0.5*sqrt(ret)/M;
    }
    
    return result;

}

double EvtXLL::fn_cth(double cth){ 

    // cos_theta_shape
    double prefactor(1.0), slope(20.0);
    double f = 0.5;
    f += prefactor * exp(- slope * ( cth + 1.0) );
    f += prefactor * exp(  slope * ( cth - 1.0) );
    f /= 1.5;

    return f;

}

double EvtXLL::fn_wt_dihadron(double mX, int chan){

   double mk(0.493), mpi(0.139),mp(0.938);
   double m_thresh(0.0);
   double f(0.0);

   if (chan == -1) {
       
       // Flat
       f = 1.0;

   } else if (chan == 0) {
       
       // Kpi
       m_thresh = mk + mpi - 0.005;
       // add a steeply falling function off the threshold 
       f += 60.0 * exp(-40.0 * (mX-m_thresh));
       // K* 892
       f += 100.0 * exp( -pow((mX -0.895)/0.05,2));
       // K2 1430
       f += 10.0 * exp( -pow((mX -1.425)/0.15,2));
       // add a slowing falling exponential function
       f += 10.0 *exp(-0.6 * (mX-0.6));
       f /= 108.378;

   } else if (chan == 1) {
       
       // KK 
       m_thresh = 2.0 * mk - 0.005;
       // add a steeply falling function off the threshold
       f += 60.0 * exp(-40.0 * (mX-m_thresh));
       // K* 892
       f += 700.0 * exp( -pow((mX -1.02)/0.005,2));
       // K2 1525
       f += 20.0 * exp( -pow((mX -1.525)/0.1,2));
       // add a slowing falling exponential function
       f += 15.0 *exp(-0.7 * (mX-m_thresh));
       f /= 716.739;

   } else if (chan == 2) {

       // pK
       m_thresh = mk + mp - 0.005;
       // add a steeply falling function off the threshold at 600 MeV
       f += 150.0 * exp(-40.0 * (mX-m_thresh));
       f += 900.0 * exp( -pow((mX -1.52)/0.015,2));
       f += 100.0 * exp( -pow((mX -1.6)/0.1,2));
       f += 100.0 * exp( -pow((mX -1.8)/0.2,2));
       // add a slowing falling exponential function
       f += 80.0 *exp(-0.6 * (mX-m_thresh));
       f /= 1043.1; 

   }
   
   if (mX < _MX_LO || mX > _MX_HI) {f = 0.0;}
   
   return f;

}


void EvtXLL::decay(EvtParticle *p) {

    // First, generate phase space, making sure mX is within its required range
    int nDaug = getNDaug();

    if (nDaug == 3 || nDaug == 4) {

	// ccbar h h and mu mu h h topologies
	bool gotPhaseSpace = this->genValidPhaseSpace(p);
	if (gotPhaseSpace == false) {
            report(ERROR,"EvtGen")<<"EvtXLL::decay could not generate valid phase space after "<<_nLoop<<" loops"<<std::endl;
	    return;
	}

    } else {

	// Should be just the psi -> mu mu part
	p->initializePhaseSpace(nDaug, getDaugs());

    }

    double prob(1e-9);
    EvtVector4R p4_parent;
    EvtVector4R p4_mom, p4_dau0, p4_dau1, p4_dau2, p4_dau3;
    EvtVector4R p4_dihadron, p4_dilepton, p4_dilepton_copy;
    p4_mom = p->getP4();
    
    double ctl(1.0), ctv(1.0), p_b(1.0), p_h(1.0), mX(1.3);   
    double wt_dihadron(1.0);
    
    if (nDaug == 2) {
    
        // just the psi -> mu mu part

	p4_mom = p->getP4();
	p4_parent = p->getParent()->getP4(); // the mother B momenta
	p4_dihadron = p->getParent()->getDaug(1)->getP4() + p->getParent()->getDaug(2)->getP4();
	
	p4_dau0 = p->getDaug(0)->getP4(); // the muon momenta
	
	p4_dihadron.applyBoostTo(p4_mom,true);
		
	ctl = p4_dihadron.dot(p4_dau0) / (p4_dihadron.d3mag()*p4_dau0.d3mag());
	
	if (ctl == ctl) {prob = fn_cth(ctl);}
	
    } else if (nDaug == 3) {

        // ccbar h h topology 
	p4_mom = p->getP4();
	p4_dau0 = p->getDaug(0)->getP4(); // the dimuon  
	p4_dau1 = p->getDaug(1)->getP4(); // h1
	p4_dau2 = p->getDaug(2)->getP4(); // h2
	p4_dihadron = p4_dau1 + p4_dau2;
	mX = p4_dihadron.mass();
	
	if (_chan == -1) { //  will generate flat in m(hh) 
	    
	    wt_dihadron = 1.0;
	    // add the mass range
	    if (getNArg() == 3 && (mX < (double)getArg(1) || mX > (double)getArg(2))) {wt_dihadron = 0.0;}
	    
	} else {
	    
	    // creates the spectrum in m(hh)
	    wt_dihadron = fn_wt_dihadron(mX,_chan);
	}
	
	p_b = fn_p_Mm1m2(p4_mom.mass(), p4_dau0.mass(), mX); 
	p_h = fn_p_Mm1m2(mX,p4_dau1.mass(), p4_dau2.mass());
	
	// boost the dilepton and the h1 to the dihadron RF
	p4_dau0.applyBoostTo(p4_dihadron,true);
	p4_dau1.applyBoostTo(p4_dihadron,true);
	ctv = p4_dau1.dot(p4_dau0) / (p4_dau1.d3mag()*p4_dau0.d3mag());
	
	if (p_b > 0.0 && p_h > 0.0 && ctv == ctv) {prob = fn_cth(ctv) * wt_dihadron /(p_b*p_h);}
	
	if (p_b < 0.01 || p_h < 0.001) {prob = 0.0;}
	
	if (prob > _MY_MAX_PROB) {prob = _MY_MAX_PROB;}

    } else if (nDaug == 4) {
	
        // mu mu h h topology
	p4_dau0 = p->getDaug(0)->getP4(); // muon1
	p4_dau1 = p->getDaug(1)->getP4(); // muon2
	p4_dau2 = p->getDaug(2)->getP4(); // h1
	p4_dau3 = p->getDaug(3)->getP4(); // h2
	p4_dilepton = p4_dau0 + p4_dau1;
	p4_dihadron = p4_dau2 + p4_dau3;
	mX = p4_dihadron.mass();
	
	if (_chan == -1){ //  will generate flat in m(hh) 
	    
	    wt_dihadron = 1.0;
	    // add the mass range
	    if (getNArg() == 3 && (mX < (double)getArg(1) || mX > (double)getArg(2))) {wt_dihadron = 0.0;}
	
	} else {
	    
	    // creates the spectrum in m(hh)
	    wt_dihadron = fn_wt_dihadron(mX,_chan);
	    
	}
	
	p_b = fn_p_Mm1m2(p4_mom.mass(), p4_dilepton.mass(), mX); 
	p_h = fn_p_Mm1m2(mX,p4_dau2.mass(), p4_dau3.mass());
	p4_dilepton_copy = p4_dilepton;
	
	// boost the dilepton and the h1 to the dihadron RF
	p4_dilepton.applyBoostTo(p4_dihadron,true);
	p4_dau2.applyBoostTo(p4_dihadron,true);
	ctv = p4_dau2.dot(p4_dilepton) / (p4_dau2.d3mag()*p4_dilepton.d3mag());
	
	// boost the dihadron and the muon1 to the dilepton RF
	p4_dihadron.applyBoostTo(p4_dilepton_copy,true);
	p4_dau0.applyBoostTo(p4_dilepton_copy,true);
	ctl = p4_dau0.dot(p4_dihadron) / (p4_dau0.d3mag()*p4_dihadron.d3mag());
	
	if (p_b > 0.0 && p_h > 0.0 && ctv == ctv && ctl == ctl) {
	    prob = fn_cth(ctl) * fn_cth(ctv) * wt_dihadron / (p_b*p_h);
	}
	
	if (p_b < 0.01 || p_h < 0.001) {prob = 1e-8;}
	
	if (prob > _MY_MAX_PROB) {prob = _MY_MAX_PROB;}

    }

    setProb(prob);

    return;

}

bool EvtXLL::genValidPhaseSpace(EvtParticle* p) {

    int nDaug = getNDaug();
    if (nDaug < 3) {return false;}

    bool isValid(false), forceDaugMassReset(true);
    int iLoop(0);

    EvtVector4R p4_dau1, p4_dau2, p4_dihadron;
    double mX(0.0);

    // For ccbar h h topology
    int id1(1), id2(2);
    // For mu mu h h topology
    if (nDaug == 4) {id1 = 2; id2 = 3;}
	

    while (isValid == false && iLoop < _nLoop) {
    
	p->initializePhaseSpace(nDaug, getDaugs(), forceDaugMassReset);
	
	// Get the mX mass
	p4_dau1 = p->getDaug(id1)->getP4(); // h1
	p4_dau2 = p->getDaug(id2)->getP4(); // h2
	p4_dihadron = p4_dau1 + p4_dau2;
	mX = p4_dihadron.mass();

	if (mX >= _MX_LO && mX <= _MX_HI) {isValid = true;}

    }
 
    return isValid;

}
