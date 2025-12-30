#include <iostream>
#define HEIGHT 200
#define WIDTH 220
#include <fstream>

using namespace std;
int main() {
    
    
  int Image[]  = {};
    
    int ImageSize = HEIGHT * WIDTH * 3;
    ofstream ImageFileThingy("imagefile.Image");
    ImageFileThingy << WIDTH << endl;
    ImageFileThingy << HEIGHT << endl;
    for(int i = 0; i < ImageSize; i = i + 3){   
        ImageFileThingy << Image[i] << " " << Image[i + 1] << " " << Image[i + 2] << endl;     
    }
    ImageFileThingy.close();

}
