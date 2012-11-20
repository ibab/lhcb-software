#include "dijet.h"

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
    } dijet_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } dijet_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } dijet_pwhg_rnd_;

    // Initialize POWHEG.
    void dijet_pwhginit_();

    // Reset the counters.
    void dijet_resetcnt_(char *string, int length);

    // Generate an event.
    void dijet_pwhgevent_();

    // Access POWHEG input data.
    double dijet_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_dijet::LHAupPowheg_dijet() {};
  
  // Destructor.
  LHAupPowheg_dijet::~LHAupPowheg_dijet() {};

  // Initialize POWHEG.
  bool LHAupPowheg_dijet::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(dijet_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    dijet_pwhginit_();
  
    // Reset all the counters.
    dijet_resetcnt_("upper bound failure in inclusive cross section", 46);
    dijet_resetcnt_("vetoed calls in inclusive cross section", 39);
    dijet_resetcnt_("upper bound failures in generation of radiation", 47);
    dijet_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_dijet::fillHepEup() {
    
    // Call POWHEG to generate event.
    dijet_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_dijet::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(dijet_heprup_.idbmup[0], dijet_heprup_.ebmup[0],
	     dijet_heprup_.pdfgup[0], dijet_heprup_.pdfsup[0]);
    setBeamB(dijet_heprup_.idbmup[1], dijet_heprup_.ebmup[1],
	     dijet_heprup_.pdfgup[1], dijet_heprup_.pdfsup[1]);
    setStrategy(dijet_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < dijet_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, dijet_heprup_.xsecup[ip]);
      addProcess( dijet_heprup_.lprup[ip], xsec, dijet_heprup_.xerrup[ip], 
		  dijet_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = dijet_heprup_.ebmup[0];
    eBeamB = dijet_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_dijet::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    dijet_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(dijet_hepeup_.idprup, dijet_hepeup_.xwgtup,
	       dijet_hepeup_.scalup, dijet_hepeup_.aqedup,
	       dijet_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < dijet_hepeup_.nup; ++ip)
      addParticle(dijet_hepeup_.idup[ip], dijet_hepeup_.istup[ip],
		  dijet_hepeup_.mothup[ip][0], dijet_hepeup_.mothup[ip][1],
		  dijet_hepeup_.icolup[ip][0], dijet_hepeup_.icolup[ip][1],
		  dijet_hepeup_.pup[ip][0], dijet_hepeup_.pup[ip][1], 
		  dijet_hepeup_.pup[ip][2], dijet_hepeup_.pup[ip][3], 
		  dijet_hepeup_.pup[ip][4], dijet_hepeup_.vtimup[ip], 
		  dijet_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(dijet_hepeup_.idup[0], dijet_hepeup_.idup[1],
	   dijet_hepeup_.pup[0][3]/eBeamA, dijet_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_dijet::libraryName() {
    return "dijet";
  }
}
