#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cstring>

#include <iostream>
#include <thread>
#include <vector>
#include <utility>
#include <algorithm>
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
#define NUM_OF_CORES 12
#define MAX_PRIME 1000000

using namespace std;

void primes();

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
    //hamming code maybe??

      outfile << bitset<8>(myString.c_str()[i]);//write the binary into the file out.bin
    }
  outfile.close();//close the binary file

  //move content of out.bin into a vector
  string filename("out.bin");
  vector<char> bytes;
  FILE* input_file = fopen(filename.c_str(), "r");
  if (input_file == nullptr){
    return EXIT_FAILURE;
  }
  char exif_file[] = {};
  unsigned char character = 0;
  int cursor = 0;
  while (!feof(input_file)){
    character = getc(input_file);
    exif_file[cursor] = character;
    cursor++;
  }
  exif_file[cursor] = '\0';

  //let's see the content of the variable - correct
  cout << strlen(exif_file)-1 << " characters to exfiltrate\n"; //wrong number - sorted
  cout << "Content of exif_file variable: ";
  for (int w=0; w<strlen(exif_file)-1; w++){
    cout << exif_file[w];
  }
  cout << "\n";
// all good up to this point
  time_t start, end;
  time_t run_time;
  unsigned long i;
  pid_t pids[NUM_OF_CORES];

  /* start of test */
  start = time(NULL);
  for (i = 0; i < NUM_OF_CORES; ++i) {
      if (!(pids[i] = fork())) {
          primes();
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
//calculate prime numbers
void primes()
{
    unsigned long i, num, primes = 0;
    cout << "process id " << getpid() << "\n";
    for (num = 1; num <= MAX_PRIME; num++) {
        for (i = 2; (i <= num) && (num % i != 0); i++);
        if (i == num)
            primes++;
    }
    printf("Calculated %d primes.\n", primes);
}
