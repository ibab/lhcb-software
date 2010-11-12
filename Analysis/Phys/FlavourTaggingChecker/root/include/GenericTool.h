#ifndef GenericTool_H 
#define GenericTool_H 1

#include <Riostream.h>
#include <cmath>
#include "TString.h"

#include "taggingutils.h"
#include "globals.h"

#include "Particle.h"

#define endreq endl
#define endmsg endl


/** @class GenericTool
 *
 *  Generic tool containing common methods to tagger and combination tools
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class GenericTool {

public:
  /// Standard constructor
  GenericTool( ){

    MeV=0.001;
    GeV=1;
    TeV=1000;
    m_PIDe_cut=m_PID_k_cut=m_PIDkp_cut=m_PIDm_cut=0;
    m_PionSame_PIDNoK_cut=m_PionSame_PIDNoP_cut=0;
  }///< Constructor

  ~GenericTool( ){} ///< Destructor
 
  //=============================================
  double pol(double x, double a0, 
	     double a1=0, double a2=0, 
	     double a3=0, double a4=0) {
    double res = a0;
    if(a1) res += a1*x;
    if(a2) res += a2*x*x;
    if(a3) res += a3*x*x*x;
    if(a4) res += a4*x*x*x*x;
    return res;
  }///< return n-th degree polynomial by coefficients

  /// check if particle properties satisfy a given PID hypothesis by looking at deltaLL of RICH, muon and calorimetric systems
  bool checkPIDhypo(Particle::PID s, Particle* a) {
    
    switch (s) {
    case Particle::muon :
      if( a->hasMuonFlag() && a->muonNShared()==0 &&
          //if( a->hasMuonFlag() && a->muonNShared()==0 && a->cloneDist()==-1 &&
          a->PIDm() > m_PIDm_cut ) return true;
      return false;
    case Particle::electron :
      if( a->inAccHcal() && a->PIDe() > m_PIDe_cut ) return true;
      //if( a->inAccHcal() && a->PIDe() > m_PIDe_cut && a->cloneDist()==-1 ) return true;
      return false;
    case Particle::kaon_opposite :
      //if( a->PIDk()!=0 && a->PIDk()!=-1000.0  && a->PIDk() > m_PID_k_cut && 
      if( a->PIDk()!=0 && a->PIDk()!=-1000.0  && a->PIDk() > m_PID_k_cut && a->cloneDist()==-1 && 
	           a->PIDk()-a->PIDp() > m_PIDkp_cut)  return true;
      return false;
    case Particle::kaon_same :
      if( a->PIDk()  && a->PIDk()> m_KaonSPID_kS_cut ) return true;
      //if( a->PIDk()  && a->PIDk()> m_KaonSPID_kS_cut && a->cloneDist()==-1 ) retrun true;
      //((a->PIDk())-(a->PIDp()))> m_KaonSPID_kpS_cut ) return true;
      return false;
    case Particle::pion :
      bool pidpass=false;
      if( a->PIDk()==0 ) pidpass=true;
      if( a->PIDk()!=0 ) if(a->PIDk() < m_PionSame_PIDNoK_cut &&
                            a->PIDp() < m_PionSame_PIDNoP_cut) pidpass=true;

      //if (a->cloneDist()!=-1) pidpass=false;

      /* if( a->hasMuonFlag() && a->muonNShared()==0 */
      /* 	  && a->PIDm() > m_PIDm_cut ) return false; */
      /* if( a->inAccHcal() && a->PIDe() > m_PIDe_cut ) return false;  */
      /* if( pidpass ) return true; */

      //if( pidpass ) if( a->absID()==211 ) return true;
      if( pidpass ) return true;
    }
    return false;
  }


  double MeV, GeV, TeV;

  //store the raw nnet output
  double m_rnet_mu, m_rnet_ele, m_rnet_k, m_rnet_kS, m_rnet_pS, m_rnet_vtx;

 protected:
  double m_PIDe_cut,m_PID_k_cut,m_PIDkp_cut,
    m_PIDm_cut,m_PionSame_PIDNoK_cut, m_PionSame_PIDNoP_cut; 
  double m_KaonSPID_kS_cut, m_KaonSPID_kpS_cut;

  void print(std::vector<double>& par, double rnet, double pn) {
    if(DBGLEVEL>1) return;
    if(par.size()!=10) err()<<"GenericTool: par.size()!=10"<<endmsg;
    
    debug()<<"par = "<<par.at(0)<<" "<<par.at(1)<<" "<<par.at(2)
	   <<" "<<par.at(3)<<" "<<par.at(4)<<" "<<par.at(5)
	   <<" "<<par.at(6)<<" "<<par.at(7)<<" "<<par.at(8)
	   <<" "<<par.at(9)<<endmsg;
    debug()<<" rnet="<<rnet<<" pn="<<pn<<endmsg;
  }

};
#endif // GenericTool_H
