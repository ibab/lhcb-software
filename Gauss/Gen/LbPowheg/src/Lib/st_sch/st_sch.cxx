#include "st_sch.h"

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
    } st_sch_heprup_;
    
    // The event Les Houches common block.
    extern struct {
      int nup, idprup;
      double xwgtup, scalup, aqedup, aqcdup;
      int idup[500], istup[500], mothup[500][2], icolup[500][2];
      double pup[500][5], vtimup[500],spinup[500];
    } st_sch_hepeup_;

    // The random number common block.
    extern struct {
      int rnd_numseeds, rnd_initialseed, rnd_iwhichseed;
      char rnd_cwhichseed[4];
      int rnd_i1, rnd_i2;
    } st_sch_pwhg_rnd_;

    // Initialize POWHEG.
    void st_sch_pwhginit_();

    // Reset the counters.
    void st_sch_resetcnt_(char *string, int length);

    // Generate an event.
    void st_sch_pwhgevent_();

    // Access POWHEG input data.
    double st_sch_powheginput_(char *string, int length);
  }

  // Constructor.
  LHAupPowheg_st_sch::LHAupPowheg_st_sch() {};
  
  // Destructor.
  LHAupPowheg_st_sch::~LHAupPowheg_st_sch() {};

  // Initialize POWHEG.
  bool LHAupPowheg_st_sch::fillHepRup() {
    
    // Set multiple random seeds to none.
    strcpy(st_sch_pwhg_rnd_.rnd_cwhichseed, "none");
  
    // Initialize POWHEG.
    st_sch_pwhginit_();
  
    // Reset all the counters.
    st_sch_resetcnt_("upper bound failure in inclusive cross section", 46);
    st_sch_resetcnt_("vetoed calls in inclusive cross section", 39);
    st_sch_resetcnt_("upper bound failures in generation of radiation", 47);
    st_sch_resetcnt_("vetoed radiation", 16);
  
    return true;
  }

  // Generate an event.
  bool LHAupPowheg_st_sch::fillHepEup() {
    
    // Call POWHEG to generate event.
    st_sch_pwhgevent_();
  
    return true;
  }

  // Set the initialization info.
  bool LHAupPowheg_st_sch::setInit() {

    // Call the routine that does the job.
    if (!fillHepRup()) return false;

    // Store beam and strategy info. 
    setBeamA(st_sch_heprup_.idbmup[0], st_sch_heprup_.ebmup[0],
	     st_sch_heprup_.pdfgup[0], st_sch_heprup_.pdfsup[0]);
    setBeamB(st_sch_heprup_.idbmup[1], st_sch_heprup_.ebmup[1],
	     st_sch_heprup_.pdfgup[1], st_sch_heprup_.pdfsup[1]);
    setStrategy(st_sch_heprup_.idwtup);

    // Store process info. Protect against vanishing cross section.
    for (int ip = 0; ip < st_sch_heprup_.nprup; ++ip) {
      double xsec = max( 1e-10, st_sch_heprup_.xsecup[ip]);
      addProcess( st_sch_heprup_.lprup[ip], xsec, st_sch_heprup_.xerrup[ip], 
		  st_sch_heprup_.xmaxup[ip] );
    }

    // Store the beam energies to calculate x values later.
    eBeamA = st_sch_heprup_.ebmup[0];
    eBeamB = st_sch_heprup_.ebmup[1];    
    // Done.
    return true;
  }

  // Set the event info.
  bool LHAupPowheg_st_sch::setEvent(int idProcIn = 0) {

    // In some strategies the type of the next event has been set.
    st_sch_hepeup_.idprup = idProcIn;

    // Call the routine that does the job.
    if (!fillHepEup()) return false;

    // Store process info.
    setProcess(st_sch_hepeup_.idprup, st_sch_hepeup_.xwgtup,
	       st_sch_hepeup_.scalup, st_sch_hepeup_.aqedup,
	       st_sch_hepeup_.aqcdup);

    // Store particle info.
    for (int ip = 0; ip < st_sch_hepeup_.nup; ++ip)
      addParticle(st_sch_hepeup_.idup[ip], st_sch_hepeup_.istup[ip],
		  st_sch_hepeup_.mothup[ip][0], st_sch_hepeup_.mothup[ip][1],
		  st_sch_hepeup_.icolup[ip][0], st_sch_hepeup_.icolup[ip][1],
		  st_sch_hepeup_.pup[ip][0], st_sch_hepeup_.pup[ip][1], 
		  st_sch_hepeup_.pup[ip][2], st_sch_hepeup_.pup[ip][3], 
		  st_sch_hepeup_.pup[ip][4], st_sch_hepeup_.vtimup[ip], 
		  st_sch_hepeup_.spinup[ip]);

    // Store x values (here E = pup[ip][3]), but note incomplete info.
    setPdf(st_sch_hepeup_.idup[0], st_sch_hepeup_.idup[1],
	   st_sch_hepeup_.pup[0][3]/eBeamA, st_sch_hepeup_.pup[1][3]/eBeamB,
	   0., 0., 0., false);

    // Done.
    return true;
  }

  // Print the library name.
  string LHAupPowheg_st_sch::libraryName() {
    return "st_sch";
  }
}
