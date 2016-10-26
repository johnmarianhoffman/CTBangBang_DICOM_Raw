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

    /* Series reading */
    // Read a folder as a series
    raw_series s("/home/john/Code/CTBangBang_DICOM_Raw/tests/test_data/mayo_dicom_raw/");
    s.load_series_metadata();
    s.print_series_metadata();

    // Read frame metadata
    s.load_frame_metadata();
    //for (unsigned int i=0; i<s.get_number_of_projections(); i++){
    //    std::cout << s.get_instance_number((size_t)i) << "," << s.get_table_position((size_t)i) << "," << s.get_tube_angle((size_t)i) <<"," << s.get_tube_current((size_t)i) <<  std::endl;        
    //}

    // Read pixeldata of one frame
    float * f= new float[s.get_n_channels()*s.get_n_rows()];
    size_t n_channels;
    size_t n_rows;    
    s.load_frame(1000,f,&n_channels,&n_rows);

    std::cout << n_channels*n_rows << std::endl;
    
    float_debug(f,n_channels*n_rows,"/home/john/Desktop/test_frame.bin");
    
    // Read all frame data
    // allocate the memory for all of our projections
    n_channels=s.get_n_channels();
    n_rows=s.get_n_rows();
    size_t n_proj=s.get_number_of_projections();
    float * sinogram=new float[n_channels*n_rows*n_proj];
    for (unsigned int i=0;i<n_proj;i++){
        //std::cout << "loading frame: " << i+1 << "/" << n_proj << std::endl;
        size_t offset= i*n_rows*n_channels;
        s.load_frame(i,&sinogram[offset],&n_channels,&n_rows);
    }
    
    float_debug(sinogram,n_channels*n_rows*n_proj,"/home/john/Desktop/sinogram_test.bin");

    delete[] sinogram;

    // Read a subset of frame data
    // Extract data based on a start and stop table position    
    double start=-650;
    double end=-700;

    size_t start_idx=s.get_instance_number(start);
    size_t end_idx=s.get_instance_number(end);

    std::cout << "Start: " << (long) start_idx << std::endl;
    std::cout << "End: "   << (long) end_idx << std::endl;

    n_proj=end_idx-start_idx+1;

    sinogram=new float[n_channels*n_rows*n_proj];
    
    for (unsigned int i=0; i< n_proj; i++){
        std::cout << "Frame: " << i << "/" << n_proj << std::endl;
        size_t offset= i*n_rows*n_channels;
        s.load_frame(i + start_idx-1,&sinogram[offset],&n_channels,&n_rows);
    }

    float_debug(sinogram,n_channels*n_rows*n_proj,"/home/john/Desktop/short_sinogram_test.bin");
    
    delete[] sinogram;
        
    std::cout << "Done running tests" << std::endl;
    return 0;
}
