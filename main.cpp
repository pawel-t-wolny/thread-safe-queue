/**
  * Assignment: synchronization
  * Operating Systems
  */

/**
  Hint: F2 (or Control-klik) on a functionname to jump to the definition
  Hint: Ctrl-space to auto complete a functionname/variable.
  */

// function/class definitions you are going to use
#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// although it is good habit, you don't have to type 'std::' before many objects by including this line
using namespace std;

// define the configuration parameters that are needed for your program
int numberOfThreads = 2;

// define the global variabeles needed for your program
std::mutex m;
std::vector<bool> primeNumberCandidates;

// Parses a string to form a vector of arguments. The seperator is a space char (' ').
void parseArguments(const std::vector<std::string>& arguments) {
	if (arguments.size() == 0) {
		// this should never be the case (but if it occurs, handle it gracefully)
		abort();
	}
	if (arguments.size() == 1) {
		// 1 argument given, is normally the name of the program itself
		// print help/errors/usage always on the standard error stream, so it is not seen by accident as regular output
		std::cerr << "Usage: " << arguments[0] << " [max-number-to-test] [number-of-threads; default is 2]" << std::endl;
		exit(0);  // exit the program
	}
	// ....
	// to display the arguments on standard error:
	// for (const auto& arg : arguments)
	//   std::cerr << arg << std::endl;
}

void searchPrimeNumbers() {
}

void displayPrimeNumbers() {
}

int main(int argc, char* argv[]) {
	std::vector<std::string> arguments;
	// we have to bridge the C strings to the C++ strings
	// the next line will probably hurt your eyes (don't try to understand it)
	std::copy(&argv[0], &argv[argc], std::back_inserter(arguments));

	// parse the command line
	parseArguments(arguments);

	// execute the program
	searchPrimeNumbers();

	// display the results
	displayPrimeNumbers();

	return 0;
}
