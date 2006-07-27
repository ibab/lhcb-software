#ifndef LESTER_PRESS_ANY_KEY_H
#define LESTER_PRESS_ANY_KEY_H

#include <stdio.h>
#include <string>
#include <ostream>

inline char pressAnyKey(const std::string & message="Press any key to continue", std::ostream & os = std::cerr) {
	os << message << std::endl;
	system("stty raw");
	const char c = getchar();
	system("stty cooked");
	return c;
};

inline char pressAnyKeyQQuit(const std::string & message="Press q to quit, or any other key to continue.", std::ostream & os = std::cerr) {
	os << message << std::endl;
	system("stty raw");
	const char c = getchar();
	system("stty cooked");
	if (c=='q' || c=='Q') {
	  exit(0);
	};
	return c;
};

#endif
