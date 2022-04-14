//#include <iostream>
//#include "string"
//#include "ogrsf_frmts.h"
//#include "thread"
//
//#ifndef GISDATA_LOCAL_PATH
//#define GISDATA_LOCAL_PATH  ("E:/testdata/cg/dlst.gdb");
//#endif
//
//#ifndef GISTABLE
//#define GISTABLE "FW_FangWu_A"
//#endif
//
//#ifndef LOCAL_ENTITY_CODE_FIELD
//#define LOCAL_ENTITY_CODE_FIELD "ENTIID"
//#endif
//
//#ifndef TABLE_ENTITY_CODE_FIELD
//#define TABLE_ENTITY_CODE_FIELD "entityid"
//#endif
//
//// 2000坐标系下  1代表111.2公里
//inline double oneMeter() {
//    return 1 / 111.2 / 1000;
//}
//
//using namespace std;
//
//void doSpatialWork(int num) {
//    int distance = 50;
//    string dataTable = GISTABLE;
//    string codeField = LOCAL_ENTITY_CODE_FIELD;
//    string dataPath = GISDATA_LOCAL_PATH;
//    string wktStr1 = "point(102.82909 24.858875)";
//    string wktStr2 = "point(0 0)";
//
//    string wktStr = wktStr1;
//    int i =0;
//    while (true) {
//        try {
//            auto *poDS = static_cast<GDALDataset *>(
//                GDALOpenEx(dataPath.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
//            if (poDS == nullptr) {
//                return;
//            }
//
//            OGRGeometry *poGeometry;
//            OGRGeometryFactory::createFromWkt(wktStr.c_str(), nullptr, &poGeometry);
//
//            string entityNumField = LOCAL_ENTITY_CODE_FIELD;
//            string sqlStr = "select " + entityNumField + " from " + dataTable;
//
//            // 真实距离
//            const double bufferDistance = oneMeter() * distance;
//            auto buffer = poGeometry->Buffer(bufferDistance);
//
//            OGRLayer *poLayer = poDS->ExecuteSQL(sqlStr.c_str(), buffer, nullptr);
//
//            delete buffer;
//            delete poGeometry;
//            if (poLayer == nullptr) {
//                GDALClose(poDS);
//                return;
//            }
//
//            if (poLayer->GetFeatureCount() > 0) {
//                OGRFeature *poFeature = poLayer->GetNextFeature();
//                string entityId =  poFeature->GetFieldAsString(codeField.c_str());
//                OGRFeature::DestroyFeature(poFeature);
//            }
//
//            delete poLayer;
//            GDALClose(poDS);
//        }
//        catch (exception &e) {
//            cout << e.what() << endl;
//        }
//        ++i;
//
//        if (i % 50 == 0)
//            cout << "num=" <<num << " ==> " << i << endl;
//    }
//}
//
//void test()
//{
//    std::thread t1(doSpatialWork, 1);
//    std::thread t2(doSpatialWork, 2);
//    std::thread t3(doSpatialWork, 3);
//
//    t1.join();
//    t2.join();
//    t3.join();
//}
//
//int main() {
//    GDALAllRegister();
//
////    test();
//    doSpatialWork(1);
//    return 0;
//}
