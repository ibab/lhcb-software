
//-----------------------------------------------------------------------------
/** @file RichCommonBase.h
 *
 *  Header file for RICH base class : RichCommonBase
 *
 *  CVS Log :-
 *  $Id: RichCommonBase.h,v 1.7 2007-02-01 17:24:54 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-27
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHCOMMONBASE_H
#define RICHKERNEL_RICHCOMMONBASE_H 1

// Interfaces
#include "RichKernel/IRichToolRegistry.h"

// Gaudi
#include "GaudiKernel/IRegistry.h"

// Forward declarations
class IJobOptionsSvc;

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

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

    /// Destructor
    virtual ~CommonBase( );

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
                                    const IInterface * parent = 0,
                                    const bool commonTool = false ) const
    {
      // Check consistency
      if ( parent && commonTool )
      {
        this -> Error( "Tool " + nickName + " cannot be common and private !" );
        return NULL;
      }

      // Construct name
      const std::string fullname =
        ( commonTool || parent ? iName : toolRegistry()->toolName(iName) );

      // If private tool - Check Context option
      if ( !parent )
      {
        if ( !setContext( toolRegistry()->toolName(iName) ) )
        {
          this -> Error( "Problem setting Context for '"+fullname+"'" );
          return NULL;
        }
      }

      // get tool
      pTool =
        this -> template tool<TOOL>( toolRegistry()->toolType(nickName),
                                     fullname,
                                     parent );
      if ( this -> msgLevel(MSG::DEBUG) )
      {
        this -> debug() << " Acquired tool '" << pTool->name()
                        << "' of type '" << toolRegistry()->toolType(nickName) << "'" << endreq;
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
                                    const IInterface * parent = 0,
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
          this -> debug() << " Forced release for tool '" << pTool->name() << "'" << endreq;
        }
        release( pTool );
        pTool = NULL;
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
      if (!m_toolReg)
      {
        m_toolReg =
          this -> template tool < IToolRegistry > ( "Rich::ToolRegistry", m_regName );
      }
      return m_toolReg;
    }

    /// Pointer to Job Options Service
    IJobOptionsSvc* joSvc() const;

  private: // private methods

    /** @brief Set the Context option for given public tool
     *
     *  Private solution to the problem that "Context" is not set for public tools
     *  This solution uses the context as defined by the Tool registry to set the
     *  Context for all public tools.
     *
     *  @param name Tool name
     *
     *  @return Status Code indicating if setting was successful or not
     */
    StatusCode setContext( const std::string & name ) const;

  private: // data

    /// Pointer to tool registry
    mutable const IToolRegistry * m_toolReg;

    /// Pointer to job options service
    mutable IJobOptionsSvc * m_jos;

    /// Runtime name for RichToolRegistry
    std::string m_regName;

  };

}

#endif // RICHKERNEL_RICHCOMMONBASE_H
