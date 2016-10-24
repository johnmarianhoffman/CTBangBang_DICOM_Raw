#!/bin/bash

src_dir=/home/john/Code/CTBangBang_DICOM_Raw

for f in `ls $src_dir/tests/test_data`
do
    echo $f
    $src_dir/raw_test $src_dir/tests/test_data/$f
    echo 
done
