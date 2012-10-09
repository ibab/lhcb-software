#ifndef LBALPGEN_ALPGENPRODUCTION_H 
#define LBALPGEN_ALPGENPRODUCTION_H 1

// Include files
#include "LbPythia/PythiaProduction.h"

// Forward declarations
class IRndmGenSvc ;
class IRndmEngine ;

/** @class AlpGenProduction AlpGenProduction.h 
 *  
 *  Interface tool to produce events with AlpGen
 * 
 *  @author Stephane Tourneur
 *  @author Patrick Robbe
 *  @date   2012-07-13
 */


// //new job option cuts communicated to fortran:
//   extern "C" {
//     struct {
//       float letamin,  betamin, letamin1, betamin1;
      
//     } etacut_;    
//   }


class AlpGenProduction : public PythiaProduction {
 public:
  /// Standard constructor
  AlpGenProduction( const std::string & type , 
                    const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~AlpGenProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method

  virtual StatusCode finalize( ) ; ///< Finalize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;

 protected:

 private:
  StatusCode generateWeightedEvents( ) ;
  void generateUnweightedEvents( ) ;
  void Makelink2pdftable(int ndns);
  
  int           m_nevents ;
  IRndmGenSvc * m_randSvc ;
  IRndmEngine * m_engine ;
  std::string   m_fileLabel ;

  //alpgen simulation parameters (for zqq-like processes)
  // Nomenclature: Use "m_" + same variable names as has to be used in alpgen 
  int  m_nevxiter; //number of events per iteration to generate alpgen's phase space grid in alpgen mode 1. Do tests!"
  int m_niter; //number of iterations to generate alpgen's phase space grid in alpgen mode 1.
  int m_nwgtev; //number of weighted events to generate by alpgen in mode 1.  Do tests!
  unsigned short int m_ihrd; //Hard process code : 1:WQQ - 2:ZQQ - 7:QQQQ 13:top 
  int m_ndns; //parton density set (cf alpgen for codes) 5: CTEQ5L
  unsigned short int m_iqopt; //scale option (process dependent)
  float m_qfac; // Q scale rescaling factor
  unsigned short int m_ickkw; //0 : no matching of Matrix Elements and Parton Shower - 1: matching (mlm scheme)
  float m_ktfac; // scale factor for ckkw alphas scale
  unsigned short int m_njets; 
  unsigned short int m_ihvy; //heavy flavour type for procs like WQQ, ZQQ, 2Q, etc(4=c, 5=b, 6=t)
  unsigned short int m_ihvy2; //2nd heavy flavour type for procs like 4Q
  float m_mc; //charm mass
  float m_mb; 
  float m_mt; 
  float m_ptjmin;
  float m_ptbmin; 
  float m_ptcmin;
  float m_ptlmin;
  float m_etajmax; //max|eta| for light jets
  float m_etabmax; 
  float m_etacmax;
  float m_etalmax; //max abs(eta) for charged leptons
  float m_etalmin; //New cut: min eta for charged leptons
  float m_etabmin; //New cut: min eta for b quarks
  float m_eta1lmin; //New cut: min eta for 1 charged lepton
  float m_eta1bmin; //New cut: min eta for 1 b quark
  float m_drjmin; // min deltaR(j-j), deltaR(Q-j) [j=light jet, Q=c/b]
  float m_drbmin; //min deltaR(b-b) (procs with explicit b)
  float m_drcmin; //min deltaR(c-c) (procs with explicit c)
  float m_drlmin; //min deltaR between charged lepton and light jets
  //unsigned short int m_ilep; //Z*/gamma fin state: 0=lept (1 family) 1=nu (3 fam)
  float m_mllmin; //min dilepton inv mass
  float m_mllmax; //max dilepton inv mass
  //Selection of single-top process:
  //For imode=0,1:
  //4 single-top processes can be selected:
  //itopprc=1: t+q (njets=0)
  //itopprc=2: t+b (njets=0)
  //itopprc=3: t+W(W->f fbar")+jets (njets=0,1)
  //itopprc=4: t+b+W(W->f fbar')+jets (njets=0,1)
  unsigned short int m_itopprc;
  unsigned short int m_iwdecmode; //W decay modes, in imode=2 - 1:electron 2: muon
  unsigned short int m_itdecmode; //top (or t-tbar) decay modes, in imode=2
  unsigned short int m_izdecmode; //Z decay modes, in imode=2 - 1:electron 2: muon
  float m_xlclu; //lambda value for ckkw alpha (match shower alpha) - default -1
  int m_lpclu; //loop order for ckkw alpha (match shower alpha) - default -1
  unsigned short int m_cluopt; //kt scale option. 1:kt propto pt, 2:kt propto mt
  
};
#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
