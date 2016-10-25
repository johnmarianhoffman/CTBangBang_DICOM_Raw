// This is a library of functions utilizing the open source DICOM
// reading package DCMTK to read the vendor-neutral file format
// proposed in:
//
// Chen, B., Duan, X., Yu, Z., Leng, S., Yu, L., & McCollough,
// C. (2015). Technical Note: Development and validation of an open
// data format for CT projection data. Medical Physics, 42(12),
// 6964–6972.
//
// Copyright (c) John Hoffman, 2016

#define __DEBUG__

#include <iostream>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"

#include "dicom_v1_dcmtk.h"

#include "dicom_scanner.hpp"

//    Nrows:	64
//    CollSlicewidth:	0.6
//    StartPos:	-6.0
//    EndPos:	367.7
//    PitchValue:	19.2
//    AcqFOV:	500.0
//    ReconFOV:	310.0
//    Readings:	48900
//    Xorigin:	2.0
//    Yorigin:	0.0
//    Zffs:	1
//    Phiffs:	0
//    Scanner:	definitionas.scanner
//    FileType:	1
//    FileSubType:	0
//    RawOffset:	0
//    Nx:	512
//    Ny:	512
//    ImageOrientationPatient:	[[-1, 0, 0], [0, 1, 0]]
//    RawDataDir:	/home/john/Desktop/emphysema_pipeline_test/library/raw/100
//    RawDataFile:	55913664e5df7c90a940c4a1c27ea655
//    OutputDir:	/home/john/Desktop/emphysema_pipeline_test/library/recon/100/55913664e5df7c90a940c4a1c27ea655_k1_st5.0
//    OutputFile:	55913664e5df7c90a940c4a1c27ea655_d100_k1_st5.0.img
//    ReconKernel:	1
//    SliceThickness:	5.0
//    AdaptiveFiltration:	1.0


#include "dcdeftag_dicom_raw.h"

int dicom_v1_dcmtk_build_ctbb_scanner(std::string filepath){
    std::cout << "BUILIDING SCANNER FILE FROM " << filepath << std::endl;
    
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly
    DcmDataset *dataset=fileformat.getDataset();

    ctbb_scanner scanner;
    
    if (status.good())
        {            
            //RSrcToIso
            float r_f;
            if (dataset->findAndGetFloat32(DCM_DetectorFocalCenterRadialDistanceArray,r_f).good()){
                scanner.set_r_f(r_f);
            }
            std::cout << scanner.get_r_f() << std::endl;

            //RSrcToDet
            float r_src_det;
            if (dataset->findAndGetFloat32(DCM_ConstantRadialDistance,r_src_det).good()){
                scanner.set_r_src_det(r_src_det);
            }
            std::cout << scanner.get_r_src_det() << std::endl;

            // NProjTurn
            Uint16 n_proj_turn;
            if (dataset->findAndGetUint16(DCM_NumberofSourceAngularSteps,n_proj_turn).good()){
                scanner.set_n_proj_turn((size_t)n_proj_turn);
            }
            std::cout << scanner.get_n_proj_turn() << std::endl;

            // NChannels
            Uint16 n_channels;
            if (dataset->findAndGetUint16(DCM_NumberofDetectorColumns,n_channels).good()){
                scanner.set_n_channels((size_t)n_channels);
            }
            std::cout << scanner.get_n_channels() << std::endl;            

            // FanAngleInc:
            float d_channel;
            if (dataset->findAndGetFloat32(DCM_DetectorElementTransverseSpacing,d_channel).good()){
                scanner.set_fan_angle_increment_from_spacing(d_channel);
            }
            std::cout << scanner.get_fan_angle_increment() << std::endl;
            
            // Not yet supported by CTBangBang
            OFString hu_calibration_factor;
            if (dataset->findAndGetOFString(DCM_HUCalibrationFactor,hu_calibration_factor).good()){
                scanner.set_hu_calibration_factor(std::stof(hu_calibration_factor.c_str()));
            }
            std::cout << scanner.get_hu_calibration_factor() << std::endl;

            // Central channel, row
            const float * central_elem;
            size_t n_elem;
            if (dataset->findAndGetFloat32Array(DCM_DetectorCentralElement,central_elem,&n_elem,false).good()){
                scanner.set_central_channel(central_elem[0]);
                scanner.set_central_row(central_elem[1]);
            }

            // Theta cone
            float d_row;
            if (dataset->findAndGetFloat32(DCM_DetectorElementAxialSpacing,d_row).good()){
                scanner.set_theta_cone_from_spacing(d_row);
            }
            std::cout << scanner.get_theta_cone() << std::endl;            
        }

    #ifdef __DEBUG__
    scanner.save_to_file("/home/john/Desktop/test.scanner");
    #endif

    std::cout << "DONE" << std::endl;
    
    return 0;
}

