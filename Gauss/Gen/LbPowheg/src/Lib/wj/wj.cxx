#include "wj.h"

namespace Powheg {

  // External FORTRAN hooks to POWHEG.
  extern "C" {
    
    // The parameter Les Houches common block.
    extern struct {
      int idbmup[2];
      double ebmup[2];
      int pdfgup[2], pdfsup[2], idwtup, nprup;
      double xsecup[100], xerrup[100], xmaxup[100];
      int lprup[100];
    } wj_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } wj_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } wj_pwhg_rnd_;

    // Initialize POWHEG.
    void wj_pwhginit_();

    // Reset the counters.
    void wj_resetcnt_(char *string, int length);

    // Generate an event.
    void wj_pwhgevent_();

    // Access POWHEG input data.
    double wj_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_wj::LHAupPowheg_wj() {};
  
  // Destructor.
  LHAupPowheg_wj::~LHAupPowheg_wj() {};

  // Initialize POWHEG.
  bool LHAupPowheg_wj::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(wj_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    wj_pwhginit_();
  
    // Reset all the counters.
    wj_resetcnt_("upper bound failure in inclusive cross section", 46);
    wj_resetcnt_("vetoed calls in inclusive cross section", 39);
    wj_resetcnt_("upper bound failures in generation of radiation", 47);
    wj_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_wj::fillHepEup() {
    
    // Call POWHEG to generate event.
    wj_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_wj::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(wj_heprup_.idbmup[0], wj_heprup_.ebmup[0],
	     wj_heprup_.pdfgup[0], wj_heprup_.pdfsup[0]);
    setBeamB(wj_heprup_.idbmup[1], wj_heprup_.ebmup[1],
	     wj_heprup_.pdfgup[1], wj_heprup_.pdfsup[1]);
    setStrategy(wj_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < wj_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, wj_heprup_.xsecup[ip]);
      addProcess( wj_heprup_.lprup[ip], xsec, wj_heprup_.xerrup[ip], 
		  wj_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = wj_heprup_.ebmup[0];
    eBeamB = wj_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_wj::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    wj_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(wj_hepeup_.idprup, wj_hepeup_.xwgtup,
	       wj_hepeup_.scalup, wj_hepeup_.aqedup,
	       wj_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < wj_hepeup_.nup; ++ip)
      addParticle(wj_hepeup_.idup[ip], wj_hepeup_.istup[ip],
		  wj_hepeup_.mothup[ip][0], wj_hepeup_.mothup[ip][1],
		  wj_hepeup_.icolup[ip][0], wj_hepeup_.icolup[ip][1],
		  wj_hepeup_.pup[ip][0], wj_hepeup_.pup[ip][1], 
		  wj_hepeup_.pup[ip][2], wj_hepeup_.pup[ip][3], 
		  wj_hepeup_.pup[ip][4], wj_hepeup_.vtimup[ip], 
		  wj_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(wj_hepeup_.idup[0], wj_hepeup_.idup[1],
	   wj_hepeup_.pup[0][3]/eBeamA, wj_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_wj::libraryName() {
    return "wj";
  }
}
