#include "vbf_wp_wp.h"

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
    } vbf_wp_wp_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } vbf_wp_wp_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } vbf_wp_wp_pwhg_rnd_;

    // Initialize POWHEG.
    void vbf_wp_wp_pwhginit_();

    // Reset the counters.
    void vbf_wp_wp_resetcnt_(char *string, int length);

    // Generate an event.
    void vbf_wp_wp_pwhgevent_();

    // Access POWHEG input data.
    double vbf_wp_wp_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_vbf_wp_wp::LHAupPowheg_vbf_wp_wp() {};
  
  // Destructor.
  LHAupPowheg_vbf_wp_wp::~LHAupPowheg_vbf_wp_wp() {};

  // Initialize POWHEG.
  bool LHAupPowheg_vbf_wp_wp::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(vbf_wp_wp_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    vbf_wp_wp_pwhginit_();
  
    // Reset all the counters.
    vbf_wp_wp_resetcnt_("upper bound failure in inclusive cross section", 46);
    vbf_wp_wp_resetcnt_("vetoed calls in inclusive cross section", 39);
    vbf_wp_wp_resetcnt_("upper bound failures in generation of radiation", 47);
    vbf_wp_wp_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_vbf_wp_wp::fillHepEup() {
    
    // Call POWHEG to generate event.
    vbf_wp_wp_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_vbf_wp_wp::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(vbf_wp_wp_heprup_.idbmup[0], vbf_wp_wp_heprup_.ebmup[0],
	     vbf_wp_wp_heprup_.pdfgup[0], vbf_wp_wp_heprup_.pdfsup[0]);
    setBeamB(vbf_wp_wp_heprup_.idbmup[1], vbf_wp_wp_heprup_.ebmup[1],
	     vbf_wp_wp_heprup_.pdfgup[1], vbf_wp_wp_heprup_.pdfsup[1]);
    setStrategy(vbf_wp_wp_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < vbf_wp_wp_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, vbf_wp_wp_heprup_.xsecup[ip]);
      addProcess( vbf_wp_wp_heprup_.lprup[ip], xsec, vbf_wp_wp_heprup_.xerrup[ip], 
		  vbf_wp_wp_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = vbf_wp_wp_heprup_.ebmup[0];
    eBeamB = vbf_wp_wp_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_vbf_wp_wp::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    vbf_wp_wp_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(vbf_wp_wp_hepeup_.idprup, vbf_wp_wp_hepeup_.xwgtup,
	       vbf_wp_wp_hepeup_.scalup, vbf_wp_wp_hepeup_.aqedup,
	       vbf_wp_wp_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < vbf_wp_wp_hepeup_.nup; ++ip)
      addParticle(vbf_wp_wp_hepeup_.idup[ip], vbf_wp_wp_hepeup_.istup[ip],
		  vbf_wp_wp_hepeup_.mothup[ip][0], vbf_wp_wp_hepeup_.mothup[ip][1],
		  vbf_wp_wp_hepeup_.icolup[ip][0], vbf_wp_wp_hepeup_.icolup[ip][1],
		  vbf_wp_wp_hepeup_.pup[ip][0], vbf_wp_wp_hepeup_.pup[ip][1], 
		  vbf_wp_wp_hepeup_.pup[ip][2], vbf_wp_wp_hepeup_.pup[ip][3], 
		  vbf_wp_wp_hepeup_.pup[ip][4], vbf_wp_wp_hepeup_.vtimup[ip], 
		  vbf_wp_wp_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(vbf_wp_wp_hepeup_.idup[0], vbf_wp_wp_hepeup_.idup[1],
	   vbf_wp_wp_hepeup_.pup[0][3]/eBeamA, vbf_wp_wp_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_vbf_wp_wp::libraryName() {
    return "vbf_wp_wp";
  }
}
