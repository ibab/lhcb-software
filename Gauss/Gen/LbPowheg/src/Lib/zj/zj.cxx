#include "zj.h"

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
    } zj_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } zj_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } zj_pwhg_rnd_;

    // Initialize POWHEG.
    void zj_pwhginit_();

    // Reset the counters.
    void zj_resetcnt_(char *string, int length);

    // Generate an event.
    void zj_pwhgevent_();

    // Access POWHEG input data.
    double zj_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_zj::LHAupPowheg_zj() {};
  
  // Destructor.
  LHAupPowheg_zj::~LHAupPowheg_zj() {};

  // Initialize POWHEG.
  bool LHAupPowheg_zj::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(zj_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    zj_pwhginit_();
  
    // Reset all the counters.
    zj_resetcnt_("upper bound failure in inclusive cross section", 46);
    zj_resetcnt_("vetoed calls in inclusive cross section", 39);
    zj_resetcnt_("upper bound failures in generation of radiation", 47);
    zj_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_zj::fillHepEup() {
    
    // Call POWHEG to generate event.
    zj_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_zj::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(zj_heprup_.idbmup[0], zj_heprup_.ebmup[0],
	     zj_heprup_.pdfgup[0], zj_heprup_.pdfsup[0]);
    setBeamB(zj_heprup_.idbmup[1], zj_heprup_.ebmup[1],
	     zj_heprup_.pdfgup[1], zj_heprup_.pdfsup[1]);
    setStrategy(zj_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < zj_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, zj_heprup_.xsecup[ip]);
      addProcess( zj_heprup_.lprup[ip], xsec, zj_heprup_.xerrup[ip], 
		  zj_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = zj_heprup_.ebmup[0];
    eBeamB = zj_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_zj::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    zj_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(zj_hepeup_.idprup, zj_hepeup_.xwgtup,
	       zj_hepeup_.scalup, zj_hepeup_.aqedup,
	       zj_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < zj_hepeup_.nup; ++ip)
      addParticle(zj_hepeup_.idup[ip], zj_hepeup_.istup[ip],
		  zj_hepeup_.mothup[ip][0], zj_hepeup_.mothup[ip][1],
		  zj_hepeup_.icolup[ip][0], zj_hepeup_.icolup[ip][1],
		  zj_hepeup_.pup[ip][0], zj_hepeup_.pup[ip][1], 
		  zj_hepeup_.pup[ip][2], zj_hepeup_.pup[ip][3], 
		  zj_hepeup_.pup[ip][4], zj_hepeup_.vtimup[ip], 
		  zj_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(zj_hepeup_.idup[0], zj_hepeup_.idup[1],
	   zj_hepeup_.pup[0][3]/eBeamA, zj_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_zj::libraryName() {
    return "zj";
  }
}
