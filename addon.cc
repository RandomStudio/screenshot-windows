#define ERR_NO_JPEG_ENCODER -1

#include "minmax.h"
#include <node.h>
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

namespace addon {

using namespace Gdiplus;

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Integer;
using v8::Value;

// PRIVATE FUNCTIONS

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
  UINT num = 0;  // number of image encoders
  UINT size = 0; // size of the image encoder array in bytes

  ImageCodecInfo* pImageCodecInfo = NULL;

  GetImageEncodersSize(&num, &size);
  if(size == 0)
    return -1; // Failure

  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
  if(pImageCodecInfo == NULL)
    return -1; // Failure

  GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT j = 0; j < num; ++j) {
    if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j; // Success
    }
  }

  free(pImageCodecInfo);
  return -1; // Failure
}

int32_t GetScreenshotResult() {
  return 0;
}

// EXPOSED FUNCTIONS

void GetWidth(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CXSCREEN)));
}

void GetHeight(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CYSCREEN)));
}

void TakeScreenshot(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetScreenshotResult()));
}

// INITIALIZE

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getWidth", GetWidth);
  NODE_SET_METHOD(exports, "getHeight", GetHeight);
  NODE_SET_METHOD(exports, "takeScreenshot", TakeScreenshot);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace addon
