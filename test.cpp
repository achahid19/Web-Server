#include <iostream>
#include <map>

int	main(void) {
	std::map<std::string, std::string> map;

	map.insert(std::make_pair("key", "value"));
	std::cout << map["key"] << std::endl;

	return 0;
}
