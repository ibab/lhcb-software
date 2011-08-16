// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include <iostream>

#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DiskResidentEventList.h"
using namespace std;
using namespace MINT;

int testDalitzEvents(){

  // make list of particles:
  // mother, followed by final states:
  DalitzEventPattern  pdg(421, 321, -321, 211, -211);

  int Nevents=500;

  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);

  cout << "eventList.size() " << eventList.size() << endl;

  for(DalitzEventList::iterator it = eventList.begin();
      it != eventList.end();
      it++){
    cout << "iterating through event list" << endl;
    cout << "phase space: " << it->phaseSpace() << endl;
  }

  eventList.Start();
  cout << "eventList.Start() worked " << endl;
  int counter=0;
  while(eventList.Next()){
    cout << counter++ << ") ";
    IDalitzEvent* ptr = eventList.currentEvent();
    if(0 == ptr){
      cout << " ptr is zero !!" << endl;
    }else{
      cout << " ptr is OK." << endl;
    }
    cout << " phase space: "
	 << eventList.currentEvent()->phaseSpace();

    cout << " (will delete now if phase space > 6.e-18)" << endl;
    if(eventList.currentEvent()->phaseSpace() > 6.e-18){
      eventList.Delete();
    }
    cout << endl;
  }

  cout << "\n\n and again, after removing events with"
       << " a phase-space factor > 6.e-18" << endl;
  counter=0;
  eventList.Start();
  while(eventList.Next()){
    cout << counter++ << ") ";
    IDalitzEvent* ptr = eventList.currentEvent();
    if(0 == ptr){
      cout << " ptr is zero !!" << endl;
    }else{
      cout << " ptr is OK." << endl;
    }
    cout << " phase space: "
	 << eventList.currentEvent()->phaseSpace()
	 << endl;
    cout << endl;
  }

  cout << "\n\n Now save the eventList to a file" << endl;
  std::string fname = "eventList.root";
  //  eventList.save(fname);
  DiskResidentEventList ntpListFile(eventList, fname);
  ntpListFile.Close();

  cout << " the filename is " << fname << " and the format"
       << " is that of a root ntuple." << endl;

  cout << "Now reading it back in as DalitzEventList:" << endl;
  DalitzEventList readBackList;
  readBackList.fromFile(fname);

  cout << " number of events saved = " << eventList.size()
       << ", and read back in = " << readBackList.size()
       << endl;

  if(eventList.size() != readBackList.size()){
    cout <<    ">>>>>> SO SOMETHING HAS GONE WRONG!!! "
	 << "\n        returning -1" 
	 << endl;
    return -1;
  }

  cout << "Now reading it back in as a DiskResidentEventList:" << endl;
  DiskResidentEventList ntpList("eventList.root");
  //  DiskResidentEventList ntpList2("eventList.root");

  if(readBackList.size() > 5){
    cout << "Now comparing first 5 entries." << endl;
  }else{
    cout << "Now comparing entries." << endl;
  }
  for(unsigned int i=0; 
      i< readBackList.size() 
	&& i < ntpList.size() 
	//	&& i < ntpList2.size() 
	&& i < 5;
      i++){
    cout << "evt " << i << "  original:     " << eventList[i] << endl;
    cout << "evt " << i << ", the copy:     " << readBackList[i] << endl;
    cout << "evt " << i << ", the 2nd copy: " << ntpList(i) << endl;
    //    cout << "evt " << i << ", the 3rd copy: " << ntpList2(i) << endl;
    cout << " ---------------------------------- "
	 << endl;
  }

  cout << "now testing autosave by adding many, many events to newNtpList" 
       << endl;
  DiskResidentEventList newNtpList(pdg, "funnyFileName.root");
  int N = 1000000000;
  //int N=100000;
  //int N=10;
  for(int i=1; i <= N; i++){
    if(i < 5 || 0 == i%10000){
      cout << "adding " << i << " th event." << endl;
    }
    DalitzEvent evt(pdg, gRandom);
    //DalitzEvent evt(pdg);// dummy events - faster.
    //readBackList.Add(evt);
    newNtpList.Add(evt);
    ntpList.Add(evt);
  }
  newNtpList.save();
  ntpList.Close();
  return 0;
}

int main(){
  return testDalitzEvents();
}

//
