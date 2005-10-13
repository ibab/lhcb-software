// $Id: PythiaAlg.h,v 1.3 2005-10-13 21:45:42 robbep Exp $

#ifndef LBPYTHIA_PYTHIAALG_H
#define LBPYTHIA_PYTHIAALG_H 1

#include <string>

#include "Generators/GenBaseAlg.h"

#include "LbPythia/Pydat1.h"
#include "LbPythia/Pydat2.h"
#include "LbPythia/Pydat3.h"
#include "LbPythia/Pysubs.h"
#include "LbPythia/Pypars.h"
#include "LbPythia/Pydatr.h"
#include "LbPythia/Pymssm.h"
#include "LbPythia/Pyint2.h"

// forward declaration
class IParticlePropertySvc ;
class IEvtGenTool ;

typedef std::vector<std::string> CommandVector;

/** @class PythiaAlg PythiaAlg.h LbPythia/PythiaAlg.h
 *  
 *  Interface algorithm with Pythia.
 *  Original code from ATLAS modeled after CDF code.
 *
 *  @author Witek Pokorski
 *  @author Gloria Corti
 *  @author Patrick Robbe
 *  @author Angelo Carbone
 *  @date   2003-10-10
 */

class PythiaAlg : public GenBaseAlg {
public:

  /// Standard constructor 
  PythiaAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~PythiaAlg();

  /** Initialize PYTHIA generator
   *  @return Status Code
   */
  virtual StatusCode genInitialize();

  /** Main function: call PYTHIA generator
   *  @return Status Code
   */
  virtual int callGenerator();

  /** Finalize PYTHIA generator
   *  @return Status Code
   */
  virtual StatusCode genFinalize();

  /** Fill an HepMC::Event from the HEPEVT common block
   *  @param[out] evt The converted HepMC::Event 
   *  @return Status Code
   */
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

  /** Analyse the event 
   *  @retval true if the event pass the generator level selection criteria
   *  @param[in] evt The HepMC::Event to study
   */
  virtual bool selectEvt( HepMC::GenEvent* evt ) ;
 
  /** Pre-initialize the Algorithm
   *  @retval Status Code
   */
  virtual StatusCode preInitialize() ;

  /** Pre-finalize the Algorithm
   *  @retval Status Code
   */
  virtual StatusCode preFinalize()   ;

  /// Turn Off Pythia Fragmentation ( MSTJ(1) = 0 ) 
  virtual void turnOffPythiaFragmentation( ) ;

  /// Turn On Pythia Fragmentation ( MSTJ(1) = 1 ) 
  virtual void turnOnPythiaFragmentation( ) ;

  /// returns 1 if the event contains a B hadron or a b / b bar quark
  virtual int isBBbarEvent( ) ;

  /// save parton event ( CALL PYEDIT(21) )
  virtual void savePartonEvent( ) ;

  /// hadronize stored Pythia event ( CALL PYEXEC ) 
  virtual void hadronize( ) ;

  /** convert Pythia common blocks to StdHep  ( CALL LUNHEP ) and returns 
   *  number of b quarks 
   */
  virtual int convertToStdHep( ) ;

  /// load parton event in Pythia common ( CALL PYEDIT(22) )
  virtual void loadPartonEvent ( ) ;

  /// retrieve the hard scatter parameters
  virtual void hardProcessInfo(HardInfo* hardInfo);

protected:
  /// Parse Pythia commands from a string vector
  StatusCode parsePythiaCommands( const CommandVector & theVector ) ;

  /** @defgroup pythiablocks Pythia Common Blocks
   *  PYTHIA common blocks accessors
   *  @{
   */
  /** PYDAT1 Accessor */
	Pydat1& pydat1()  { return m_pydat1 ; }
  /** PYDAT2 Accessor */
	Pydat2& pydat2()  { return m_pydat2 ; }
  /** PYDAT3 Accessor */
	Pydat3& pydat3()  { return m_pydat3 ; }
  /** PYSUBS Accessor */
	Pysubs& pysubs()  { return m_pysubs ; }
  /** PYPARS Accessor */
	Pypars& pypars()  { return m_pypars ; } 
  /** PYDATR Accessor */
	Pydatr& pydatr()  { return m_pydatr ; }
  /** PYMSSM Accessor */
	Pymssm& pymssm()  { return m_pymssm ; }
  /** PYINT2 Accessor */
	Pyint2& pyint2()  { return m_pyint2 ; }
  /* @} */

  double m_pbeam ;    ///< Incident proton energy (GeV)

  std::string m_frame  ;  	 ///< FRAME string
  std::string m_beam   ;     ///< BEAM string
  std::string m_target ;     ///< TARGET string
  double m_win         ;     ///< WIN

	CommandVector m_userPythiaCommandVector ; ///< User Commands to setup pythia
  CommandVector m_basePythiaCommandVector ; ///< Basic commands to setup Pythia

  /// Particle Property Svc Interface 
  IParticlePropertySvc * m_ppSvc ;

  /// EvtGen Tool interface
  IEvtGenTool * m_evtTool ;

  /// Print Pythia parameters
  void printPythiaParameter( ) ;

  
private:  
  /// +/- crossing angle in the vertical plane (microrad)
  double m_vangle; 
  
  /// +/- crossing angle in the horizotanl plane (microrad)
  double m_hangle; 
  
  /// emittance (nm)
  double m_emit;
  
  /// beta* (m)
  double m_beta; 
  
  /// angular smearing
  double m_anglesmear; 

  /// Switch to fixed target(=true)
  bool m_fixed;

  /// Argument of CALL PYLIST after each PYTHIA event generation
	int  m_eventlistlevel;

  /// Argument of CALL PYLIST after PYTHIA initialization
	int  m_initlistlevel;

  /// Name of output file where all Pythia output will be redirected
	std::string  m_envval1;

  /// FORTRAN unit number to open to redirect pythia output
	int  m_fortout;

  /// Argument of CALL PYSTAT at the end of the job
  int m_pystatlevel ;

	/// event counter
	int m_events ;

  /// PYSUBS Common Block data
	Pysubs m_pysubs;
  
	/// PYDAT1 Common Block data
	Pydat1 m_pydat1;  
  
	/// PYPARS Common Block data
	Pypars m_pypars;
  
	/// PYDAT2 Common Block data
	Pydat2 m_pydat2;
  
	/// PYDAT3 Common Block data
	Pydat3 m_pydat3;  
  
	/// PYDATR Common Block data
	Pydatr m_pydatr;
  
	/// PYINT2 Common Block data
	Pyint2 m_pyint2;	
  
	/// PYMSSM (SUSY) Common Block data
	Pymssm m_pymssm;

  /// Minimum mass of the particles to generate as signal
  double m_minBMass ;

  /// Id of the minimum mass particle to generate as signal
  int    m_targetIdMinBMass ;

  /// Number of particles of requested type in one event
  int m_obtainedSignalType ;

  /// Total branching fraction of the signal decay mode
  double m_signalBr ;  
};

#endif  // LBPYTHIA_PYTHIAALG_H
