#include "vbf_h.h"

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
    } vbf_h_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } vbf_h_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } vbf_h_pwhg_rnd_;

    // Initialize POWHEG.
    void vbf_h_pwhginit_();

    // Reset the counters.
    void vbf_h_resetcnt_(char *string, int length);

    // Generate an event.
    void vbf_h_pwhgevent_();

    // Access POWHEG input data.
    double vbf_h_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_vbf_h::LHAupPowheg_vbf_h() {};
  
  // Destructor.
  LHAupPowheg_vbf_h::~LHAupPowheg_vbf_h() {};

  // Initialize POWHEG.
  bool LHAupPowheg_vbf_h::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(vbf_h_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    vbf_h_pwhginit_();
  
    // Reset all the counters.
    vbf_h_resetcnt_("upper bound failure in inclusive cross section", 46);
    vbf_h_resetcnt_("vetoed calls in inclusive cross section", 39);
    vbf_h_resetcnt_("upper bound failures in generation of radiation", 47);
    vbf_h_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_vbf_h::fillHepEup() {
    
    // Call POWHEG to generate event.
    vbf_h_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_vbf_h::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(vbf_h_heprup_.idbmup[0], vbf_h_heprup_.ebmup[0],
	     vbf_h_heprup_.pdfgup[0], vbf_h_heprup_.pdfsup[0]);
    setBeamB(vbf_h_heprup_.idbmup[1], vbf_h_heprup_.ebmup[1],
	     vbf_h_heprup_.pdfgup[1], vbf_h_heprup_.pdfsup[1]);
    setStrategy(vbf_h_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < vbf_h_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, vbf_h_heprup_.xsecup[ip]);
      addProcess( vbf_h_heprup_.lprup[ip], xsec, vbf_h_heprup_.xerrup[ip], 
		  vbf_h_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = vbf_h_heprup_.ebmup[0];
    eBeamB = vbf_h_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_vbf_h::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    vbf_h_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(vbf_h_hepeup_.idprup, vbf_h_hepeup_.xwgtup,
	       vbf_h_hepeup_.scalup, vbf_h_hepeup_.aqedup,
	       vbf_h_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < vbf_h_hepeup_.nup; ++ip)
      addParticle(vbf_h_hepeup_.idup[ip], vbf_h_hepeup_.istup[ip],
		  vbf_h_hepeup_.mothup[ip][0], vbf_h_hepeup_.mothup[ip][1],
		  vbf_h_hepeup_.icolup[ip][0], vbf_h_hepeup_.icolup[ip][1],
		  vbf_h_hepeup_.pup[ip][0], vbf_h_hepeup_.pup[ip][1], 
		  vbf_h_hepeup_.pup[ip][2], vbf_h_hepeup_.pup[ip][3], 
		  vbf_h_hepeup_.pup[ip][4], vbf_h_hepeup_.vtimup[ip], 
		  vbf_h_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(vbf_h_hepeup_.idup[0], vbf_h_hepeup_.idup[1],
	   vbf_h_hepeup_.pup[0][3]/eBeamA, vbf_h_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_vbf_h::libraryName() {
    return "vbf_h";
  }
}
