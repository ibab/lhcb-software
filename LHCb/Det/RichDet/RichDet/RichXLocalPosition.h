// $Id: RichXLocalPosition.h,v 1.3 2001-10-31 16:50:12 rihill Exp $
#ifndef RICHDET_RICHXLOCALPOSITION_H
#define RICHDET_RICHXLOCALPOSITION_H 1

#include <cassert>
//#include "RichGlobal/Definitions.h"

#include "RichDet/Rich.h"
#include "RichDet/LocalPosition.h"

class RichXLocalPosition : public LocalPosition {

public:
  RichXLocalPosition (Rich::RichID rich,
                      const int side,
                      const double x,
                      const double y);

  virtual ~RichXLocalPosition();

  virtual const Rich::RichID rich () const;

  virtual const int side () const;

  virtual const double x () const;

  virtual const double y () const;

  // Additional Public Declarations

protected:
  // Additional Protected Declarations

private:
  // Additional Private Declarations

private:
  // Data Members for Class Attributes

  Rich::RichID m_rich;

  int m_side;

  double m_x;

  double m_y;

  // Additional Implementation Declarations

};

inline const Rich::RichID RichXLocalPosition::rich () const
{
  return m_rich;
}

inline const int RichXLocalPosition::side () const
{
  return m_side;
}

inline const double RichXLocalPosition::x () const
{
  return m_x;
}

inline const double RichXLocalPosition::y () const
{
  return m_y;
}

#endif // RICHDET_RICHXLOCALPOSITION_H
