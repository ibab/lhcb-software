#include "ReadFile.h"
using namespace std;



ReadFile::ReadFile(std::string fileName)
{
	std::vector<std::string> info;
	ifstream file(fileName.c_str());
	std::string line;

	while ( getline(file, line))
	{
		info.push_back(line);
	}

	int size = info.size();

	for (int j =0; j<size ; j++){
		vec.push_back(info[j]);
	}

}

std::vector<std::string>& ReadFile::Lines()
{
	return vec;
}
