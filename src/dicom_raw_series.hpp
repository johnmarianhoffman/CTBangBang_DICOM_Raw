#pragma once

#include <dirent.h>
#include <vector>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"

#include "dicom_raw_frame.hpp"
#include "dcdeftag_dicom_raw.h"

class raw_series{

private:

    std::string directory_;    
    std::vector <std::string> file_list_;

    double spiral_pitch_factor_;
    double table_feed_;
    float acquisition_fov_;
    int z_ffs_;
    int phi_ffs_;
    size_t n_rows_;

    dicom_frame * frame_array_;
    
public:

    raw_series(std::string directory_path){
        directory_ = directory_path;

        DIR * dir;
        struct dirent *ent;
        if ((dir = opendir(directory_.c_str()))!=NULL){
            while ((ent = readdir(dir))!=NULL){
                std::string filename(ent->d_name);                
                if (filename!="." && filename !="..")
                    raw_series::add_file(filename);
            }
            closedir(dir);
        }
        else{
            std::cerr << "Failed to get list of files in directory. Could not create raw series." << std::endl;            
        }
    }

    ~raw_series(){
        if (frame_array_)
            delete[] frame_array_;
    }
    
    void add_file(std::string filepath);
    void list_files();
    void print_series_metadata();
    void load_series_metadata();
    void load_frame_metadata();
};

void raw_series::add_file(std::string filepath){
    file_list_.push_back(directory_+ "/" + filepath);
}

void raw_series::list_files(){
    for (unsigned int i=0;i<file_list_.size();i++){
        std::cout<< file_list_[i] << std::endl;
    }    
}

void raw_series::print_series_metadata(){
    std::cout << "Pitch: "<< spiral_pitch_factor_ <<  std::endl;
    std::cout << "Acquisition FOV: " << acquisition_fov_ <<  std::endl;
    std::cout << "N_Rows:" << n_rows_ <<  std::endl;
    std::cout << "Z_ffs: " <<  z_ffs_ <<   std::endl;    
    std::cout << "Phi_ffs: " << phi_ffs_ <<  std::endl;
}

void raw_series::load_series_metadata(){
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(file_list_[0].c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    DcmDataset *dataset=fileformat.getDataset();

    if (status.good()){
        double spiral_pitch_factor;
        if (dataset->findAndGetFloat64(DCM_SpiralPitchFactor,spiral_pitch_factor).good()){
            spiral_pitch_factor_=spiral_pitch_factor;
        }
        //std::cout << "Spiral pitch factor: " << spiral_pitch_factor << std::endl;

        double table_feed;
        if (dataset->findAndGetFloat64(DCM_TableFeedPerRotation,table_feed).good()){
            table_feed_=table_feed;
        }
        //std::cout << "table_feed: " << table_feed << std::endl;

        OFString data_collection_diameter; //Acquisition FOV
        if (dataset->findAndGetOFString(DCM_DataCollectionDiameter,data_collection_diameter).good()){
            acquisition_fov_=std::atof(data_collection_diameter.c_str());
        }
        //std::cout << "data_collection_diameter: " << data_collection_diameter << std::endl;
        
        OFString ffs_mode; // value represenation is a "code string"
        if (dataset->findAndGetOFString(DCM_FlyingFocalSpotMode,ffs_mode).good()){
            if (ffs_mode=="FFSNONE"){
                z_ffs_=0;
                phi_ffs_=0;
            }
            else if (ffs_mode=="FFSXY"){
                z_ffs_=0;
                phi_ffs_=1;
            }
            else if (ffs_mode=="FFSZ"){
                z_ffs_=1;
                phi_ffs_=0;
            }
            else if (ffs_mode=="FFSXYZ"){
                z_ffs_=1;
                phi_ffs_=1;
            }
            else if (ffs_mode=="FFSDIAG"){                
            }
            else{
            }
                
        }
        //std::cout << "ffs_mode:" << ffs_mode  << std::endl;
        
        Uint16 n_rows;
        if (dataset->findAndGetUint16(DCM_NumberofDetectorRows,n_rows).good()){
            n_rows_=(size_t)n_rows;
        }
        //std::cout << "n_rows:" << n_rows  << std::endl;        
    }

    std::cout << "DONE" << std::endl;
}

void raw_series::load_frame_metadata(){
    frame_array_=new dicom_frame[file_list_.size()];

    for (unsigned int i=0;i<file_list_.size();i++){
        dicom_frame tmp_frame(file_list_[i],false);
        frame_array_[tmp_frame.get_instance_number()]=tmp_frame;
    }
}
