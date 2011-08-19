// $Id: CameraTool.h,v 1.10 2010-03-24 17:08:10 nmangiaf Exp $
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
#include <map>

// Forward declarations
class client;
// ROOT
class TH1D;
class TH2D;
// DIM
class DimService;
class DimCommand;

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
 *  @author Nicola Mangiafave
 *  @date   2010-02-26
 */

class CameraTool : public GaudiTool,
                   virtual public ICameraTool {

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
   * Sets the configurations parameters to handle the message sending to both Camera and PVSS.
   * To be stated before sending a message.
   * It sets the parameters of the tool. If the tool is shared among different algorithms,
   * it will change the parameters for ALL the algorithms. If you don't want that,
   * remember to reset the parameters at the end of the execute method.
   *
   * \param  sendMessagesToPVSS Sets m_SendMessagesToPVSS.
   * \param  warning_PVSS Sets m_WARNING_PVSS
   * \param  error_PVSS Sets m_ERROR_PVSS
   * \return int Returns 1.
   */
  int SetCameraToPVSSConfig(bool sendMessagesToPVSS = true,
                            MessageLevel warning_PVSS = ICameraTool::WARNING,
                            MessageLevel error_PVSS = ICameraTool::ERROR);
  /*!
   * Sends a message both to Camera and to PVSS.
   * Sends to Camera all the information that has been added via Append functions since the last SendAndClear.
   * The first three parameters refer to Camera, the last three refer to PVSS
   * \param  c_l Message level. 1,2,3 represent info, warning, error messages respectively.
   * \param  c_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  c_what One line message summary to send, briefly describing reason for the message.
   * \param  p_l Message level. 4,5 represent warning_PVSS and error_PVSS messages respectively.
   *           The message is sent to PVSS regardless m_SendMessagesToPVSS.
   * \param  p_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  p_what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                   MessageLevel p_l,const std::string& p_who,const std::string& p_what);

  /*!
   * Sends a message both to Camera and to PVSS.
   * Sends to Camera all the information that has been added via Append functions since the last SendAndClear.
   * The first three parameters refer to Camera, the last three refer to PVSS
   * \param  c_l Message level. 1,2,3 represent info, warning, error messages respectively.
   * \param  c_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  c_what One line message summary to send, briefly describing reason for the message.
   * \param  p_l Message level. 4,5 represent warning_PVSS and error_PVSS messages respectively.
   *           The message is sent to PVSS regardless m_SendMessagesToPVSS.
   * \param  p_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  p_what One line message summary to send, briefly describing reason for the message.
   * \param  messagePeriod Minimal number of seconds between two identical messages.
   * \return int Returns 1.
   */
  int SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                   MessageLevel p_l,const std::string& p_who,const std::string& p_what,
                   int messagePeriod);

  /*!
   * Sends a message both to Camera and to PVSS. It additionally sends to Camera a time stamp.
   * Sends to Camera all the information that has been added via Append functions since the last SendAndClear.
   * The first three parameters refer to Camera, the last three refer to PVSS
   * \param  c_l Message level. 1,2,3 represent info, warning, error messages respectively.
   * \param  c_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  c_what One line message summary to send, briefly describing reason for the message.
   * \param  p_l Message level. 4,5 represent warning_PVSS and error_PVSS messages respectively.
   *           The message is sent to PVSS regardless m_SendMessagesToPVSS.
   * \param  p_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  p_what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                     MessageLevel p_l,const std::string& p_who,const std::string& p_what);

  /*!
   * Sends a message both to Camera and to PVSS. It additionally sends to Camera a time stamp.
   * Sends to Camera all the information that has been added via Append functions since the last SendAndClear.
   * The first three parameters refer to Camera, the last three refer to PVSS
   * \param  c_l Message level. 1,2,3 represent info, warning, error messages respectively.
   * \param  c_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  c_what One line message summary to send, briefly describing reason for the message.
   * \param  p_l Message level. 4,5 represent warning_PVSS and error_PVSS messages respectively.
   *           The message is sent to PVSS regardless m_SendMessagesToPVSS.
   * \param  p_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  p_what One line message summary to send, briefly describing reason for the message.
   * \param  messagePeriod Minimal number of seconds between two identical messages.
   * \return int Returns 1.
   */
  int SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                     MessageLevel p_l,const std::string& p_who,const std::string& p_what,
                     int messagePeriod);

  /*!
   * Sends all the information that has been added via Append functions since the last SendAndClear.
   * \param  o_l Message level. 1,2,3,4,5 represent info, warning, error, warning_PVSS and error_PVSS
   *         messages respectively. The message is always sent at least to Camera. If l = 2 or 3
   *         the message is sent to PVSS only if m_SendMessagesToPVSS == true. If l = 4 or 5
   *         the message is sent to PVSS.
   * \param  o_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  o_what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClear(MessageLevel o_l,const std::string& o_who,const std::string& o_what);

  /*!
   * Sends all the information that has been added via Append functions since the last SendAndClear.
   * \param  o_l Message level. 1,2,3,4,5 represent info, warning, error, warning_PVSS and error_PVSS
   *         messages respectively. The message is always sent at least to Camera. If l = 2 or 3
   *         the message is sent to PVSS only if m_SendMessagesToPVSS == true. If l = 4 or 5
   *         the message is sent to PVSS.
   * \param  o_who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  o_what One line message summary to send, briefly describing reason for the message.
   * \param  messagePeriod Minimal number of seconds between two identical messages.
   * \return int Returns 1.
   */
  int SendAndClear(MessageLevel o_l,const std::string& o_who,const std::string& o_what,
                   int messagePeriod);

  /*!
   * Wrapper for the SendAndClear function, it additionally sends a time stamp with the message.
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error, warning_PVSS and error_PVSS
   *         messages respectively. The message is always sent at least to Camera. If l = 2 or 3
   *         the message is sent to PVSS only if  m_SendMessagesToPVSS == true. If l = 4 or 5
   *         the message is sent to PVSS.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what);

  /*!
   * Wrapper for the SendAndClear function, it additionally sends a time stamp with the message.
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error, warning_PVSS and error_PVSS
   *         messages respectively. The message is always sent at least to Camera. If l = 2 or 3
   *         the message is sent to PVSS only if  m_SendMessagesToPVSS == true. If l = 4 or 5
   *         the message is sent to PVSS.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \param  messagePeriod Minimal number of seconds between two identical messages.
   * \return int Returns 1.
   */
  int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what,
                     int messagePeriod);

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

  /// Clear all messages without sending
  void Clear();

