
//--------------------------------------------------------------------------------
/** @file RichMap.h
 *
 *  Header file for utility has map for the RICH : Rich::HashMap
 *
 *  CVS Log :-
 *  $Id: RichMap.h,v 1.1 2005-01-13 12:21:18 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMAP_H
#define RICHKERNEL_RICHMAP_H 1

// STL
#include <map>

/** @class RichMap RichMap.h RichKernel/RichMap.h
 *
 *  A utility class providing a standard map object.
 *  Provides a convenient single point of definition for optimisation studies.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */

template < class KEY, class VALUE >
class RichMap : public std::map < KEY , VALUE > { };

#endif // RICHKERNEL_RICHMAP_H
