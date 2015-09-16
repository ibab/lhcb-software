
#ifndef RICHKERNEL_ICAMERATOOL_H 
#define RICHKERNEL_ICAMERATOOL_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IHistogramSvc.h"

// Forward declarations
#include <iosfwd>

// ROOT
class TH1;
class TH1D;
class TH2D;
class TF1;

static const InterfaceID IID_ICameraTool ( "ICameraTool", 1, 0 );

/** @class ICameraTool ICameraTool.h ICameraTool.h
 *   
 *
 *  @author Claus Buszello
 *  @author Nicola Mangiafave
 *  @date   2007-05-30
 */
class ICameraTool : virtual public IAlgTool 
{

public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICameraTool; }

public:
  
  enum MessageLevel { NONE = 0, 
                      INFO = 1,
                      WARNING = 2, 
                      ERROR = 3,
                      WARNING_PVSS = 4,
                      ERROR_PVSS = 5,
                      CAM_COMMAND = 9 
                     };

public:
  
  virtual int SendAndClear(MessageLevel o_l,const std::string& o_who,const std::string& o_what)=0;
  virtual int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what)=0; 

  virtual int Append(TH2D * H,const char *opts=NULL)=0;
  inline  int Append( const TH2D * H,const char *opts=NULL)
  { return Append( const_cast<TH2D*>(H), opts ); }

  virtual int Append(TH1D * H,const char *opts=NULL)=0;
  inline  int Append( const TH1D * H,const char *opts=NULL)
  { return Append( const_cast<TH1D*>(H), opts ); }

  virtual int Append(TH1 * H,const char *opts=NULL)=0;
  inline  int Append( const TH1 * H,const char *opts=NULL)
  { return Append( const_cast<TH1*>(H), opts ); }
 
  virtual int Append(TF1  * F,const char *opts=NULL)=0;

  virtual int Append(const char * T,const char * C)=0;
  inline  int Append(const char * T,const std::string& C) { return Append(T,C.c_str()); }
  inline  int Append(const std::string& T,const std::string& C) { return Append(T.c_str(),C.c_str()); }

  virtual int Append(const char * C)=0;
  inline  int Append(const std::string & C) { return Append(C.c_str()); }

  virtual int Append(const char * T,void * C, int sz)=0;
  inline  int Append(const std::string& T,void * C, int sz) { return Append(T.c_str(),C,sz); }

  // For unknown reasons if I put the following method declaration just 
  // after MessageLevel they give seg fault
  virtual int SendAndClear(MessageLevel o_l,const std::string& o_who,const std::string& o_what, 
                           int messagePeriod)=0;
  virtual int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what, 
                             int messagePeriod)=0; 

  virtual int SetCameraToPVSSConfig(bool sendMessagesToPVSS = true, 
                                    MessageLevel warning_PVSS = ICameraTool::WARNING, 
                                    MessageLevel error_PVSS = ICameraTool::ERROR)=0;

  virtual int SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what, 
                           MessageLevel p_l,const std::string& p_who,const std::string& p_what)=0;

  virtual int SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what, 
                             MessageLevel p_l,const std::string& p_who,const std::string& p_what)=0;

  virtual int SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what, 
                           MessageLevel p_l,const std::string& p_who,const std::string& p_what, 
                           int messagePeriod)=0;

  virtual int SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what, 
                             MessageLevel p_l,const std::string& p_who,const std::string& p_what, 
                             int messagePeriod)=0;

  virtual void Clear() = 0;

};

std::ostream& operator<<(std::ostream &, ICameraTool::MessageLevel);

#endif // RICHKERNEL_ICAMERATOOL_H
