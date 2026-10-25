// SPDX-License-Identifier: Apache-2.0
// File:        version.h
// Description: 由 third_party/README.md 描述的流程手工生成（对应 Tesseract 5.5.0）
//
// 原仓库中本文件由 CMake 从 version.h.in 生成，源码包内不含。
// 版本号必须与实际链接的 libtesseract-5.dll 一致。

#ifndef TESSERACT_API_VERSION_H_
#define TESSERACT_API_VERSION_H_

// clang-format off

#define TESSERACT_MAJOR_VERSION 5
#define TESSERACT_MINOR_VERSION 5
#define TESSERACT_MICRO_VERSION 0

#define TESSERACT_VERSION          \
  (TESSERACT_MAJOR_VERSION << 16 | \
   TESSERACT_MINOR_VERSION <<  8 | \
   TESSERACT_MICRO_VERSION)

#define TESSERACT_VERSION_STR "5.5.0"

// clang-format on

#endif // TESSERACT_API_VERSION_H_
