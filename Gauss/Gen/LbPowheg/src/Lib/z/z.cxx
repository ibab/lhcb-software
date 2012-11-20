#include "z.h"

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
    } z_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } z_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } z_pwhg_rnd_;

    // Initialize POWHEG.
    void z_pwhginit_();

    // Reset the counters.
    void z_resetcnt_(char *string, int length);

    // Generate an event.
    void z_pwhgevent_();

    // Access POWHEG input data.
    double z_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_z::LHAupPowheg_z() {};
  
  // Destructor.
  LHAupPowheg_z::~LHAupPowheg_z() {};

  // Initialize POWHEG.
  bool LHAupPowheg_z::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(z_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    z_pwhginit_();
  
    // Reset all the counters.
    z_resetcnt_("upper bound failure in inclusive cross section", 46);
    z_resetcnt_("vetoed calls in inclusive cross section", 39);
    z_resetcnt_("upper bound failures in generation of radiation", 47);
    z_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_z::fillHepEup() {
    
    // Call POWHEG to generate event.
    z_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_z::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(z_heprup_.idbmup[0], z_heprup_.ebmup[0],
	     z_heprup_.pdfgup[0], z_heprup_.pdfsup[0]);
    setBeamB(z_heprup_.idbmup[1], z_heprup_.ebmup[1],
	     z_heprup_.pdfgup[1], z_heprup_.pdfsup[1]);
    setStrategy(z_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < z_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, z_heprup_.xsecup[ip]);
      addProcess( z_heprup_.lprup[ip], xsec, z_heprup_.xerrup[ip], 
		  z_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = z_heprup_.ebmup[0];
    eBeamB = z_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_z::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    z_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(z_hepeup_.idprup, z_hepeup_.xwgtup,
	       z_hepeup_.scalup, z_hepeup_.aqedup,
	       z_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < z_hepeup_.nup; ++ip)
      addParticle(z_hepeup_.idup[ip], z_hepeup_.istup[ip],
		  z_hepeup_.mothup[ip][0], z_hepeup_.mothup[ip][1],
		  z_hepeup_.icolup[ip][0], z_hepeup_.icolup[ip][1],
		  z_hepeup_.pup[ip][0], z_hepeup_.pup[ip][1], 
		  z_hepeup_.pup[ip][2], z_hepeup_.pup[ip][3], 
		  z_hepeup_.pup[ip][4], z_hepeup_.vtimup[ip], 
		  z_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(z_hepeup_.idup[0], z_hepeup_.idup[1],
	   z_hepeup_.pup[0][3]/eBeamA, z_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_z::libraryName() {
    return "z";
  }
}
