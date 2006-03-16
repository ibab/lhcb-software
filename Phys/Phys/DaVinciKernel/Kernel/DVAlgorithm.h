#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/AlgFactory.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"

// from DaVinciTools
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IAlgorithm2ID.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IParticleDescendants.h"

/** @class DVAlgorithm DVAlgorithm.h Kernel/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 *  16/07/2004: P. Koppenburg: Make it a GaudiTupleAlg
 *  11/11/2004: P. Koppenburg: Adapt to next get<> and put<>. Merge with PreDV.
 */
class DVAlgorithm : public GaudiTupleAlg {
public:

  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVAlgorithm( ){ }; ///< Destructor
  
  /// Overridden from Gaudi Algo to produce a warning if not called by user
  StatusCode setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  StatusCode initialize ();  

  // Overridden from Gaudi Algorithm
  StatusCode finalize ();  

  // Overridden from Gaudi Algorithm
  std::string getDecayDescriptor(){return m_decayDescriptor;};  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (std::string outputLocationString);  

  /// get algorithm ID
  int getAlgorithmID();
 
  /// Accessor for PhysDesktop Tool
  inline IPhysDesktop* desktop()const{
    return getTool<IPhysDesktop>(m_desktopName,m_desktop,this) ;
  }

  /// Accessor for Vertex Fitter Tool
  inline IVertexFit* vertexFitter(int index=0)const{
    return getTool<IVertexFit>(index,m_vertexFitNames,m_vertexFit,this);
  }

  /// Accessor for Geometrical Displacement Calculation Tool
  inline IGeomDispCalculator* geomDispCalculator()const{
    return getTool<IGeomDispCalculator>(m_geomToolName,m_geomTool,this);
  }

  /// Accessor for Particle Filter Tool
  inline IParticleFilter* particleFilter(int index=0)const{
    return getTool<IParticleFilter>(index,m_filterNames,m_filter,this);
  }

  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap()const{
    return getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }

  /// Accessor for Algorithm2ID Tool
  inline IAlgorithm2ID* algorithmID()const{
    return getTool<IAlgorithm2ID>(m_algorithm2IDToolName,m_algorithm2IDTool);
  }

  /// Tagging Tool
  inline IBTaggingTool* flavourTagging()const{
    return getTool<IBTaggingTool>(m_taggingToolName,m_taggingTool);
  }

  /// Descnedants
  inline IParticleDescendants* descendants()const{
    return getTool<IParticleDescendants>(m_descendantsName,m_descendants);
  }

  /// Accessor for ParticlePropertySvc
  inline IParticlePropertySvc* ppSvc() const {return m_ppSvc;};

protected:

  /** helper protected function to load the tool on-demand 
   *  by index 
   *  @param index tool index 
   *  @param names list of tools typ/names 
   *  @param tools the list of tools 
   *  @param ptr the pointer to this or NULL for private or common tools
   *  @return tool with given index 
   */
  template<class TYPE> 
  TYPE* getTool ( const size_t index, 
                  const std::vector<std::string>& names , 
                  std::vector<TYPE*>& tools,
                  const IInterface* ptr=NULL ) const ;

  /** helper protected function to load the tool on-demand  
   *  @param name of tool
   *  @param tool 
   *  @param ptr the pointer to this or NULL for private or common tools
   *  @return tool 
   */
  template<class TYPE> 
    TYPE* getTool ( const std::string& name, 
                    TYPE*& tool,
                    const IInterface* ptr=NULL ) const ;

private:

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools() ;

  /// Method to create SelResult container
  StatusCode fillSelResult() ;

  /// Reference to desktop tool
  mutable IPhysDesktop* m_desktop;
  /// Concrete type desktop
  std::string m_desktopName;

private:
  /// Reference to Vertex Fitter
  mutable std::vector<IVertexFit*> m_vertexFit;
  /// Concrete type of vertex fitter
  std::vector<std::string> m_vertexFitNames;

  /// Reference to geometrical displacement Calculation.
  mutable IGeomDispCalculator* m_geomTool;  
  /// Concrete type of geom tool
  std::string m_geomToolName;

  /// Reference to CheckOverlap
  mutable ICheckOverlap* m_checkOverlap;
  /// Concrete type of CheckOverlap tool
  std::string m_checkOverlapName;

  /// Reference to ParticleFilter
  mutable std::vector<IParticleFilter*> m_filter;
  /// Concrete Type of ParticleFilter tool
  std::vector<std::string> m_filterNames;  

  /// Reference to Algorithm2ID
  mutable IAlgorithm2ID* m_algorithm2IDTool;
  /// Concrete Type of IAlgorithm2ID tool
  std::string m_algorithm2IDToolName;

  /// Reference to FlavourTagging
  mutable IBTaggingTool* m_taggingTool;
  /// Concrete Type of FlavourTagging tool
  std::string m_taggingToolName;

  /// Reference to ParticleDescendants
  mutable IParticleDescendants* m_descendants;
  /// Concrete Type of ParticleDescendants  tool
  std::string m_descendantsName;

  /// Reference to ParticlePropertySvc
  mutable IParticlePropertySvc* m_ppSvc;

private:
  /// Decay description (Property)
  std::string m_decayDescriptor;
  /// Avoid writing SelResult object in TES (Property)
  bool m_avoidSelResult;
  /// Avoid printing SelResult statistics 
  /// (cannot be switched off by OutputLevel)
  bool m_printSelResult;
  

  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Count number of times selection filter is used 
  /// (= number of times alg is called)
  int m_countFilterWrite ;
  /// Number of passing events
  int m_countFilterPassed ;

  /// - For GaudiAlgorithm -
  /// Algorithm ID
  int m_algorithmID ;

  /// Switch PreloadTools to false no to preload any tools.
  /// This will have the effect that they will be loaded on demand, when needed,
  /// at any event. This option is thus only recommended for use of DVAlgorithm
  /// to do something unrelated to physics analysis.
  bool m_preloadTools;

};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------


#endif // DAVINCIKERNEL_DVALGORITHM_H
