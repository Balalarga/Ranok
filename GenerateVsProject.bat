@echo off
    mkdir VsProject
    cd VsProject
    cmake -DCMAKE_BUILD_TYPE="Debug" ..
    cd ..
    pause
@echo on
