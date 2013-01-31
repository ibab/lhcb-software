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
// Module: EvtBcVHad.cc
//
// Description: Module to implement Bc -> psi + (n pi) + (m K) decays
//
// Modification history:
//
//    AVL     Jan 29, 2013        Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGenModels/EvtBcVHad.hh"



EvtBcVHad::~EvtBcVHad() {}

std::string EvtBcVHad::getName(){ return "BC_VHAD";}

EvtDecayBase* EvtBcVHad::clone() {  return new EvtBcVHad;}

//======================================================
void EvtBcVHad::init(){
    
    checkNArg(1);
    checkSpinParent(EvtSpinType::SCALAR);
    checkSpinDaughter(0,EvtSpinType::VECTOR);
    for (int i=1; i<=(getNDaug()-1);i++) {
      checkSpinDaughter(i,EvtSpinType::SCALAR);
    };

    if(getNDaug()<2 || getNDaug()>6) {
      report(ERROR,"EvtGen") << "Have not yet implemented this final state in BcVNpi model" << endl;
      report(ERROR,"EvtGen") << "Ndaug="<<getNDaug() << endl;
      for ( int id=0; id<(getNDaug()-1); id++ ) 
	report(ERROR,"EvtGen") << "Daug " << id << " "<<EvtPDL::name(getDaug(id)).c_str() << endl;
      return;
    }

  

   idVector = getDaug(0).getId();
    whichfit = int(getArg(0)+0.1);
    ffmodel = new EvtBCVFF2(idVector,whichfit);
    
    wcurr = new EvtWHad();
    
// determine the code of final hadronic state
    EvtIdSet thePis("pi+","pi-","pi0");
    EvtIdSet theK("K+","K-");
    if( getNDaug()==2 && thePis.contains(getDaug(1))  )  
      out_code=1;
    else if( getNDaug()==3 && thePis.contains(getDaug(1)) && thePis.contains(getDaug(2)) )  
      out_code=2;
    else if( getNDaug()==4 && thePis.contains(getDaug(1)) && thePis.contains(getDaug(2)) && thePis.contains(getDaug(3)) )  
      out_code=3;
    else if( getNDaug()==5 && thePis.contains(getDaug(1)) && thePis.contains(getDaug(2)) && thePis.contains(getDaug(3)) && thePis.contains(getDaug(4)) )  
      out_code=4;
    else if( getNDaug()==6 && thePis.contains(getDaug(1)) && thePis.contains(getDaug(2)) 
	&& thePis.contains(getDaug(3)) && thePis.contains(getDaug(4)) && thePis.contains(getDaug(5)) )  
      out_code=5;
    else if( getNDaug()==4 && theK.contains(getDaug(1)) && theK.contains(getDaug(2)) && thePis.contains(getDaug(3)) )  
      out_code=6;
    else if( getNDaug()==4 && theK.contains(getDaug(1)) && thePis.contains(getDaug(2)) && thePis.contains(getDaug(3)) )  
      out_code=7;
    
//     cout<<"[AVL] BcVHad: whichfit ="<<whichfit<<"  idVector="<<idVector<<"  out_code="<<out_code<<endl;
    
    nCall = 0;
}

//======================================================
void EvtBcVHad::initProbMax() {
    if(out_code==3) {
      if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 1 && getNDaug()==4) setProbMax(42000.);
      else if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 2 && getNDaug()==4) setProbMax(16000.); 
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 1 && getNDaug()==4) setProbMax(1200.);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 2 && getNDaug()==4) setProbMax(2600.);
    }
    else if(out_code==5) {
      if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 1 && getNDaug()==6) setProbMax(720000.);
      else if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 2 && getNDaug()==6) setProbMax(471817.);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 1 && getNDaug()==6) setProbMax(40000.);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 2 && getNDaug()==6) setProbMax(30000.);  
    }
    else if(out_code==6) {
      if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 1) setProbMax(50000.);
      else if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 2) setProbMax(20000.0);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 1) setProbMax( 2100.0);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 2) setProbMax( 1700.00);
    }
    else if(out_code==7) {
      if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 1) setProbMax(2.2e+06);
      else if(idVector == EvtPDL::getId("J/psi").getId() && whichfit == 2) setProbMax( 830000);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 1) setProbMax(92000.0);
      else if(idVector == EvtPDL::getId("psi(2S)").getId() && whichfit == 2) setProbMax(93000.0);
    }
    else {
	    report(ERROR,"EvtGen") << "Have not yet implemented this final state in BC_VHAD model" << endl;
	    report(ERROR,"EvtGen") << "Ndaug="<<getNDaug() << endl;
	    int id;
	    for ( id=0; id<(getNDaug()-1); id++ ) 
	    report(ERROR,"EvtGen") << "Daug " << id << " "<<EvtPDL::name(getDaug(id)).c_str() << endl;
	    ::abort();
    };
}



