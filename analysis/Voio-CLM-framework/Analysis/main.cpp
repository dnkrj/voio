//
//  main.cpp
//  VOIO_analysis
//
//  Created by Laurynas Karazija on 25/02/15.
//
//

#include <stdio.h>
#include <fstream>
#include <vector>

#include "gifcv.h"
#include "analysis.h"
using namespace std;



int main (int argc, char **argv){
    vector<string> arguments;
    for(int i = 1; i < argc; ++i)
    {
        arguments.push_back(string(argv[i]));
    }
    string filename = "";
    string readfile;
    int numOfsec = 3;
    CLMstatery strg;
    vector<Timestamp> results;
    //cout<<results.size()<<endl;
//    ofstream out(filename+"timestamps.txt");
//    for(int i = 0; i < results.size(); i++){
//        out<<results[i].getStart()<<" "<<results[i].getEnd()<<endl;
//    }
    if (arguments.size() >=1){
        filename= arguments[0];
        cout<<"Reading from: "<<filename;
        results = strg.processVideoComp(filename, numOfsec, true);
        readfile = filename;
    }
    
    int num = 8;
    
    if(results.size() <num){
        num =results.size();
    }
    
    
    
    Filter filter;
    //filter.extractVids(readfile, "out/", 0, results);
    cout<<"DONE !!!"<<endl;
    return 0;
}