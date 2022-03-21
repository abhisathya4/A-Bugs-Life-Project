#include <iostream>
#include "../includes/AntProc.h"

using namespace std;
using namespace cv;

////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

    //    if (argc < 2) {
//        std::cerr << "Uh oh, the file passed was not in the correct format\n"
//                  << "Here is an example of a CORRECT input for A Bug's Life\n"
//                  << "/Users/username/Documents/AntVideo.mp4 \n";
//    }

    cout << "\n";
    cout << "Thank you for choosing to use A Bug's Life for your ant video processing needs!\n";
//              << "Please pass the absolute path of your Ant video \n"
//              << "The following extensions are accepted: .mp4 \n";

    AntProc ranger("../resources/AntsVideoTest.mp4", 300, 300);
    ranger.ContourFinder();

    cout << "Good luck and good riddance!" << endl;
}