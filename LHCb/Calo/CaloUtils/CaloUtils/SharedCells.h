// $Id: SharedCells.h,v 1.7 2007-08-24 21:28:04 odescham Exp $ 
// =========================================================================== 
#ifndef CALOUTILS_SHAREDCELLS_H 
#define CALOUTILS_SHAREDCELLS_H 1
/// =========================================================================== 
#include <cmath>
// Include files
#include "GaudiKernel/Point3DTypes.h"

/** @namespace SharedCells SharedCells.h CaloUtils/SharedCells.h
 *
 *  set of inline functions used for CaloSharedCellAlg 
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   04/07/2001
 */

  
namespace SharedCells
{

  //  =========================================================================
  /** Redistribute the energy proportional to a given weights 
   *  @param   List    a "list" of clusters for given cell 
   *  @param   Weight  "list"  of weights  
   *  @return  status code 
   */
  template <class L, class W>
  inline StatusCode redistribute( L& List , const W& Weight )
  {
    ///
    if( List.size() != Weight.size() ) { return StatusCode::FAILURE; }
    /// total weight 
    double wTot = std::accumulate( Weight.begin() , Weight.end() , 0.0 );
    /// redistribute the energy
    for( typename L::iterator iC = List.begin() ; List.end() != iC ; ++iC )
      {
        const LHCb::CaloCluster* cluster = iC->first ;
        /// ignore artificial zeros
        if( 0 == cluster ) { continue ; }     ///< CONTINUE !!!
        const unsigned index = iC - List.begin() ;
        const double   frac  = Weight[index] / wTot ;
        iC->second->setFraction( frac );
      }  
    ///
    return StatusCode::SUCCESS;
  };
  //  =========================================================================
  
