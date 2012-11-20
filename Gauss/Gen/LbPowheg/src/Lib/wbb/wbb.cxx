#include "wbb.h"

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
    } wbb_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } wbb_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } wbb_pwhg_rnd_;

    // Initialize POWHEG.
    void wbb_pwhginit_();

    // Reset the counters.
    void wbb_resetcnt_(char *string, int length);

    // Generate an event.
    void wbb_pwhgevent_();

    // Access POWHEG input data.
    double wbb_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_wbb::LHAupPowheg_wbb() {};
  
  // Destructor.
  LHAupPowheg_wbb::~LHAupPowheg_wbb() {};

  // Initialize POWHEG.
  bool LHAupPowheg_wbb::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(wbb_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    wbb_pwhginit_();
  
    // Reset all the counters.
    wbb_resetcnt_("upper bound failure in inclusive cross section", 46);
    wbb_resetcnt_("vetoed calls in inclusive cross section", 39);
    wbb_resetcnt_("upper bound failures in generation of radiation", 47);
    wbb_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_wbb::fillHepEup() {
    
    // Call POWHEG to generate event.
    wbb_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_wbb::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(wbb_heprup_.idbmup[0], wbb_heprup_.ebmup[0],
	     wbb_heprup_.pdfgup[0], wbb_heprup_.pdfsup[0]);
    setBeamB(wbb_heprup_.idbmup[1], wbb_heprup_.ebmup[1],
	     wbb_heprup_.pdfgup[1], wbb_heprup_.pdfsup[1]);
    setStrategy(wbb_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < wbb_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, wbb_heprup_.xsecup[ip]);
      addProcess( wbb_heprup_.lprup[ip], xsec, wbb_heprup_.xerrup[ip], 
		  wbb_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = wbb_heprup_.ebmup[0];
    eBeamB = wbb_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_wbb::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    wbb_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(wbb_hepeup_.idprup, wbb_hepeup_.xwgtup,
	       wbb_hepeup_.scalup, wbb_hepeup_.aqedup,
	       wbb_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < wbb_hepeup_.nup; ++ip)
      addParticle(wbb_hepeup_.idup[ip], wbb_hepeup_.istup[ip],
		  wbb_hepeup_.mothup[ip][0], wbb_hepeup_.mothup[ip][1],
		  wbb_hepeup_.icolup[ip][0], wbb_hepeup_.icolup[ip][1],
		  wbb_hepeup_.pup[ip][0], wbb_hepeup_.pup[ip][1], 
		  wbb_hepeup_.pup[ip][2], wbb_hepeup_.pup[ip][3], 
		  wbb_hepeup_.pup[ip][4], wbb_hepeup_.vtimup[ip], 
		  wbb_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(wbb_hepeup_.idup[0], wbb_hepeup_.idup[1],
	   wbb_hepeup_.pup[0][3]/eBeamA, wbb_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_wbb::libraryName() {
    return "wbb";
  }
}
