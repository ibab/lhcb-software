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
// Module: EvtPHOTOS.cc
//
// Description: This routine takes the particle *p and applies
//              the PHOTOS package to generate final state radiation
//              on the produced mesons.
//
// Modification history:
//
//    RYD     October 1, 1997        Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPhotonParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtPHOTOS.hh"

#ifdef WIN32
extern "C" {
  void __stdcall BEGEVTGENSTOREX(int *,int *,int *,int *,
                                int *,int *,int *,int *,
                                double *,double *,double *, 
                                double *,double *,double *, 
                                double *,double *,double *);
  void __stdcall BEGEVTGENGETX(int *,int *,int *,int *,
			      int *,int *,int *,int *,
			      double *,double *,double *, 
			      double *,double *,double *, 
			      double *,double *,double *);
  //  void __stdcall HEPLST(int *);
  void __stdcall PHOTOS(int *);
  void __stdcall PHOINI( ) ;
}
#else
extern "C" void begevtgenstorex_(int *,int *,int *,int *,
                                int *,int *,int *,int *,
                                double *,double *,double *, 
                                double *,double *,double *, 
                                double *,double *,double *);

extern "C" void begevtgengetx_(int *,int *,int *,int *,
			      int *,int *,int *,int *,
			      double *,double *,double *, 
			      double *,double *,double *, 
			      double *,double *,double *);

// extern "C" void heplst_(int *);

extern "C" void photos_(int *);

extern "C" void phoini_();
#endif

void EvtPHOTOS::doRadCorr( EvtParticle *p){

  static int first=1;

  //added by Lange Jan4,2000
  static EvtId GAMM=EvtPDL::getId("gamma");

  if (first) {

    first=0;
#ifdef WIN32
    PHOINI() ;
#else
    phoini_();
#endif
  }

  int entry,eventnum,numparticle,istat,partnum,mother;
  int daugfirst,dauglast;

  int numparticlephotos;

  double px,py,pz,e,m,x,y,z,t;

  px=0.0;
  py=0.0;
  pz=0.0;
  e=p->mass();
  m=p->mass();
  x=0.0;
  y=0.0;
  z=0.0;
  t=0.0;
  
  entry=1;
  eventnum=1;
  numparticle=1;
  istat=2;
  partnum=EvtPDL::getStdHep(p->getId());
  mother=0;
  daugfirst=2;
  dauglast=1+p->getNDaug();

#ifdef WIN32
  BEGEVTGENSTOREX(&entry,&eventnum,&numparticle,&istat,&partnum,
                  &mother,&daugfirst,&dauglast,
		  &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#else
  begevtgenstorex_(&entry,&eventnum,&numparticle,&istat,&partnum,
                  &mother,&daugfirst,&dauglast,
		  &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#endif

  int i;

  for(i=0;i<p->getNDaug();i++){

    px=p->getDaug(i)->getP4().get(1);
    py=p->getDaug(i)->getP4().get(2);
    pz=p->getDaug(i)->getP4().get(3);
    e=p->getDaug(i)->getP4().get(0);
    m=p->getDaug(i)->mass();
    x=0.0;
    y=0.0;
    z=0.0;
    t=0.0;
    
    entry+=1;
    eventnum=1;
    numparticle+=1;
    istat=1;
    partnum=EvtPDL::getStdHep(p->getDaug(i)->getId());
    mother=1;
    daugfirst=0;
    dauglast=0;    

#ifdef WIN32
    BEGEVTGENSTOREX(&entry,&eventnum,&numparticle,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#else
    begevtgenstorex_(&entry,&eventnum,&numparticle,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#endif    

  }

  //can't use heplst since the common block used by the BaBar
  //implementation of PHOTOS  is renamed due to real*4 vs real*8
  //problems.

  //int mlst=1;

  //heplst_(&mlst);

  entry=1;

  //  report(INFO,"EvtGen") << "Doing photos " << 
  // EvtPDL::name(p->getId()) << std::endl;
#ifdef WIN32
  PHOTOS(&entry);
#else
  photos_(&entry);
#endif
  //  report(INFO,"EvtGen") << "done\n";
#ifdef WIN32
  BEGEVTGENGETX(&entry,&eventnum,&numparticlephotos,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#else
  begevtgengetx_(&entry,&eventnum,&numparticlephotos,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#endif    

  //report(INFO,"EvtGen") << "numparticlephotos:"<<numparticlephotos
  // <<std::endl;
  
  if (numparticle==numparticlephotos) return;

  EvtVector4R new4mom;

  int np;

  for(i=0;i<p->getNDaug();i++){

    entry=i+2;

#ifdef WIN32
    BEGEVTGENGETX(&entry,&eventnum,&np,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#else
    begevtgengetx_(&entry,&eventnum,&np,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#endif

    //this is needed to ensure that photos does not
    //change the masses. But it will violate energy conservation!
    double mp=p->getDaug(i)->mass();
    e=sqrt(mp*mp+px*px+py*py+pz*pz);
        
    new4mom.set(e,px,py,pz);

    p->getDaug(i)->setP4(new4mom);

  }

  for(entry=numparticle+1;entry<=numparticlephotos;entry++){

#ifdef WIN32
    BEGEVTGENGETX(&entry,&eventnum,&np,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#else
    begevtgengetx_(&entry,&eventnum,&np,&istat,&partnum,
		    &mother,&daugfirst,&dauglast,
		    &px,&py,&pz,&e,&m,&x,&y,&z,&t);
#endif
        
    new4mom.set(e,px,py,pz);

    //new4mom.dump();

    EvtPhotonParticle* gamma;
    gamma=new EvtPhotonParticle;
    gamma->init(GAMM,new4mom);
    //    report(INFO,"EvtGen") << gamma << " " << p << " "<< px << " " 
    //<< py << " " << pz << " " << p->getNDaug() << " " << 
    //EvtPDL::name(p->getId())<<" " << entry << " " <<numparticlephotos
    //<<std::endl;
    gamma->addDaug(p);

//    p->getDaug(i)->set_type(EvtSpinType::PHOTON);

  }
  return ;
}

