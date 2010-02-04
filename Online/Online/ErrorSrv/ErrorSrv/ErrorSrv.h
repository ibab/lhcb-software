#ifndef GAUDIONLINE_ERRORSRV_H
#define GAUDIONLINE_ERRORSRV_H

#include <string>

namespace LHCb     {

  enum {
    b_Log       = 0x1,
    b_Display   = 0x2,
    b_Info      = 0x4,
    b_Warning   = 0x8,
    b_Error     = 0x10,
    b_Alarm     = 0x20,
    b_Clear     = 0x40,
    b_Init      = 0x80,
    b_seen      = 0x10000,
    f_Info      = b_Info    +         b_Display,
    f_Warning   = b_Warning + b_Log + b_Display,
    f_Error     = b_Error   + b_Log + b_Display,
    f_Alarm     = b_Alarm   + b_Log + b_Display,
    f_Init      = b_Init    + b_Log,
    f_log_only  = b_Warning + b_Log
  };

  struct DiskRecord {
    int  flag;
    int  time[2];
    int  msg_len;
    int  info_len;
    char node[12];
    char process[64];
    char partition[16];
    char facility[32];
    char failure[32];
    char message[4];
    size_t sizeOf() const {
      return sizeof(DiskRecord) - 4 + msg_len + info_len + 3;
    }
    void setMessage(const std::string& m) {
      msg_len = m.length();
      ::strncpy(message,m.c_str(),msg_len);
      message[msg_len] = 0;
      message[msg_len+1] = 0;
    }
    void setInfo(const std::string& i) {
      info_len = i.length();
      ::strncpy(message+msg_len+1,i.c_str(),info_len);
      message[msg_len+1+info_len] = 0;
      message[msg_len+2+info_len] = 0;
    }
    const char* info()  const {    return message+msg_len+1; }
  };


  class ErrorSrv  {
  public:
    /// Default constructor.
    ErrorSrv();
    /// Destructor.
    virtual ~ErrorSrv();
    /// Initialize the error logger
    int initialize(const std::string& partition, const std::string& facility);
    /// Setup error facility
    int setFacility(const std::string& partition, const std::string& facility);
    /// Report error condition
    int report(const std::string& failure, int flag, const std::string& msg="");
    /// Report with user defined (hidden) information field
    int reportUser(const std::string& failure, int flag, const std::string& msg, const std::string& user_text);
    /// Full report call: all fields need to be specified.
    int fullReport(const std::string& partition, const std::string& facility, const std::string& failure, int flag, const std::string& msg,const std::string& user="");


    /// Register to errors of a given facility
    //int register(int flag, const std::string& partition);
    /// Unregister from errors of a given facility
    //int cancelRegister(int flag, const std::string& partition);

    int open(const std::string& fifo);
  private:
    std::string    m_facility;
    std::string    m_partition;
    int            m_tryN;
    int            m_fifo;
    virtual int print(char *format,...);
  };
}       // End namespace LHCb
#endif  // GAUDIONLINE_ERRORSRV_H
