#include <iostream>
#include <functional>
#include <string>

int main(int argc, char *argv[])
{
	std::hash<std::string> hash;
	size_t n = hash("camera description");
	std::cout << n << std::endl;

	return 0;
}