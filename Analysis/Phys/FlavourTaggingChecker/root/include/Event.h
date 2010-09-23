#ifndef Event_H
#define Event_H 1

#include <vector>
#include <TVector3.h>
#include <Riostream.h>
#include "Particle.h"
#include "Vertex.h"
#include "globals.h"

using namespace std;

/** @class Event
 *
 *  This class stores the property of the event
 *   by loading the Ntuple. NtpTag class fills the needed information
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class Event {

 public:
 Event( ):
  m_SignalParts(10),
    m_Particles(200),
    m_vertices(200)
      {  
	m_eventNr=m_runNr=0;
	m_hasOscillated=0, m_Multiplicity=0;
	m_eventType=0; m_L0=0; m_Hlt1=0; m_Hlt2=0; m_TrueTag=0;
	m_L0TisTos=0; m_HltTisTos=0; m_backgroundCat=-1;
	m_StdOmega=0.5; m_StdTag=0; m_StdTagCat=0; m_Btau=0; m_BtauErr=-1; 
	m_SecVtxChi2=0; m_SecVtxCharge=0; m_pileup=0;
	m_SecVtxVector=m_RecVertex=m_MCVertex=m_BSVertex=TVector3(0,0,0);
	m_TaggersFromDV=0; m_TaggersDecisionFromDV=0;
	m_SignalB=m_OppositeB=NULL;
	m_SignalParts.clear(); 
	m_Particles.clear();   
	m_vertices.clear();    
	m_TauChi2=0;
	m_ProcessNr=0;
      } ///< Constructor

 /*  Event(const Event& p) { */
