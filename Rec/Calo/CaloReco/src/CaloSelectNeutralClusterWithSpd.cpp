// ============================================================================
// Local 
// ============================================================================
#include "CaloSelectClusterWithSpd.h"
// ============================================================================
/** @class CaloSelectNsutralClusterWithSpd
 *  Simple seleclton of newural clusters based on Spd information 
 *  @author Olivier Deschamps 
 *  @author Vanya BELYAEV 
 */
// ============================================================================
class CaloSelectNeutralClusterWithSpd : public CaloSelectClusterWithSpd 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectNeutralClusterWithSpd>;
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

  bool sel = cut() >= n_hit ( *cluster ) ;
  counter("selected clusters") += (int) sel;
  return sel;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// constructor 
  CaloSelectNeutralClusterWithSpd
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent )
    : CaloSelectClusterWithSpd ( type , name , parent )
  {}
  /// virtual destructor 
  virtual ~CaloSelectNeutralClusterWithSpd() {}
  // ==========================================================================
private:  
  // ==========================================================================
  /// the default constructor is disabled 
  CaloSelectNeutralClusterWithSpd(); // the default constructor is disabled 
  /// the copy constructor is disabled 
  CaloSelectNeutralClusterWithSpd
  ( const CaloSelectNeutralClusterWithSpd& ); 
  /// the assignement constructor is disabled 
  CaloSelectNeutralClusterWithSpd& 
  operator=(const CaloSelectNeutralClusterWithSpd& );
  // ==========================================================================
};
// ============================================================================
DECLARE_TOOL_FACTORY( CaloSelectNeutralClusterWithSpd )
// ============================================================================
