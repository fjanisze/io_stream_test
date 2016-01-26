#include <iostream>
#include <chrono>
using namespace std;

constexpr int buffer_size{ 1<<9 };
constexpr int num_of_runs{ 500000 };
char buffer[buffer_size];

int main()
{
	cout<<"Buffer size: "<<buffer_size<<endl;
	std::ios::sync_with_stdio(false);
	auto start = std::chrono::high_resolution_clock::now();
	for(int i{0};i<num_of_runs;i++){
		scanf("%s",buffer);
		//cin >> buffer;
	}
	auto end = std::chrono::high_resolution_clock::now();
	cout<<"Result: "<<chrono::duration_cast<chrono::nanoseconds>(end-start).count()/num_of_runs<<endl;

}
