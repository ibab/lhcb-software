#ifndef FRAMEWORK_STATUS_H
#define FRAMEWORK_STATUS_H


/*
 *    Framework namespace declaration
 */
namespace Framework {

  class Status {
    int m_code;
  public:
    enum { ERROR=0, SUCCESS=1 };
    Status() : m_code(1) {    }
    Status(int code) : m_code(code) {}
    Status(const Status& c) : m_code(c.m_code) {}
    operator int() const { return m_code; }
    bool operator==(const Status& s) const { return m_code == s.m_code; }
    bool operator==(int code) const        { return m_code == code;     }
    bool operator==(long code) const       { return m_code == code;     }
    bool isSuccess() const        { return (m_code&SUCCESS) == SUCCESS; }
  };

}

#endif /* FRAMEWORK_STATUS_H */
