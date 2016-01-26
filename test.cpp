#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <tuple>
#include <vector>
using namespace std;

typedef long long ll;
const ll buffer_size{ 1 << 9 };
const int num_of_runs{ 500000 };
char buffer[buffer_size];

void generate_test_data()
{
	for(ll i{0};i<buffer_size;i++)
	{
		buffer[i] = 'a'+i%26;
	}
	buffer[buffer_size-1] = ' ';
}

void run_cout_endl_test()
{
	for(int i{0};i<num_of_runs;i++)
		cout<<buffer<<endl;
}


void run_cout_newline_test()
{
	for(int i{0};i<num_of_runs;i++)
		cout<<buffer<<'\n';
}

void run_printf_test()
{
	for(int i{0};i<num_of_runs;i++)
		printf("%s\n",buffer);
}

string run_test(const std::string& text,
		std::function<void()> test)
{
	stringstream output_str;
	auto start = chrono::high_resolution_clock::now();
	test();
	auto stop = chrono::high_resolution_clock::now();
	output_str<<text<<" "<<chrono::duration_cast<chrono::nanoseconds>(stop-start).count()/num_of_runs<<"ns"<<endl;
	return output_str.str();
}

namespace{
constexpr int test_name_idx{ 0 };
constexpr int test_function_idx{ 1 };
constexpr int test_time_idx{ 2 };
}

class benchmark
{
	using test_function = std::function<void()>;
	vector<tuple<
		string, //Name of the test
		test_function, //'reference' to the test function
		ll //Test execution time
		>> tests;
	void run_tests(){
		for(auto& current_test:tests)
		{
			auto start = chrono::high_resolution_clock::now();
			get<test_function_idx>(current_test)();
			auto stop = chrono::high_resolution_clock::now();
			get<test_time_idx>(current_test) = chrono::duration_cast<chrono::nanoseconds>(stop-start).count()/num_of_runs;
		}
	}
public:
	void add_test(const string& name,test_function test){
		tests.emplace_back(make_tuple(name,test,0));
	}

	string collect_results(){
		run_tests();
		int best_time_idx{0};
		for(size_t i{0};i<tests.size();i++)
		{
			if(get<test_time_idx>(tests[i])<get<test_time_idx>(tests[best_time_idx]))
				best_time_idx = i;
		}
		auto best_time = get<test_time_idx>(tests[best_time_idx]);
		stringstream output;
		//Update the relative execution time values
		//and prepare the output sstring
		for(size_t i{0};i<tests.size();i++)
		{
			double current_time = get<test_time_idx>(tests[i]);
			double relative_time = (current_time>=best_time ? current_time/best_time : best_time/current_time);
			output<<"Executing \""<<get<test_name_idx>(tests[i])<<"\": "<<current_time<<"ns ("<<relative_time<<")"<<endl;
		}
		return output.str();
	}
};

int main(int argc,char** argv)
{
	string user_message;
	if(argc>1){
		//Assuming this will always work :)
		user_message = argv[1];
	}
	generate_test_data();
	benchmark bench;
	bench.add_test("COUT with ENDL",run_cout_endl_test);
	bench.add_test("COUT with NL",run_cout_newline_test);
	bench.add_test("PRINTF",run_printf_test);

	ofstream output("results.txt", std::ios::app | std::ios::out);
	if(!output) throw runtime_error("Unable to open the results.txt file\n");
	if(!user_message.empty())
		output<<"## "<<user_message<<endl;
	output<<"Buffer size: "<<buffer_size<<" bytes, number of iterations "<<num_of_runs<<"\n";

	//Run the tests and write to file the output
	output << bench.collect_results();
	output << "Turning OFF the STDIO SYNC:\n";
	std::cout.sync_with_stdio(false);
	output << bench.collect_results();
}
