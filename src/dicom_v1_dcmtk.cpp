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


#define DCM_HUCalibrationFactor DcmTagKey(0x0018,0x0061)

#define DCM_NumberofSourceAngularSteps DcmTagKey(0x7033,0x1013)
#define DCM_FlyingFocalSpotMode DcmTagKey(0x7033,0x100E)
#define DCM_NumberofDetectorRows DcmTagKey(0x7029,0x1010)
#define DCM_NumberofDetectorColumns DcmTagKey(0x7029,0x1011)
#define DCM_DetectorElementTransverseSpacing DcmTagKey(0x7029,0x1002)
#define DCM_DetectorElementAxialSpacing DcmTagKey(0x7029,0x1006)
#define DCM_DetectorFocalCenterRadialDistanceArray DcmTagKey(0x7031,0x1003)
#define DCM_ConstantRadialDistance DcmTagKey(0x7031,0x1031)
#define DCM_DetectorCentralElement DcmTagKey(0x7031,0x1033)
#define DCM_DetectorFocalCenterAxialPositionArray DcmTagKey(0x7031,0x1002)
#define DCM_DetectorFocalCenterAngularPositionArray DcmTagKey(0x7031,0x1001)

int dicom_v1_dcmtk_build_ctbb_scanner(std::string filepath){
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
    
    return 0;
}

// function to extract metadata
int dicom_v1_dcmtk_extract_metadata(std::string filepath){

    OFCondition status;
    DcmFileFormat fileformat;
    status = fileformat.loadFile(filepath.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength);//, ERM_metaOnly

    if (status.good())
    {
        OFString sopClassUID, xferUID, TubeAngle;
        if (fileformat.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClassUID).good())
	    COUT << "SOP Class UID: " << sopClassUID << OFendl;
        if (fileformat.getMetaInfo()->findAndGetOFString(DCM_TransferSyntaxUID, xferUID).good())
            COUT << "Transfer Syntax UID: " << xferUID << OFendl;

        OFString spf;
	if (fileformat.getDataset()->findAndGetOFString(DCM_SpiralPitchFactor,spf).good())
            COUT << "Spiral pitch factor: " << spf << OFendl;
        
        OFString table_feed;
	if (fileformat.getDataset()->findAndGetOFString(DCM_TableFeedPerRotation, table_feed).good())
            COUT << "table_feed: " << table_feed << OFendl;

        OFString image_orientation_patient;
	if (fileformat.getDataset()->findAndGetOFString(DCM_ImageOrientationPatient, image_orientation_patient).good())
            COUT << "Image_Orientation_Patient: " << image_orientation_patient << OFendl;

        OFString acq_fov;
	if (fileformat.getDataset()->findAndGetOFString(DCM_DataCollectionDiameter, acq_fov).good())
            COUT << "acq_fov: " << acq_fov << OFendl;

        OFString ffs_mode;
	if (fileformat.getDataset()->findAndGetOFString(DCM_FlyingFocalSpotMode, ffs_mode).good())
            COUT << "ffs_mode: " << ffs_mode << OFendl;

        OFString n_rows;
	if (fileformat.getDataset()->findAndGetOFString(DCM_NumberofDetectorRows, n_rows).good())
            COUT << "n_rows: " << n_rows << OFendl;

        OFString table_pos;
	if (fileformat.getDataset()->findAndGetOFString(DCM_TablePosition, n_rows).good())
            COUT << "n_rows: " << n_rows << OFendl;

        OFString d_row;
	if (fileformat.getDataset()->findAndGetOFString(DCM_DetectorElementTransverseSpacing, d_row).good())
            COUT << "d_row: " << d_row << OFendl;
        
        //fileformat.print(COUT);
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
