#include "wz.h"

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
    } wz_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } wz_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } wz_pwhg_rnd_;

    // Initialize POWHEG.
    void wz_pwhginit_();

    // Reset the counters.
    void wz_resetcnt_(char *string, int length);

    // Generate an event.
    void wz_pwhgevent_();

    // Access POWHEG input data.
    double wz_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_wz::LHAupPowheg_wz() {};
  
  // Destructor.
  LHAupPowheg_wz::~LHAupPowheg_wz() {};

  // Initialize POWHEG.
  bool LHAupPowheg_wz::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(wz_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    wz_pwhginit_();
  
    // Reset all the counters.
    wz_resetcnt_("upper bound failure in inclusive cross section", 46);
    wz_resetcnt_("vetoed calls in inclusive cross section", 39);
    wz_resetcnt_("upper bound failures in generation of radiation", 47);
    wz_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_wz::fillHepEup() {
    
    // Call POWHEG to generate event.
    wz_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_wz::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(wz_heprup_.idbmup[0], wz_heprup_.ebmup[0],
	     wz_heprup_.pdfgup[0], wz_heprup_.pdfsup[0]);
    setBeamB(wz_heprup_.idbmup[1], wz_heprup_.ebmup[1],
	     wz_heprup_.pdfgup[1], wz_heprup_.pdfsup[1]);
    setStrategy(wz_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < wz_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, wz_heprup_.xsecup[ip]);
      addProcess( wz_heprup_.lprup[ip], xsec, wz_heprup_.xerrup[ip], 
		  wz_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = wz_heprup_.ebmup[0];
    eBeamB = wz_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_wz::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    wz_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(wz_hepeup_.idprup, wz_hepeup_.xwgtup,
	       wz_hepeup_.scalup, wz_hepeup_.aqedup,
	       wz_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < wz_hepeup_.nup; ++ip)
      addParticle(wz_hepeup_.idup[ip], wz_hepeup_.istup[ip],
		  wz_hepeup_.mothup[ip][0], wz_hepeup_.mothup[ip][1],
		  wz_hepeup_.icolup[ip][0], wz_hepeup_.icolup[ip][1],
		  wz_hepeup_.pup[ip][0], wz_hepeup_.pup[ip][1], 
		  wz_hepeup_.pup[ip][2], wz_hepeup_.pup[ip][3], 
		  wz_hepeup_.pup[ip][4], wz_hepeup_.vtimup[ip], 
		  wz_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(wz_hepeup_.idup[0], wz_hepeup_.idup[1],
	   wz_hepeup_.pup[0][3]/eBeamA, wz_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_wz::libraryName() {
    return "wz";
  }
}
