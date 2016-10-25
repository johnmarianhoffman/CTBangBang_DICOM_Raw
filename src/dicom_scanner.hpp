#ifndef CTBB_SCANNER_H
#define CTBB_SCANNER_H

#include <iostream>
#include <fstream>

class ctbb_scanner {

    float r_f_;
    float r_src_det_;
    size_t n_proj_turn_;
    size_t n_channels_;
    float central_channel_;
    float fan_angle_increment_;
    float hu_calibration_factor_;
    float theta_cone_;

    float central_row_;

public:

    ctbb_scanner(){}

    ~ctbb_scanner(){}
    
    void set_r_f(float r_f){r_f_=r_f;}
    void set_r_src_det(float r_src_det){r_src_det_=r_src_det;}
    void set_n_proj_turn(size_t n_proj_turn){n_proj_turn_=n_proj_turn;}
    void set_n_channels(size_t n_channels){n_channels_=n_channels;}
    void set_central_channel(float central_channel){central_channel_=central_channel;}
    void set_central_row(float central_row){central_row_=central_row;}    
    void set_fan_angle_increment_(float fan_angle_increment){fan_angle_increment_=fan_angle_increment;}    
    void set_hu_calibration_factor(float hu_calibration_factor){hu_calibration_factor_=hu_calibration_factor;}
    void set_theta_cone(float theta_cone){theta_cone_=theta_cone;}
    
    float  get_r_f(){return r_f_ ;}
    float  get_r_src_det(){return r_src_det_ ;}
    long   get_n_proj_turn(){return n_proj_turn_ ;}
    size_t get_n_channels(){return n_channels_ ;}
    float  get_central_channel(){return central_channel_;}
    float  get_fan_angle_increment(){return fan_angle_increment_ ;}
    float  get_hu_calibration_factor(){return hu_calibration_factor_ ;}
    float  get_theta_cone(){return theta_cone_;}
    
    void set_fan_angle_increment_from_spacing(float axial_spacing);
    void set_theta_cone_from_spacing(float transverse_spacing);
    int  save_to_file(std::string filepath);    
};

void ctbb_scanner::set_fan_angle_increment_from_spacing(float transverse_spacing){
    //Note that "transverse" refers to the width of each detector channel (at the detector)
    fan_angle_increment_=atan(transverse_spacing/r_src_det_);
}

void ctbb_scanner::set_theta_cone_from_spacing(float axial_spacing){
    // Note that "axial" refers to the width of each detector row (at the detector)
    
    // Note this will cause problems with our sample dataset from Mayo
    // (but Ima say the problem lies in their conversion tool
    // implementation)

    // Namely, they test dataset has dimensions 736x32, no flying
    // focal spot, and a specified grid spacing of 1.285x2.189 at the
    // detector; this translates to 1.2 mm rows at isocenter, which is
    // not a configuration known to exist on the Definition AS.
    
    theta_cone_=2*atan((central_row_-1)*axial_spacing/r_src_det_);
}

int ctbb_scanner::save_to_file(std::string filepath){
    std::ofstream outfile;
    outfile.open(filepath, std::ios::out);

    int status=0;

    if (outfile.is_open()){

        outfile << "% Auto-generated geometry file from DICOM raw data %" << std::endl;
        outfile << "% Accuracy of this file depends on accuracy how the file was written %" << std::endl;
        outfile << "% All physical units are in millimeters or radians %" << std::endl;
        outfile << std::endl;
        outfile << "RSrcToIso:             "  << r_f_ << std::endl ; 
        outfile << "RSrcToDet:             "  << r_src_det_<< std::endl ;
        // Anode angle is 
        // Hard coded at 7 degrees.  This is not accurate for most
        // scanners and will need to be computed from the ffs offset field
        // in the dicom header. Update coming soon.
        outfile << "AnodeAngle:            "  << 0.1222 << std::endl ; 
        outfile << "FanAngleInc:           "  << fan_angle_increment_<< std::endl ; 
        outfile << "ThetaCone:             "  << theta_cone_<< std::endl ; 
        outfile << "CentralChannel:        "  << central_channel_<< std::endl ; 
        outfile << "NProjTurn:             "  << n_proj_turn_<< std::endl ; 
        outfile << "NChannels:             "  << n_channels_<< std::endl ; 
        outfile << "ReverseRowInterleave:  "  << 0 << std::endl ; 
        outfile << "ReverseChanInterleave: "  << 0  << std::endl ;

        outfile.close();
    }
    else{
        status=1;
    }

    return status;
}

#endif
