#include "hvq.h"

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
    } hvq_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } hvq_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } hvq_pwhg_rnd_;

    // Initialize POWHEG.
    void hvq_pwhginit_();

    // Reset the counters.
    void hvq_resetcnt_(char *string, int length);

    // Generate an event.
    void hvq_pwhgevent_();

    // Access POWHEG input data.
    double hvq_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_hvq::LHAupPowheg_hvq() {};
  
  // Destructor.
  LHAupPowheg_hvq::~LHAupPowheg_hvq() {};

  // Initialize POWHEG.
  bool LHAupPowheg_hvq::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(hvq_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    hvq_pwhginit_();
  
    // Reset all the counters.
    hvq_resetcnt_("upper bound failure in inclusive cross section", 46);
    hvq_resetcnt_("vetoed calls in inclusive cross section", 39);
    hvq_resetcnt_("upper bound failures in generation of radiation", 47);
    hvq_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_hvq::fillHepEup() {
    
    // Call POWHEG to generate event.
    hvq_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_hvq::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(hvq_heprup_.idbmup[0], hvq_heprup_.ebmup[0],
	     hvq_heprup_.pdfgup[0], hvq_heprup_.pdfsup[0]);
    setBeamB(hvq_heprup_.idbmup[1], hvq_heprup_.ebmup[1],
	     hvq_heprup_.pdfgup[1], hvq_heprup_.pdfsup[1]);
    setStrategy(hvq_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < hvq_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, hvq_heprup_.xsecup[ip]);
      addProcess( hvq_heprup_.lprup[ip], xsec, hvq_heprup_.xerrup[ip], 
		  hvq_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = hvq_heprup_.ebmup[0];
    eBeamB = hvq_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_hvq::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    hvq_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(hvq_hepeup_.idprup, hvq_hepeup_.xwgtup,
	       hvq_hepeup_.scalup, hvq_hepeup_.aqedup,
	       hvq_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < hvq_hepeup_.nup; ++ip)
      addParticle(hvq_hepeup_.idup[ip], hvq_hepeup_.istup[ip],
		  hvq_hepeup_.mothup[ip][0], hvq_hepeup_.mothup[ip][1],
		  hvq_hepeup_.icolup[ip][0], hvq_hepeup_.icolup[ip][1],
		  hvq_hepeup_.pup[ip][0], hvq_hepeup_.pup[ip][1], 
		  hvq_hepeup_.pup[ip][2], hvq_hepeup_.pup[ip][3], 
		  hvq_hepeup_.pup[ip][4], hvq_hepeup_.vtimup[ip], 
		  hvq_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(hvq_hepeup_.idup[0], hvq_hepeup_.idup[1],
	   hvq_hepeup_.pup[0][3]/eBeamA, hvq_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_hvq::libraryName() {
    return "hvq";
  }
}
