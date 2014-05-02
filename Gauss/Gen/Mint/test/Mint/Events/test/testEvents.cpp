// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT
#include <iostream>

#include "Mint/IEventList.h"
#include "Mint/EventList.h"
//#include "Mint/EventAccess.h"

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
    cout << i << ")\t";
    cout << dL[i] << endl;
  }
  cout << endl;
  
  return 0;
}


//
