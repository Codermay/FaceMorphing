#ifndef Error(x)
#define Error(x) {\
	std::cout << "_ERROR_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cout << "        " << x << std::endl; \
	system("Pause"); \
	exit(-1); \
}
#endif
#ifndef Warnning(x)
#define Warnning(x) {\
	std::cout << "_WARNNING_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cout << "           " << x << std::endl; \
}
#endif