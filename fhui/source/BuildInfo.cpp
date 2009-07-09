#include "StdAfx.h"
#include "BuildInfo.h"
#include "svn_rev.h"

#define FHUI_BUILD_VERSION_APPENDIX ""
#define FHUI_BUILD_VERSION() String::Format("{0}{1}{2}", FHUI_REVISION_NUMBER, FHUI_BUILD_VERSION_APPENDIX, FHUI_REVISION_MODIFIED ? " (modified)" : "")

namespace FHUI
{

String^ BuildInfo::GetVersion()
{
    return FHUI_BUILD_VERSION();
}

} // end namespace FHUI
