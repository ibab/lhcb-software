
#include "Mint/DalitzPdfSaveInteg.h"

#include "Mint/IDalitzEventList.h"
#include <string>
#include "TRandom.h"
#include "TRandom3.h"
#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"
#include "Mint/IFastAmplitudeIntegrable.h"

using namespace std;
using namespace MINT;

DalitzPdfSaveInteg::DalitzPdfSaveInteg(const DalitzEventPattern& pat
				       , double precision
				       , const std::string& integInputFiles
				       , const std::string& integEvtFile
				       , const std::string& topUpIntegOption
				       , MinuitParameterSet* mps
				       , const std::string& integOutputFile
				       )
  : DalitzPdfBaseFastInteg(pat, 0, mps, precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _integratorEventFile(integEvtFile)
    , _integratorOutputFile(integOutputFile)
 {
   //   _pat = pat;
   setIntegratorFileName(integInputFiles);

   cout << "pset pointer in DalitzPdfSaveInteg " << mps << endl;
   cout << "amps pointer in DalitzPdfSaveInteg " << _amps << endl;
   
   if("" == integOutputFile) _integratorOutputFile=integInputFiles;
   
   // here, SignalGenerator is used by FromFileGenerator, to fill
   // up missing events in case more are needed than found in the
   // file.  Since we don't know with which random seed the
   // events in the file were generated, we supply a random
   // number generator with randomised seed.
   _localRnd = new TRandom3(time(0));
    if("topUp" == topUpIntegOption){
      if(_pat.empty()){
       cout << "cannot make SignalGenerator, pattern is empty" << endl;
       cout << "bailing out" << endl;
       throw "no pattern, no signal generator";
     }
     _sgGen =  new SignalGenerator(_pat, _localRnd);
     _sgGen->setWeighted();
     _sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
    }else{
      _sgGen = 0;
    }
    _fileGen   = new FromFileGenerator(_integratorEventFile, _sgGen);
    this->setEventGenerator(_fileGen);
 }

DalitzPdfSaveInteg::DalitzPdfSaveInteg( const DalitzEventPattern& pat
				       , IFastAmplitudeIntegrable* amps
				       , double precision
				       , const std::string& integInputFiles
				       , const std::string& integEvtFile
				       , const std::string& topUpIntegOption
				       , const std::string& integOutputFile
				       )
  : DalitzPdfBaseFastInteg(pat, 0, amps, precision)
  , _localRnd(0)
  , _sgGen(0)
  , _fileGen(0)
  , _integratorEventFile(integEvtFile)
  , _integratorOutputFile(integOutputFile)
 {
   //_pat = pat;
   setIntegratorFileName(integInputFiles);
   
   if("" == integOutputFile) _integratorOutputFile=integInputFiles;
   
   // here, SignalGenerator is used by FromFileGenerator, to fill
   // up missing events in case more are needed than found in the
   // file.  Since we don't know with which random seed the
   // events in the file were generated, we supply a random
   // number generator with randomised seed.
   _localRnd = new TRandom3(time(0));
   if("topUp" == topUpIntegOption){
     if(0 == _amps){
       cout << "cannot make SignalGenerator, _amps are empty" << endl;
       cout << "bailing out" << endl;
       throw "no amps, no signal generator";
     }
     if(_pat.empty()){
       cout << "cannot make SignalGenerator, pattern is empty" << endl;
       cout << "bailing out" << endl;
       throw "no pattern, no signal generator";
     }
     _sgGen =  new SignalGenerator(_pat, _localRnd);
     _sgGen->setWeighted();
     _sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
   }else{
     _sgGen = 0;
   }
   _fileGen   = new FromFileGenerator(_integratorEventFile, _sgGen);
    this->setEventGenerator(_fileGen);
  }

//IFastAmplitudeIntegrable* DalitzPdfSaveInteg::getAmpSum(){ return _amps;}

DalitzPdfSaveInteg::~DalitzPdfSaveInteg(){
  saveIntegrator(_integratorOutputFile);
  if(0 != _fileGen)  delete _fileGen;
  if(0 != _sgGen)    delete _sgGen;
  if(0 != _localRnd) delete _localRnd;
}
//

