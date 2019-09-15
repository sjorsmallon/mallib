#include "fault.h"
#include "datatypes.h"
#include <assert.h>

//----------------------------------------------------------------------------
// FaultHandler
//----------------------------------------------------------------------------
void FaultHandler(const char* file, unsigned short line)
{
#if WIN32
	// If you hit this line, it means one of the ASSERT macros failed.
    DebugBreak();	
#endif

	assert(0);
}