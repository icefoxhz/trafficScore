cmake_minimum_required(VERSION 3.21)
project(trafficScore)

set(CMAKE_CXX_STANDARD 11)

#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_BUILD_TYPE "Debug")

# 关闭警告
add_definitions(-w)

set(INC_DIR
        "E:/tools/windows/c++/thirdLib/gdal/FileGDB_API_VS2017/include"
        "E:/tools/windows/c++/thirdLib/gdal/gdal302_lib/include"
        "E:/tools/windows/c++/thirdLib/database/sourceCodes/soci-4.0.3_win/include"
        "E:/tools/windows/postgis/postgresql-9.5.25-1-windows-x64/include"
        "E:/tools/windows/c++/thirdLib/fmt-master/include"
        "E:/tools/windows/c++/thirdLib/json/json-3.10.1/include"
        "E:/tools/windows/c++/thirdLib/log/log4cplus-2.0.6/include"
        "C:/app/saber/product/11.2.0/client_x64/oci/include"
#        "E:/tools/windows/c++/thirdLib/concurrent/oneapi-tbb-2021.5.0/include/oneapi/tbb/detail"
#        "E:/tools/windows/c++/thirdLib/concurrent/oneapi-tbb-2021.5.0/include/oneapi/tbb"
        "E:/tools/windows/c++/thirdLib/concurrent/oneapi-tbb-2021.5.0/include"
        "E:/tools/windows/c++/thirdLib/yaml/yaml-cpp-yaml-cpp-0.6.3/include"
        )

set(LINK_DIR
        "E:/tools/windows/c++/thirdLib/gdal/FileGDB_API_VS2017/lib64"
        "E:/tools/windows/c++/thirdLib/gdal/gdal302_lib/lib"

        "E:/tools/windows/postgis/postgresql-9.5.25-1-windows-x64/lib"

        "C:/app/saber/product/11.2.0/client_x64/oci/lib/msvc"

        "E:/tools/windows/c++/thirdLib/concurrent/oneapi-tbb-2021.5.0/lib/intel64/vc14"

        #  Release和Debug，对应dll也要替换
#        "E:/tools/windows/c++/thirdLib/database/sourceCodes/soci-4.0.3_win/build/lib/Release"
        "E:/tools/windows/c++/thirdLib/database/sourceCodes/soci-4.0.3_win/build/lib/Debug"

#        "E:/tools/windows/c++/thirdLib/fmt-master/build/Release"
        "E:/tools/windows/c++/thirdLib/fmt-master/build/Debug"

#        "E:/tools/windows/c++/thirdLib/log/log4cplus-2.0.6/msvc14/x64/bin.Release"
        "E:/tools/windows/c++/thirdLib/log/log4cplus-2.0.6/msvc14/x64/bin.Debug"

#        "E:/tools/windows/c++/thirdLib/yaml/yaml-cpp-yaml-cpp-0.6.3/build/Release"
        "E:/tools/windows/c++/thirdLib/yaml/yaml-cpp-yaml-cpp-0.6.3/build/Debug"
        )

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_executable(trafficScore
        main.cpp
        gdbGenerator/PgGenerator.cpp
        gdbGenerator/PgGenerator.h
        gdbGenerator/BaseGenerator.cpp
        gdbGenerator/BaseGenerator.h
        tools/GdbTools.cpp
        tools/GdbTools.h
        tools/CommonTools.cpp
        tools/CommonTools.h
        tools/MyMacros.h
        tools/DataBaseTools.cpp
        tools/DataBaseTools.h
        gdbGenerator/OracleGenerator.cpp
        gdbGenerator/OracleGenerator.h
        modelCreator/SimpleModel.cpp
        modelCreator/SimpleModel.h
        scoreCalculator/IScore.h
        scoreCalculator/SimpleCalculator.cpp
        scoreCalculator/SimpleCalculator.h
        tools/MyLogger.h
        tools/ThreadPool.h)

target_link_libraries(${CMAKE_PROJECT_NAME}
        FileGDBAPI.lib
        gdal_i.lib
        soci_postgresql_4_0.lib
        soci_oracle_4_0.lib
        soci_core_4_0.lib
        oci.lib
        oraocci11.lib
        libpq.lib
        tbb.lib

#        fmt.lib
        fmtd.lib

#        log4cplus.lib
        log4cplusSD.lib

#        libyaml-cppmd.lib
        libyaml-cppmdd.lib
        )