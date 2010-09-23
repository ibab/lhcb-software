#ifndef PARTICLE_H
#define PARTICLE_H 1

#include "Riostream.h"
#include <cmath>
#include <cstdlib>
#include "TLorentzVector.h"
#include "TVector3.h"
#include "globals.h"

using namespace std;
class Particle;
typedef std::vector<Particle*> Particles;

/** @class GenericTool
 *
 *  Class representing particle properties, filled by NtpTag class
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class Particle {

 public:
  
  Particle(){
    m_ComesFromSVtx=0;
    m_ID=0; m_MotherID=0; m_AncestorID=0; 
    m_fromB=0; m_xFlag=0; 
    m_MCID=0; m_TrackType=0; m_Charge=0, m_veloch=m_EOverP=0;
    m_PIDe=m_PIDm=m_PIDk=m_PIDp=-999; m_IPs=0; m_IP=0; m_LCS=0;
    m_p  =TLorentzVector(0,0,0,0);
    m_mcp=TLorentzVector(0,0,0,0);
    m_MuonNShared=0;
    m_InAccHcal=0;
    m_likelihood=0;
    m_MuonFlag=0;
    m_DOCA=0;
    m_IPerr=0;
    m_docaSV=0;
    m_IPSV=0;
    m_IPPU=0;
    m_distPhi=0;
    m_endVertexPosition=TVector3(0,0,0);
    m_osc=0;
  } ///< constructor

  Particle(TLorentzVector& momentum, int id ){
    Particle();
    this->setMomentum(momentum);
    this->setID(id);    
  }///<  TLorentzVector constructor

  /* Particle(const Particle& p) { */
