#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

#include <iostream>
#include <thread>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <mutex>
#include <future>
#include <chrono>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NUM_OF_CORES 8
#define MAX_PRIME 100000

using namespace std;

void do_primes()
{
    unsigned long i, num, primes = 0;
    for (num = 1; num <= MAX_PRIME; ++num) {
        for (i = 2; (i <= num) && (num % i != 0); ++i);
        if (i == num)
            ++primes;
    }
    printf("Calculated %d primes.\n", primes);
}



int main(){
  string myString = "";
  string line;
  ifstream infile("file.txt");//open the file we're interested in
  if (infile.is_open()){
    while (getline(infile,line)){
      myString = myString + line;//save the content into the myString variable
    }
    infile.close();//close the file
  }
  ofstream outfile;
  outfile.open("out.bin");//open the file we will use for exfiltration later
  for (std::size_t i = 0; i < myString.size(); ++i){ //convert text to binary ready form exfiltration
      outfile << bitset<8>(myString.c_str()[i]);//write the binary into the file out.bin
    }
  outfile.close();//close the binary file


  time_t start, end;
    time_t run_time;
    unsigned long i;
    pid_t pids[NUM_OF_CORES];

    /* start of test */
    start = time(NULL);
    for (i = 0; i < NUM_OF_CORES; ++i) {
        if (!(pids[i] = fork())) {
            do_primes();
            exit(0);
        }
        if (pids[i] < 0) {
            perror("Fork");
            exit(1);
        }
    }
    for (i = 0; i < NUM_OF_CORES; ++i) {
        waitpid(pids[i], NULL, 0);
    }
    end = time(NULL);
    run_time = (end - start);
    printf("This machine calculated all prime numbers under %d %d times "
           "in %d seconds\n", MAX_PRIME, NUM_OF_CORES, run_time);
    return 0;

}
