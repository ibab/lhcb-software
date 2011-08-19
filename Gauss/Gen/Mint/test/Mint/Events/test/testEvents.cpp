// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT
#include <iostream>

#include "Mint/IEventList.h"
#include "Mint/EventList.h"
#include "Mint/EventAccess.h"

using namespace std;
using namespace MINT;
/* the first part shows how 
   to declare and use simple
   EventLists.

   The second part will show how to use
   more complicated features, espe
   different storage and return types
   (important with interfaces).
*/

int testSimpleEvents();
int testComplicatedEvents();

int main(){
  cout << "testSimpleEvents()\n"<<endl;
  testSimpleEvents();
  cout << "\n\ntestComplicatedEvents()\n" << endl;
  testComplicatedEvents();
  return 0;
}

int testSimpleEvents(){

  EventList<int> dL;
  for(int i=0; i<20; i++){
    dL.Add(i);
  }
  cout << "made a list, it now has " << dL.size() << " events" << endl;
  cout << " those are: " << endl;
  for(unsigned int i=0; i<dL.size(); i++){
    cout << dL[i] << ", ";
  }
  cout << endl;
  
  // Differnet ways of looping:
  cout << "they are also:" << endl;
  for(EventList<int>::iterator it = dL.begin();
      it != dL.end(); it++){
    cout << *it << ", ";
  }
  cout << endl;
  cout << "as well as:" << endl;
  dL.Start();
  do{
    cout << *(dL.currentEvent())  << ", ";
    if( (*(dL.currentEvent()))%2) dL.Delete();
  }while(dL.Next());
  // Not that dL.currentEvent() returns a 
  // pointer. This can be of different
  // type (i.e. interface) than the
  // stored class - see below for example.

  cout << endl;
  cout << "after deleting the odd ones:" << endl;
  dL.Start();
  do{
    cout << *(dL.currentEvent()) << ", ";
  }while(dL.Next());
  cout << endl;
  
  // EventAccess. Any class inheriting from EventAccess
  // gets to see the current event (and the entire list)
  // EventAccess only gets the return type (first template
  // argument of EventList<ReturnType, StorageType>
  // as template argument
  //
  cout << "Now testing event accesss " << endl;
  EventAccess<int> ea((IEventList<int>*)&dL);
  dL.Start();
  do{
    cout << *(dL.currentEvent())  << ", ";
    cout << "in EventAccss " << *(ea.getEvent())
	 << endl;
  }while(dL.Next());


  return 0;
}


/* This second part shows how to use the
   option to have different storage and
   return types in the EventList template
   (for use with interfaces)
*/


class InterfaceClass{
public:
  virtual double getVal()=0;

  virtual ~InterfaceClass(){}
};

class StorageClass : virtual public InterfaceClass{
  double _v1, _v2;
public:
  StorageClass(double v1=0, double v2=0)
    :_v1(v1), _v2(v2) {}

  virtual double getVal(){
    return _v1*_v2;
  }
  virtual ~StorageClass(){}
};

class SquareEventAccess : EventAccess<InterfaceClass>{
public:
  SquareEventAccess(IEventList<InterfaceClass>* eventList)
    : EventAccess<InterfaceClass>(eventList)
  {}

  SquareEventAccess(const SquareEventAccess& other)
    : IBasicEventAccess<InterfaceClass>() // compiler wants it.
    , IEventAccess<InterfaceClass>() // compiler wants it.
    , EventAccess<InterfaceClass>(other)
  {}

  double getSquare(){
    InterfaceClass* ptr = getEvent();
    if(0 == ptr) return -9999;
    double d=ptr->getVal();
    return d*d;
  }
};



int testComplicatedEvents(){

  EventList<InterfaceClass, StorageClass> List;
  for(int i=0; i<10; i++){
    StorageClass a(i, i+1);
    List.Add(a);
  }

  EventAccess<InterfaceClass> Access_plain((IEventList<InterfaceClass>*)&List);
  SquareEventAccess Access_square(&List);

  List.Start();
  do{
    cout << " Event from list " << (List.currentEvent())->getVal();
    cout << ", Just access " << Access_plain.getEvent()->getVal();
    cout << ", Square event " << Access_square.getSquare();
    cout << endl;
  }while(List.Next());

  return 0;
}
//
