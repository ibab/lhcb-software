// $Id: IDetectorElement.h,v 1.21 2008-09-23 12:32:15 marcocle Exp $
// $ID:  $
#ifndef  DETDESC_IDETECTORELEMENT_H
#define  DETDESC_IDETECTORELEMENT_H 1

// Include files
#include <iostream>
#include <string>

// Framework include files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/Point3DTypes.h"

// Forward declarations
class IGeometryInfo;
class IAlignment;
class ICalibration;
class IReadOut;
class ISlowControl;
class IFastControl;

class Condition;
class ParamValidDataObject;

/** the unique interface identifier
 * ( unique interface identifier , major & minor versions)
 */
static const InterfaceID IID_IDetectorElement( 156 , 2 , 2 );

/** @interface IDetectorElement IDetectorElement.h "DetDesc/IDetectorElement.h"
 *
 *  An abstract Interface accees
 *   the node in DetectorDescription tree.
 *  Just delegates all questions to right guys.
 *
 *  @author Sebastien Ponce
 *  @author Vanya Belyaev
 *  @author Marco Clemencic <marco.clemencic@cern.ch>
 */

class IDetectorElement : virtual public IInterface
{
  ///
 public:
  ///
  typedef std::vector<IDetectorElement*>   IDEContainer;
  ///
 public:

  /** retrieve the unique interface identifier
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IDetectorElement; }

  /// "accessor":  name/identifier of the Detector Element
  virtual          const std::string&   name       ()   const = 0;

  /// Check if the condition called 'name' is in the list of conditionrefs.
  virtual bool hasCondition(const std::string &name) const = 0;

  /// Return the SmartRef for the condition called 'name'.
  virtual SmartRef<Condition> condition(const std::string &name) const = 0;

  ///
  /// delegations:
  ///
  /// "accessor":  delegates the IGeometryInfo Interface
  virtual                IGeometryInfo* geometry   ()         = 0;
  /// "accessor":  delegates the IGeometryInfo Interface (const)
  virtual          const IGeometryInfo* geometry   ()   const = 0;
  /// "accessor":  delegates the IAlignment Interface
  virtual                IAlignment*    alignment  ()         = 0;
  /// "accessor":  delegates the IAlignment Interface (const)
  virtual          const IAlignment*    alignment  ()   const = 0;
  /// "accessor":  delegates the ICalibration Interface
  virtual                ICalibration*  calibration()         = 0;
  /// "accessor":  delegates the ICalibration Interface (const)
  virtual          const ICalibration*  calibration()   const = 0;
  /// "accessor":  delegates the IReadOut Interface
  virtual                IReadOut*      readOut    ()         = 0;
  /// "accessor":  delegates the IReadOut Interface (const)
  virtual          const IReadOut*      readOut    ()   const = 0;
  /// "accessor":  delegates the ISlowControl Interface
  virtual                ISlowControl*  slowControl()         = 0;
  /// "accessor":  delegates the ISlowControl Interface (const)
  virtual          const ISlowControl*  slowControl()   const = 0;
  /// "accessor":  delegates the IFastControl Interface
  virtual                IFastControl*  fastControl()         = 0;
  // "accessor":  delegates the IFastControl Interface (const)
  virtual          const IFastControl*  fastControl()   const = 0;
  /// Another way of accessiong the information: "pseudo-conversion"
  /// cast to       IGeometryInfo*
  virtual operator       IGeometryInfo*()                     = 0;
  /// cast to const IGeometryInfo*
  virtual operator const IGeometryInfo*()               const = 0;
  /// cast to       IAlignment*
  virtual operator       IAlignment*   ()                     = 0;
  /// cast to const IAlignment*
  virtual operator const IAlignment*   ()               const = 0;
  /// cast to       ICalibration*
  virtual operator       ICalibration* ()                     = 0;
  /// cast to const ICalibration*
  virtual operator const ICalibration* ()               const = 0;
  /// cast to       IReadOut*
  virtual operator       IReadOut*     ()                     = 0;
  /// cast to const IReadOut*
  virtual operator const IReadOut*     ()               const = 0;
  /// cast to       ISlowControl*
  virtual operator       ISlowControl* ()                     = 0;
  /// cast to const ISlowControl*
  virtual operator const ISlowControl* ()               const = 0;
  /// cast to       IFastControl*
  virtual operator       IFastControl* ()                     = 0;
  /// cast to const IFastControl*
  virtual operator const IFastControl* ()               const = 0;

  /// more convenient set of conversion operators:

  /// cast to       IGeometryInfo&
  virtual operator       IGeometryInfo&()                     = 0;
  /// cast to const IGeometryInfo&
  virtual operator const IGeometryInfo&()               const = 0;
  /// cast to       IAlingment&
  virtual operator       IAlignment&   ()                     = 0;
  /// cast to const IAlignment&
  virtual operator const IAlignment&   ()               const = 0;
  /// cast to       ICalibration&
  virtual operator       ICalibration& ()                     = 0;
  /// cast to const ICalibration&
  virtual operator const ICalibration& ()               const = 0;
  /// cast to       IReadOut&
  virtual operator       IReadOut&     ()                     = 0;
  /// cast to const IReadOut&
  virtual operator const IReadOut&     ()               const = 0;
  /// cast to       ISlowControl&
  virtual operator       ISlowControl& ()                     = 0;
  /// cast to const ISlowControl&
  virtual operator const ISlowControl& ()               const = 0;
  /// cast to       IFastControl&
  virtual operator       IFastControl& ()                     = 0;
  /// cast to const IFastControl&
  virtual operator const IFastControl& ()               const = 0;

  /// some functions to simplify the navigation
  /// (according to feedback after release 3)
  /// pointer to parent IDetectorElement (const version)
  virtual IDetectorElement*  parentIDetectorElement() const  = 0 ;
  /// (reference to) container of pointers to child detector elements
  virtual IDetectorElement::IDEContainer&
  childIDetectorElements() const = 0;
  /// iterators for manipulation of daughter elements
  /// begin iterator
  virtual IDetectorElement::IDEContainer::iterator childBegin() = 0;
  virtual IDetectorElement::IDEContainer::const_iterator childBegin() const = 0;
  /// end   iterator
  virtual IDetectorElement::IDEContainer::iterator childEnd() = 0 ;
  virtual IDetectorElement::IDEContainer::const_iterator childEnd() const = 0 ;
  /// functions for  listing of objects, used in overloaded << operations
  virtual std::ostream&  printOut      ( std::ostream& ) const = 0;
  /// reset to the initial state
  virtual       IDetectorElement* reset()       = 0 ;

  /**
   * Method used to access the ParamValidDataObject methods from IDetectorElement
   * interface.
   */
  virtual const ParamValidDataObject *params() const = 0;

  /**
   * Return a sensitive volume identifier for a given point in the
   * global reference frame.
   */

  virtual const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos ) const=0;

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize() = 0;

  /// destructor
  virtual ~IDetectorElement() ;

};
///
inline std::ostream& operator<<( std::ostream&           os ,
                                 const IDetectorElement& de )
{
  return de.printOut(os);
};
///
inline std::ostream& operator<<( std::ostream&           os ,
				 const IDetectorElement* de )
{
  return
    ( (0 == de) ? (os << "IDetectorElement* points to NULL") : (os << *de) ) ;
};

/// ===========================================================================
#endif  ///<   DETDESC_IDETECTORELEMENT_H
/// ===========================================================================









