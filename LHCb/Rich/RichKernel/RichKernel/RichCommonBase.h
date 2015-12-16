
//-----------------------------------------------------------------------------
/** @file RichCommonBase.h
 *
 *  Header file for RICH base class : Rich::CommonBase
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-27
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHCOMMONBASE_H
#define RICHKERNEL_RICHCOMMONBASE_H 1

// Interfaces
#include "RichKernel/IRichToolRegistry.h"
#include "RichKernel/IRichDetectorTool.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SerializeSTL.h"
#include "GaudiKernel/DataObject.h"

// Forward declarations
class IJobOptionsSvc;
class DeRich;

// Some defines for debug/verbose messages...
#define _ri_debug if( msgLevel(MSG::DEBUG)   ) debug()
#define _ri_verbo if( msgLevel(MSG::VERBOSE) ) verbose()

namespace Rich
{
  using GaudiUtils::operator<<;

  //-----------------------------------------------------------------------------
  /** @class CommonBase RichCommonBase.h RichKernel/RichCommonBase.h
   *
   *  Base class providing common functionality for all RICH tools and algorithms
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-08-27
   */
  //-----------------------------------------------------------------------------

  template <class PBASE>
  class CommonBase : public PBASE
  {

  public:

    /// Standard algorithm-like constructor
    CommonBase( const std::string& name,
                ISvcLocator* pSvcLocator );

    /// Standard tool-like constructor
    CommonBase( const std::string& type,
                const std::string& name,
                const IInterface* parent );

    /// Standard Converter-like Constructor
    CommonBase( long storage_type, 
                const CLID &class_type, 
                ISvcLocator *svc = NULL );

    /// Destructor
    virtual ~CommonBase( );

  public:

    /** Initialization of the algorithm after creation
     *
     * @return The status of the initialization
     * @retval StatusCode::SUCCESS Initialization was successful
     * @retval StatusCode::FAILURE Initialization failed
     */
    virtual StatusCode initialize();

    /** Finalization of the algorithm before deletion
     *
     * @return The status of the finalization
     * @retval StatusCode::SUCCESS Finalization was successful
     * @retval StatusCode::FAILURE Finalization failed
     */
    virtual StatusCode finalize();

  public:

    /** Returns the full location of the given object in the Data Store
     *
     *  @param pObj Data object
     *
     *  @return Location of given data object
     */
    inline std::string objectLocation( const DataObject * pObj ) const
    {
      return ( !pObj ? "Null DataObject !" :
               (pObj->registry() ? pObj->registry()->identifier() : "UnRegistered") );
    }

    /** Returns a vector with available Rich detectors
     *
     *  @param pObj Data object
     *
     *  @return std::vector<DeRich> with Rich detectors
     */
    inline const std::vector<DeRich*>& deRichDetectors( ) const
    {
      return ( this->richDetectorTool()->deRichDetectors() );
    }

    /** @brief Returns a pointer to the tool associated to a given nickname.
     *
     *  Optionally also allows a particular instance name to be given.
     *
     *  Uses the RichToolRegistry tool to convert tool nicknames
     *  in the appropriate class name.
     *
     *  @param nickName   The nickname of the requested tool
     *  @param iName      The instance name of the requested tool
     *  @param pTool      Returned pointer to the requested tool
     *  @param parent     Pointer to parent (used to access private tools)
     *  @param commonTool Flags if this tool should be acquired as common (true) or
     *                    via the tool registry as a context specifc tool (false)
     *
     *  @return Pointer to the tool associated to the given nickname
     */
    template <typename TOOL>
    inline const TOOL* acquireTool( const std::string & nickName,
                                    const std::string & iName,
                                    const TOOL*& pTool,
                                    const IInterface * parent = nullptr,
                                    const bool commonTool = false ) const
    {
      // Check consistency
      if ( parent && commonTool )
      {
        this -> Error( "Tool " + nickName + " cannot be common and private !" ).ignore();
        return nullptr;
      }

      // Construct name
      const std::string fullname =
        ( commonTool || parent ? iName : this->toolRegistry()->toolName(iName) );

      // If not private tool - Check Context and OutputLevel option
      if ( !parent )
      {
        if ( ! this -> setProperties(this->toolRegistry()->toolName(iName)) ) { return nullptr; }
      }

      // get tool
      pTool =
        this -> template tool<TOOL>( this->toolRegistry()->toolType(nickName),
                                     fullname,
                                     parent );
      if ( !pTool )
      {
        this->Exception( "Null Pointer returned by ToolSvc for "+fullname );
      }
      else if ( this -> msgLevel(MSG::DEBUG) )
      {
        this -> debug() << " Acquired tool '" << pTool->name()
                        << "' of type '" 
                        << this->toolRegistry()->toolType(nickName) << "'" << endmsg;
      }

      // return the tool pointer
      return pTool;
    }

    /** @brief Returns a pointer to the tool associated to a given nickname.
     *
     *  Uses the RichToolRegistry tool to convert tool nicknames
     *  in the appropriate class name.
     *
     *  @param nickName   The nickname of the requested tool
     *  @param pTool      Returned pointer to the requested tool
     *  @param parent     Pointer to parent (used to access private tools)
     *  @param commonTool Flags if this tool should be acquired as common (true) or
     *                    via the tool registry as a context specifc tool (false)
     *
     *  @return Pointer to the tool associated to the given nickname
     */
    template <typename TOOL>
    inline const TOOL* acquireTool( const std::string & nickName,
                                    const TOOL*& pTool,
                                    const IInterface * parent = nullptr,
                                    const bool commonTool = false ) const
    {
      return this -> acquireTool( nickName, nickName, pTool, parent, commonTool );
    }

    /** @brief Forced release of a particular tool
     *
     *  Tools are automatically released during finalisation, so this method
     *  only need be used to release a tool early, before finalisation.
     *
     *  @param pTool  Pointer to the tool to be released
     */
    template <typename TOOL>
    inline void releaseTool( TOOL *& pTool ) const
    {
      if ( pTool )
      {
        if ( this -> msgLevel(MSG::DEBUG) )
        {
          this -> debug() << " Forced release for tool '" << pTool->name() << "'" << endmsg;
        }
        this -> release( pTool );
        pTool = nullptr;
      }
      else
      {
        this -> Warning ( "Attempt to release a NULL Tool pointer" );
      }
    }

  protected: // methods

    /** Returns pointer to RICH tool registry tool
     *  Used internally by base class to convert tool nicknames
     *  in the appropriate class name
     *
     *  @return Pointer to the IRichToolRegistry interface
     */
    inline const Rich::IToolRegistry * toolRegistry() const
    {
      if ( !m_toolReg )
      {
        m_toolReg =
          this -> template tool < IToolRegistry > ( "Rich::ToolRegistry", m_regName );
      }
      return m_toolReg;
    }

    /** Returns pointer to RICH detector tool
     *  
     *
     *  @return Pointer to the IRichDetectorTool interface
     */
    inline const Rich::IDetectorTool * richDetectorTool() const
    {
      if ( !m_deRichTool )
      {
        m_deRichTool =
          this -> template tool < IDetectorTool > ( "Rich::DetectorTool", "RichDetectorTool" );
      }
      return m_deRichTool;
    }

    /// Pointer to Job Options Service
    IJobOptionsSvc* joSvc() const;

    /// String matching on context()
    bool contextContains( const std::string & str ) const;

    /** Generate a context specific TES location for the given Default location
     *  @param loc The Default TES location
     *  @return The context specific TES location for the input location
     */
    std::string contextSpecificTES( const std::string & loc ) const;

    /** Propagate a list oj job options from one object to another
     *  @param from_name The name of the object to get the options from
     *  @param to_name   The name of the oject to copy the options to
     *  @param options   List of options to copy. If empty, all options are copied.
     *  @param overwrite If true, options will be over-written in the target object 
     *                   if they are already set
     *  @return StatusCode indicating if the options where correctly copied
     */
    StatusCode 
    propagateJobOptions( const std::string & from_name,
                         const std::string & to_name,
                         const std::vector<std::string> & options = std::vector<std::string>(),
                         const bool overwrite = false ) const;
    
  private: // private methods

    /** Finds the propert object of a given name, for the given component name
     *
     *   @param name Component name
     *   @param property Property name
     *
     *   @return Pointer to the Property object if it exists
     */
    template < class PROPERTYTYPE >
    const PROPERTYTYPE * my_getProperty( const std::string & name,
                                         const std::string & property ) const;

    /** @brief Set the given Property for given public tool
     *
     *  Set the Property for public tools that do not explicitly 
     *  have it set. Uses the same settings as for the tool registry.
     *
     *  @param name     Tool name
     *  @param property The property name
     *
     *  @return Boolean indicating if setting was successful or not
     *  @retval TRUE  Setting was successful
     *  @retval FALSE Setting failed
     */
    template < class PROPERTYTYPE >
    bool my_setToolProperty( const std::string & name,
                             const std::string & property ) const;
    
    /** @brief Set the properties for given public tool
     *
     *  Private solution to the problem that properties like "Context" 
     *  are not set for public tools. This solution uses the properties as 
     *  defined by the Tool registry to set them for all public tools.
     *
     *  @param name Tool name
     *
     *  @return Boolean indicating if setting was successful or not
     *  @retval TRUE  Setting was successful
     *  @retval FALSE Setting failed
     */
    bool setProperties( const std::string & name ) const;

  private:

    /// Common Constructor initisalisations
    void initRichCommonConstructor();
    
  private: // data

    /// Pointer to tool registry
    mutable const IToolRegistry * m_toolReg = nullptr;

    /// Pointer to job options service
    mutable IJobOptionsSvc * m_jos = nullptr;

    /// Runtime name for RichToolRegistry
    std::string m_regName;

    /// Pointer to detector too for DeRich objects
    mutable const IDetectorTool * m_deRichTool = nullptr;
    
  };

}

#endif // RICHKERNEL_RICHCOMMONBASE_H
