/*

Copyright (c) Itron, Inc.
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the ""Software""), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Silver Spring Networks, Inc.
shall not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization from Silver Spring
Networks, Inc.

*/

#include <arduino.h>
#include "portability.h"


//////////////////////////////////////////////////////////////////////////
//
// This file contains code needed to bridge the differences in different Arduino compatible hardware boards.
// In fact it applies to any ARM type MCU that adheres to the CMSIS Core and Runtime Environment.
//
// Use of code specific to an MCU and hardware board is allowed here!
//
// Please contact the Itron Technical Enablement Team for details and help crafting portability to a specific board.
//
//////////////////////////////////////////////////////////////////////////


// For STM Boards that use the STM32 L4xx chip family
#if defined(STM32L4xx)
// Need to define the Serial4 class. This is specific to the STM32L4xx family.
HardwareSerial Serial4(0,1);
#endif

