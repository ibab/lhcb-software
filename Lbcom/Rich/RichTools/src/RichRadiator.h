// $Id: RichRadiator.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHRADIATOR_H
#define RICHDETTOOLS_RICHRADIATOR_H 1

// Include files
#include <vector>

//#include "RichKernel/RichDefinitions.h"
#include "RichDet/DeRichRadiator.h"

class RichX;

/** @class RichRadiator RichRadiator.h RichDetTools/RichRadiator.h
 *
 *
 *  @author Artur Barczyk
 *  @date   18/05/2002
 */

class RichRadiator {

public:

  typedef std::vector<RichRadiator>                   Collection;
  typedef std::vector<RichRadiator>::iterator         Iterator;
  typedef std::vector<RichRadiator>::const_iterator   ConstIterator;
  typedef std::vector<RichRadiator*>                  PointerCollection;
  typedef std::vector<RichRadiator*>::iterator        PointerIterator;
  typedef std::vector<RichRadiator*>::const_iterator  ConstPointerIterator;

  RichRadiator( );  //< Standard constructor

  /// Constructor from array of pointers to XML radiator objects, parent RICH detector
  /// and Radiator type
  RichRadiator( std::vector<DeRichRadiator*>&, RichX*, Rich::RadiatorType );

  RichRadiator( const RichRadiator& );   //< Copy constructor

  ~RichRadiator( ); ///< Destructor

  /// Determines the radiator entry/exit points given a position and direction
  /// Return type is whether points where successfully found or not
  bool intersectionPoints ( const HepPoint3D& position,
                            const HepVector3D& direction,
                            HepPoint3D& entryPoint,
                            HepPoint3D& exitPoint );

  /// Determines the next radiator intersection point given a position and direction
  /// Return type is whether points where successfully found or not
  bool nextIntersectionPoint ( const HepPoint3D& position,
                               const HepVector3D& direction,
                               HepPoint3D& intersection );

  /// Checks whether this point is inside the radiator medium
  bool isInside( const HepPoint3D& );

  /// Returns the radiator type
  Rich::RadiatorType id() const { return m_type; };

  /// Returns pointer to parent Rich detector
  RichX * rich() const { return m_rich; };

private: // private data

  /// Enumeration of radiator type
  Rich::RadiatorType m_type;

  /// Pointer to parent RICH detector
  RichX* m_rich;

  /// pointers to XML DetElement(s)
  std::vector<DeRichRadiator*> m_deRadiators;

};
#endif // RICHDETTOOLS_RICHRADIATOR_H
