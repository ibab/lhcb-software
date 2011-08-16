// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT

void makePull(char* fname){
  gROOT->LoadMacro("readAscii");

  TNtuple* ntp=readAscii(fname, 

}
