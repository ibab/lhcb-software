#include "ww.h"

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
    } ww_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } ww_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } ww_pwhg_rnd_;

    // Initialize POWHEG.
    void ww_pwhginit_();

    // Reset the counters.
    void ww_resetcnt_(char *string, int length);

    // Generate an event.
    void ww_pwhgevent_();

    // Access POWHEG input data.
    double ww_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_ww::LHAupPowheg_ww() {};
  
  // Destructor.
  LHAupPowheg_ww::~LHAupPowheg_ww() {};

  // Initialize POWHEG.
  bool LHAupPowheg_ww::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(ww_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    ww_pwhginit_();
  
    // Reset all the counters.
    ww_resetcnt_("upper bound failure in inclusive cross section", 46);
    ww_resetcnt_("vetoed calls in inclusive cross section", 39);
    ww_resetcnt_("upper bound failures in generation of radiation", 47);
    ww_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_ww::fillHepEup() {
    
    // Call POWHEG to generate event.
    ww_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_ww::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(ww_heprup_.idbmup[0], ww_heprup_.ebmup[0],
	     ww_heprup_.pdfgup[0], ww_heprup_.pdfsup[0]);
    setBeamB(ww_heprup_.idbmup[1], ww_heprup_.ebmup[1],
	     ww_heprup_.pdfgup[1], ww_heprup_.pdfsup[1]);
    setStrategy(ww_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < ww_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, ww_heprup_.xsecup[ip]);
      addProcess( ww_heprup_.lprup[ip], xsec, ww_heprup_.xerrup[ip], 
		  ww_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = ww_heprup_.ebmup[0];
    eBeamB = ww_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_ww::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    ww_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(ww_hepeup_.idprup, ww_hepeup_.xwgtup,
	       ww_hepeup_.scalup, ww_hepeup_.aqedup,
	       ww_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < ww_hepeup_.nup; ++ip)
      addParticle(ww_hepeup_.idup[ip], ww_hepeup_.istup[ip],
		  ww_hepeup_.mothup[ip][0], ww_hepeup_.mothup[ip][1],
		  ww_hepeup_.icolup[ip][0], ww_hepeup_.icolup[ip][1],
		  ww_hepeup_.pup[ip][0], ww_hepeup_.pup[ip][1], 
		  ww_hepeup_.pup[ip][2], ww_hepeup_.pup[ip][3], 
		  ww_hepeup_.pup[ip][4], ww_hepeup_.vtimup[ip], 
		  ww_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(ww_hepeup_.idup[0], ww_hepeup_.idup[1],
	   ww_hepeup_.pup[0][3]/eBeamA, ww_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_ww::libraryName() {
    return "ww";
  }
}
