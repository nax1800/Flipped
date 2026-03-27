#pragma once

#include "LogCategory.h"

void UELog_Internal(
    const FLogCategory& Category,
    ELogVerbosity::Type Verbosity,
    const char* Format,
    ...
);

#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity) \
    extern FLogCategory CategoryName;

#define DEFINE_LOG_CATEGORY(CategoryName, DefaultVerbosity) \
    FLogCategory CategoryName(#CategoryName, DefaultVerbosity);

#define UE_LOG(CategoryName, Verbosity, Format, ...) \
    do \
    { \
        ELogVerbosity::Type __Verbosity = \
            (ELogVerbosity::Type)(Verbosity & ELogVerbosity::VerbosityMask); \
        \
        if (__Verbosity != ELogVerbosity::NoLogging && \
            !CategoryName.IsSuppressed(Verbosity)) \
        { \
            UELog_Internal(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
        } \
    } while (0)