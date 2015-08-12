// ============================================================================
#include "CaloSelectClusterWithSpd.h"
// ============================================================================
/** @class CaloSelectChargedClusterWithSpd
 *  Simple seleclton of newural clusters based on Spd information 
 *  @author Olivier Deschamps 
 *  @author Vanya BELYAEV 
 */
// ============================================================================

class CaloSelectChargedClusterWithSpd : public CaloSelectClusterWithSpd 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectChargedClusterWithSpd>;
  // ==========================================================================
public:
  // ==========================================================================
  virtual bool select ( const LHCb::CaloCluster* cluster ) const
  { return (*this) ( cluster ) ; }
  // ==========================================================================
  virtual bool operator()( const LHCb::CaloCluster* cluster ) const 
  {
    if ( 0 == cluster ) 
    {
      Warning ( "CaloCluster* points to NULL, return false" );  
      return false ;                                                  // RETURN 
    }
    //
    bool sel = cut() < n_hit ( *cluster ) ;
    counter("selected clusters") += (int) sel;
    return sel;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// constructor 
  CaloSelectChargedClusterWithSpd
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent )
    : CaloSelectClusterWithSpd ( type , name , parent )
  {}
  /// virtual destructor 
  virtual ~CaloSelectChargedClusterWithSpd() {}
  // ==========================================================================
private:  
  // ==========================================================================
  /// the default constructor is disabled 
  CaloSelectChargedClusterWithSpd(); // the default constructor is disabled 
  /// the copy constructor is disabled 
  CaloSelectChargedClusterWithSpd
  ( const CaloSelectChargedClusterWithSpd& ); 
  /// the assignement constructor is disabled 
  CaloSelectChargedClusterWithSpd& 
  operator=(const CaloSelectChargedClusterWithSpd& );
  // ==========================================================================
};
// ============================================================================
DECLARE_TOOL_FACTORY( CaloSelectChargedClusterWithSpd )
// ============================================================================
