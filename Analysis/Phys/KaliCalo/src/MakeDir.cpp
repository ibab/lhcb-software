// $Id$
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
// Local
// ============================================================================
#include "Kali.h"
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
              m_locations1.begin() ; m_locations1.end() != item ; ++item ) 
      {
        if ( exist<DataObject>( Kali::kalify ( *item ) ) ) 
        { put ( new DataObject() , *item ) ; }
      }
      //
      for ( std::vector<std::string>::const_iterator item = 
              m_locations2.begin() ; m_locations2.end() != item ; ++item ) 
      {
        put ( new DataObject() , *item ) ; 
      }
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
      , m_locations1 () 
      , m_locations2 () 
    {
      //
      m_locations1.push_back ( "/Event/Rec/Track"  ) ;
      m_locations1.push_back ( "/Event/Raw"        ) ;
      m_locations1.push_back ( "/Event/Raw/Spd"    ) ;
      m_locations1.push_back ( "/Event/Raw/Prs"    ) ;
      m_locations1.push_back ( "/Event/Raw/Ecal"   ) ;
      m_locations1.push_back ( "/Event/Raw/Hcal"   ) ;
      //
      m_locations2.push_back ( "/Event/Rec/ProtoP" ) ;
      m_locations2.push_back ( "/Event/Rec/Calo"   ) ;
      //
      declareProperty 
        ( "Locations1" , 
          m_locations1  , 
          "The list of locations to be created" ) ;
      declareProperty 
        ( "Locations2" , 
          m_locations2  , 
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
    std::vector<std::string> m_locations1 ; // List of locations to be created
    std::vector<std::string> m_locations2 ; // List of locations to be created
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


