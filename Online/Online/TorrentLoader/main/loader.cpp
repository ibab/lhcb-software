extern "C" int torrent_runserver(int argc, char** argv);
int main(int argc, char** argv)  {
  return torrent_runserver(argc, argv);
}
