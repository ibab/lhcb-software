// $Id: CameraTool.h,v 1.4 2009-08-26 10:50:11 rogers Exp $
#ifndef CAMERATOOL_H
#define CAMERATOOL_H 1

// Include files
// Interface
#include "Camera/ICameraTool.h"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"

// CAMERA.
#include "container.h"

// from std
#include <string>
#include <vector>

// Forward declarations
class client;
class TH1D;
class TH2D;

/** @class CameraTool CameraTool.h CameraTool.h
 *     A GaudiTool that can be used by algorithms to send messages to the CAMERA server.
 *     The tool has three properties:
 *     \li ServerName This is the name of the PC where the CAMERA server is connected to.
 *     \li ServerPort This is the port number to connect to the CAMERA server on. Default
 *           is nearly always appropriate for this.
 *     \li Enabled With this set to true (default) then messages will be saved and sent by
 *           this tool. If false then any appended messages and attempts to send messages
 *           will have no action and just return success.
 *  @author Claus Buszello
 *  @date   2007-05-30
 */

class CameraTool : public GaudiTool,
                   virtual public ICameraTool
{

  int numErrBZ;
  int numErrCN;

public:
  /// Standard constructor
  CameraTool( const std::string& type,
              const std::string& name,
              const IInterface* parent );
 
  virtual ~CameraTool( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /*!
   * Sends all the information that has been added via Append functions since the last SendAndClear.
   *
   * \param  l Message level. 1,2,3 represent info, warning and error messages respectively.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClear(MessageLevel l,const std::string& who,const std::string& what);

  /*!
   * Wrapper for the SendAndClear function, it additionally sends a time stamp with the message.
   *
   * \param  l Message level. 1,2,3 represent info, warning and error messages respectively.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what);

  /*!
   * Adds a ROOT 2D histogram of doubles to be sent by CAMERA.
   *
   * \param  H Histogram to be sent.
   * \param  opts ROOT options to be applied to the histogram. Defaults to NULL.
   * \return int Returns 1.
   */
  int Append(TH2D * H, const char * opts = NULL);

  /*!
   * Adds a ROOT 1D histogram of doubles to be sent by CAMERA. 
   *
   * \param  H Histogram to be sent.
   * \param  opts ROOT options to be applied to the histogram. Defaults to NULL.
   * \return int Returns 1.
   */
  int Append(TH1D * H, const char * opts =NULL);

  /*!
   * Sends a character buffer to CAMERA.
   *
   * \param  T Identifier for the type of information being sent to CAMERA.
   * \param  C Character buffer that is to be sent to CAMERA.
   * \return int Returns 1.
   */
  int Append(const char * T,const char * C);

  /*!
   * Sends a typeless buffer to CAMERA.
   *
   * \param  T Identifier for the type of information being sent to CAMERA.
   * \param  C The buffer that is to be sent to CAMERA.
   * \param  sz Size of the buffer that is to be sent.
   * \return int Returns 1.
   */
  int Append(const char * T,void * C, int sz);

  /*!
   * Sends a text message to CAMERA.
   *
   * \param  C The C style string that is to be sent to CAMERA.
   * \return int Returns 1.
   *
   * The identifier is set by this function to TEXT.
   */
  int Append(const char * C);

  /*!
   * Sends a text message to CAMERA.
   *
   * \param  C A standard string that is to be sent to CAMERA.
   * \return int Returns 1.
   *
   * The identifier is set by this function to TEXT.
   */
  int Append(const std::string & C);

private:

  std::string m_servername; ///! Name of the server to connect to.
  int m_servport; ///! Port to connect to the server on.
 
  std::vector<std::string> m_servernames; ///! Names of the servers to connect to.
  std::vector<int> m_servports; ///! Ports to connect to the servers on.
  
  client *m_camc; ///! This is the client that interfaces with the CAMERA server and handles the sending of messages.
  std::vector<client *> m_clients; //! This is a list of possible client we start from the front and if that one is busy or down the next one is used.
  container m_out; ///! Holds messages that are to be sent to the CAMERA server.
  bool m_dosend; ///! Indicates if the messages are to be stored and sent to CAMERA or not.
  unsigned int m_lastHistoNum; ///! Part of the title hack required until MonObjects are used. Notes how many histograms are to sent.

};

#endif
