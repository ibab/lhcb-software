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
  /// Set the printout level for the checkpoint/restore mechanism
  virtual int setPrint(int new_level) = 0;
  /// Give access to the restart type after a call to "checkpoint". 0==checkpointing, 1=restart from checkpoint
  virtual int restartType() const = 0;
  /// Stop all threads to allow producing a consistent checkpoint
  virtual int stop()            = 0;
  /// Resume all threads after the checkpoint was produced
  virtual int resume()          = 0;
  /// Fork child instance
  virtual int forkInstance()    = 0;
  /// Child fork: Re-establish child to running state
  virtual int startChild()      = 0;
  
  /// Create a checkpoint file by writing to file descriptor fd
  virtual int checkpoint(int fd)= 0;

  /// Save file descriptor information to opaque memory block (allocated inside!)
  virtual int getFileDescriptors(void** ptr) = 0;
  /// Restore file descriptors from opaque memory block
  virtual int setFileDescriptors(void* ptr)  = 0;
  /// For debugging: Dump file descriptors to stdout
  virtual int dumpFileDescriptors(void* ptr) = 0;
};

CheckpointRestoreWrapper& CheckpointRestoreWrapper__instance();

#endif // CHECKPOINTRESTOREWRAPPER_H
