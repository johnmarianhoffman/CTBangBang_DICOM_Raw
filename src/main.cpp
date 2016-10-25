/* CTBangBang is GPU and CPU CT reconstruction Software */
/* Copyright (C) 2015  John Hoffman */

/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License */
/* as published by the Free Software Foundation; either version 2 */
/* of the License, or (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to the Free Software */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

/* Questions and comments should be directed to */
/* jmhoffman@mednet.ucla.edu with "CTBANGBANG" in the subject line*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "dicom_v1_dcmtk.h"

void usage(){
    exit(0);
}

#include <fstream>

#include "dicom_raw_series.hpp"

using namespace std;
inline void float_debug(float * array,size_t numel, string file){
    ofstream outfile;
    outfile.open(file,ios::out | ios::binary);
    outfile.write((char*)array,numel*sizeof(float));
    outfile.close();
}

int main(int argc, char ** argv){
    // This main function implements the test harness utilized in our
    // upcoming publication

    /* Individual file reading */
    // Test our metadata reading
    //dicom_v1_dcmtk_extract_series_metadata(argv[1]);
    //dicom_v1_dcmtk_build_ctbb_scanner(argv[1]);        
    //dicom_v1_dcmtk_ReadTubeAngle(argv[1]);
    //dicom_v1_dcmtk_ReadTablePosition(argv[1]);

    // Attempt to read frame data
    //float * frame= new float[736*32];
    //dicom_v1_dcmtk_ReadFrame(argv[1],frame);
    //float_debug(frame,736*32,std::string("/home/john/Desktop/test_frame.bin"));

    /* Full Series reading */
    // Attempt to read a folder as a series
    raw_series s("/home/john/Code/CTBangBang_DICOM_Raw/tests/test_data/mayo_dicom_raw/");
    s.list_files();
    s.load_series_metadata();
    s.print_series_metadata();
    
    //delete[] frame;
    return 0;
}
