#ifndef CHECKPOINTRESTOREWRAPPER_H
#define CHECKPOINTRESTOREWRAPPER_H


/* @class CheckpointRestoreWrapper CheckpointRestoreWrapper.h CPP/CheckpointRestoreWrapper.h
 *
 * Basic interface to interface to checkpoint / restore mechanisms for fast program startup.
 *
 * NOTE:
 * This header structure is explicitly copied to the checkpoint/restart package(s)
 * To decouple them entirely from the application
 *
 * !!!! DO NOT EVEN THINK ABOUT CHANGING IT WITHOUT KNOWNING WHAT YOU DO !!!!
 *
 * @author  M.Frank
 * @version 1.0
 */
class CheckpointRestoreWrapper  {
protected:
  /// Standard constructor
  CheckpointRestoreWrapper() {}
  /// Standard destructor
  virtual ~CheckpointRestoreWrapper() {}
public:
  /// Initialization callback with startup arguments
  virtual void init_instance(int argc, char** argv, char** environ) = 0;
};

void CheckpointRestoreWrapper__init_instance(int argc, char** argv);
CheckpointRestoreWrapper* CheckpointRestoreWrapper__instance();

#endif // CHECKPOINTRESTOREWRAPPER_H
