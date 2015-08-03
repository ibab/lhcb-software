// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DataObject.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  /** @class DataRemove
   *  Trivial algorithm to remove data in the TES
   *  @author Chris Jones Christopher.Rob.Jones@cern.ch
   *  @date 2012-11-08
   */
  // ==========================================================================
  class DataRemove : public GaudiAlgorithm
  {
    // ========================================================================
    friend class AlgFactory<Gaudi::DataRemove> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method 
    virtual StatusCode execute () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    DataRemove ( const std::string& name ,
                 ISvcLocator*       pSvc ) ;
    /// virtual destructor 
    virtual ~DataRemove() ;              
    // ========================================================================
  private:
    std::string m_dataLocation;
    // ========================================================================
  } ; // end of class DataRemove
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
Gaudi::DataRemove::DataRemove( const std::string& name ,
                               ISvcLocator*       pSvc ) 
  : GaudiAlgorithm ( name , pSvc )
{
  declareProperty( "DataLocation", m_dataLocation = "" );
}
// ============================================================================
// virtual & protected desctructor 
// ============================================================================
Gaudi::DataRemove::~DataRemove() { }
// ========================================================================
// the main method 
// ========================================================================
StatusCode Gaudi::DataRemove::execute() 
{ 
  if ( !m_dataLocation.empty() )
  {
    DataObject * data = getIfExists<DataObject>( m_dataLocation );
    if ( data )
    {
      const StatusCode sc = evtSvc()->unregisterObject( data );
      if ( sc.isSuccess() )
      {
        delete data;
        data = NULL;
      }
      else
      {
        return Error( "Failed to delete input data " + m_dataLocation, sc );
      }
    }
  }
  return StatusCode::SUCCESS;  
}
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Gaudi,DataRemove)
// ============================================================================
// The END 
// ============================================================================