//===================================
EvtVector4C EvtBcVHad::_hardCurr(EvtParticle *root_particle) {
    EvtVector4C hardCur;
    if( out_code==1) { // pi+
      hardCur = wcurr->WCurrent( root_particle->getDaug(1)->getP4() );
    }
    else if( out_code==2) { // pi+ pi0
      hardCur = wcurr->WCurrent( root_particle->getDaug(1)->getP4() , 
				  root_particle->getDaug(2)->getP4() 
				);
    }
    else if( out_code==3) {// pi+ pi+ pi-
      hardCur = wcurr->WCurrent( root_particle->getDaug(1)->getP4() , 
				  root_particle->getDaug(2)->getP4(), 
				  root_particle->getDaug(3)->getP4() 
				);
    }
    else if( out_code == 5) // Bc -> psi pi+ pi+ pi- pi- pi+ from [Kuhn, Was, hep-ph/0602162
    {
      hardCur = wcurr->WCurrent(root_particle->getDaug(1)->getP4(),
	  root_particle->getDaug(2)->getP4(),
	  root_particle->getDaug(3)->getP4(),
	  root_particle->getDaug(4)->getP4(),
	  root_particle->getDaug(5)->getP4()
      );
    }
    else if( out_code==6) { // K+ K- pi+
	  hardCur =wcurr->WCurrent_KKP( root_particle->getDaug(1)->getP4() , 
					  root_particle->getDaug(2)->getP4(), 
					  root_particle->getDaug(3)->getP4() 
					);
    }
    else if( out_code==7) {//K+ pi+ pi-
	  hardCur = wcurr->WCurrent_KPP( root_particle->getDaug(1)->getP4() , // K+
				  root_particle->getDaug(2)->getP4(),      // pi+
				  root_particle->getDaug(3)->getP4()       // pi-
				); 
    }
    else {
	    report(ERROR,"EvtGen") << "Have not yet implemented this final state in BCNPI model" << endl;
	    report(ERROR,"EvtGen") << "Ndaug="<<getNDaug() << endl;
	    int id;
	    for ( id=0; id<(getNDaug()-1); id++ ) 
	    report(ERROR,"EvtGen") << "Daug " << id << " "<<EvtPDL::name(getDaug(id)).c_str() << endl;
	    ::abort();
    };  
    return hardCur;
};

//======================================================
void EvtBcVHad::decay( EvtParticle *root_particle ) {
   ++nCall;

   root_particle->initializePhaseSpace(getNDaug(),getDaugs());

   // Calculate hadronic current
    EvtVector4C hardCur=_hardCurr(root_particle);

    EvtVector4R
	    p4b(root_particle->mass(), 0., 0., 0.),                  // Bc momentum
	    p4meson=root_particle->getDaug(0)->getP4(),     		   // J/psi momenta
	    Q=p4b-p4meson;
    double Q2=Q.mass2();



// calculate Bc -> V W form-factors
	double a1f, a2f, vf, a0f;
	double m_meson = root_particle->getDaug(0)->mass();
	double m_b = root_particle->mass();
	ffmodel->getvectorff(root_particle->getId(),
				root_particle->getDaug(0)->getId(),
				Q2,
				m_meson,
				&a1f, 
				&a2f, 
				&vf, 
				&a0f);
	double a3f = ((m_b+m_meson)/(2.0*m_meson))*a1f -
	      ((m_b-m_meson)/(2.0*m_meson))*a2f;

// calculate Bc -> V W current
	EvtTensor4C H;
	H = a1f*(m_b+m_meson)*EvtTensor4C::g();
	H.addDirProd((-a2f/(m_b+m_meson))*p4b,p4b+p4meson);
	H+=EvtComplex(0.0,vf/(m_b+m_meson))*dual(EvtGenFunctions::directProd(p4meson+p4b,p4b-p4meson));
	H.addDirProd((a0f-a3f)*2.0*(m_meson/Q2)*p4b,p4b-p4meson);
	EvtVector4C  Heps=H.cont2(hardCur);
	
	for(int i=0; i<4; i++) {
		EvtVector4C  eps=root_particle->getDaug(0)->epsParent(i).conj(); // psi-meson polarization vector
		EvtComplex amp=eps*Heps;
		vertex(i,amp);
	};

}

