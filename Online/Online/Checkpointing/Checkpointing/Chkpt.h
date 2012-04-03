#ifndef CHECKPOINTING_CHKPT_H
#define CHECKPOINTING_CHKPT_H


extern "C" {
  int  checkpointing_write_checkpoint(int fd);
  int  checkpointing_restart_type();
  int  checkpointing_show_sig_actions();
  int  checkpointing_init_checkpoints();
  int  checkpointing_initialize_parent(int argc, char** argv, char** environment);
  int  checkpointing_resume_process();
  int  checkpointing_stop_process();
  int  checkpointing_set_utgid(const char* new_utgid);
  int  checkpointing_force_utgid(const char* new_utgid);
  int  checkpointing_set_print_level(int new_level);
  int  checkpointing_update_environment();
  int  checkpointing_dump_file_descriptors(void* ptr);
  int  checkpointing_set_file_descriptors(void* ptr);
  int  checkpointing_get_file_descriptors(void** ptr);
  int  checkpointing_start_child();
  int  checkpointing_fork_process();
  int  checkpointing_current_thread_id();
  int  checkpointing_set_lib_directory(const char* lib_dir);
  int  checkpointing_set_save_flags(int flags);
  int  checkpointing_set_restart_flags(int flags);
}

#endif // CHECKPOINTING_CHKPT_H
