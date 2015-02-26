//
//  main.cpp
//  VOIO_analysis
//
//  Created by Laurynas Karazija on 25/02/15.
//
//

#include <stdio.h>
#include "analysis.h"
#include <fstream>
#include <vector>

using namespace std;
int main (int argc, char **argv){
    string filename = "/Users/laurynaskarazija/videos/multi_face.avi";
    CLMstatery strg;
    vector<Timestamp> results = strg.processVideoComp(filename, 3, true);
    ofstream out(filename+"_timestamps.txt");
    for(int i = 0; i < results.size(); i++){
        out<<results[i].getStart()<<" "<<results[i].getEnd()<<endl;
    }
    return 0;
}