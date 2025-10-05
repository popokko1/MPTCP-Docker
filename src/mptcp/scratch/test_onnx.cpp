#include <onnxruntime_cxx_api.h>
#include <iostream>

int main() {
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
  Ort::SessionOptions session_options;
  Ort::Session session(env, "rf_model3.onnx", session_options);

  Ort::AllocatorWithDefaultOptions allocator;  
  auto input_name_ptr = session.GetInputNameAllocated(0, allocator);
  const char* input_name = input_name_ptr.get();
//  std::string output_name = session.GetOutputName(0);
  
  std::cout << "Input name: " << input_name << std::endl;
//  std::cout << "Output name:" << output_name << std::endl;
}
