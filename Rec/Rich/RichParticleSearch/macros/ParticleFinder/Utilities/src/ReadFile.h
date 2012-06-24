#ifndef READFILE_H
#define READFILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class ReadFile
{
public:
	ReadFile(std::string fileName);
	std::vector<std::string>& Lines();
private:
	std::vector<std::string> vec;
};


#endif
