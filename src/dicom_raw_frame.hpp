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
    size_t n_channels_;
    size_t n_rows_;

public:
    
    float * frame_=NULL;

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

    dicom_frame(const dicom_frame& source){ // Copy constructor
        filepath_.assign(source.filepath_);
        instance_number_    = source.instance_number_;
        table_position_     = source.table_position_;
        tube_angle_         = source.tube_angle_;
        tube_current_       = source.tube_current_;
        n_channels_         = source.n_channels_;
        n_rows_             = source.n_rows_;         
    }

    dicom_frame& operator=(const dicom_frame& source){ // Assignment constructor
        if (this == &source)
            return * this;
        
        filepath_.assign(source.filepath_);
        instance_number_    = source.instance_number_;
        table_position_     = source.table_position_;
        tube_angle_         = source.tube_angle_;
        tube_current_       = source.tube_current_;
        n_channels_         = source.n_channels_;
        n_rows_             = source.n_rows_;         

        return * this;
    }

    int load_metadata();
    void load_frame_data();
    void load_frame_data(float * frame);    

    size_t get_instance_number(){return instance_number_;};
    double get_table_position(){return table_position_;};
    double get_tube_angle(){return tube_angle_;};
    float get_tube_current(){return tube_current_;};
    size_t get_n_channels(){return n_channels_;};
    size_t get_n_rows(){return n_rows_;};
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
            tube_angle_=2*PI-tube_angle;
        }

        float table_position;
        if (dataset->findAndGetFloat32(DCM_DetectorFocalCenterAxialPositionArray,table_position).good()){
            table_position_=(double)table_position;
        }

        OFString tube_current;
        if (dataset->findAndGetOFString(DCM_XRayTubeCurrent,tube_current).good()){
            tube_current_=std::stof(tube_current.c_str());
        }

        uint16_t n_channels;
        if (dataset->findAndGetUint16(DCM_NumberofDetectorColumns, n_channels).good()){
            n_channels_=(size_t)n_channels;
        }

        uint16_t n_rows;
        if (dataset->findAndGetUint16(DCM_NumberofDetectorRows, n_rows).good()){
            n_rows_=(size_t)n_rows;
        }
    }

    return 0;
}

void dicom_frame::load_frame_data(){

    frame_= new float[n_channels_*n_rows_];
    
    // some declarations
    OFString rescale_slope;
    OFString rescale_intercept;
    float r_slope, r_intercept;
    const Uint16 * frame_uint16;
    
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath_.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly
    if (status.good()){
        DcmDataset * dataset=fileformat.getDataset();
        dataset->findAndGetOFString(DCM_RescaleSlope,rescale_slope);
        dataset->findAndGetOFString(DCM_RescaleIntercept,rescale_intercept);
        r_slope=std::stof(rescale_slope.c_str());
        r_intercept=std::stof(rescale_intercept.c_str());                

        frame_uint16 = new Uint16[n_channels_*n_rows_]; 

        unsigned long n_elem;
                
        if (dataset->findAndGetUint16Array(DCM_PixelData,frame_uint16,&n_elem,false).good()){
            std::cout << "Number of elements in image dataset: " << n_elem << std::endl;
            std::cout << "Rescale slope: "     << r_slope << std::endl;
            std::cout << "Rescale intercept: " << r_intercept << std::endl;

            for (unsigned int i=0;i<n_channels_*n_rows_;i++){
                frame_[i]=r_slope*(float)frame_uint16[i]+r_intercept;
            }
        }
    }         
}

void dicom_frame::load_frame_data(float * frame){

    // some declarations
    OFString rescale_slope;
    OFString rescale_intercept;
    float r_slope, r_intercept;
    const Uint16 * frame_uint16;
    
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath_.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly
    if (status.good()){
        DcmDataset * dataset=fileformat.getDataset();
        dataset->findAndGetOFString(DCM_RescaleSlope,rescale_slope);
        dataset->findAndGetOFString(DCM_RescaleIntercept,rescale_intercept);
        r_slope=std::stof(rescale_slope.c_str());
        r_intercept=std::stof(rescale_intercept.c_str());                

        frame_uint16 = new Uint16[n_channels_*n_rows_]; 

        unsigned long n_elem;
                
        if (dataset->findAndGetUint16Array(DCM_PixelData,frame_uint16,&n_elem,false).good()){
            //std::cout << "Number of elements in image dataset: " << n_elem << std::endl;
            //std::cout << "Rescale slope: "     << r_slope << std::endl;
            //std::cout << "Rescale intercept: " << r_intercept << std::endl;

            for (unsigned int i=0;i<n_channels_*n_rows_;i++){
                frame[i]=r_slope*(float)frame_uint16[i]+r_intercept;
            }
        }
    }         
}