/*     *this = p; */
/*   }///< copy constructor */

  ~Particle( ){} ///< Destructor
 
  enum Type{ Long    = 3,
	     Matched = 7,
	     Upstream= 4,
	     none    = 0
  };

  enum PID{ kaon_opposite = 321,
	    kaon_same = 3210,
	    pion      = 211,
	    electron  = 11,
	    muon      = 13
  };

  /// particle ID
  int    ID()   { return m_ID; } 
  /// absolute value of particle ID
  int    absID(){ return abs(m_ID); } 
  /// PID likelihood value for electron-pion hypothesis
  double PIDe() { return m_PIDe; } 
  /// PID likelihood value for muon-pion hypothesis
  double PIDm() { return m_PIDm; } 
  /// PID likelihood value for kaon-pion hypothesis
  double PIDk() { return m_PIDk; } 
  /// PID likelihood value for proton-pion hypothesis
  double PIDp() { return m_PIDp; } 
  /// particle is in HCAL acceptance
  double inAccHcal() { return m_InAccHcal; }
  //double RichPIDStatus() { return m_RichPIDStatus; }

  /// is flagged as a muon by the muon system
  double hasMuonFlag() { return m_MuonFlag; }

  /// position wrt PV of the decay point
  TVector3 endVertexPosition() {return m_endVertexPosition;} 

  /// likelihood value to estimate the ghost probability
  double likelihood() { return m_likelihood; }
  /// significance of the impact parameter wrt PV
  double IPs()  { return m_IPs; }
  /// impact parameter wrt PV
  double IP()   { return m_IP; }
  /// impact parameter error
  double IPerr(){ return m_IPerr; }
  /// significance of the impact parameter wrt Secondary Vertex
  double IPSV() { return m_IPSV; }
  /// smaller significance of the impact parameter wrt all other PVs
  double IPPU() { return m_IPPU; }
  /// distance of closest approach wrt Secondary Vertex
  double docaSV()   { return m_docaSV; }

  /// Last Chi Square from tracking fit
  double LCS()  { return m_LCS; }
  /// particle momentum in GeV
  double p()    { return m_p.Vect().Mag(); }
  /// particle transverse momentum in GeV
  double pt()   { return m_p.Vect().Perp(); }
  /// particle theta in rad
  double Theta(){ return m_p.Theta(); }
  /// particle phi in rad
  double Phi()  { return m_p.Phi(); }
  /// particle pseudorapidity
  double rapidity()  { return -log(atan(m_p.Theta()/2.)); }
  /// particle mass in GeV
  double M()    { return m_p.M(); } 
  /// particle charge
  int    charge(){ return m_Charge; }
  /// charge deposited in the Velo
  double VeloCharge()  { return m_veloch; }
  /// calorimetric E/P
  double eOverP()  { return m_EOverP; }
  /// number of shared hits of particle with other particles in the muon system
  int    muonNShared() {return m_MuonNShared; }
  /// retrieve 4-momentum
  TLorentzVector momentum()   { return m_p; }
  /// retrieve MC true 4-momentum
  TLorentzVector momentumMC() { return m_mcp; }
  /// Distance of closest approach of the 2 tracks forming a SV seed
  double DOCA()        { return m_DOCA ; } 
  /// smallest delta_phi between particle and all signal tracks
  double distPhi()     { return m_distPhi;}
  /// track belongs to SV formed when running DaVinci
  bool ComesFromSVtx( ){ return m_ComesFromSVtx; }
  /// particle ID of mother particle
  int  motherID( )     { return m_MotherID; }
  /// ancestor ID
  int  ancestorID( )   { return m_AncestorID; }
  /// if = 1, particle truly comes from opposite B meson, if =-1, particle truly comes from signal B (clone of a B decay track)
  int  fromB( )        { return m_fromB; }
  /// particle comes from fragmentation of signal B
  int  xFlag( )        { return m_xFlag; }
  /// true MC PID
  int  MCID( )         { return m_MCID ; }
  /// absolute value of true MC PID
  int  absMCID( )      { return abs(m_MCID) ; }
  /// track type
  int  type()          { return m_TrackType ; }

  /// Opposite side B meson has oscillated
  int  hasOscillated() { return m_osc ; }

  ///////////////////////////////////////////////////////////////
  void setHasOscillated(int a ) { m_osc=a ; }

  void setCharge(int a)   { m_Charge  = a; }
  void setID(int a)       { m_ID  = a; }
  void setPIDe(double a)  { m_PIDe= a; } 
  void setPIDm(double a)  { m_PIDm= a; } 
  void setPIDk(double a)  { m_PIDk= a; } 

  void setInAccHcal(double a)  { m_InAccHcal= a; }
  //  void setRichPIDStatus(int a) { m_RichPIDStatus = a; }
  void setHasMuonFlag(int a) { m_MuonFlag = a; }

  void setPIDp(double a)  { m_PIDp= a; } 
  void setIPs(double a)   { m_IPs = a; }
  void setIP(double a)    { m_IP = a; }
  void setIPPU(double a)    { m_IPPU = a; }
  void setIPSV(double a) { m_IPSV = a; }
  void setIPerr(double a) { m_IPerr = a; }
  void setLCS(double a)   { m_LCS = a; }
  void setVeloCharge(double a){ m_veloch = a; }
  void setEOverP(double a){ m_EOverP = a; }
  void setLikelihood(double a ) {  m_likelihood=a; }
  void setMomentum(TLorentzVector a) { m_p = a; }

  void setdocaSV(double a) {m_docaSV=a;}

  void setComesFromSVtx( bool a ) {  m_ComesFromSVtx = a; }
  void setMotherID( int a )       {  m_MotherID = a; }
  void setAncestorID( int a )     {  m_AncestorID = a; }
  void setfromB( int a )          {  m_fromB = a; }
  void setxFlag( int a )          {  m_xFlag = a; }
  void setMCMomentum(TLorentzVector a) {  m_mcp = a; }
  void setMCID( int a )           {  m_MCID = a; }
  void setType( int a )      {  m_TrackType = a; }

  void setDOCA( double a )   {  m_DOCA = a; }

  void setMuonNShared( int a )    {  m_MuonNShared = a; }

  void setEndVertexPosition(TVector3 a) { m_endVertexPosition=a; }

  void setDistPhi(double a) {m_distPhi=a;}

  /// print information about particle
  void Print() {
    if(DBGLEVEL<3) {
      cout<< " - PARTICLE"<< "   trtyp="
	  <<m_TrackType << "   ID="<<m_ID<<setprecision(4)
	  << "   P="<< this->p() << "   Pt="<< this->pt() 
	  <<endl;
      /* cout<< "    deta="<<deta<< " dphi="<<dphi  */
      /* 	  << " dQ="<< (isBs? dQ_asKaon:dQ_asPion) <<endl; */
      cout<<"    lcs="<<m_LCS
	  <<"    IPs="<<m_IPs<<"   IPPU="<<m_IPPU<<"   NSH="<<m_MuonNShared<<endl;
      cout<<"    PIDm,e,k,p   "<< m_PIDm<<"   "<<m_PIDe<<"   "
	  <<m_PIDk<<"   "<<m_PIDp<<endl;
      cout<<"    E/P="<< m_EOverP<<"   veloch="<<m_veloch<<endl;
      cout<<"    fromSVtx="<<m_ComesFromSVtx
	  <<"   fromB="<<m_fromB<<"   MCID="<<m_MCID<<endl;
    }
  } 

 private:
  bool m_ComesFromSVtx;
  int m_ID, m_MotherID, m_AncestorID, m_fromB, m_xFlag, m_MCID, 
    m_TrackType,m_Charge, m_MuonNShared, m_MuonFlag, m_osc;
  double m_PIDe,m_PIDm,m_PIDk,m_PIDp, m_IPs, m_IP, m_LCS, m_veloch,  m_EOverP;
  double m_InAccHcal, m_likelihood, m_DOCA, m_IPerr, m_IPSV, m_docaSV, m_distPhi;
  double m_IPPU;
  TLorentzVector m_p, m_mcp;
  TVector3 m_endVertexPosition;

};

#endif 