/*     *this = p; */
/*   }///< copy constructor */

  ~Event( ){
    Particles::iterator i;
    for(i=m_SignalParts.begin();i!=m_SignalParts.end(); ++i){
      if(*i) delete (*i);
    }
    for(i=m_Particles.begin();i!=m_Particles.end(); ++i){
      if(*i) delete (*i);
    }
    Vertices::iterator iv;
    for(iv=m_vertices.begin();iv!=m_vertices.end(); ++iv){
      if(*iv) delete (*iv);
    }
    if(m_OppositeB) delete m_OppositeB;

  } ///< Destructor
  
  int    runNr()	  { return m_runNr;} ///< run number
  int    eventNr()	{ return m_eventNr;} ///< event number
  double multiplicity() { return m_Multiplicity; }///< multiplicity of tracks in event

  double StdOmega() { return m_StdOmega; }///< mistag as calculated in DaVinci
  double StdTag()   { return m_StdTag; }///< tag decision as calculated in DaVinci
  double StdTagCat(){ return m_StdTagCat; }///< tag category as calculated in DaVinci
  int    eventType(){ return m_eventType; }///< event type number
  int    processNr() { return m_ProcessNr; } ///< pythia process number

  int    TrueTag()  { return m_TrueTag; } ///< true tag decision 

  int    L0()   { return m_L0; } ///< L0 trigger decision
  int    Hlt1()  { return m_Hlt1; }///< Hlt1 trigger decision
  int    Hlt2()  { return m_Hlt2; }///< Hlt2 trigger decision
  int    L0TisTos() { return m_L0TisTos; }///< L0 TisTos classification
  int    HltTisTos(){ return m_HltTisTos; }///< Hlt TisTos classification

  int    backgroundCat(){ return m_backgroundCat; }///< background category tool output

  int    pileup(){ return m_pileup; }///< number of extra primary vertices

  double Btau()      { return m_Btau; } ///< lifetime value from fitter
  double BtauErr()   { return m_BtauErr; } ///< lifetime error from fitter
  double tauChi2()   { return m_TauChi2 ; } ///< chi2 of lifetime
  double BProperTime() {
    double BMassNom  = 5.279; if(isBs()) BMassNom = 5.367;
    if(!m_SignalB || m_SignalB->p()==0) cout<<"wrong signal"<<endmsg;
    return  (m_BSVertex-m_RecVertex).Mag()*BMassNom 
      /m_SignalB->p() /0.299792458;
  }///< signal B proper time calculated from PV and B decay point distances

  TVector3 SecVtxVector() { return m_SecVtxVector; }///< position of SV
  double SecVtxChi2()     { return m_SecVtxChi2; }///< SV chi2
  double SecVtxCharge()   { return m_SecVtxCharge; }///< charge of SV

  Particles particles()   { return m_Particles; }///< retrieve all tagger candidates

  int taggersFromDV() { return m_TaggersFromDV;}///< retrieve taggers flag as calculated in DaVinci
  int taggersDecisionFromDV() { return m_TaggersDecisionFromDV;}///< retrieve taggers flag  decisions as calculated in DaVinci

  Vertices getVertices()    {return m_vertices; } ///< get Vertices in event
  bool hasSecondaryVertex() { return !m_vertices.empty(); }///< event has a SV


  //Signal
  Particles signalParts() { return m_SignalParts; }///< retrieve all signal particles 
  void clearSignalParts() { m_SignalParts.clear(); }///< clear signal particle container 

  Particle* signalB()   { return m_SignalB; }///< retrieve signal B
  Particle* oppositeB() { return m_OppositeB; }///< retrieve opposite B
  bool hasOscillated()  { return m_hasOscillated; }///< signal B has oscillated

  TVector3 BSVertex()   { return m_BSVertex; }///< retrieve signal B decay position
  TVector3 RecVertex()  { return m_RecVertex; }///< retrieve PV position
  TVector3 MCVertex()   { return m_MCVertex; }///< retrieve true PV position

  bool isBs() {  
    if(!m_SignalB) {
      cout<<"SignalB Particle was not set!"<<endmsg;
      return false;
    }
    int id = abs(m_SignalB->ID());
    if(id==531) return true; 
    return false; 
  }///< signal B meson is Bs type
  bool isBd() {  
    if(!m_SignalB) {
      cout<<"SignalB Particle was not set!"<<endmsg;
      return false;
    }
    int id = abs(m_SignalB->ID());
    if(id==511) return true; 
    return false; 
  }///< signal B meson is Bd type
  bool isBu() {  
    if(!m_SignalB) {
      cout<<"SignalB Particle was not set!"<<endmsg;
      return false;
    }
    int id = abs(m_SignalB->ID());
    if(id==521) return true; 
    return false; 
  }///< signal B meson is Bu type


  //set///
  void setRunNr(int a)  {  m_runNr = a; }
  void setEventNr(int a){  m_eventNr = a; }
  void setMultiplicity(int a){  m_Multiplicity = a; }

  void setStdOmega(double a) {  m_StdOmega = a; }
  void setStdTag(double a)   {  m_StdTag = a; }
  void setStdTagCat(double a){  m_StdTagCat = a; }
  void seteventType(int a)   {  m_eventType = a; }
  void setTrueTag(int a)     {  m_TrueTag= a; }

  void setL0(int a)       {  m_L0 = a; }
  void setL0TisTos(int a) {  m_L0TisTos = a; }
  void setHltTisTos(int a){  m_HltTisTos = a; }

  void setbackgroundCat(int a){  m_backgroundCat = a; }

  void setHlt1(int a )  { m_Hlt1=a; }
  void setHlt2(int a )  { m_Hlt2=a; }

  void setBtau(double a)      {  m_Btau = a; }
  void setBtauErr(double a)   {  m_BtauErr = a; };

  void setSecVtxVector(TVector3 a) {  m_SecVtxVector = a; }
  void setSecVtxChi2(double a)     {  m_SecVtxChi2 = a; }
  void setSecVtxCharge(double a)   {  m_SecVtxCharge = a; }

  void setSignalParts(Particles a){  m_SignalParts = a; }
  void setSignalB(Particle* a)   {  m_SignalB = a; }
  void setOppositeB(Particle* a) {  m_OppositeB = a; }
  void sethasOscillated(bool a) {  m_hasOscillated = a; }

  void setRecVertex(TVector3 a) {  m_RecVertex = a; }
  void setBSVertex(TVector3 a)  {  m_BSVertex  = a; }
  void setMCVertex(TVector3 a)  {  m_MCVertex  = a; }

  void setpileup(int a)  {  m_pileup  = a; }

  void setVertices(Vertices a) {m_vertices = a;}
  
  void setTaggersFromDV(int a ) {m_TaggersFromDV=a;}
  void setTaggersDecisionFromDV(int a ) {m_TaggersDecisionFromDV=a;}

  void setTauChi2(double a ) { m_TauChi2=a ; }
  void setProcessNr(int a ){ m_ProcessNr=a; }
  void addToParticles(Particle* a)   { return m_Particles.push_back(a); }
  void setParticles(Particles a)  {  m_Particles  = a; }
  void addToSignalParts(Particle* a){ m_SignalParts.push_back(a); }

  void Print() {
    //if(DBGLEVEL<3) {
      cout<< "=================================================== "
	  << m_runNr << " " << m_eventNr <<endl;
      cout<< "TAG EVENT: "<<m_runNr<< "   " << m_eventNr<<endl;
      cout<< "           Trig="<< m_Hlt2<<m_Hlt1<<m_L0
	  <<"   multip="<<m_Multiplicity
	  << "   L0TisTos=" << m_L0TisTos
	  << "   bkgCat=" << m_backgroundCat<<endl;
      cout<< "           RecPV_z="<< m_RecVertex.z()
	  << "   OppoB.ID=" << (m_OppositeB!=NULL? m_OppositeB->ID():0)
	  << "   Oscill=" <<m_hasOscillated<<endl;
      //}
  } ///< printout event information


 private:

  bool      m_hasOscillated;
  int       m_eventType, m_L0, m_Hlt1, m_Hlt2, m_TrueTag, m_pileup;
  int       m_L0TisTos, m_HltTisTos, m_backgroundCat, m_Multiplicity;
  int       m_TaggersFromDV, m_TaggersDecisionFromDV;
  double    m_StdOmega, m_StdTag, m_StdTagCat, m_Btau, m_BtauErr; 
  double    m_SecVtxChi2, m_SecVtxCharge, m_TauChi2;
  TVector3  m_SecVtxVector, m_RecVertex, m_BSVertex, m_MCVertex;
  Particles m_SignalParts, m_Particles;
  Particle  *m_SignalB, *m_OppositeB;
  Vertices  m_vertices;
  int       m_ProcessNr, m_eventNr, m_runNr;

};

#endif 
