// Include files 
// ============================================================================
#include <unistd.h>
#include <sys/wait.h>

// STL 
#include <fstream>

// Boost
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"

// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Local 
// ============================================================================
//#include "LbPowheg/Powheg.h"
#include "PowhegProduction.h"
#include "LbPowheg/ReadLHE.h"
// ============================================================================
/** @class PowhegProduction PowhegProduction.cpp
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProduction::PowhegProduction
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : LbPythia::ReadLHE ( type , name , parent ) , m_LUN ( 0 )
  {	
  
    PythiaProduction::m_userProcess    = 3 ; ///< see LHEREADPROCESS
    
    declareInterface< IProductionTool >( this ) ;
    
    //=============================================================================
    // POWHEG - BOX simulation parameters
    //=============================================================================
    declareProperty( "numevts", m_numevts = 3, "number of events to be generated");
    declareProperty( "ih1", m_ih1 = 1, "hadron 1 (1 for protons, -1 for antiprotons)");
    declareProperty( "ih2", m_ih2 = 1, "hadron 2 (1 for protons, -1 for antiprotons)");
    declareProperty( "ebeam1", m_ebeam1 = 7000, "energy of beam 1 [GeV] ");
    declareProperty( "ebeam2", m_ebeam2 = 7000, "energy of beam 2 [GeV] ");
    declareProperty( "lhans1", m_lhans1 = 10550, "pdf set for hadron 1 (LHA numbering)");
    declareProperty( "lhans2", m_lhans2 = 10550, "pdf set for hadron 2 (LHA numbering)");
    declareProperty( "iseed", m_iseed = 1082, "initalize number random sequence");
    declareProperty( "withnegweights", m_withnegweights = 1, "(default 0) if on (1) use negative weights");
    
    // Parameters that control the grid for Born variables generation
    declareProperty( "ncall1", m_ncall1 = 1000, "number of calls for initializing the integration grid");
    declareProperty( "itmx1", m_itmx1 = 10, "number of iterations for initializing the integration grid");
    declareProperty( "ncall2", m_ncall2 = 1000, "number of calls for computing the integral and finding upper bound");
    declareProperty( "itmx2", m_itmx2 = 10, "number of iterations for computing the integral and finding upper bound");
    declareProperty( "foldcsi", m_foldcsi = 1, "number of folds on csi integration");
    declareProperty( "foldy", m_foldy = 1, "number of folds on  y  integration");
    declareProperty( "foldphi", m_foldphi = 1, "number of folds on phi integration");
    declareProperty( "nubound", m_nubound = 1000, "number of calls to set up the upper bounding norms for radiation");
    declareProperty( "use_old_grid", m_use_old_grid = 0, "if 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)");
    declareProperty( "use_old_ubound", m_use_old_ubound = 0, "if 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate"); 
    
    // Wbb production parameters
    //=============================================================================
    declareProperty( "idvecbos", m_idvecbos = -24, "PDG code for vector boson to be produced ( W+:24 W-:-24 )");
    declareProperty( "vdecaymode", m_vdecaymode = 2, "1: electronic decay; 2: muonic decay");
    
    // WZ boson production parameters
    //=============================================================================
    declareProperty( "vdecaymodeW", m_vdecaymodeW = -11 , "PDG code for charged decay product of the W boson (11:e-;-11:e+;...)");
    declareProperty( "vdecaymodeZ", m_vdecaymodeZ = 13 , "PDG code for lepton Z boson (11:e-; 12: ve; ...) ");
    declareProperty( "mllmin", m_mllmin = 20 , "default 0.1 GeV this is minimum invar mass for Z leptons");
    declareProperty( "zerowidth", m_zerowidth = 0 ,"if 1 (true) use zero width approximatrion (default 0)");
    declareProperty( "withinterference", m_withinterference = 1 , "if 1 (true) include interference for like flavour charged leptons (default 1)");
    declareProperty( "dronly", m_dronly = 0 , "if 1 (true) only include double resonant diagrams (default 0)");
    declareProperty( "wllmin", m_wllmin = 0.01, "default 1d-5 GeV this is min invar mass for W leptons");	/// check value
    // CKM matrix
    declareProperty( "diagCKM", m_diagCKM = 0 , "if 1 (true) then diagonal CKM (default 0)");
    // anom couplings
    declareProperty( "delg1_z", m_delg1_z = 0 , "Delta_g1(Z)");
    declareProperty( "delg1_g", m_delg1_g = 0 , "Delta_g1(Gamma)");
    declareProperty( "lambda_z", m_lambda_z = 0 , "Lambda(Z)");
    declareProperty( "lambda_g", m_lambda_g = 0 , "Lambda(gamma)");
    declareProperty( "delk_g", m_delk_g = 0 , "Delta_K(Gamma)");
    declareProperty( "delk_z", m_delk_z = 0, "Delta_K(Z)");
    declareProperty( "tevscale", m_tevscale = 2 , "Form-factor scale, in TeV");	/// check value

    // ZZ boson production parameters
    //=============================================================================    
    declareProperty( "vdecaymodeZ1", m_vdecaymodeZ1 = 11 , "PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)");
    declareProperty( "vdecaymodeZ2", m_vdecaymodeZ2 = 13 , "PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)");
    //declareProperty( "mllmin", m_mllmin = 20 , "minimum invariant mass of lepton pairs from Z decay");
    
   
    // heavy quark production parameters  ( bb, tt )
    //=============================================================================
    declareProperty( "ndns1", m_ndns1 = 131 , "pdf for hadron 1 (hvqpdf numbering)");
    declareProperty( "ndns2", m_ndns2 = 131 , "pdf for hadron 2 ");
    declareProperty( "qmass", m_qmass = 172 ,"mass of heavy quark in GeV");
    declareProperty( "facscfact", m_facscfact = 1 ,"factorization scale factor: mufact=muref*facscfact");
    declareProperty( "renscfact", m_renscfact = 1 , "renormalization scale factor: muren=muref*renscfact");
    declareProperty( "iymax", m_iymax = 1 , "<= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)");
    declareProperty( "ixmax", m_ixmax = 1 , "<= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)");
    declareProperty( "xupbound", m_xupbound = 2 , "increase upper bound for radiation generation");
    declareProperty( "topdecaymode", m_topdecaymode = 20000 , "particles(antiparticles) in the final state: 20000 : both top go into b l nu (with the appropriate signs)");
    declareProperty( "tdec_wmass", m_tdec_wmass = 80.4 ,"Parameter for the generation of spin correlations in t tbar decays: W mass for top decay");
    declareProperty( "tdec_wwidth", m_tdec_wwidth = 2.141 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_bmass", m_tdec_bmass = 5 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_twidth", m_tdec_twidth = 1.31 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_elbranching", m_tdec_elbranching = 0.108 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_emass", m_tdec_emass = 0.00051 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_mumass", m_tdec_mumass = 0.1057 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_taumass", m_tdec_taumass = 1.777 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_dmass", m_tdec_dmass = 0.100 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_umass", m_tdec_umass = 0.100 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_smass", m_tdec_smass = 0.200 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_cmass", m_tdec_cmass = 1.5 , "Parameter for the generation of spin correlations in t tbar decays");
    declareProperty( "tdec_sin2cabibbo", m_tdec_sin2cabibbo = 0.051 , "Parameter for the generation of spin correlations in t tbar decays");
    
    setProperty ( "InputFile" , "pwgevents.lhe" ) ;
  }

      
  
//=============================================================================
// Destructor 
//============================================================================= 
   PowhegProduction::~PowhegProduction() {} ///< destructor 

// ============================================================================
// tool initialization 
// ============================================================================
StatusCode PowhegProduction::initialize (const char *process_name)
{
  // Prepare input file for POWHEG - BOX
  
    std::ofstream g( "powheg.input" ) ;
    g << "numevts " << m_numevts << std::endl
      << "ih1 " << m_ih1 << std::endl
      << "ih2 " << m_ih2 << std::endl
      << "ebeam1 " << m_ebeam1 << std::endl
      << "ebeam2 " << m_ebeam2 << std::endl
      << "lhans1 " << m_lhans1 << std::endl
      << "lhans2 " << m_lhans2 << std::endl
      << "withnegweights " << m_withnegweights << std::endl
      << "iseed " << m_iseed << std::endl			/// ToDo: remake to use Gauss seeds
      << "foldcsi " << m_foldcsi << std::endl
      << "foldphi " << m_foldphi << std::endl
      << "foldy " << m_foldy << std::endl
      << "use-old-grid " << m_use_old_grid << std::endl
      << "use-old-ubound " << m_use_old_ubound << std::endl;
   
  if ( process_name == "Wbb" )
  {
    g << "ncall1 " << 10000 << std::endl
      << "itmx1 " << 10 << std::endl
      << "ncall2 " << 10000 << std::endl
      << "itmx2 " << 10 << std::endl
      << "nubound " << 1000000 << std::endl
      << "idvecbos " << m_idvecbos << std::endl
      << "vdecaymode " << m_vdecaymode << std::endl ;
  }
  
  if ( process_name == "WZ" )
  {
    g << "ncall1 " << 40000 << std::endl
      << "itmx1 " << 4 << std::endl
      << "ncall2 " << 10000 << std::endl
      << "itmx2 " << 5 << std::endl
      << "nubound " << 10000 << std::endl
      << "vdecaymodeW " << m_vdecaymodeW << std::endl
      << "vdecaymodeZ " << m_vdecaymodeZ << std::endl
      << "mllmin " << m_mllmin << std::endl
      << "zerowidth " << m_zerowidth << std::endl
      << "withinterference " << m_withinterference << std::endl
      << "dronly " << m_dronly << std::endl
      << "wllmin " << m_wllmin << std::endl
      << "diagCKM " << m_diagCKM << std::endl
      << "delg1_z " << m_delg1_z << std::endl
      << "delg1_g " << m_delg1_g << std::endl
      << "lambda_z " << m_lambda_z << std::endl
      << "lambda_g " << m_lambda_g << std::endl  
      << "delk_g " << m_delk_g << std::endl
      << "delk_z " << m_delk_z << std::endl 
      << "tevscale " << m_tevscale << std::endl;
  }
  
if ( process_name == "ZZ" )
  {
    g << "ncall1 " << 300000 << std::endl
      << "itmx1 " << 3 << std::endl
      << "ncall2 " << 1000000 << std::endl
      << "itmx2 " << 5 << std::endl
      << "nubound " << 500000 << std::endl
      << "vdecaymodeZ1 " << m_vdecaymodeZ1 << std::endl
      << "vdecaymodeZ2 " << m_vdecaymodeZ2 << std::endl
      << "mllmin " << m_mllmin << std::endl;
  }
  
if ( process_name == "bb" )					/// ToDo: verify output !!!
  {
    g << "qmass " <<m_qmass << std::endl
	<< "facscfact " <<m_facscfact << std::endl
	<< "renscfact " <<m_renscfact << std::endl   
	<< "ncall1 " << 1000000 << std::endl
      << "itmx1 " << 5 << std::endl
      << "ncall2 " << 1000000 << std::endl
      << "itmx2 " << 5 << std::endl
      << "nubound " << 10000 << std::endl
      << "iymax " << m_iymax << std::endl
      << "ixmax " << m_ixmax << std::endl
      << "xupbound " << m_xupbound << std::endl;
  }
  
if ( process_name == "tt" )					/// ToDo: verify output !!!
  {
    g << "qmass " << m_qmass << std::endl
	<< "facscfact " << m_facscfact << std::endl
	<< "renscfact " << m_renscfact << std::endl
	<< "topdecaymode " << m_topdecaymode << std::endl
	<< "tdec/wmass " << m_tdec_wmass << std::endl
	<< "tdec/wwidth " << m_tdec_wwidth << std::endl
	<< "tdec/bmass " << m_tdec_bmass << std::endl
	<< "tdec/twidth " << m_tdec_twidth << std::endl
	<< "tdec/elbranching " << m_tdec_elbranching << std::endl
	<< "tdec/emass " << m_tdec_emass << std::endl
	<< "tdec/mumass " << m_tdec_mumass << std::endl
	<< "tdec/taumass " << m_tdec_taumass << std::endl
	<< "tdec/dmass " << m_tdec_dmass << std::endl
	<< "tdec/umass " << m_tdec_umass << std::endl
	<< "tdec/smass " << m_tdec_smass << std::endl
	<< "tdec/cmass  " << m_tdec_cmass  << std::endl
	<< "tdec/sin2cabibbo  " << m_tdec_sin2cabibbo  << std::endl
	<< "ncall1 " << 10000 << std::endl
      << "itmx1 " << 5 << std::endl
      << "ncall2 " << 100000 << std::endl
      << "itmx2 " << 5 << std::endl
      << "nubound " << 100000 << std::endl
      << "iymax " << m_iymax << std::endl
      << "ixmax " << m_ixmax << std::endl
      << "xupbound " << m_xupbound << std::endl;
  }

  g.close() ;
  
  Makelink2PDFfile();
  generateLHE(process_name);

  //Initialize of Pythia done here
  StatusCode sc = LbPythia::ReadLHE::initialize( ) ;


  if ( sc.isFailure() ) return sc ;

  return sc ;
}

StatusCode PowhegProduction::finalize ()
{ 
  // Do some cleaning of files
  boost::filesystem::remove( boost::filesystem::path( "powheg.input" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgborngrid.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgbtlgrid.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgcounters.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwggrid.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwghistnorms.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgrmngrid.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgstat.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgubound.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgxgrid.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwhg_checklimits" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "cteq6m" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "pwgevents.lhe" ) ) ;
  
  
  //Finalize of Pythia done here
  StatusCode sc = LbPythia::ReadLHE::finalize() ;


  return sc ;
}

//============================================================================
// Generate  events with Powheg-BOX and wrtite to .lhe file
//============================================================================
StatusCode PowhegProduction::generateLHE(const char *process_name){
  
  pid_t pid; 
  int status;
  
  pid=fork();

    // Attempt to fork and check for errors 
	if( (pid) < 0){
		exit(-1);	// something went wrong
	}

  if(pid){			// A positive (non-negative) PID indicates the parent process	
    wait( &status );				
  }
  else{				// A zero PID indicates that this is the child process
    char* pwhgexe;
    if ( "UNKNOWN" != System::getEnv( "POWHEGEXE" ) ) {
 	std::string powhegexe ;
	// POWHEGEXE is defined at requirement file of LbPowheg package
	
	if ((process_name == "tt") || (process_name == "bb"))
	  powhegexe = System::getEnv( "POWHEGEXE" )+"/"+"hvq";				// heavy quark production
	else 
	  powhegexe = System::getEnv( "POWHEGEXE" )+"/"+ (std::string)process_name;
	
	// needed for execl:
	pwhgexe = (char*)malloc( sizeof( char ) *(powhegexe.length() +1) );
	strcpy( pwhgexe, powhegexe.c_str() );
 	}
 	else {
		exit (-1);
		}	
		// RUN POWHEG-BOX
		if(execl(pwhgexe, process_name, NULL) == -1){
			 exit(status);
		}
		else { 
		  exit(status); 
		}
    }
  return StatusCode::SUCCESS ;
}

StatusCode PowhegProduction::Makelink2PDFfile()
{
  
  // get working direktory path
  char* cwd;
  if ((cwd = getcwd(NULL, sizeof(cwd)*1024)) == NULL) {
    perror("pwd");
    exit(2);
  }
  
  pid_t pid; 
  int status;
  
  // get cteq6m location
  std::string arg ;
if ( "UNKNOWN" != System::getEnv( "CTEQPDF" ) ) {
    arg = System::getEnv( "CTEQPDF" )+"/cteq6m.LHpdf";
  }
  char* link_cteq6m = (char*)malloc( sizeof( char ) * (arg.length() +1) );
  strcpy( link_cteq6m, arg.c_str() );
  
  std::string temp2 = (std::string)cwd + "/cteq6m";
  
  char* link_cwd_cteq6m = (char*)malloc( sizeof( char ) * (temp2.length() +1) );
  strcpy( link_cwd_cteq6m, temp2.c_str() );
  
  // Attempt to fork and check for errors 
  if( (pid=fork()) == -1){
    // something went wrong 
    exit(1);
  }
  
  if(pid){		// A positive (non-negative) PID indicates the parent process	
    wait( &status );				
  }
  else{		// A zero PID indicates that this is the child process	
    if (execlp("ln","ln","-s", link_cteq6m, link_cwd_cteq6m, NULL) == -1){
	exit(status);
	}
    }
  
  // free alocated memory
  free (link_cteq6m);
  free (link_cwd_cteq6m);
  
  return StatusCode::SUCCESS;
}

// ============================================================================
// The END 
// ============================================================================
