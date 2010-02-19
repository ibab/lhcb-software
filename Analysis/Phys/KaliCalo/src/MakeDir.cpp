// $Id: MakeDir.cpp,v 1.1 2010-02-19 12:03:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  class MakeDir : public GaudiAlgorithm
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<Kali::MakeDir> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method 
    virtual StatusCode execute()              // the only one essential method 
    {
      //
      for ( std::vector<std::string>::const_iterator item = 
              m_locations.begin() ; m_locations.end() != item ; ++item ) 
      { put ( new DataObject() , *item ) ; }
      //
      return StatusCode::SUCCESS ;
    } 
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param psvc pointer to Service Locator 
     */
    MakeDir  ( const std::string& name ,  // algorithm instance name 
               ISvcLocator*       pSvc )  // poiunetr to Service locator
      : GaudiAlgorithm ( name , pSvc ) 
      , m_locations () 
    {
      //
      m_locations.push_back ( "Rec/ProtoP" ) ;
      m_locations.push_back ( "Rec/Calo"   ) ;
      //
      declareProperty 
        ( "Locations" , 
          m_locations , 
          "The list of locations to be created" ) ;
    }
    /// virtual & protected destructor 
    virtual ~MakeDir() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    MakeDir () ;                         // the default constructor is disabled 
    /// the copy constructor is disabled 
    MakeDir ( const MakeDir& ) ;            // the copy constructor is disabled 
    /// the assignement operato is disabled 
    MakeDir& operator=( const MakeDir& ) ;       // the assignement is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// The list of locations to be created
    std::vector<std::string> m_locations ; // List of locations to be created
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace Kali 
// ============================================================================
// The Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,MakeDir)
// ============================================================================
// The END 
// ============================================================================


