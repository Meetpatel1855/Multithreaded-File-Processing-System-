

#ifndef SENECA_PROCESSDATA_H
#define SENECA_PROCESSDATA_H

#include <string>

namespace seneca
{
	
	void computeVarFactor(const int*, int, int, double, double&);
	void computeAvgFactor(const int*, int, int, double&);

	class ProcessData
	{
		int total_items{};
		int* data{};
		int num_threads{ 0 }; 
		double* averages{ nullptr };  
		double* variances{ nullptr };
		int* p_indices{ nullptr }; 
	public:
		ProcessData(const std::string&, int n_threads);
		ProcessData(const ProcessData&) = delete;
		ProcessData& operator=(const ProcessData&) = delete;
		~ProcessData();
		operator bool() const;
		int operator()(const std::string&, double&, double&);
	};
}

#endif

