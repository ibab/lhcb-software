/** @file IRichTrSegMaker.h
 *
 *  Header file for tool interface : IRichTrSegMaker
 *
 *  CVS Log :-
 *  $Id: IRichTrSegMaker.h,v 1.4 2004-07-26 17:53:17 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2003-10-28
 */

#ifndef RICHKERNEL_IRICHTRSEGMAKER_H
#define RICHKERNEL_IRICHTRSEGMAKER_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
class HepPoint3D;
class HepVector3D;

// Event model
class ContainedObject;

// Rich Kernel
class RichTrackSegment;

/** @class IRichTrSegMaker IRichTrSegMaker.h RichDetTools/IRichTrSegMaker.h
 *
 *  Interface for tools to create RichTrackSegments from various tracking objects.
 *
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichTrSegMaker( "IRichTrSegMaker", 1, 0 );

class IRichTrSegMaker : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichTrSegMaker; }

  /** Create RichTrackSegments for a given tracking object. Using tracking information
   *  to find intersection points, and directions at those points, with the three
   *  radiator media.
   *
   *  @param track    Pointer to tracking object to create RichTrackSegments from
   *  @param segments Returned vector of RichTrackSegment objects
   *
   *  @return Number of RichTrackSegment objects created
   *  @retval 0   Unable to create any segments - Track did not traverse any radiator
   *  @retval 1-3 Track traversed at least one radiator
   */
  virtual int constructSegments ( const ContainedObject * track,
                                  std::vector<RichTrackSegment>& segments ) const = 0;
  
};

#endif // RICHKERNEL_IRICHTRSEGMAKER_H
