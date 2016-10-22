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
int dicom_v1_dcmtk_extract_metadata(std::string filepath);
