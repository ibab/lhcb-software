#ifndef LBPOWHEG_POWHEGPRODUCTION_H 
#define LBPOWHEG_POWHEGPRODUCTION_H 1
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Generators  
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/Pypars.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
#include "LbPythia/ReadFile.h"
#include "LbPythia/ReadLHE.h"
// ============================================================================
/** @class PowhegProduction PowhegProduction.h
 */
class PowhegProduction : public LbPythia::ReadLHE
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProduction> ;
  // =========================================================================
public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProduction ( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );
  
  
  virtual ~PowhegProduction() ; ///< destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ; ///< Finalize method
  
protected: 
  /// specific initialization
  void powhegInitialize( const std::string& processName ) ;
  
private:
  // the default constructor is disabled 
  PowhegProduction() ;
  // the copy constructor is disabled 
  PowhegProduction ( const PowhegProduction& ) ;
  // assignement operator is disabled 
  PowhegProduction& operator=( const PowhegProduction& ) ;

  /// logical file unit  (F77)
  int         m_LUN     ;                         // logical file unit  (F77)
  
  StatusCode generateLHE(const std::string& process_name) ;
  
  StatusCode Makelink2PDFfile();
  
  //=============================================================================    
  // POWHEG - BOX simulation parameters
  //=============================================================================
  
  // Nomenclature: Use "m_" + same variable names as has to be used in powheg.input
  
  int m_numevts; 	// number of events to be generated
  int m_ih1;		// hadron 1 (1 for protons, -1 for antiprotons)
  int m_ih2;		// hadron 2 (1 for protons, -1 for antiprotons)
  double m_ebeam1;	// energy of beam 1 [GeV]
  double m_ebeam2;	// energy of beam 2 [GeV]
  int m_lhans1;		// pdf set for hadron 1 (LHA numbering)
  int m_lhans2;		// pdf set for hadron 2 (LHA numbering)
  int m_iseed; 		// initalize number random sequence
  int m_withnegweights; 	// (default 0) if on (1) use negative weights
  
  // Parameters that control the grid for Born variables generation
  int m_ncall1;		// number of calls for initializing the integration grid
  int m_itmx1; 		// number of iterations for initializing the integration grid
  int m_ncall2; 	// number of calls for computing the integral and finding upper bound
  int m_itmx2;		// number of iterations for computing the integral and finding upper bound
  int m_foldcsi;	// number of folds on csi integration
  int m_foldy; 		// number of folds on  y  integration
  int m_foldphi; 	// number of folds on phi integration
  int m_nubound; 	// number of calls to set up the upper bounding norms for radiation
  
  int m_use_old_grid;	// if 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)
  int m_use_old_ubound;	// if 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate
  
  // Wbb production parameters
  //=============================================================================    
  int m_idvecbos;	// PDG code for vector boson to be produced ( W+:24 W-:-24 )
  int m_vdecaymode;	// 1: electronic decay; 2: muonic decay
  
  // WZ boson production parameters  
  //=============================================================================
  int m_vdecaymodeW;		// PDG code for charged decay product of the W boson (11:e-;-11:e+;...)
  int m_vdecaymodeZ;		// PDG code for lepton Z boson (11:e-; 12: ve; ...) 
  double m_mllmin;		// default 0.1 GeV this is minimum invar mass for Z leptons
  int m_zerowidth;		// if 1 (true) use zero width approximatrion (default 0)
  int m_withinterference;	// if 1 (true) include interference for like flavour charged leptons (default 1)
  int m_dronly;			// if 1 (true) only include double resonant diagrams (default 0)
  double m_wllmin;		// default 1d-5 GeV this is min invar mass for W leptons
  // CKM matrix	
  int m_diagCKM;		// if 1 (true) then diagonal CKM (default 0)
  // anom couplings
  double m_delg1_z;		// Delta_g1(Z)
  double m_delg1_g;		// Delta_g1(Gamma)
  double m_lambda_z;		// Lambda(Z)
  double m_lambda_g;		// Lambda(gamma)
  double m_delk_g;		// Delta_K(Gamma)
  double m_delk_z;		// Delta_K(Z)
  double m_tevscale;		// Form-factor scale, in TeV
  
  // ZZ boson production parameters  
  //=============================================================================
  int m_vdecaymodeZ1; 		// PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)
  int m_vdecaymodeZ2; 		// PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)
  //double m_mllmin;		// Minimum invariant mass of lepton pairs from Z decay
  
  // heavy quark production parameters  ( bb, tt )
  //=============================================================================
  int m_ndns1; 			// pdf for hadron 1 (hvqpdf numbering)
  int m_ndns2;			// pdf for hadron 2
  double m_qmass;			// mass of heavy quark in GeV
  int m_facscfact;		// factorization scale factor: mufact=muref*facscfact
  int m_renscfact; 		// renormalization scale factor: muren=muref*renscfact
  int m_iymax; 			// <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)
  int m_ixmax; 			// <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)
  int m_xupbound;			// increase upper bound for radiation generation
  int m_topdecaymode;		//an integer of 5 digits that are either 0, or 2, representing in 
					// the order the maximum number of the following particles(antiparticles)
					// in the final state: e  mu tau up charm
					// For example
					// 22222    All decays (up to 2 units of everything)
					// 20000    both top go into b l nu (with the appropriate signs)
					// 10011    one top goes into electron (or positron), the other into (any) hadrons,
					//          or one top goes into charm, the other into up
					// 00022    Fully hadronic
					// 00002    Fully hadronic with two charms
					// 00011    Fully hadronic with a single charm
					// 00012    Fully hadronic with at least one charm
  
  // Parameters for the generation of spin correlations in t tbar decays
  double m_tdec_wmass; 		//  W mass for top decay
  double m_tdec_wwidth;
  double m_tdec_bmass;
  double m_tdec_twidth;
  double m_tdec_elbranching;
  double m_tdec_emass;
  double m_tdec_mumass;
  double m_tdec_taumass;
  double m_tdec_dmass;
  double m_tdec_umass;
  double m_tdec_smass;
  double m_tdec_cmass;
  double m_tdec_sin2cabibbo;
  
} ;

#endif // LBPOWHEG_POWHEGPRODUCTION_H 
