# Miraganic Error Handling

An error handling plugin for Unreal Engine, meant to handle errors originating in both C++ code and Blueprints.

## Goals

- Make it possible to pass errors between C++ and Blueprint.
- Make defining error types easier in both C++ and Blueprint.
- Pass errors across interfaces. This isn't possible with a static enum type.
- Give feedback to players depending on the error that occurs. (E.g., 'Can't deposit items because the item stack is full')
- Allow functions to decide whether they want to handle errors or simply log them.

## Overview

The plugin uses an `FECResult` type for handling errors. This type is essentially an 'Any enum'; it can hold a single enum value from any enumerator. This means we can define our errors as simple enumerators:

### C++

```cpp
UENUM()
enum class EDivisionResult : uint8
{
    Success = 0,
    // Tried to divide by zero.
    DivideByZero
}
```

Each result can be either a `Success` or `Failure`. FECResult always treats the enum value **0** as `Success`, and anything else as `Failure`. The enum value describes which error occurred (in this case, `DivideByZero`).

To return a result in C++:

```cpp
FECResult SafeDivide(int32& OutValue, int32 A, int32 B)
{
    if (B == 0)
    {
        // Will implicitly convert from any enum to FECResult
        return EDivisionResult::DivideByZero;
    }

    OutValue = A / B;
    // Can also use FECResult::Success()
    return EDivisionResult::Success;
}
```

And to handle an error:

```cpp
// Adding the 'ErrorCategory' metadata tag makes the error visible to Blueprint. 
UENUM(meta = (ErrorCategory))
enum class EMyFunctionResult : uint8
{
    Success = 0,
    UsedFortyTwo
}

FECResult MyFunction(int32& OutValue, int32 A, int32 B)
{
    const FECResult DivideResult = SafeDivide(OutValue, A, B);
    
    // Can check for specific errors.
    if (DivideResult == EDivisionResult::DivideByZero)
    {
        // Do some handling here...
    }

    // Can pass the result to the caller
    if (DivideResult.IsFailure())
    {
        return DivideResult;
    }

    // Can mix and match different enum types
    if (A == 42)
    {
        return EMyFunctionResult::UsedFortyTwo;
    }

    return EMyFunctionResult::Success;
}
```

### Blueprint

Results can be defined in blueprint using the `Error Category` asset:

![](/docs/static/img/bp_error_category.png)

Error categories are very similar to defining enumerations.

Results in blueprint can be selected using a handy combo box:

![](/docs/static/img/bp_result_combobox.png)

And to handle results, you may use the `Switch on Result` node:

![](/docs/static/img/bp_result_switch.png)

Or you may opt to simply log failures:

![](/docs/static/img/bp_result_log.png)
