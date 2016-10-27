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

#include <iostream>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"

#include "dcdeftag_open_raw.h"
#include "dicom_v1_dcmtk.h"

// function to extract metadata
int dicom_v1_dcmtk_extract_metadata(std::string filepath){

    OFCondition status;
    DcmFileFormat fileformat;

    std::cout << filepath.c_str() << std::endl;
    status = fileformat.loadFile(filepath.c_str());//, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_metaOnly

    if (status.good())
    {
	std::cout << "DCMTK IS A PIECE OF FUCKING SHIT LIBRARY" << std::endl;

	std::cout << fileformat.getMetaInfo()->tagExists(DcmTagKey(0x0020,0x000D)) << OFendl;
	
        OFString sopClassUID, xferUID;
	COUT << fileformat.getMetaInfo()->tagExists(DCM_MediaStorageSOPClassUID) << OFendl;
        if (fileformat.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClassUID).good())
	    COUT << "SOP Class UID: " << sopClassUID << OFendl;
        if (fileformat.getMetaInfo()->findAndGetOFString(DCM_TransferSyntaxUID, xferUID).good())
            COUT << "Transfer Syntax UID: " << xferUID << OFendl;
	
	OFString AcqFOV;
	COUT << DCM_DataCollectionDiameter << OFendl;
	if (fileformat.getMetaInfo()->findAndGetOFString(DCM_DataCollectionDiameter, AcqFOV).good())
	    COUT << "AcqFOV: " << AcqFOV << OFendl;
    }

    return 0;
}


