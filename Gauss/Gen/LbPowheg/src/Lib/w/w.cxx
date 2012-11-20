#include "w.h"

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
    } w_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } w_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } w_pwhg_rnd_;

    // Initialize POWHEG.
    void w_pwhginit_();

    // Reset the counters.
    void w_resetcnt_(char *string, int length);

    // Generate an event.
    void w_pwhgevent_();

    // Access POWHEG input data.
    double w_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_w::LHAupPowheg_w() {};
  
  // Destructor.
  LHAupPowheg_w::~LHAupPowheg_w() {};

  // Initialize POWHEG.
  bool LHAupPowheg_w::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(w_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    w_pwhginit_();
  
    // Reset all the counters.
    w_resetcnt_("upper bound failure in inclusive cross section", 46);
    w_resetcnt_("vetoed calls in inclusive cross section", 39);
    w_resetcnt_("upper bound failures in generation of radiation", 47);
    w_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_w::fillHepEup() {
    
    // Call POWHEG to generate event.
    w_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_w::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(w_heprup_.idbmup[0], w_heprup_.ebmup[0],
	     w_heprup_.pdfgup[0], w_heprup_.pdfsup[0]);
    setBeamB(w_heprup_.idbmup[1], w_heprup_.ebmup[1],
	     w_heprup_.pdfgup[1], w_heprup_.pdfsup[1]);
    setStrategy(w_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < w_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, w_heprup_.xsecup[ip]);
      addProcess( w_heprup_.lprup[ip], xsec, w_heprup_.xerrup[ip], 
		  w_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = w_heprup_.ebmup[0];
    eBeamB = w_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_w::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    w_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(w_hepeup_.idprup, w_hepeup_.xwgtup,
	       w_hepeup_.scalup, w_hepeup_.aqedup,
	       w_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < w_hepeup_.nup; ++ip)
      addParticle(w_hepeup_.idup[ip], w_hepeup_.istup[ip],
		  w_hepeup_.mothup[ip][0], w_hepeup_.mothup[ip][1],
		  w_hepeup_.icolup[ip][0], w_hepeup_.icolup[ip][1],
		  w_hepeup_.pup[ip][0], w_hepeup_.pup[ip][1], 
		  w_hepeup_.pup[ip][2], w_hepeup_.pup[ip][3], 
		  w_hepeup_.pup[ip][4], w_hepeup_.vtimup[ip], 
		  w_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(w_hepeup_.idup[0], w_hepeup_.idup[1],
	   w_hepeup_.pup[0][3]/eBeamA, w_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_w::libraryName() {
    return "w";
  }
}
