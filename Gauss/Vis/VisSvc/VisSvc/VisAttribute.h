// $Id: VisAttribute.h,v 1.3 2009-06-03 14:21:06 jonrob Exp $
#ifndef VISATTRIBUTE_H 
#define VISATTRIBUTE_H 1

// Include files
#include "VisSvc/Color.h"

/** @class VisAttribute VisAttribute.h
 *  This encapsulated the definition of visualization attributes for either
 *  a logical volume or a material. Included attributes are :
 *     - color
 *     - visibility
 *     - opening state at display time :
 *       open means that the children are displayed
 *     - display mode : plain or wireframe
 *
 *  @author Sebastien Ponce
 *  @date   2002-06-25
 */
class VisAttribute {

public:
  enum Visibility {VISIBLE, NOT_VISIBLE, NO_VISIBILITY};
  enum OpenStatus {OPENED, CLOSED, NO_STATUS};
  enum DisplayMode {PLAIN, WIRE_FRAME, NO_MODE};
  
public:
  /// Default constructor
  VisAttribute () :
    m_color(),
    m_visible(NO_VISIBILITY),
    m_openStatus(NO_STATUS),
    m_displayMode(NO_MODE)
  {};

  /// Standard constructor
  VisAttribute (Visibility visible,
                OpenStatus openStatus,
                DisplayMode displayMode,
                Color color) :
    m_color(color),
    m_visible(visible),
    m_openStatus(openStatus),
    m_displayMode(displayMode)
  {};  

  /**
   * Get accessor to member m_color
   * @return the current value of m_color
   */
  const Color color() const {
    return m_color;
  }

  /**
   * Get accessor to member m_visible
   * @return the current value of m_visible
   */
  Visibility visible() const {
    return m_visible;
  }

  /**
   * Get accessor to member m_openStatus
   * @return the current value of m_openStatus
   */
  OpenStatus openStatus() const {
    return m_openStatus;
  }

  /**
   * Get accessor to member m_displayMode
   * @return the current value of m_displayMode
   */
  DisplayMode displayMode() const {
    return m_displayMode;
  }

  /**
   * Merges an attribute with this one.
   * This one has precedence but every undefined member will
   * be defined using the other attribute
   * @param attr the attribute to merge with this one
   */
  void merge(const VisAttribute attr) {
    if (!m_color.isValid()) m_color = attr.color();
    if (NO_VISIBILITY == m_visible) m_visible = attr.visible();
    if (NO_STATUS == m_openStatus) m_openStatus = attr.openStatus();
    if (NO_MODE == m_displayMode) m_displayMode = attr.displayMode();
    return;
  }

  /**
   * equality operator for VisAttribute
   */
  bool operator== (const VisAttribute& v) const {
    return m_visible == v.visible() && 
      m_openStatus == v.openStatus() &&
      m_displayMode == v.displayMode() &&
      m_color == v.color();
  }

  /**
   * unequality operator for VisAttribute
   */
  bool operator!= (const VisAttribute& v) const {
    return !this->operator==(v);
  }
  
private:
  Color m_color;
  Visibility m_visible;
  OpenStatus m_openStatus;
  DisplayMode m_displayMode;

};
#endif // VISATTRIBUTE_H
