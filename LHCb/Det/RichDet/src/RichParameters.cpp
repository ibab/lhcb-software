// $Id: RichParameters.cpp,v 1.2 2001-10-26 13:02:13 rihill Exp $
// Include files
#include <fstream>
#include <strstream>
#include <stdexcept>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// SicbCnv
#include "SicbCnv/SicbFortran.h"

// local
#include "RichDet/RichParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichParameters
//
// 12/07/2001 : Christopher Rob Jones
//-----------------------------------------------------------------------------

static ToolFactory<RichParameters> s_factory;
const IToolFactory& RichParametersFactory = s_factory;

RichParameters::RichParameters(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : AlgTool ( type, name, parent ) {

  // Obtain Parameter file from JobOptions
  declareProperty("ParameterFile",m_parameterFile = "RichAnalysis.Parameter" );

  updateDone = false;

  // Update parameter Map
  updateParaMap();

}

RichParameters::~RichParameters() {};

void RichParameters::updateParaMap ()
{
  if ( !updateDone ) {
    updateDone = true;

    // initialize messsage service
    MsgStream log(msgSvc(), name());

    ifstream parameterStream(m_parameterFile.c_str());
    if ( !parameterStream ) {
      log << MSG::FATAL << "Cannot open parameter file "
          << m_parameterFile << endreq;
    } else {
      this->readStream(parameterStream);
    }

  }

}
int RichParameters::getValue (const std::string &key,
                              const int defValue) const
{
  // initialize messsage service
  MsgStream log(msgSvc(), name());

  map<std::string,std::string>::const_iterator found = m_parameterMap.find(key);
  if ( found  == m_parameterMap.end() ) {
    log << MSG::ERROR << "Parameter " << key 
        << " not found, returning default value " << defValue << endreq;
    return defValue;
  } else {
    istrstream ist( (*found).second.c_str(), (*found).second.size() );
    int value;
    ist >> value;
    log << MSG::DEBUG << "Retrieved Rich parameter " << key
        << " with value " << value << endreq;
    return value;
  };

}

double RichParameters::getValue (const std::string &key,
                                 const double defValue)
  const
{
  // initialize messsage service
  MsgStream log(msgSvc(), name());

  map<std::string,std::string>::const_iterator found = m_parameterMap.find(key);
  if ( found == m_parameterMap.end() ) {
    log << MSG::ERROR << "Parameter " << key 
        << " not found, returning default value " << defValue << endreq;
    return defValue;
  } else {
    istrstream ist( (*found).second.c_str(), (*found).second.size() );
    double value;
    ist >> value;
    log << MSG::DEBUG << "Retrieved Rich parameter " << key
        << " with value " << value << endreq;
    return value;
  };

}

std::string RichParameters::getValue (const std::string &key,
                                      const std::string &defValue)
  const
{
  // initialize messsage service
  MsgStream log(msgSvc(), name());

  map<std::string,std::string>::const_iterator found = m_parameterMap.find(key);
  if ( found == m_parameterMap.end() ) {
    log << MSG::ERROR << "Parameter " << key 
        << " not found, returning default value " << defValue << endreq;
    return defValue;
  } else {
    log << MSG::DEBUG << "Retrieved Rich parameter " << key
        << " with value " << (*found).second << endreq;
    return (*found).second;
  };

}

bool RichParameters::getValue (const std::string &key,
                               const bool defValue) const
{
  // initialize messsage service
  MsgStream log(msgSvc(), name());

  // Update parameter Map
  // updateMap();

  map<std::string,std::string>::const_iterator found = m_parameterMap.find(key);
  if ( found == m_parameterMap.end() ) {
    log << MSG::ERROR << "Parameter " << key 
        << " not found, returning default value " << defValue << endreq;
    return defValue;
  } else {
    istrstream ist( (*found).second.c_str(), (*found).second.size() );
    bool value;
    ist >> value;
    log << MSG::DEBUG << "Retrieved Rich parameter " << key
        << " with value " << value << endreq;
    return value;
  };

}

void RichParameters::readStream (istream &parameterStream)
{

  // initialize messsage service
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "Reading Rich Parameter file : "
      << m_parameterFile << endreq;

  for ( std::string line = nextLine(parameterStream);
        line.size() > 0;
        line = nextLine(parameterStream) ) {

    unsigned int i1 = line.find_first_not_of(' ');

    // skip blank lines

    if ( i1 == line.size() ) continue;

    // skip comments

    if ( line.at(i1) == '#' ) continue;

    int i2 = line.find_first_of(' ',i1);
    std::string key = line.substr(i1,i2-i1);
    i1 = line.find_first_not_of(' ',i2);
    i2 = line.find_last_not_of(' ')+1;
    std::string value = line.substr(i1,i2-i1);

    // Add to Map
    log << MSG::DEBUG << "Read " << key << " as "
        << value << endreq;
    m_parameterMap[key] = value;

  }

  // Read Rich CDF files
  readCDF();

}

// CRJ: New code to Read CDF values as well.
void RichParameters::readCDF () {

  // initialize messsage service
  MsgStream log(msgSvc(), name());

  int iName = 0;

  // Read RIC1 from Rich.cdf
  int nRIC1par = 53;
  std::string RIC1names[] = {
    "XSGAS1","YSGAS1","ZSGAS1","XEGAS1","YEGAS1","ZEGAS1","XXGAS1","AXGAS1",
    "TWALAER1","TWALHPD1","TWALINN1","TWALOUT1","TWALBAK1","TWALMID1",
    "DAERO1","TAERO1","XAERO1","YAERO1","DRAERO1",
    "RCMIR1","XMIR1","YMIR1","ZMIR1","TMIR1",
    "DHPD1","THPD1","XHPD1","YHPD1","XPHPD1","YPHPD1","ZPHPD1","AAHPD1",
    "ABHPD1","ACHPD1",
    "DHOLS1","DHOLE1",
    "SELGAS1E1","SELGAS1E2","SELGAS1F1","SELGAS1F2","GAS1MOLW","GAS1RHO",
    "SELAERO1E1","SELAERO1E2","SELAERO1F1","SELAERO1F2",
    "EPHOGAS1T","EPHOGAS1B",
    "EPHOAERO1T","EPHOAERO1B",
    "AERO1RAL1","AERO1RAL2",
    "REFMIRSPH1"};
  int* intRIC1read = new int[nRIC1par];
  float* floatRIC1read = (float*) intRIC1read;
  log << MSG::DEBUG << "Reading CDF detector RIC1" << endreq;
  SicbFortran::utdget("RIC1","G",nRIC1par, intRIC1read );
  for ( iName = 0; iName<nRIC1par; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRIC1read[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RIC1."+RIC1names[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRIC1read;

  // Read RIC2 from Rich.cdf
  int nRIC2par = 42;
  std::string RIC2names[] = {
    "XSGAS2","YSGAS2","ZSGAS2","XEGAS2","YEGAS2","ZEGAS2",
    "TWALUP2","TWALINN2","TWALOUT2","TWALBAK2","TWALMID2","TWALHPD2","TSHIELD2",
    "RCMIR2","XMIR2","YMIR2","ZMIR2","TMIR2",
    "XREFA2","ZREFA2","XREFB2","ZREFB2","YREF2","TREF2",
    "APHPD2","XPHPD2","ZPHPD2","YHPD2","ZHPD2","THPD2",
    "DHOLS2","DHOLE2",
    "SELGAS2E1","SELGAS2E2","SELGAS2F1","SELGAS2F2","GAS2MOLW","GAS2RHO",
    "EPHOGAS2T","EPHOGAS2B",
    "REFMIRSPH2","REFMIRFLA2"};
  int* intRIC2read = new int[nRIC2par];
  float* floatRIC2read = (float*) intRIC2read;
  log << MSG::DEBUG << "Reading CDF detector RIC2" << endreq;
  SicbFortran::utdget("RIC2","G",nRIC2par, intRIC2read );
  for ( iName = 0; iName<nRIC2par; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRIC2read[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RIC2."+RIC2names[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRIC2read;

  // Read RHPD from Rich.cdf
  int nRHPDpar = 22;
  int* intRHPDread = new int[nRHPDpar];
  float* floatRHPDread = (float*) intRHPDread;
  log << MSG::DEBUG << "Reading CDF detector RHPD" << endreq;
  SicbFortran::utdget("RHPD","G",nRHPDpar, intRHPDread );
  for ( iName = 0; iName<nRHPDpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRHPDread[iName]);
    tmpString += tmpChar;
    sprintf(tmpChar,"%d",iName+1);
    std::string tmpName = "CDF.RHPD.HPDQEFF(";
    tmpName += tmpChar;
    tmpName += ")";
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRHPDread;

  // Read RPMT from Rich.cdf
  int nRPMTpar = 21;
  std::string RPMTnames[] = {
    "Lhist","Nacross","Ndown","PMT_pitch","PMT_pixsiz","PMT_colleff",
    "th_lens","r_lens","ri_lens","offset_lens","slant_lens","m_tiltpmt(1)",
    "m_tiltpmt(2)","m_tiltpmt(3)","m_tiltpmt(4)","Ledges","e_mean",
    "aoff_pmt","adc_cut_pmt","enc","Ltrav","Lhpd","mumetalthick"};
  int* intRPMTread = new int[nRPMTpar];
  float* floatRPMTread = (float*) intRPMTread;
  log << MSG::DEBUG << "Reading CDF detector RPMT" << endreq;
  SicbFortran::utdget("RPMT","G",nRPMTpar, intRPMTread );
  for ( iName = 0; iName<nRPMTpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRPMTread[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RPMT."+RPMTnames[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRPMTread;

  // Read RPIX from Rich.cdf
  int nRPIXpar = 25;
  std::string RPIXnames[] = {
    "widpix","npix","NCOL(1)","NROW(1)","IXSTAG(1)","CORNERHPD(1,1)",
    "CORNERHPD(2,1)","NCOL(2)","NROW(2)","IXSTAG(2)","CORNERHPD(1,2)",
    "CORNERHPD(2,2)","widhpd","esig","aoff","calib","sigcut","ince",
    "gapincm","square","pixsiz","peract","winradcurv","winthick","psf"};
  int* intRPIXread = new int[nRPIXpar];
  float* floatRPIXread = (float*) intRPIXread;
  log << MSG::DEBUG << "Reading CDF detector RPIX" << endreq;
  SicbFortran::utdget("RPIX","G",nRPIXpar, intRPIXread );
  for ( iName = 0; iName<nRPIXpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRPIXread[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RPIX."+RPIXnames[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRPIXread;

  // Read RIPR from Rich.cdf
  int nRIPRpar = 61;
  std::string RIPRnames[] = {
    "NRCTKTY","RNNOM(1)","RNNOM(2)","RNNOM(3)","NGEOM","PLSSHPD","PLSSPMT",
    "PGEOEFF","PPIXSIZ","TERHPD(1,1)","TERHPD(2,1)","TERHPD(3,1)",
    "TERHPD(1,2)","TERHPD(2,2)","TERHPD(3,2)","TERHPD(1,3)","TERHPD(2,3)",
    "TERHPD(3,3)","TERPMT(1,1)","TERPMT(2,1)","TERPMT(3,1)","TERPMT(1,2)",
    "TERPMT(2,2)","TERPMT(3,2)","TERPMT(1,3)","TERPMT(2,3)","TERPMT(3,3)",
    "ESCL20(1)","ESCL20(2)","ESCL20(3)","ESCL301(1)","ESCL301(2)",
    "ESCL302(1)","ESCL302(2)","ESCL32","RBOUND(1)","RBOUND(2)","RBOUND(3)",
    "RBOUND(4)","RBINARY","IURCDIG(1)","IURCDIG(2)","IURCDIG(3)","IURCDIG(4)",
    "IURCDIG(5)","IURCREC(1)","IURCREC(2)","IURCREC(3)","IURCREC(4)",
    "IURCREC(5)","ROI(1)","ROI(2)","ROI(3)","R1C301","R1M301","R1C302","R1M302",
    "R1FUDGE","R2C32","R2M32","R2FUDGE"};
  int* intRIPRread = new int[nRIPRpar];
  float* floatRIPRread = (float*) intRIPRread;
  log << MSG::DEBUG << "Reading CDF detector RIPR" << endreq;
  SicbFortran::utdget("RIPR","G",nRIPRpar, intRIPRread );
  for ( iName = 0; iName<nRIPRpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRIPRread[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RIPR."+RIPRnames[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRIPRread;

  // Read RIPI from Rich.cdf
  int nRIPIpar = 26;
  std::string RIPInames[] = {
    "GAMMATH(1)","GAMMATH(2)","GAMMATH(3)",
    "SIGPH(1)","SIGPH(2)","SIGPH(3)",
    "HYPMASS(1)","HYPMASS(2)","HYPMASS(3)","HYPMASS(4)","HYPMASS(5)",
    "CPCOMP(1)","CPCOMP(2)","CPCOMP(3)","CPCOMP(4)","CPCOMP(5)",
    "CVCOMP(1)","CVCOMP(2)","CVCOMP(3)","CVCOMP(4)","CVCOMP(5)",
    "CBEST(1)","CBEST(2)","CBEST(3)","CBEST(4)","CBEST(5)"};
  int* intRIPIread = new int[nRIPIpar];
  float* floatRIPIread = (float*) intRIPIread;
  log << MSG::DEBUG << "Reading CDF detector RIPI" << endreq;
  SicbFortran::utdget("RIPI","R",nRIPIpar, intRIPIread );
  for ( iName = 0; iName<nRIPIpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRIPIread[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RIPI."+RIPInames[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRIPIread;

  // Read RIPH from Rich.cdf
  int nRIPHpar = 36;
  std::string RIPHnames[] = {
    "EMAX(1)","EMAX(2)","EMAX(3)","A0(1)","A0(2)","E0(1)","E0(2)",
    "CCOEF","RNA(1)","RNA(2)","RNA(3)","RNA(4)","RNA(5)","HPDGEOMEFF",
    "HPDQE(1)","HPDQE(2)","HPDQE(3)","HPDQE(4)","HPDQE(5)","HPDQE(6)",
    "HPDQE(7)","HPDQE(8)","HPDQE(9)","HPDQE(10)","HPDQE(11)","HPDQE(12)",
    "HPDQE(13)","HPDQE(14)","HPDQE(15)","HPDQE(16)","HPDQE(17)","HPDQE(18)",
    "HPDQE(19)","HPDQE(20)","HPDQE(21)","HPDQE(22)"};
  int* intRIPHread = new int[nRIPHpar];
  float* floatRIPHread = (float*) intRIPHread;
  log << MSG::DEBUG << "Reading CDF detector RIPH" << endreq;
  SicbFortran::utdget("RIPH","R",nRIPHpar, intRIPHread );
  for ( iName = 0; iName<nRIPHpar; iName++){
    // Add to Map via some formating hocus pocus
    std::string tmpString = "";
    char tmpChar[50];
    sprintf(tmpChar,"%f",floatRIPHread[iName]);
    tmpString += tmpChar;
    std::string tmpName = "CDF.RIPH."+RIPHnames[iName];
    m_parameterMap[tmpName] = tmpString;
    log << MSG::DEBUG << "Read " << tmpName << " as "
        << tmpString << endreq;
  }
  delete intRIPHread;
}

std::string RichParameters::nextLine (istream &ist)
{
  // read in one line
  char cline[1024];
  char eol;
  std::string line;
  while( !ist.eof() && line.size() == 0 ) {
    ist.get(cline,1024,'\n');
    line = cline;
    if ( ist.eof() ) {
      eol = '\n';
    } else {
      ist.get(eol);
    };
  };

  // protection against very long lines
  while ( !ist.eof() && eol != '\n' ) {
    ist.get(cline,1024,'\n');
    line += cline;
    if ( ist.eof() ) {
      eol = '\n';
    } else {
      ist.get(eol);
    };
  };

  return line;

}
