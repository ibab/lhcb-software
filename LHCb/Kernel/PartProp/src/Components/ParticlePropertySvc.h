// $Id: ParticlePropertySvc.h,v 1.1.1.1 2008-12-01 18:27:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
#ifndef PARTICLEPROPERTYSVC_PARTICLEPROPERTYSVC_H
#define PARTICLEPROPERTYSVC_PARTICLEPROPERTYSVC_H
// ============================================================================
// Include Files
// ============================================================================
// STD&STL 
// ============================================================================
#include <map>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"

class IFileAccess;

// ============================================================================
/** @class ParticlePropertySvc ParticlePropertySvc.h 
 *
 *  This service provides access to particle properties.
 *  The settable property of this service is the 
 *  ParticlePropertiesFile, an ASCII file containing the 
 *  list of properties for the particles.
 *  The default file is: 
 *      "$PARAMFILESROOT/data/ParticleTable.txt"
 *      or if $PARAMFILESROOT is not defined 
 *      "ParticleTable.txt"
 * 
 *  @author Iain Last
 *  @author Gloria Corti 
 *  8/11/1999 change default file from
 *  $CDF/particle.cdf to $LHCBDBASE/cdf/particle.cdf on Unix 
 *  and use environment variable also on WinNT
 *  Also introduce finding of particle by StdHepID
 *
 *  @author Vanya BELYAEV
 *  @date 2006-09-22 
 *  1) Modified to add possibility to redefine properties of existing 
 *  particles and to read addiitonal files.
 *  New propeety "OtherFiles" (default is empty vector) is introduces.
 *  Service parsed additional files after the main one.
 * 
 *    @code 
 *
 *    // read additional properties 
 *    ParticlePropertySvc.OtherFiles = {
 *       "$SOMELOCATION1/file1.txt" , 
 *       "$SOMELOCATION2/file2.txt"  
 *    } ;
 *
 *    @endcode 
 *
 *  2) Add possibility to modify only certain partiles through the 
 *  new property "Particles"  (default is enpty list), 
 *  Each line is interpreted as a line in particle data table, e.g.
 *
 *   @code
 *    
 *   // redefine the properties of H_20 and H_30 particles:
 *   ParticlePropertySvc.Particles = {
 *       "H_20 88 35 0.0 120.0 9.4e-26 Higgs'0 35 0.0e+00" ,
 *       "H_30 89 36 0.0  40.0 1.0e-12      A0 36 0.0e+00"
 *     } ;
 *
 *   @endcode 
 *
 *  The replaces/modified particles are reported.
 */
