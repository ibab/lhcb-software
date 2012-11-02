#include "Mint/IDalitzEventList.h"

#include "Mint/PhaseBinning.h"
#include "Mint/ScpBinning.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBoxSet.h"

#include "Mint/DalitzHistoStackSet.h"

#include "TCanvas.h"
#include "TH1D.h"
#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <iostream>

using namespace MINT;
using namespace std;


PhaseBinning::PhaseBinning(const char* filename,
							DalitzEventPattern pdg,
							const char* treename)
{
	bool dbThis = true;
	 if (dbThis)
	 {
		 std::cout << "Opening File " << filename << std::endl;
	 }
	 _pdg = pdg;
	TFile* in_file = new TFile(filename,"READ");
	TTree* in_tree = dynamic_cast<TTree*>(in_file->Get(treename));
	if (dbThis) std::cout << "Got Tree " << treename << std::endl;

	int entries = in_tree->GetEntries();
	_nbins = entries;
	if (dbThis) std::cout << "Got Tree with entires " << entries << std::endl;

	createBinning(pdg);

	 int NEntries, NEntriesCC;
	 in_tree->SetBranchAddress("NEntries",&NEntries);
	 in_tree->SetBranchAddress("NEntriesCC",&NEntriesCC);


	 double maxPhase, minPhase;
	 in_tree->SetBranchAddress( "MaxPhase",&maxPhase);
	 in_tree->SetBranchAddress( "MinPhase",&minPhase);

	 for (int i = 0; i< entries; i++)
	 {
		 if (dbThis) std::cout << "Get entry " << i << std::endl;

		 in_tree->GetEntry(i);
		 if (dbThis) std::cout << "Add data " << NEntries << std::endl;
		 (_boxSets[i]).addData(NEntries);
		 if (dbThis) std::cout << "Add MC " << NEntriesCC << std::endl;
		 (_boxSets[i]).addMC(NEntriesCC);
		 _nData = _nData + NEntries;
		 _nDataCC = _nDataCC + NEntriesCC;

		(_boxSets[i]).printBoxInfo();

		std::cout << "Data: " << (_boxSets[i]).nData() << std::endl;
		 if (dbThis)
		 {
			 std::cout << " Box " << i << " Min Phase: " << minPhase;
			 std::cout << " Max Phase: " << maxPhase << std::endl;
		 }

	}
	 in_file->Close();
}

int PhaseBinning::createBinning(IDalitzEventList* events ){

  if(0 == events) return 0;
  if(0 == events->size()) return 0;
  const IDalitzEvent* evt0 = events->getREvent(0);
  DalitzEventPattern pat = evt0->eventPattern();
  if(0 == evt0) return 0;
  _pdg = pat;
  ScpBoxSet boxSet;
    for(unsigned int i=0; i < _nbins; i++){
    	ScpBox box(evt0->eventPattern());
  		boxSet.add(box);
  		_boxSets.push_back(boxSet);
  		boxSet.clear();
    }
  return numBins();
}


int PhaseBinning::createBinning(DalitzEventPattern pdg ){
  ScpBoxSet boxSet;
    for(unsigned int i=0; i < _nbins; i++){
    	ScpBox box(pdg);
  		boxSet.add(box);
  		_boxSets.push_back(boxSet);
  		boxSet.clear();
    }
  return numBins();
}




void PhaseBinning::fillData(IDalitzEventList* data){
  data->Start();
  double phase = 0;
  while(data->Next()){
    bool foundBox=false;
    std::complex<double> amp = _fas->getVal(data->getEvent());
    phase = Phase(amp);

       for (int i = 0; i < _nbins; i++)
       {
    	   if (phase > -999)
    	   {
			if (phase > MinPhase(i) && phase < MaxPhase(i))
			{
				_boxSets[i].addData(data->getEvent(),data->getEvent()->getWeight());
				_nData++;
				foundBox=true;
			}
    	   }
       }
  }
}

double PhaseBinning::MaxPhase(int bin)
{
	return (2*3.14159265/(_nbins))*(bin+1);
}

double PhaseBinning::MinPhase(int bin)
{
	return (2*3.14159265/(_nbins))*(bin);
}

double PhaseBinning::Phase(std::complex<double> amp)
{
	bool dbThis =false;

    double real =  amp.real();
    double im = amp.imag();

    if (dbThis)
	{
		std::cout << "Real " << amp.real() << std::endl;
		std::cout << "Imag " << amp.imag() << std::endl;
	}
	real =  amp.real();
	im = amp.imag();

	double phase = atan2(im,real);

	if (phase < 0)
	{
		phase = phase + 2*3.14159265;
	}

	if (real == 0 && im == 0)
	{
		phase = -9999.0;
	}

	return phase;

}

