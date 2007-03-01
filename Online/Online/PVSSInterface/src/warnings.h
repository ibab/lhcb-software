#ifdef _WIN32
  // Disable warning C4786: identifier was truncated to '255' characters in the debug information
  #pragma warning ( disable : 4786 )
  // Disable warning C4291: no matching operator delete found; memory will not be freed if initialization throws an exception
  #pragma warning ( disable : 4291 )
  // Disable warning C4250: inheritance via dominance
  #pragma warning ( disable : 4250 )
#endif
