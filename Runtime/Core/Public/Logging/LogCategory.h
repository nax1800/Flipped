#pragma once

#include "LogVerbosity.h"

class FLogCategory
{
public:
    FLogCategory(const char* InName, ELogVerbosity::Type InDefaultVerbosity);

    const char* GetName() const { return Name; }
    ELogVerbosity::Type GetVerbosity() const { return Verbosity; }

    bool IsSuppressed(ELogVerbosity::Type InVerbosity) const
    {
        ELogVerbosity::Type VerbosityLevel =
            (ELogVerbosity::Type)(InVerbosity & ELogVerbosity::VerbosityMask);

        if (VerbosityLevel == ELogVerbosity::NoLogging)
            return true;

        return VerbosityLevel > Verbosity;
    }

    void SetVerbosity(ELogVerbosity::Type InVerbosity)
    {
        Verbosity = InVerbosity;
    }

private:
    const char* Name;
    ELogVerbosity::Type Verbosity;
};
