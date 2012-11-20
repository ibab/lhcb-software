#include "zz.h"

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
    } zz_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } zz_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } zz_pwhg_rnd_;

    // Initialize POWHEG.
    void zz_pwhginit_();

    // Reset the counters.
    void zz_resetcnt_(char *string, int length);

    // Generate an event.
    void zz_pwhgevent_();

    // Access POWHEG input data.
    double zz_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_zz::LHAupPowheg_zz() {};
  
  // Destructor.
  LHAupPowheg_zz::~LHAupPowheg_zz() {};

  // Initialize POWHEG.
  bool LHAupPowheg_zz::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(zz_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    zz_pwhginit_();
  
    // Reset all the counters.
    zz_resetcnt_("upper bound failure in inclusive cross section", 46);
    zz_resetcnt_("vetoed calls in inclusive cross section", 39);
    zz_resetcnt_("upper bound failures in generation of radiation", 47);
    zz_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_zz::fillHepEup() {
    
    // Call POWHEG to generate event.
    zz_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_zz::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(zz_heprup_.idbmup[0], zz_heprup_.ebmup[0],
	     zz_heprup_.pdfgup[0], zz_heprup_.pdfsup[0]);
    setBeamB(zz_heprup_.idbmup[1], zz_heprup_.ebmup[1],
	     zz_heprup_.pdfgup[1], zz_heprup_.pdfsup[1]);
    setStrategy(zz_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < zz_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, zz_heprup_.xsecup[ip]);
      addProcess( zz_heprup_.lprup[ip], xsec, zz_heprup_.xerrup[ip], 
		  zz_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = zz_heprup_.ebmup[0];
    eBeamB = zz_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_zz::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    zz_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(zz_hepeup_.idprup, zz_hepeup_.xwgtup,
	       zz_hepeup_.scalup, zz_hepeup_.aqedup,
	       zz_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < zz_hepeup_.nup; ++ip)
      addParticle(zz_hepeup_.idup[ip], zz_hepeup_.istup[ip],
		  zz_hepeup_.mothup[ip][0], zz_hepeup_.mothup[ip][1],
		  zz_hepeup_.icolup[ip][0], zz_hepeup_.icolup[ip][1],
		  zz_hepeup_.pup[ip][0], zz_hepeup_.pup[ip][1], 
		  zz_hepeup_.pup[ip][2], zz_hepeup_.pup[ip][3], 
		  zz_hepeup_.pup[ip][4], zz_hepeup_.vtimup[ip], 
		  zz_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(zz_hepeup_.idup[0], zz_hepeup_.idup[1],
	   zz_hepeup_.pup[0][3]/eBeamA, zz_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_zz::libraryName() {
    return "zz";
  }
}
