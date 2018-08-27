#define ERR_NO_JPEG_ENCODER -1

#include <node.h>
#include <windows.h>

namespace addon {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Integer;
using v8::Value;

// PRIVATE FUNCTIONS

// EXPOSED FUNCTIONS

void GetWidth(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CXSCREEN)));
}

void GetHeight(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), GetSystemMetrics(SM_CYSCREEN)));
}

void TakeScreenshot(const FunctionCallbackInfo<Value>& args) {
  return args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 0));
}

// INITIALIZE

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getWidth", GetWidth);
  NODE_SET_METHOD(exports, "getHeight", GetHeight);
  NODE_SET_METHOD(exports, "takeScreenshot", TakeScreenshot);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace addon
