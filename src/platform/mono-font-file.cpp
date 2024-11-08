#include "./font-file.hpp"

namespace tl
{

static uint8_t __SRC_PLATFORM_FONT_MONO_TLSF[] = {
  0x35, 0x0a, 0x37, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x0a, 0x0a, 0x0a,
  0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30,
  0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30,
  0x20, 0x30, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30,
  0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30,
  0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x20, 0x30,
  0x0a, 0x30, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
  0x20, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a,
  0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x30,
  0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x0a,
  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a,
  0x20, 0x30, 0x30, 0x20, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a,
  0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x30,
  0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30,
  0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30,
  0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30,
  0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x30, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a,
  0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a,
  0x30, 0x0a, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30,
  0x0a, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30,
  0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x0a, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30,
  0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30,
  0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30,
  0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30,
  0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30,
  0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x30,
  0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x20, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30,
  0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30,
  0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x20,
  0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30,
  0x30, 0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
  0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30,
  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a,
  0x20, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30,
  0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30,
  0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30,
  0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a,
  0x30, 0x30, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x30, 0x20, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x30,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a,
  0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30,
  0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x30, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a,
  0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x30,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20,
  0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20,
  0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x20,
  0x30, 0x0a, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x30, 0x0a, 0x30,
  0x30, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a,
  0x20, 0x20, 0x30, 0x0a, 0x30, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20,
  0x30, 0x0a, 0x20, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x30, 0x0a, 0x30, 0x0a, 0x20, 0x20, 0x30, 0x0a, 0x20, 0x30,
  0x0a, 0x20, 0x20, 0x30, 0x30, 0x0a, 0x20, 0x30, 0x0a, 0x20, 0x20, 0x30,
  0x0a, 0x30, 0x0a, 0x0a, 0x0a, 0x20, 0x30, 0x20, 0x30, 0x0a, 0x30, 0x20,
  0x30, 0x0a, 0x0a, 0x0a, 0x0a
};
static int __SRC_PLATFORM_FONT_MONO_TLSF_LEN = 2537;

static sprite_font mono_font;

const sprite_font& font_file_interface_get_sprite_font()
{
	mono_font.content = __SRC_PLATFORM_FONT_MONO_TLSF;
	mono_font.char_count = __SRC_PLATFORM_FONT_MONO_TLSF_LEN;
	return mono_font;
}

}

