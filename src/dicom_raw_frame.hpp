#pragma once

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"

#include "dcdeftag_dicom_raw.h"

#define PI 3.141596653689793f

class dicom_frame{

    std::string filepath_;
    size_t instance_number_;
    double table_position_;
    double tube_angle_;
    float tube_current_;

public:
    
    float * frame_;

    dicom_frame(){}
        
    dicom_frame(std::string filepath, bool preload_frame){
        filepath_=filepath;
        dicom_frame::load_metadata();
        if (preload_frame){
            dicom_frame::load_frame_data();
        }
    }

    ~dicom_frame(){
        if (frame_)
            delete[] frame_;
    }

    int load_metadata();
    void load_frame_data();

    size_t get_instance_number(){return instance_number_;};
};

int dicom_frame::load_metadata(){
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath_.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    DcmDataset *dataset=fileformat.getDataset();

    if (status.good()){
        OFString instance_number;
        if (dataset->findAndGetOFString(DCM_InstanceNumber,instance_number).good()){
            instance_number_=(size_t)std::stoi(instance_number.c_str());
        }

        float tube_angle;
        if (dataset->findAndGetFloat32(DCM_DetectorFocalCenterAngularPositionArray,tube_angle).good()){
            tube_angle_=PI-tube_angle;
        }

        float table_position;
        if (dataset->findAndGetFloat32(DCM_DetectorFocalCenterAxialPositionArray,table_position).good()){
            table_position_=table_position;
        }

        OFString tube_current;
        if (dataset->findAndGetOFString(DCM_XRayTubeCurrent,tube_current).good()){
            tube_current_=std::stof(tube_current.c_str());
        }
    }

    return 0;
}

void dicom_frame::load_frame_data(){

}
