#include <stdio.h>
#include <vector>
#include "TFile.h"
#include "evtsel.h"

int main(int argc, char **argv){

   // Check that the input is correct
   if(argc != 5) {
      cerr << "Usage: " << argv[0] << " <configfile> <input> <output> <isMC> " << endl;
      cerr << "<configfile> = config file" << endl;
      cerr << "<input>      = input file" << endl;
      cerr << "<output>     = output file where the histos/trees will be written" << endl;
      cerr << "<MC/DATA>    = MC or DATA" << endl;
      return 1;
   }

   TString config(argv[1]);
   TString input(argv[2]);
   TString output(argv[3]);
   TString MC(argv[4]);
   Bool_t isMC;

   if(MC=="MC") isMC = 1; else if(MC=="DATA") isMC = 0; else {cerr << "<isMC> not set properly" << endl; exit(1);}

   cout << argv[0] << " will use:" << endl;
   cout << "<configfile> = " << config << endl;
   cout << "<input>      = " << input << endl;
   cout << "<output>     = " << output << endl;
   cout << "<isMC>       = ";
   if(isMC) { cout << "true"; } else { cout << "false"; }
   cout << endl;

   TFile *file = new TFile(input,"read");

   TFile *savefile = new TFile(output,"recreate");

   evtsel *mysel = new evtsel(config,file,savefile,isMC);

   mysel->loop();

   savefile->cd();
   savefile->Write();
   mysel->finish();

   file->Close();
   savefile->Close();
   file->Delete();
   savefile->Delete();

   cout << "RunJES ended." << endl << endl;
   return 1;
}

