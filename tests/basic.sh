#!/bin/bash

src_dir=/home/john/Code/CTBangBang_DICOM_Raw

export DCMDICTPATH=$src_dir/resources/DICOM-CT-PD-dict_v8.txt:$DCMDICTPATH

for f in `ls $src_dir/tests/test_data`
do
    echo $f
    $src_dir/raw_test $src_dir/tests/test_data/$f
    echo 
done
