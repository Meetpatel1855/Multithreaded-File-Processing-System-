#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include "ProcessData.h"
#include "ProcessData.h" // intentional

int cout = 0; // won't compile if headers don't follow convention regarding namespaces

void printHead(std::string str); // prints header text, embeds str into the text
void printFoot(); // prints footer text
void compareFiles(const std::string& fileName1, const std::string& fileName2);

int main(int argc, char** argv)
{
	std::cout << "Command Line:\n";
	std::cout << "--------------------------\n";
	for (int i = 0; i < argc; ++i)
		std::cout << std::setw(3) << i + 1 << ": " << argv[i] << '\n';
	std::cout << "--------------------------\n\n";

	std::string fname_source = "data_int.dat", fname_target = "data_int_target.dat";

	if (argc > 1)
		fname_source = argv[1];
	if (argc > 2)
		fname_target = argv[2];

	{
		::printHead("Average and variance of record-values using 1 thread");
		std::chrono::steady_clock::time_point t_before, t_after;

		t_before = std::chrono::steady_clock::now();
		double avg = 0.0, var = 0.0;
		seneca::ProcessData pd_obj(fname_source, 1); // creates function object 
		if (pd_obj) {
			try {
				pd_obj(fname_target, avg, var); // invokes function object for "data_int.dat"
				::compareFiles(fname_source, fname_target);
			}
			catch (std::string& msg) {
				std::cout << msg << std::endl;
			}
		}
		t_after = std::chrono::steady_clock::now();
		std::cout << "Data statistics (\"" << fname_source << "\"):" << std::endl;
		std::cout.setf(std::ios::fixed);
		std::cout << " Average:  " << std::setw(16) << avg << std::endl;
		std::cout << " Variance: " << std::setw(16) << var << std::endl;
		std::cout.unsetf(std::ios::fixed);
		std::cout << "Time required to compute average and variance: "
			<< (std::chrono::duration_cast<std::chrono::milliseconds>(t_after - t_before)).count()
			<< " milliseconds." << std::endl;
		::printFoot();
		std::cout << std::endl << std::endl;
	}

	{
		::printHead("Average and variance of record-values using 2 threads");
		std::chrono::steady_clock::time_point t_before, t_after;

		t_before = std::chrono::steady_clock::now();
		double avg = 0.0, var = 0.0;
		seneca::ProcessData pd_obj(fname_source, 2); // creates function object 
		pd_obj(fname_target, avg, var); // invokes function object for "data_int.dat"
		t_after = std::chrono::steady_clock::now();

		std::cout << "Data statistics (\"" << fname_source << "\"):" << std::endl;
		std::cout.setf(std::ios::fixed);
		std::cout << " Average:  " << std::setw(16) << avg << std::endl;
		std::cout << " Variance: " << std::setw(16) << var << std::endl;
		std::cout.unsetf(std::ios::fixed);
		std::cout << "Time required to compute average and variance: "
			<< (std::chrono::duration_cast<std::chrono::milliseconds>(t_after - t_before)).count()
			<< " milliseconds." << std::endl;
		::printFoot();
		std::cout << std::endl << std::endl;
	}

	{
		::printHead("Average and variance of record-values using 4 threads");
		std::chrono::steady_clock::time_point t_before, t_after;

		t_before = std::chrono::steady_clock::now();
		double avg = 0.0, var = 0.0;;
		seneca::ProcessData pd_obj(fname_source, 4); // creates function object 
		pd_obj(fname_target, avg, var); // invokes function object for "data_int.dat"
		t_after = std::chrono::steady_clock::now();

		std::cout << "Data statistics (\"" << fname_source << "\"):" << std::endl;
		std::cout.setf(std::ios::fixed);
		std::cout << " Average:  " << std::setw(16) << avg << std::endl;
		std::cout << " Variance: " << std::setw(16) << var << std::endl;
		std::cout.unsetf(std::ios::fixed);
		std::cout << "Time required to compute average and variance: "
			<< (std::chrono::duration_cast<std::chrono::milliseconds>(t_after - t_before)).count()
			<< " milliseconds." << std::endl;
		::printFoot();
	}

	return cout;
}

void printHead(std::string str)
{
	std::cout << std::setw(90) << std::setfill('=') << '=' << std::endl << std::setfill(' ');
	std::cout << std::setw(45 - str.length() / 2) << ' ' << str << std::endl;
	std::cout << std::setw(90) << std::setfill('-') << '-' << std::endl << std::setfill(' ');
}

void printFoot()
{
	std::cout << std::setw(90) << std::setfill('=') << '=' << std::endl;
}

void compareFiles(const std::string& fileName1, const std::string& fileName2)
{
	std::ifstream file1(fileName1), file2(fileName2);
	int cnt = 0;
	while (file1 && file2)
	{
		if (file1.get() != file2.get())
			throw std::string("The output file doesn't have the correct binary content as the input file.\n"
				"  Byte " + std::to_string(cnt) + " is wrong.\n"
				"  Check the 'operator()' implementation to fix.\n");
		++cnt;
	}
	std::cout << "Read [" << cnt << "] bytes from the files; the content is the same.\n";
}