private:
  std::string m_servername; ///! Name of the server to connect to.
  int m_servport; ///! Port to connect to the server on.

  std::vector<std::string> m_servernames; ///! Names of the servers to connect to.
  std::vector<int> m_servports; ///! Ports to connect to the servers on.

  client *m_camc; ///! This is the client that interfaces with the CAMERA server and handles the sending of messages.
  DimService *m_DIMService; ///! DIM service to publish messages on PVSS
  std::vector<client *> m_clients; //! This is a list of possible client we start from the front and if that one is busy or down the next one is used.
  container m_out; ///! Holds messages that are to be sent to the CAMERA server.
  bool m_dosend; ///! Indicates if the messages are to be stored and sent to CAMERA or not.
  unsigned int m_lastHistoNum; ///! Part of the title hack required until MonObjects are used. Notes how many histograms are to sent.
  MessageLevel m_msgLev; //!< Camera message level.
  std::string m_who; //<! Name of the partition, service, algorithm or tasks that sends the message to Camera.
  std::string m_what; //<! Message body sent to Camera.
  MessageLevel m_PVSSmsgLev; //!< PVSS message level.
  std::string m_PVSSwho; //<! Name of the partition, service, algorithm or tasks that sends the message to PVSS.
  std::string m_PVSSwhat; //<! Message body sent to PVSS.
  bool m_SendMessagesToPVSS; ///! Send error messages to PVSS when the message level is <=  3
  MessageLevel m_WarningPVSS; ///! Ex. if WarningPVSS = ICameraTool::INFO at a message with camera message level = INFO will correspond a PVSS message with WARNIG level.
  std::string m_WARNING_PVSS; ///! Property for m_WarningPVSS. It's a string, i.e. if you want WarningPVSS = ICameraTool::INFO you need WARNING_PVSS = "INFO"
  MessageLevel m_ErrorPVSS; ///! Ex. if ErrorPVSS = ICameraTool::INFO at a message with camera message level = INFO will correspond a PVSS message with ERROR level.
  std::string m_ERROR_PVSS; ///! Property for m_ErrorPVSS. It's a string, i.e. if you want ErrorPVSS = ICameraTool::INFO you need ERROR_PVSS = "INFO"
  bool m_skipCameraToPVSSFlag; ///! skip the CameraToPVSS call in SendAndClear(MessageLevel l,const std::string& o_who,const std::string& o_what)
  std::string m_message; ///! Message to be sent to PVSS
  int m_messagePeriod; ///! Minimal number of seconds between two identical messages.
  std::map<std::string, std::map<std::string, std::vector<int> > > m_MessageOwnerLevelMap; ///! Keys: message owner (who) + message level (l), Values: m_MessageBodyMap.
  std::map<std::string, std::vector<int> > m_MessageBodyMap; ///! Keys: message body with no numbers (StripMessage(what)), Values: vetor[0]=time last message received, vector[1]=number of previous messages aborthed.
  bool m_MessageRateCheckFlag; ///! if true the message rate is checked
  time_t m_currentTime; ///! Time arrival of the last message.
  int m_HistoToEntries; ///! Numbers of entries equivalent to the size filled by an histogram (318 KB).
  int m_MaxTextEntries; ///! Maximum number of entries in a text only appended message. Calculated by considering a message period of 5 seconds.
  int m_MaxNMessagesPerAlg; ///! Maximum number of different messages an algorithm can send. (different == the text in the message body is different, the numbers in the message body are not checked).
  /*!
   * It removes the number from the message body (what).
   *
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \return std::string Returns the string what without numbers.
   */
  std::string StripMessage(const std::string &what);
  /*!
   * It checks the rate at which the message is sent. If the previous identical message
   * was sent less than m_messagePeriod seconds before, the message must not be sent and the attached
   * extended info are deleted. The number of eliminated messages is reported in the extra info of the
   * next identical message sent.
   * N.B.: Two messages are identical if they have the same 1 and who plus what
   * must differ only by numbers.
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error messages respectively.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \param  messagePeriod Minimal number of seconds between two identical messages.
   * \param  IsPVSSMessageFlag If true that's a PVSS message ==> no extrainfo is appended.
   * \return int Returns true if the message must be sent, false if it must not.
   */
  bool MessageRateCheck(const MessageLevel l, 
                        const std::string& who,
                        const std::string& what,
                        const int messagePeriod,
                        const bool IsPVSSMessageFlag = false);
  /*!
   * Given a MessageLevel (int) it gives back the corresponding text suited for PVSS messages.
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error messages respectively.
   * \return std::string Returns the corresponding text suited for PVSS messages.
   *
   */
  std::string NumToTextMessage(const MessageLevel l);
  /*!
   * Replaces m_msgLev, m_who and m_what with the 3 parameters.
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error messages respectively.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   *
   */
  void ReplaceMessageParameters(const MessageLevel l, 
                                const std::string& who, 
                                const std::string& what);
  /*!
   * Replaces m_msgLev, m_who, m_what with, m_PVSSmsgLev, m_PVSSwho and m_what with the 6 parameters
   * \param  l Message level. 1, 2, 3 represent info, warning, error messages respectively.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \param  PVSSl Message level. 4, 5 represent warning_PVSS, error_PVSS messages respectively.
   * \param  PVSSwho Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  PVSSwhat One line message summary to send, briefly describing reason for the message.
   */
  void ReplaceMessageParameters(const MessageLevel l, 
                                const std::string& who, 
                                const std::string& what, 
                                const MessageLevel PVSSl, 
                                const std::string& PVSSwho, 
                                const std::string& PVSSwhat);
  /*!
   * Replaces m_PVSSmsgLev, m_PVSSwho and m_PVSSwhat with the 3 parameters
   * \param  PVSSl Message level. 4, 5 represent warning_PVSS, error_PVSS messages respectively.
   * \param  PVSSwho Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  PVSSwhat One line message summary to send, briefly describing reason for the message.
   */
  void ReplacePVSSMessageParameters(const MessageLevel PVSSl, 
                                    const std::string& PVSSwho, 
                                    const std::string& PVSSwhat);
  /*!
   * Given a Camera message level (between 1 and 3) returns a PVSS message level based on
   * m_WarningPVSS and m_ErrorPVSS.
   *
   * \param  l Message level.
   * \return int Returns 0 if l is != m_WarningPVSS or m_ErrorPVSS, otherwise return 1.
   *
   */
  int CameraToPVSSMessageLevel(const MessageLevel l);
  /*!
   * Handles the message sending to both Camera and PVSS
   *
   * \param  l Message level. 1,2,3,4,5 represent info, warning, error, warning_PVSS and error_PVSS
   *         messages respectively. The message is always sent at least to Camera. If l = 2 or 3
   *         the message is also sent to PVSS only if  m_SendMessagesToPVSS == true. If l = 4 or 5
   *         the message is sent also to PVSS.
   * \param  who Used to identify the sender of the message. Typically set to the name of the
   *           submitting algorithm.
   * \param  what One line message summary to send, briefly describing reason for the message.
   * \return int Returns 1.
   */
  int CameraToPVSS(const MessageLevel l,
                   const std::string& who,
                   const std::string& what, 
                   const int messagePeriod = 0);
  /*!
   * Sends the message to PVSS. This function is used internally, the users should use SendAndClear or
   * SendAndClearTS. The parameters are m_PVSSmsgLev, m_PVSSwho and m_PVSSwhat.
   *
   */
  int SendToPVSS(int messagePeriod = 0);
};

#endif
