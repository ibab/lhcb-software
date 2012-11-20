#include "gg_h.h"

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
    } gg_h_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } gg_h_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } gg_h_pwhg_rnd_;

    // Initialize POWHEG.
    void gg_h_pwhginit_();

    // Reset the counters.
    void gg_h_resetcnt_(char *string, int length);

    // Generate an event.
    void gg_h_pwhgevent_();

    // Access POWHEG input data.
    double gg_h_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_gg_h::LHAupPowheg_gg_h() {};
  
  // Destructor.
  LHAupPowheg_gg_h::~LHAupPowheg_gg_h() {};

  // Initialize POWHEG.
  bool LHAupPowheg_gg_h::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(gg_h_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    gg_h_pwhginit_();
  
    // Reset all the counters.
    gg_h_resetcnt_("upper bound failure in inclusive cross section", 46);
    gg_h_resetcnt_("vetoed calls in inclusive cross section", 39);
    gg_h_resetcnt_("upper bound failures in generation of radiation", 47);
    gg_h_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_gg_h::fillHepEup() {
    
    // Call POWHEG to generate event.
    gg_h_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_gg_h::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(gg_h_heprup_.idbmup[0], gg_h_heprup_.ebmup[0],
	     gg_h_heprup_.pdfgup[0], gg_h_heprup_.pdfsup[0]);
    setBeamB(gg_h_heprup_.idbmup[1], gg_h_heprup_.ebmup[1],
	     gg_h_heprup_.pdfgup[1], gg_h_heprup_.pdfsup[1]);
    setStrategy(gg_h_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < gg_h_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, gg_h_heprup_.xsecup[ip]);
      addProcess( gg_h_heprup_.lprup[ip], xsec, gg_h_heprup_.xerrup[ip], 
		  gg_h_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = gg_h_heprup_.ebmup[0];
    eBeamB = gg_h_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_gg_h::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    gg_h_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(gg_h_hepeup_.idprup, gg_h_hepeup_.xwgtup,
	       gg_h_hepeup_.scalup, gg_h_hepeup_.aqedup,
	       gg_h_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < gg_h_hepeup_.nup; ++ip)
      addParticle(gg_h_hepeup_.idup[ip], gg_h_hepeup_.istup[ip],
		  gg_h_hepeup_.mothup[ip][0], gg_h_hepeup_.mothup[ip][1],
		  gg_h_hepeup_.icolup[ip][0], gg_h_hepeup_.icolup[ip][1],
		  gg_h_hepeup_.pup[ip][0], gg_h_hepeup_.pup[ip][1], 
		  gg_h_hepeup_.pup[ip][2], gg_h_hepeup_.pup[ip][3], 
		  gg_h_hepeup_.pup[ip][4], gg_h_hepeup_.vtimup[ip], 
		  gg_h_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(gg_h_hepeup_.idup[0], gg_h_hepeup_.idup[1],
	   gg_h_hepeup_.pup[0][3]/eBeamA, gg_h_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_gg_h::libraryName() {
    return "gg_h";
  }
}
