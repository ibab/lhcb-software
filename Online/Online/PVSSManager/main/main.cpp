namespace PVSS {
  int pvss_exec_manager(int argc, const char **argv);
}
int main (int argc, char** argv)  {
  return PVSS::pvss_exec_manager(argc,(const char**)argv);
}
