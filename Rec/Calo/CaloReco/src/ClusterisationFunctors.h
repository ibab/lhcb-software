// $Id: ClusterisationFunctors.h,v 1.4 2005-03-07 15:37:15 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $
// ===========================================================================
#ifndef CALOALGS_CLUSTERISATIONFUNCTORS_H 
#define CALOALGS_CLUSTERISATIONFUNCTORS_H 1
// ===========================================================================

/** @namespace ClusterisationFunctors ClusterisationFunctors.h 
 *  
 *  collectioon of several useful functors for clusterisation
 *  algorithms  
 *
 *  @author Ivan Belyaev
 *  @date   07/07/2001
 */

// ===========================================================================
/**  pure technical method
 *   (just a copy of std::transform with
 *   a minor "improvement")
 */
template <class InputIterator, class OutputIterator, class UnaryOperation>
inline OutputIterator 
transform_ref(InputIterator   first  , 
              InputIterator   last   ,
              OutputIterator  result , 
              UnaryOperation& op     );
// ===========================================================================

namespace ClusterisationFunctors 
{
  /** @class Is_A_LocalMaximum
   *
   * The "main" functor - desides if given digit is a local maximum or not 
   * 
   * DET is any class which provides us with method
   *      const CaloNeighbours& neighborCells( const CaloCellID& ) 
   * DIGITS is any class with implemented method  
   *     CaloDigit* operator() ( const CaloCellID& )  
   *  (random/direct access to CaloDigit* via CaloCellID index  ) 
   */
  template <class DET, class DIGITS>
  class Is_A_Local_Maximum 
    : public std::unary_function< const CaloDigit* , bool>
  {
  public:
    
    /** standard constructor 
     *  @param det    pointer to detector information
     *  @param digits random access to digits via CaloCellID index 
     */ 
    Is_A_Local_Maximum( const DET*    det    , 
                        const DIGITS& digits ) 
      : m_det    ( det     ) 
      , m_digits ( &digcol ){};  
    // destructor 
    Is_A_Local_Maximum(){};
    
    /** the only main and essential method 
     *  @param  digit   poinetr to CaloDigit* object 
     *  @return "true" if given digits if a local maximum
     */
    inline bool operator() ( const CaloDigit* digit ) const 
    {
      // NULL pointer is never local maximum!
      if( 0 == digit ) { return false; }  	  
      // vector of neighbors cell IDs 
      const CaloNeighbors* CellIDs = 
        &m_det->neighborCells( digit->cellID() );
      //  no neighbors found 
      if( CellIDs->empty() ) { return false;} 
      // transform neighbour cell IDs  container into container of digits 
      typedef std::vector<const CaloDigit*> DigSeq;
      DigSeq   cells( CellIDs->size() , 0 );
      transform_ref ( CellIDs->begin() ,
                      CellIDs->end  () , 
                      cells.begin   () ,
                      *m_digits        );  
      // try to find the neighbour with larger energy 
      CaloDataFunctor::Less_by_Energy<CaloDigit*> Cmp();
      DigSeq::const_iterator it = 
        std::find_if( cells.begin() , 
                      cells.end  () ,
                      std::not1( std::bind2nd( Cmp, digit ) ) ); 
      // is greater element found or not?
      return ( ( cells.end() == it ) ? true : false );  
    };
    ///      
  private:  
    /// source of detector information 
    const DET*    m_det    ;    
    /// random aceess to digits via CaloCellID 
    const DIGITS* m_digits ;    
    ///
  };
  
  
  /** @class SeedFinder 
   *
   * The "sophisticated" functor 
   *   if the digit is a local maximum, 
   *             it creates the CaloCluster object and fill it 
   *   if the digit is not a local maximum, return NULL pointer 
   * 
   * DET is any class with provides with 
   *       const CaloNeighbours& neighborCells( const CaloCellID& ) 
   * DIGITS is any class with implemented 
   *       CaloDigit* operator() ( const CaloCellID& )  
   *     (random/direct access to CaloDigit* via CaloCellID index  ) 
   *
   *  @author Ivan Belyaev
   *  @date   07/07/2001
   */
  template <class DET, class DIGITS>
  class SeedFinder:
    public std::unary_function<const CaloDigit*, CaloCluster*>
  {
  ///
  public:
    ///
    /** standard constructor 
     *  @param det    pointer to detector information
     *  @param digcol direct/random access to digits via cellID index
     */
    SeedFinder( const DET* det    , 
                DIGITS&    digcol ) 
      : m_det   ( det     ) 
    , m_digits( &digcol ){}
    // destructor 
    ~SeedFinder(){};
    
    /** the only one and essential method 
     *  @param  digit    pointer to calodigit object 
     *  @return cluster, if digit is a SEED,   0 - otherwise
     */
    inline CaloCluster*  operator() ( const CaloDigit* digit ) const
    {
      // avoid long names 
      using namespace CaloDataFunctor ;
      using namespace CaloDigitStatus ;
      // NULL pointer is never local maximum!
      if( 0 == digit )                        { return 0; } ///< RETURN
      // vector of neighbors cell IDs 
      const CaloNeighbors* CellIDs = 
        &m_det->neighborCells( digit->cellID() );
      if( CellIDs->empty() )                  { return 0; } ///< RETURN
      // transform neighbour cell IDs  container into container of digits 
      typedef std::vector<const CaloDigit*> DigSeq;
      DigSeq cells( CellIDs->size() , 0 ) ;
      transform_ref ( CellIDs->begin()  , 
                      CellIDs->end  ()  , 
                      cells.begin   ()  ,
                      *m_digits         ); 
      // try to find the neighbour with larger energy 
      Less_by_Energy<const CaloDigit*> Less;
      for( DigSeq::const_iterator it = cells.begin(); 
           cells.end() != it ; ++it ) 
        { if( Less( digit , *it ) ) { return 0 ; } }
      // 
      // DigSeq::const_iterator it = 
      // std::find_if( cells.begin () , 
      //              cells.end   () , 
      //              std::not1   ( std::bind2nd ( Less , digit ) ) ) ;
      // this digit is NOT local maximum 
      // if( cells.end() != it )                 { return 0; } /// < RETURN!
      // this digit IS a local maximum! 
      CaloCluster* cluster = new CaloCluster();
      // add this digit into cluster with status = SeedCell  
      typedef CaloCluster::Entry   Entry;
      cluster->entries().push_back( Entry( digit , 
                                           ( SeedCell         | 
                                             LocalMaximum     |  
                                             UseForEnergy     |  
                                             UseForPosition   | 
                                             UseForCovariance ) ) ) ;
      // add all other digits into cluster with "undefined" status  
      for( DigSeq::const_iterator iter = cells.begin();
           cells.end() != iter ; ++ iter) 
        { 
          cluster->entries().push_back( Entry( *iter , 
                                               ( Undefined        | 
                                                 UseForEnergy     |  
                                                 UseForPosition   | 
                                                 UseForCovariance ) ) ) ; }
      // set cluster status 	  
      cluster->setStatus( 1 ) ;  
      // return cluster 
      return cluster;
      //
    };
    ///
  private:
    
    // detector information about neighbouring cells
    const   DET*    m_det     ;    
    // random access to digits
    mutable DIGITS* m_digits  ;    
    
  };
  

}; ///< end of name space ClusterisationFunctors


// ===========================================================================
/*  pure technical method
 * (just a copy of std::transform with
 *   a minor "improvement")
 */
template <class InputIterator, class OutputIterator, class UnaryOperation>
inline OutputIterator transform_ref(InputIterator   first  , 
                                    InputIterator   last   ,
                                    OutputIterator  result , 
                                    UnaryOperation& op     ) ///< "&" added!
{
  for ( ; first != last; ++first, ++result) { *result = op(*first); }
  return result;
};
// ===========================================================================

// ===========================================================================
#endif ///< CALOALGS_CLUSTERISATIONFUNCTORS_H
// ===========================================================================
