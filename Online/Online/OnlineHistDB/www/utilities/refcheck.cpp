//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/www/utilities/refcheck.cpp,v 1.1 2007-07-13 15:55:26 ggiacomo Exp $
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

#include "TFile.h"
#include "TKey.h"
#include "TH1.h"

bool searchHistName(std::string name, std::vector<std::string> &HistNames) {
 std::vector<std::string>::iterator it;
 for (it = HistNames.begin();it != HistNames.end(); ++it) {
   if (name == *it) {
     HistNames.erase(it);
     return true;
   }
 }
 return false;
}

void header() {
  cout << "<br><table border=1> <tr><td colspan=2><font size=+2 color='blue'><B>Histograms found in file:</B></font></td></tr>"<<endl;
}
void trailer() {
  cout << "</table>"<<endl;
}


int main (int narg,char **argv) {
  int out=1;
  std::vector<std::string> HistNames;

  if (narg>2) {
    header();
    std::string Task(*(argv+1));
    std::string Filename(*(argv+2));
    for (int i=3;i<narg;i++) 
      HistNames.push_back(std::string(*(argv+i)));

    TFile f(Filename.data(),"READ");
    TIter next(f.GetListOfKeys());
    TKey *k;
    while (k=(TKey*)next()) {
      //      cout << "Found object " << k->GetName() << " class " << k->GetClassName()<<endl;
      TObject *ho = k->ReadObj();
      if (TH1* h = dynamic_cast<TH1*> (ho)) {
	cout << "<tr><td>  " << k->GetName() ;
	if (searchHistName(h->GetName(), HistNames)) {
	  cout << "<td align='left'><img src='ok.png'></td></tr>" <<endl;
	}
	else {
	  cout << "<td align='left'><img src='warning.png'> unknown histogram</td></tr>" <<endl;
	}
      }
    }
    f.Close();
    // missing histograms
    std::vector<std::string>::iterator it;
    if(HistNames.size() >0)  
      cout << "<tr><td colspan=2> &nbsp </td></tr><tr><td colspan=2><font size=+2 color='blue'><B>Missing Histograms:</B></font></td></tr>"<<endl;
    for (it = HistNames.begin();it != HistNames.end(); ++it) {
      cout << "<tr><td> "<<*it <<"<td align='left'><img src='warning.png'></tr>"<<endl; 
    }

    trailer();
    out=0;
  }
  return out;

}
