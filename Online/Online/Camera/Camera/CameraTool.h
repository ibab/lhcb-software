// $Id: CameraTool.h,v 1.1.1.1 2009-02-02 11:45:34 rogers Exp $
#ifndef CAMERATOOL_H
#define CAMERATOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Camera/ICameraTool.h"            // Interface

// CAMERA.
#include "container.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

//#include "AIDA/IHistogram1D.h"
//#include "GaudiUtils/Aida2ROOT.h"

// Forward declarations
class client;
class TH1D;
class TH2D;

/** @class CameraTool CameraTool.h CameraTool.h
 *
 *  @author Claus Buszello
 *  @date   2007-05-30
 */

class CameraTool : public GaudiTool,
                   virtual public ICameraTool
{

public:
  /// Standard constructor
  CameraTool( const std::string& type,
              const std::string& name,
              const IInterface* parent );
 
  virtual ~CameraTool( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  int SendAndClear(int l,const std::string& who,const std::string& what);
  int SendAndClearTS(int l,const std::string& who,const std::string& what);
  int Append(TH2D * H, const char * opts = NULL);
  int Append(TH1D * H, const char * opts =NULL);
  int Append(const char * T,const char * C);
  int Append(const char * T,void * C, int sz);
  int Append(const char * C);
  int Append(const std::string & C);

private:

  std::string m_servername;
  int m_servport;
  client *m_camc;
  container m_out;
  bool m_dosend;
  unsigned int m_lastHistoNum;

};

#endif
