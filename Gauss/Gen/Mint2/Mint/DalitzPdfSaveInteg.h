#ifndef DALITZ_PDF_SAVE_INTEG_HH
#define DALITZ_PDF_SAVE_INTEG_HH

#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventList.h"
#include "Mint/IEventGenerator.h"

#include <string>

class TRandom;
class SignalGenerator;
class FromFileGenerator;
namespace MINT
{
	class MinuitParameterSet;
}
class IFastAmplitudeIntegrable;

class DalitzPdfSaveInteg // saves and retrieves the integration result
  : public DalitzPdfBaseFastInteg
  {
protected:
  TRandom* _localRnd;
  SignalGenerator* _sgGen;
  FromFileGenerator* _fileGen;
  std::string _integratorEventFile;
  std::string _integratorOutputFile; // be default same as input
public:
  double un_normalised_noPs(IDalitzEvent& evt){
    double ampSq =  getAmps()->RealVal(evt);
    return ampSq;
  }

  DalitzPdfSaveInteg( const DalitzEventPattern& pat
		      ,   double precision=1.e-4
		     , const std::string& integInputFiles="Integrator"//could be "intg1,intg2,intg2"
		     , const std::string& integEvtFile="integEvtFile.root"
		     , const std::string& topUpIntegOption="topUp"
		     , MINT::MinuitParameterSet* mps=0
		     , const std::string& integOutputFile=""
		     // default: integratorOutputFile = integratorInputFile
		     // (set in constructor)
		     );
  DalitzPdfSaveInteg(const DalitzEventPattern& pat
		     , IFastAmplitudeIntegrable* amps
		     , double precision=1.e-4
		     , const std::string& integInputFiles="Integrator"//could be "intg1,intg2,intg2"
		     , const std::string& integEvtFile="integEvtFile.root"
		     , const std::string& topUpIntegOption="topUp"
		     , const std::string& integOutputFile=""
		     // default: integratorOutputFile = integratorInputFile
		     // (set in constructor)
		     );
  //  IFastAmplitudeIntegrable* getAmpSum();

  ~DalitzPdfSaveInteg();

};
#endif
//
