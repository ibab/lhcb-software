// $Id: RichSide.h,v 1.5 2003-04-15 17:50:49 jonrob Exp $
#ifndef RICHKERNEL_RICHSIDE_H
#define RICHKERNEL_RICHSIDE_H 1

/** @enumeration Side RichSide.h RichKernel/RichSide.h
 *
 *  Enumeration for Rich1 detector splits
 *
 *  @author Chris Jones
 *  @date   2002-06-19
 */

namespace Rich {

  enum Side {
    top,
    bottom,
    left = 0,
    right = 1
  };

  /// Std Vector typedef
  typedef std::vector<Side> RichSideVector;

}

#endif // RICHKERNEL_RICHSIDE_H
