// $Id: ClusterFunctors.h,v 1.1.1.1 2001-11-02 14:39:53 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/10/28 19:14:09  ibelyaev
// update for newer CaloEvent package
//
// Revision 1.4  2001/07/17 20:00:48  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.3  2001/07/12 21:54:22  ibelyaev
// adaptation for Win2K
//
// Revision 1.2  2001/07/07 12:58:59  ibelyaev
// new clusterisation algorithm
//
// Revision 1.1  2001/07/04 18:15:53  ibelyaev
// Shared cell algorimth implementation
//
// ============================================================================
#ifndef CALOUTILS_CLUSTERFUNCTORS_H
#define CALOUTILS_CLUSTERFUNCTORS_H 1
// ============================================================================
// Include files
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloDataFunctor.h"

class DeCalorimeter;

/** @namespace ClusterFunctors ClusterFunctors.h CaloUtils/ClusterFunctors.h
 *
 *  collection of useful functors for dealing with
 *  CaloCluster objects
 *
 *  @author Ivan Belyaev
 *  @date   04/07/2001
 */

namespace ClusterFunctors
{

  typedef CaloCluster::Digits::iterator            iterator            ;
  typedef CaloCluster::Digits::const_iterator      const_iterator      ;
  typedef std::pair<iterator,iterator>             iterator_pair       ;
  typedef std::pair<const_iterator,const_iterator> const_iterator_pair ;


  /**  Calculate the "energy" of the cluster as a sum of
       energies of its digits, weighted with energy fractions
       @param   cl  pointer to cluster
       @return      "energy" of cluster
  */
  double  energy( const CaloCluster* cl );

  /**  Calculate the "energy" of the cluster as a sum of
   *   energies of its digits, weighted with energy fractions
   *
   *   "IT" could be either iterator or const_iterator
   *
   *   @param    begin iterator to first element of sequence
   *   @param    end   iterator to last+1 element of sequance
   *   @return      "energy" of sequence
   */
  template <class IT>
  inline  double  energy( IT begin , IT end ) 
  { 
    return CaloDataFunctor::clusterEnergy( begin, end ); 
  }

  /**  Calculate the "energy", X and Y position
   *   of the cluster as a sum of energies/x/y of 
   *   its digits, weighted with energy fractions
   *   @param   cl  pointer to cluster object
   *   @param   de  pointer to DeCalorimeter object
   *   @param   e   energy
   *   @param   x   x-position
   *   @param   y   y-position
   *   @return    status code
   */
  StatusCode  calculateEXY( const  CaloCluster*   cl ,
                            const  DeCalorimeter* de ,
                            double& e                ,
                            double& x                ,
                            double& y                );
  
  /**  Calculate the "energy", X and Y position
   *   of the cluster as a sum of
   *   energies/x/y of its digits,
   *   weighted with energy fractions
   *
   *   "IT" could be either iterator or const_iterator
   *
   *   @param   begin iterator to first element of sequence
   *   @param   end   iterator to last+1 element of sequance
   *   @param   de  pointer to DeCalorimeter object
   *   @param   e   energy
   *   @param   x   x-position
   *   @param   y   y-position
   *   @return    status code
   */
  template <class IT, class DE>
  inline StatusCode  calculateEXY( IT begin  , IT end    , DE de        ,
                                   double& e , double& x , double&   y  )
  {
    return CaloDataFunctor::calculateClusterEXY( begin , 
                                                 end   , 
                                                 de    , 
                                                 e     , 
                                                 x     , 
                                                 y     ) ;
  };
  
  /**  useful function to determine, if clusters have
   *   at least one common cell.
   *
   *   For invalid arguments return "false"
   *
   *   @param   cl1   pointer to first  cluster
   *   @param   cl2   pointer to second cluster
   *   @return "true" if clusters have at least 1 common cell
   */
  bool overlapped( const CaloCluster* cl1 ,
                   const CaloCluster* cl2 ) ;

  /**  useful function to find first common digit
   *   from two sequences. It returns the pair of
   *   iterators (first one for first sequence and
   *   the secons one for second sequences).
   *
   *   "IT" could be either iterator or const_iterator
   *
   *   @param  begin1 iterator pointing to 1st    element of 1st sequence
   *   @param  end1   iterator pointing to last+1 element of 1st sequence
   *   @param  begin2 iterator pointing to 1st    element of 2nd sequence
   *   @param  end2   iterator pointing to last+1 element of 2nd sequence
   *   @return pair of iterators
   */
  template<class IT>
  inline std::pair<IT,IT> commonDigit( IT begin1 , IT end1 ,
                                       IT begin2 , IT end2 )
  {
    return CaloDataFunctor::clusterCommonDigit( begin1 , 
                                                end1   , 
                                                begin2 , 
                                                end2   );
  };
  
  /** useful function to locate the digit within the sequence
   *
   *   "IT" could be either iterator or const_iterator
   *
   *   @param begin iterator pointing to the 1st    element of sequence
   *   @param end   iterator pointing to teh last+1 element of sequence
   *   @param digit pointer to CaloDigit
   *   @return location of digit within the sequence
   *
   */
  template<class IT>
  inline IT locateDigit( IT begin   ,   IT end  ,
                         const CaloDigit* digit )
  {
    return CaloDataFunctor::clusterLocateDigit( begin , end , digit );
  };

  /** useful function to locate the digit with given status
   *  from sequence of digits
   *
   *   "IT" could be either iterator or const_iterator
   *
   *   @param begin  iterator pointing to the 1st    element of sequence
   *   @param end    iterator pointing to teh last+1 element of sequence
   *   @param st     status
   *   @return location of digit within the sequence
   */
  template <class IT>
  inline IT locateDigit( IT begin  , IT end            ,
                         const DigitStatus::Status& st )
  {
    return CaloDataFunctor::clusterLocateDigit( begin, end , st );
  };

}; /// end of name space


// ============================================================================
#endif ///< CALOUTILS_CLUSTERFUNCTORS_H
// ============================================================================
