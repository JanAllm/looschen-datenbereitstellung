// include/MyCppLibrary.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ReturnData {
    int qInt1;
    int qInt2;
    int qInt3;
    int qInt4;
    int qInt5;
   
};

__declspec(dllexport) ReturnData ProcessImage(unsigned char* img, int size, int width, int height, int channels, int iInt1, int iInt2, int iInt3, int iInt4, int iInt5, int iInt6);

#ifdef __cplusplus
}
#endif