void PhaseBinning::fillDataCC(IDalitzEventList* data){
  data->Start();
  double phase = 0;
      FitAmpSum* fasCC = new FitAmpSum(_fas);
  //    FitAmpSum* fasCC = new FitAmpSum((DalitzEventPattern)EventPatternBar, &fasSet);
      fasCC->CPConjugateSameFitParameters();

  while(data->Next()){

    IDalitzEvent* Evt = data->getEvent();

    vector<TLorentzVector> PArray(5);

    PArray[0] = Evt->p(0);
    PArray[1] = Evt->p(1);
    PArray[2] = Evt->p(2);
    PArray[3] = Evt->p(3);
    PArray[4] = Evt->p(4);

    DalitzEventPattern pat = Evt->eventPattern();

    DalitzEventPattern patCC = pat.makeCPConjugate();

  	DalitzEvent* EvtCC = new DalitzEvent(patCC, PArray);

    bool foundBox=false;
//    std::complex<double> amp = _fas->getVal(data->getEvent());
    std::complex<double> amp = _fas->getVal(EvtCC);

    phase = Phase(amp);

    for (int i = 0; i < _nbins; i++)
    {
    	if (phase > -999)
    	{
			if (phase > MinPhase(i) && phase < MaxPhase(i))
			{
				_boxSets[i].addMC((data->getEvent()),data->getEvent()->getWeight());
				_nDataCC++;
				foundBox=true;
			}
    	}
    }

  }
}

double PhaseBinning::setEvents(IDalitzEventList* data
				    , IDalitzEventList* dataCC
				    ){
  bool dbThis=false;
  if(dbThis) cout << "PhaseBinning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == dataCC) return -9999;
 
  if(0 == numBins()) createBinning(data);
  if(dbThis) cout << "...number of scp bins = " << numBins() << endl;
  resetEventCounts();
  if(dbThis) cout << "...about to fill in the data " << endl;
  fillData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  fillDataCC(dataCC);
  if(dbThis) cout << "... fillMC done, now setting norm factors" << endl;
  setBoxesNormFactors();
  if(dbThis) cout << " done the norm factors, now sorting by chi2" << endl;
  sortByScp();
  if(dbThis) cout << " PhaseBinning::setEventsAndPdf done" << endl;
  return 0;
}

void PhaseBinning::Save(const char* name)
{

	 stringstream ss;//create a stringstream
	 ss << name;
	 ss << "_";
	 ss << _nbins;//add number to the stream
	 ss << ".root";
	 std::string FileOutput = ss.str();

	 TFile* out_file = new TFile(FileOutput.c_str(),"RECREATE");
	 TTree *out_Tree = new TTree("PhaseBinning","PhaseBinning");


	 int NEntries, NEntriesCC;
	 out_Tree->Branch( "NEntries",&NEntries, "NEntries/I");
	 out_Tree->Branch( "NEntriesCC",&NEntriesCC,"NEntriesCC/I");
	 double maxPhase, minPhase;
	 out_Tree->Branch( "MaxPhase",&maxPhase,"MaxPhase/D");
	 out_Tree->Branch( "MinPhase",&minPhase,"MinPhase/D");


	 int size = (int)(_boxSets).size();

	 for (int i = 0; i< size; i++)
	 {

		 NEntries = (_boxSets[i]).nData();
		 NEntriesCC = (_boxSets[i]).nMC();
		 maxPhase = MaxPhase(i);
		 minPhase = MinPhase(i);
		 out_Tree->Fill();

	 }
	 out_file->Write();
	 out_file->Close();

}

PhaseBinning PhaseBinning::operator+ (PhaseBinning param)
{
	PhaseBinning temp;
	bool dbThis = true;

	if (dbThis)	std::cout << "Adding PhaseBinning " << std::endl;
	if (this->ndof() != param.ndof())
	{
		if (dbThis) std::cout << "Not the same number of bins can not add ";
		if (dbThis) std::cout << temp.ndof() << ", " << param.ndof() << std::endl;
		return temp;
	}

	temp.createBinning(param._pdg);

	if (dbThis) std::cout << "Box Set Size " << param._boxSets.size() << std::endl;
	int size = param._boxSets.size();
	for (int i = 0; i < size; i++)
	{

		if (dbThis) std::cout << "Adding data " << _boxSets[i].nData() << std::endl;
		int data = _boxSets[i].nData();
		temp._boxSets[i].addData(data);
		if (dbThis) std::cout << "Adding data " << param._boxSets[i].nData() << std::endl;
		temp._boxSets[i].addData(param._boxSets[i].nData());

		if (dbThis) std::cout << "Adding MC " << _boxSets[i].nMC() << std::endl;
		temp._boxSets[i].addMC(_boxSets[i].nMC());
		temp._boxSets[i].addMC(param._boxSets[i].nMC());

	}

	return (temp);
}

std::ostream& operator<<(std::ostream& os, const PhaseBinning& c2b){
  c2b.print(os);
  return os;
}
//
