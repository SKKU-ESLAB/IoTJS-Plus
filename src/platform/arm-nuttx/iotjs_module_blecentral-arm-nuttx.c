#if defined(__NUTTX__)

#if defined(TARGET_BOARD) && TARGET_BOARD == STM32F4DIS

#include "iotjs_def.h"
#include "./iotjs_module_blecentral-arm-nuttx-stm32.inl.h"

#else

#include "iotjs_def.h"
#include "./iotjs_module_blecentral-arm-nuttx-general.inl.h"

#endif

#endif // __NUTTX__
