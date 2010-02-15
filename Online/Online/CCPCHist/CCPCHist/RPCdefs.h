#ifndef RPCDEFS_H
#define RPCDEFS_H
enum RPCCommType
{
  RPCCIllegal,
  RPCCRead,
  RPCCReadAll,
  RPCCClear,
  RPCCClearAll,
  RPCCDirectory
};
class RPCComm
{
public:
  RPCCommType c;
  //char RPCCommand;
};
class RPCReply
{
public:
  int status;
  RPCCommType comm;
};
class RPCCommRead : public RPCComm
{
public:
  char which;
};
class RPCCommClear : public RPCComm
{
public:
  char which;
};
#endif
