// This is the header file for a library of functions utilizing the
// open source DICOM reading package DCMTK to read the vendor-neutral
// file format proposed in:
//
// Chen, B., Duan, X., Yu, Z., Leng, S., Yu, L., & McCollough,
// C. (2015). Technical Note: Development and validation of an open
// data format for CT projection data. Medical Physics, 42(12),
// 6964–6972.
//
// Copyright (c) John Hoffman, 2016

#include <string>

// function to extract metadata
int dicom_v1_dcmtk_extract_series_metadata(std::string filepath);
int dicom_v1_dcmtk_extract_frame_metadata(std::string filepath);
int dicom_v1_dcmtk_build_ctbb_scanner(std::string filepath);
float dicom_v1_dcmtk_ReadTubeAngle(std::string filepath);
float dicom_v1_dcmtk_ReadTablePosition(std::string filepath);
void dicom_v1_dcmtk_ReadFrame(std::string filepath,float * frame);
