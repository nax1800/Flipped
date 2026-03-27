#include "pch.h"
#include "Logging/LogCategory.h"

FLogCategory::FLogCategory(const char* InName, ELogVerbosity::Type InDefaultVerbosity)
    : Name(InName)
    , Verbosity(InDefaultVerbosity)
{
}