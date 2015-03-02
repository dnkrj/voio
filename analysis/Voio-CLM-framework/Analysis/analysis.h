//
//  analysis.h
//  VOIO_analysis
//
//  Created by Laurynas Karazija on 25/02/15.
//
//

#ifndef VOIO_analysis_analysis_h
#define VOIO_analysis_analysis_h


#endif

#include "Strategy.h"
class CLMstatery : Strategy
{
public:
    std::vector<Timestamp> processVideoComp(const std::string& filename, int secondsPerClip, bool verbose = false, bool output_to_file = false);
    //calls above with default arguments
    std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};