  //  =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to a total(summed)
   *  energy of clusters. 
   *  the method is iterative(recursive)! 
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *  @param   List  a "list" of clusters for given cell 
   *  @param   NiT   number of iterations 
   *  @return  status code 
   */
  template <class L>
  inline StatusCode summedEnergyAlgorithm( L& List , int NiT )
  {
    /// does one need more iterations? 
    if( 0 >= NiT         ) { return StatusCode::SUCCESS ; }  
    /// empty cluster list ( nothing to be done)
    if( 0 == List.size() ) { return StatusCode::SUCCESS ; }
    ///
    /// prepare the container of weights 
    std::vector<double>  weight( List.size() , 1.0 );
    /// loop over all clusters and calculate the weight for each cluster  
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const LHCb::CaloCluster* cluster = iC->first ;
        /// ignore artificial zeros 
        if( 0 == cluster ) { continue ; }     ///< CONTINUE !!!
        const unsigned index = iC - List.begin() ;
        /// calculate the energy of each cluster
        const double   eClu  = 
          LHCb::ClusterFunctors::energy( cluster->entries().begin() ,
                                   cluster->entries().end  () ) ;
        /// define the weight for this cluster 
        weight[index]  = eClu ;
      }
    /// make redistribution of energy
    StatusCode sc = SharedCells::redistribute( List , weight );
    if(!sc.isSuccess())return sc;
    /// make iteration  
    return SharedCells::summedEnergyAlgorithm( List , --NiT  ) ;  
  };
  //  =========================================================================
  
  //  =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *
   *  Error codes:
   *   - 220  - no digit of given type is found !
   *   - 221  - digit pointer equals to NULL!
   * 
   *  @param   List   a "list" of clusters for given cell 
   *  @param   type   "type" of seed cell 
   *  @return  status code 
   */
  template <class L, class T>
  inline StatusCode seedEnergyAlgorithm( L& List , const T& type )
  {
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    ///< collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const LHCb::CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }  
        /// locate (first) seed cell 
        LHCb::CaloCluster::Entries::const_iterator iSeed =
          LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                        cluster->entries().end  () ,
                                        type                      );
        /// success ???
        if( cluster->entries().end() == iSeed ) { return StatusCode(220) ; }
        const LHCb::CaloDigit* seed = iSeed->digit() ;
        if( 0 == seed )                        { return StatusCode(221) ; }
        /// get the energy of the seed cell 
        const double     e  = seed->e() ;
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        weight[index]  = e  ;
      }
    /// redistribute the  energy  
    return SharedCells::redistribute( List , weight );
  };
  //  =========================================================================
  
  //  =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  weighted with distance function between center of the seed cell and 
   *  the given cell center
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *
   *  Error codes:
   *   - 220  - no digit of given type is found !
   *   - 221  - digit pointer equals to NULL!
   *   - 222  - detector information is invalid (no detector)
   *
   *  @param   List   a "list" of clusters for given cell 
   *  @param   Det    source of geometry information 
   *  @param   type   "type" of seed cell 
   *  @param   ID     cell ID of given cell 
   *  @param   size   vector of shower size measures 
   *  @return  status code 
   */
  template <class L, class DE, class T, class SIZE>
  inline StatusCode seedDistanceAlgorithm( L&                List , 
                                           const DE&         Det  ,
                                           const T&          type ,
                                           const LHCb::CaloCellID& ID   ,
                                           const SIZE&       size )
  {
    /// valid detector information ??
    if( !Det                 ) { return StatusCode(222)     ; }
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    const Gaudi::XYZPoint position( Det->cellCenter( ID ) );
    /// collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const LHCb::CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }  
        /// locate (first) seed cell 
        LHCb::CaloCluster::Entries::const_iterator iSeed =
          LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                        cluster->entries().end  () ,
                                        type                      );
        /// success ???
        if( cluster->entries().end() == iSeed ) { return StatusCode(220) ; }
        const LHCb::CaloDigit* seed = iSeed->digit() ;
        if( 0 == seed                        ) { return StatusCode(221) ; }
        /// get the energy of the seed cell
        const double     e  = seed->e() ;
        /// calculate the distance between seed cell and give cell 
        const double  dist  = 
          ( position - Det->cellCenter( seed->cellID() ) ).R(); //mag->R
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        /// calculate weight 
        weight[index]  = 
          e * exp( -1.0 * dist / size[ ID.area() ] ) ;
      }
    /// redistribute the energies  
    return SharedCells::redistribute( List , weight );
  };
  //  =========================================================================


  //  =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  weighted with distance function between center of gravity of cluster 
   *  and center of given cell 
   *  method is iterative(recursive) 
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *
   *  Error codes:
   *   - 222  - detector information is invalid (no detector)
   *   - error codes from calculateEXY method 
   *
   *  @param   List   a "list" of clusters for given cell 
   *  @param   Det    source of geometry information 
   *  @param   ID     cell ID of given cell 
   *  @param   size   vector of shower size measures 
   *  @param   NiT    number of iterations 
   *  @return  status code 
   */
  template <class L, class DE, class SIZE>
  inline StatusCode summedDistanceAlgorithm( L&                List , 
                                             const DE&         Det  ,
                                             const LHCb::CaloCellID& ID   ,
                                             const SIZE&       size ,
                                             int               NiT  )
  {
    /// does one need more iterations ?
    if( 0 >= NiT             ) { return StatusCode::SUCCESS ; }
    /// valid detector information ??
    if( !Det                 ) { return StatusCode(222)     ; }
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    const Gaudi::XYZPoint  pos( Det->cellCenter( ID ) );
    /// collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const LHCb::CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }
        double x , y , e ;
        StatusCode sc = 
          LHCb::ClusterFunctors::calculateEXY( cluster->entries().begin() ,
                                         cluster->entries().end  () ,
                                         Det , e , x , y           );
        /// success ???
        if( sc.isFailure() ) { return sc ; }
        /// calculate the distance between seed cell and give cell 
        const double  dist  = 
          sqrt( ( pos.x() - x ) * ( pos.x() - x ) +  
                ( pos.y() - y ) * ( pos.y() - y ) ) ;
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        /// calculate weight 
        weight[index]  = 
          e * exp( -1.0 * dist / size[ ID.area() ] ) ;
      }
    /// redistribute the energies   
    StatusCode sc = SharedCells::redistribute( List , weight );
    if(!sc.isSuccess())return sc;
    /// make iterations 
    return 
      SharedCells::summedDistanceAlgorithm( List, Det , ID , size, --NiT ) ;
  };
  //  =========================================================================


}; ///< end of namespace 
 


/// =========================================================================== 
#endif // COMPONENT_SHAREDCELLS_H
/// =========================================================================== 
