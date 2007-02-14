// $Id: HltSequences.h,v 1.3 2007-02-14 17:23:51 cattanem Exp $
#ifndef HLTBASE_HLTSEQUENCES_H 
#define HLTBASE_HLTSEQUENCES_H 1

#include "HltBase/ESequences.h"
#include "HltBase/EFunctions.h"

/** @file
 *  Common HLT sequences: operation to be perform in containers
 *
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

/// Namespace of Hlt functions, filters and sequences
namespace Hlt {

  /* map: 
   *      apply a function to all objects in a container,
   *      store the results into the object with an integer key
   *      if the object has the key before the function will not be applied
   *
   */
  template <class INPUT, class FUNCTION> 
  inline  void map(const INPUT& ci, 
                   const FUNCTION& fun, 
                   int key) { 
    Estd::map(ci.begin(),ci.end(),fun,key);
  }  

  /* select: 
   *      apply a filter to the objects in a container,
   *      those which pass the filter will be stored in the output container
   *
   */
  template <class INPUT, class FILTER, class OUTPUT> 
    void select(const INPUT& ci, 
                const FILTER& filter, 
                OUTPUT& co) {
    Estd::select(ci.begin(),ci.end(),filter,std::back_inserter(co));
  }

  /* map_first: 
   *      apply a bifunction to the objects in a container,
   *      the second argument of the bifunction is passed as an argument
   *      the result it is stored inside the object with a given key
   *    i.e: INPUT: container of tracks
   *         ARG:   a vertex
   *         FUN:   impact Parameter function (track, vertex)
   *         Key:   IP key
   */
  template <class INPUT, class FUNCTION, class ARG> 
  inline  void map_first(INPUT& ci1, 
                         const ARG& o2, 
                         const FUNCTION& fun, 
                         int key) {
    Estd::map_first(o2,ci1.begin(),ci1.end(),fun,key);
  }      


  /* map_min_abs_value:
   *    located in the objects of the first container the abs minimum value
   *    of the function applied over the second container. Key1 is the key
   *    for the computed value, and key2 the key to indicate the position 
   *    of the object of the second container which gives the abs min value.
   *   i.e:  INPUT: list of tracks
   *         INPUT2: list of vertices
   *         FUNCTION: impact parameter (track,vertex)
   *         key1: key to indicate the IP
   *         key2: key to indicate the vertex associated to the track
   *       In this example in the list of tracks will be store the min impact
   *       parameter with respect any vertex with the key1, and in the key2 it
   *       will be stored the index of the vertex with to which the tracks has
   *       its smallest IP.
   *
   */
  template <class INPUT, class INPUT2, class FUNCTION> 
    inline  void map_abs_min_value(INPUT& ci, 
				   const INPUT2& ci2,
				   const FUNCTION& fun, 
				   int key1, 
				   int key2 = -2) {
    Estd::map_compare_value(ci.begin(),ci.end(),ci2.begin(),ci2.end(),
			    fun,Estd::abs_min(),key1,key2);
  }
   

};
#endif // HLTBASE_HLTSEQUENCES_H