int dicom_v1_dcmtk_extract_series_metadata(std::string filepath){

    std::cout << "EXTRACTING SERIES METADATA FROM " << filepath << std::endl;

    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    DcmDataset *dataset=fileformat.getDataset();

    if (status.good()){
        double spiral_pitch_factor;
        if (dataset->findAndGetFloat64(DCM_SpiralPitchFactor,spiral_pitch_factor).good()){
        }
        std::cout << "Spiral pitch factor: " << spiral_pitch_factor << std::endl;

        double table_feed;
        if (dataset->findAndGetFloat64(DCM_TableFeedPerRotation,table_feed).good()){
        }
        std::cout << "table_feed: " << table_feed << std::endl;

        OFString data_collection_diameter; //Acquisition FOV
        if (dataset->findAndGetOFString(DCM_DataCollectionDiameter,data_collection_diameter).good()){            
        }
        std::cout << "data_collection_diameter: " << data_collection_diameter << std::endl;
        
        OFString ffs_mode; // value represenation is a "code string"
        if (dataset->findAndGetOFString(DCM_FlyingFocalSpotMode,ffs_mode).good()){
        }
        std::cout << "ffs_mode:" << ffs_mode  << std::endl;
        
        Uint16 n_rows;
        if (dataset->findAndGetUint16(DCM_NumberofDetectorRows,n_rows).good()){
        }
        std::cout << "n_rows:" << n_rows  << std::endl;
        
    }

    std::cout << "DONE" << std::endl;
    
    return 0;
}

// function to extract metadata
int dicom_v1_dcmtk_extract_frame_metadata(std::string filepath){

    // Extract the following from the file:
    // 
    // Instance number (Frame index)
    // Series ID (To make sure we're not mixing datasets)
    // Tube angle (Needed for reconstruction)
    // Table position (needed for subselecting regions)
    // Tube current (because why not)

    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    DcmDataset *dataset=fileformat.getDataset();

    if (status.good()){
        OFString instance_number;
        if (dataset->findAndGetOFString(DCM_InstanceNumber,instance_number).good()){
            
        }
    }

    return 0;
}

void dicom_v1_dcmtk_ReadFrame(std::string filepath,float * frame){

    // some declarations
    uint16_t n_channels;
    uint16_t n_rows;
    OFString rescale_slope;
    OFString rescale_intercept;
    float r_slope, r_intercept;
    const Uint16 * frame_uint16;
    
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly
    if (status.good()){
        DcmDataset * dataset=fileformat.getDataset();
        if (dataset->findAndGetUint16(DCM_NumberofDetectorColumns, n_channels).good()){
            if (dataset->findAndGetUint16(DCM_NumberofDetectorRows, n_rows).good()){

                dataset->findAndGetOFString(DCM_RescaleSlope,rescale_slope);
                dataset->findAndGetOFString(DCM_RescaleIntercept,rescale_intercept);
                r_slope=std::stof(rescale_slope.c_str());
                r_intercept=std::stof(rescale_intercept.c_str());                

                frame_uint16 = new Uint16[n_channels*n_rows]; 

                unsigned long n_elem;
                
                if (dataset->findAndGetUint16Array(DCM_PixelData,frame_uint16,&n_elem,false).good()){
                    std::cout << "Number of elements in image dataset: " << n_elem << std::endl;
                    std::cout << "Rescale slope: "     << r_slope << std::endl;
                    std::cout << "Rescale intercept: " << r_intercept << std::endl;

                    for (int i=0;i<n_channels*n_rows;i++){
                        frame[i]=r_slope*(float)frame_uint16[i]+r_intercept;
                    }
                }
            }
        }
    }    
}

float dicom_v1_dcmtk_ReadTubeAngle(std::string filepath){
    
    float ta=0;
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    if (status.good()){
        float alpha_loc;
	if (fileformat.getDataset()->findAndGetFloat32(DCM_DetectorFocalCenterAngularPositionArray, alpha_loc).good())
            COUT << "tube_angle_current_frame_rad: " << alpha_loc << OFendl;        
    }
    return ta;    
}


float dicom_v1_dcmtk_ReadTablePosition(std::string filepath){
    float tp;    
    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    if (status.good()){
        if (fileformat.getDataset()->findAndGetFloat32(DCM_DetectorFocalCenterAxialPositionArray, tp).good())
            COUT << "table_position_current_frame: " << tp << OFendl;
    }
    return tp;
}
