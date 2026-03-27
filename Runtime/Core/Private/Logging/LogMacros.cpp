#include "pch.h"
#include "Logging/LogMacros.h"

static const char* VerbosityToString(ELogVerbosity::Type Verbosity)
{
    switch (Verbosity & ELogVerbosity::VerbosityMask)
    {
    case ELogVerbosity::Fatal:        return "Fatal";
    case ELogVerbosity::Error:        return "Error";
    case ELogVerbosity::Warning:      return "Warning";
    case ELogVerbosity::Display:      return "Display";
    case ELogVerbosity::Verbose:      return "Verbose";
    case ELogVerbosity::VeryVerbose:  return "VeryVerbose";
    case ELogVerbosity::Log:          return "Log";
    default:                          return "";
    }
}

void UELog_Internal(
    const FLogCategory& Category,
    ELogVerbosity::Type Verbosity,
    const char* Format,
    ...
)
{
    ELogVerbosity::Type VerbosityLevel =
        (ELogVerbosity::Type)(Verbosity & ELogVerbosity::VerbosityMask);

    // Print category
    printf("%s:", Category.GetName());

    // Unreal omits ":Log:" for normal Log verbosity
    if (VerbosityLevel != ELogVerbosity::Log)
    {
        printf("%s:", VerbosityToString(VerbosityLevel));
    }

    printf(" ");

    // Print formatted message
    va_list Args;
    va_start(Args, Format);
    vprintf(Format, Args);
    va_end(Args);

    printf("\n");

    // Fatal crashes
    if (VerbosityLevel == ELogVerbosity::Fatal)
    {
        printf("Fatal error encountered. Exiting.\n");
        abort();
    }

    // Break flag
    if (Verbosity & ELogVerbosity::BreakOnLog)
    {
#if defined(_MSC_VER)
        __debugbreak();
#endif
    }
}