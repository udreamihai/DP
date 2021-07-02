
// screen -L -Logfile file.txt  /dev/ttyACM0 9600

#include <iostream>
#include <fstream>
#include <vector>

#include <sstream>
#include <bitset>
#include <string>

#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;



string BinaryStringToText(string binaryString) {
    string text = "";
    stringstream sstream(binaryString);
    while (sstream.good())
    {
        bitset<8> bits;
        sstream >> bits;
        text += char(bits.to_ulong());
    }
    return text;
}



int main()
{
    std::ifstream fin("file.txt");

    std::vector<float> data; //use vector because we don't know the lenght
    std::vector<int> spikedata; //store the distance between spikes
    std::vector<int> binarymessage;

    float element;
    while (fin >> element)
    {
        data.push_back(element); //build our vector
        //std::cout << element << std::endl;
    }
    float benchmark = data[1];
    for (std::vector<float>::size_type i = 0; i != data.size()-1; i++) {
        //std::cout << data[i] << std::endl;
        if (benchmark*1.5 < data[i]){
          if (benchmark*1.5 > data[i+1]){
            spikedata.push_back(i);
          }
          //std::cout << "Spike detected at element " << i << endl;
        }
    }
    for (std::vector<int>::size_type i=0; i != spikedata.size()-1; i++){
      //std::cout << spikedata[i] << std::endl;
      //std::cout << spikedata[i+1] << " - " << spikedata[i] << " = " << spikedata[i+1] - spikedata[i] << ", adding ";
      if (spikedata[i+1] - spikedata[i] <= 15){
        //std::cout << " 0" << std::endl;
        binarymessage.push_back(0);
      }
      else if (spikedata[i+1] - spikedata[i] > 15){
        //std::cout << " 1" << std::endl;
        binarymessage.push_back(1);
      }
    }
    for (std::vector<int>::size_type i = 0; i != binarymessage.size(); i++){
      //std::cout << binarymessage[i] << " ";
    }
    //std::cout << std::endl;


    std::ostringstream oss;
    if (!binarymessage.empty()){
      // Convert all but the last element to avoid a trailing ","
      std::copy(binarymessage.begin(), binarymessage.end()-1,
          std::ostream_iterator<int>(oss, ""));

      // Now add the last element with no delimiter
      oss << binarymessage.back();
    }

    //std::cout << oss.str() << std::endl;
    std::cout << "Received message: " << BinaryStringToText(oss.str());
}
