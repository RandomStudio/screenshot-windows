#define ERR_NO_JPEG_ENCODER       -1
#define ERR_FAILED_TO_SAVE        -2
#define ERR_FAILED_TO_CREATE_DC   -3
#define ERR_FAILED_TO_CREATE_BMP  -4
#define ERR_FAILED_TO_BIT_BLT     -5

#include "minmax.h"
#include <cstring>
#include <node.h>
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

namespace addon {

using namespace Gdiplus;

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
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

int SaveBitmap(HBITMAP &hbm, WCHAR *filename, ULONG quality, int width, int height) {
  int result = 0;

   // Initialize GDI+
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  CLSID             encoderClsid;
  EncoderParameters encoderParameters;

  // Get the JPEG encoder class identifier
  if (GetEncoderClsid(L"image/jpeg", &encoderClsid) < 0) {
    result = ERR_NO_JPEG_ENCODER;
    goto done;
  }

  // Set quality for the JPEG encoder
  encoderParameters.Count = 1;
  encoderParameters.Parameter[0].Guid = EncoderQuality;
  encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
  encoderParameters.Parameter[0].NumberOfValues = 1;
  encoderParameters.Parameter[0].Value = &quality;

  // Create the Bitmap and save it to disk as JPEG
  Bitmap * originalBmp = new Bitmap(hbm, NULL);
  Bitmap * finalBmp;

  if (originalBmp->GetWidth() == width && originalBmp->GetHeight() == height) {
    finalBmp = originalBmp;
  } else {
    finalBmp = new Bitmap(width, height, originalBmp->GetPixelFormat());
    Graphics * graphics = Graphics::FromImage(finalBmp);
    graphics->SetSmoothingMode(SmoothingModeDefault);
    graphics->SetInterpolationMode(InterpolationModeBicubic);
    graphics->DrawImage(originalBmp, 0, 0, width, height);
  }

  if (finalBmp->Save(filename, &encoderClsid, &encoderParameters) != Ok) {
    result = ERR_FAILED_TO_SAVE;
    goto done;
  }

done:
  GdiplusShutdown(gdiplusToken);
  return result;
}

int32_t GetScreenshotResult(WCHAR *filename, ULONG quality, int width, int height) {
  int result = 0;
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

  HDC hdcScreen;
  HDC hdcMemDC = NULL;
  HBITMAP hbmScreen = NULL;

  // Retrieve the handle to the display device context for the screen
  hdcScreen = GetDC(NULL);

  // Create a compatible DC which is used in a BitBlt from the window DC
  hdcMemDC = CreateCompatibleDC(hdcScreen);
  if(!hdcMemDC) {
    result = ERR_FAILED_TO_CREATE_DC;
    goto done;
  }

  // Create a compatible bitmap from the Window DC
  hbmScreen = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
  if (!hbmScreen) {
    result = ERR_FAILED_TO_CREATE_BMP;
    goto done;
  }

  // Select the compatible bitmap into the compatible memory DC
  SelectObject(hdcMemDC, hbmScreen);

  // Bit block transfer into our compatible memory DC
  if(!BitBlt(hdcMemDC, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY)) {
    result = ERR_FAILED_TO_BIT_BLT;
    goto done;
  }

  result = SaveBitmap(hbmScreen, filename, quality, width, height);

done:
  DeleteObject(hbmScreen);
  DeleteObject(hdcMemDC);
  ReleaseDC(NULL, hdcScreen);
  return result;
}

// EXPOSED FUNCTIONS

void GetWidth(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CXSCREEN)));
}

void GetHeight(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CYSCREEN)));
}

void TakeScreenshot(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() < 4) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type of arguments")));
    return;
  }

  String::Utf8Value filenameUtf8(isolate, args[0]->ToString());
  size_t            filenameWcLength = std::strlen(*filenameUtf8) + 1;
  WCHAR *           filenameWc = new WCHAR[filenameWcLength];
  size_t            convertedChars = 0;
  mbstowcs_s(&convertedChars, filenameWc, filenameWcLength, *filenameUtf8, _TRUNCATE);

  int quality = args[1]->Int32Value();
  int width = args[2]->Int32Value();
  int height = args[3]->Int32Value();
  int result = GetScreenshotResult(filenameWc, quality, width, height);

  return args.GetReturnValue().Set(Integer::New(isolate, result));
}

// INITIALIZE

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getWidth", GetWidth);
  NODE_SET_METHOD(exports, "getHeight", GetHeight);
  NODE_SET_METHOD(exports, "takeScreenshot", TakeScreenshot);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace addon
