#ifndef RichBase_h
#define RichBase_h

// Include files
#include <map>
#include<vector>
#include <cmath>

#include "RichReadout/PixelProperties.h"
#include "RichKernel/RichSmartID.h"
#include "Event/Kernel.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"

class Pixel;
class PixelProperties;
class RichRegistry;

class RichBase {

public:

  RichBase();

  RichBase(RichBase&b);

  ~RichBase();

protected:

  enum BaseState
    {
      OPEN, /*!< the base can be initalized */
      CLOSED,/*!< the base cannot be initalized but channels can be setup */
      UNSETTABLE /*!< the base is CLOSED and channels cannot be setup */
    };

private:

  string  myName;
  BaseState MyState;

public:

  bool IsInOpenState() const
  {
    return MyState==OPEN ? true : false;
  }

  bool IsInSettableState() const
  {
    return MyState!=UNSETTABLE ? true : false;
  }

  void CloseState()
  {
    if(MyState==OPEN)
      MyState=CLOSED;
  }

  void OpenState()
  {
    if(MyState==CLOSED)
      MyState=OPEN;
  }

  void UnsettableState()
  {
    MyState=UNSETTABLE;
  }


  string GetName() {

    return myName;
  };


  void SetName( string s) {
    myName = s;
  }

  void clear();

  void init(){};

public:

  RichBase& operator=(const RichBase&);

  void upDate( std::vector<unsigned int> );

  Pixel next(Pixel ) const;

  long size() const;

  PixelProperties* DecodeUniqueID(const unsigned int &) const;

  typedef std::map<unsigned int, PixelProperties*, less<unsigned int> > activemap;

  activemap* activePixels;

  activemap::const_iterator begin() const;

  activemap::const_iterator end() const;

private:

  friend RichRegistry;

};


#endif