class ParticlePropertySvc 
  :         public Service 
  , virtual public IParticlePropertySvc 
{
public:  
  // typedefs for container type
  typedef ParticleProperty* mapped_type;
  typedef std::map< int,         mapped_type > MapID       ;
  typedef std::map< std::string, mapped_type > MapName     ;
  typedef std::map< int,         mapped_type > MapStdHepID ;
  typedef std::map< int,         mapped_type > MapPythiaID ;
  typedef IParticlePropertySvc::VectPP VectPP;
  typedef IParticlePropertySvc::const_iterator const_iterator;
  typedef IParticlePropertySvc::iterator iterator;
  typedef std::set<ParticleProperty*>    Set ;
  // Inherited Service overrides:

  /// Initialise the service.
  virtual StatusCode initialize();  
  /// Finalise the service.
  virtual StatusCode finalize();
  /// Query the interfaces.
  virtual StatusCode queryInterface 
  ( const InterfaceID& riid, void** ppvInterface );
  /// Service type.
  virtual const InterfaceID& type() const;
  /** Create a new particle property.
   *  @attention ParticlePropertySvc *IS* the owner if this property! 
   *  @paramparticle String name of the particle.
   *  @param geantId Geant3 ID of the particle.
   *  @param jetsetId Jetset ID of the particle.
   *  @param charge Particle charge (/e).
   *  @param mass Particle mass (GeV).
   *  @param tlife Particle lifetime (s).   
   *  @param evtName String name of the particle in EvtGen.
   *  @param pythiaId Pythia ID of the particle.
   *  @return StatusCode - SUCCESS if the particle property was added.
   */
  virtual StatusCode push_back
  ( const std::string& particle ,
    int                geantId  , 
    int                jetsetId ,  
    double             charge   , 
    double             mass     , 
    double             tlife    ,
    const std::string& evtName  , 
    int                pythiaId , 
    double             maxWidth ) ;
  /** Add a new particle property.
   *  @attention ParticlePropertySvc is not the owner of this property! 
   *  @param pp, a particle property class.
   *  @return StatusCode - SUCCESS if the particle property was added.
   */
  virtual StatusCode push_back( ParticleProperty* pp );
  /// Get a const reference to the begining of the container.
  virtual const_iterator begin() const { return m_vectpp.begin() ; }
  /// Get a const reference to the end of the container.
  virtual const_iterator end  () const { return m_vectpp.end() ; }
  /// Get the container size.
  virtual int size() const { return m_vectpp.size() ; };
  /// Retrieve a property by geant3 id.
  virtual ParticleProperty* find( int geantId ) 
  { return m_idmap[ geantId ] ; }
  /// Retrieve a property by particle name.
  virtual ParticleProperty* find( const std::string& name )
  { return m_namemap[ name ] ; }
  /// Retrieve a property by StdHep id
  virtual ParticleProperty* findByStdHepID( int stdHepId )
  { return m_stdhepidmap[ stdHepId ] ; }
  /// Retrieve a property by Pythia id
  virtual ParticleProperty* findByPythiaID( int pythiaID ) 
  { return m_pythiaidmap[ pythiaID ]; }
  /// Erase a property by geant3 id.
  virtual StatusCode erase( int geantId )
  { return erase ( find ( geantId ) ) ; }
  /// Erase a property by particle name.
  virtual StatusCode erase( const std::string& name )
  { return erase ( find ( name ) ) ; }
  /// Erase a property by StdHep id ???
  virtual StatusCode eraseByStdHepID( int stdHepId )
  { return erase( findByStdHepID ( stdHepId ) ) ; }
  /** Standard Constructor.
   *  @param  name   String with service name
   *  @param  svc    Pointer to service locator interface
   */
  ParticlePropertySvc
  ( const std::string& name , ISvcLocator* svc );
  /// Destructor.
  virtual ~ParticlePropertySvc();
protected:
  /** helper (protected) function to 
   *  find an antiparticle for the given particle ID (StdHepID)
   *  @param pp pointer to particle property 
   *  @return pointer to antiparticle
   */
  const ParticleProperty* anti 
  ( const ParticleProperty* pp ) const ;
  /** helper (protected) function to set the valid 
   *  particle<-->antiparticle relations
   *  @return status code 
   */
  StatusCode setAntiParticles() ;
  /// rebuild "the linear container" from the map 
  StatusCode rebuild() ;
  /// remove particle property from all maps 
  StatusCode erase ( const ParticleProperty* pp ) ;
  /// parse the file 
  StatusCode parse ( const std::string& file ) ;
  // Text file parsing routine. 
  StatusCode parse();       
  // treat additional particles 
  StatusCode addParticles() ;
  // compare 2 entries
  bool diff 
  ( const ParticleProperty* o              , 
    const ParticleProperty* n              , 
    const MSG::Level        l = MSG::DEBUG ) const ;
private:
  typedef std::vector<std::string> Files ;
  typedef std::vector<std::string> Particles ;
  
  std::string m_filename; ///< Filename of the particle properties file
  Files m_other ; ///< additional file names 
  // properties to be redefined  explicitely 
  Particles m_particles ;

  VectPP      m_vectpp;          ///< Vector of all particle properties
  MapID       m_idmap;           ///< Map for geant IDs
  MapName     m_namemap;         ///< Map for particle names
  MapStdHepID m_stdhepidmap;     ///< Map for StdHep Ids
  MapPythiaID m_pythiaidmap;     ///< Map for Pythia Ids
  
  // local storage of ALL properties  
  Set                           m_owned    ;
  std::set<std::string>         m_replaced ;
  
  IFileAccess *m_fileAccess;
};

// =============================================================================
// The END 
// =============================================================================
#endif
// =============================================================================


