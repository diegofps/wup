# wup
Personal Weightless NN Repository

# Getting Started

Clone the repository. Example:

```bash
mkdir -p ~/Sources && cd ~/Sources
git clone --recursive https://github.com/diegofps/wup.git
```

Include the library and use the namespace.

```cpp
#include <wup/wup.hpp>

using namespace wup;
```

In one of your **.cpp** files, add the following. For instance, before the main function:

```cpp
WUP_STATICS;
```

Compile you program with at least c++11 and include the library directory

```bash
# Normal compilation
clang++ main.cpp -o main -O3 -Wall -std=c++11 -I<PATH_TO_LIB>/wup/cpp/include
```

Extra flags:

```bash
    -DWUP_NO_MPICH : Disable MPICH utilities.
    -DWUP_NO_OPENCV : Disable opencv utilities.
    -DWUP_NO_ZIP : Disable zlib utilities.
    -DWUP_UNSAFE : Ignore security bounds check in Bundle, Bundle3D, etc.
```

# Print Methods

Output methods to help printing messages.

```cpp
#include <wup/wup.hpp>
using namespace wup;

// Basic print like python. Adds spaces and new line at the end.
print("ramBits:", 30); // Will write "ramBits: 30\n"

// Basic print without spaces and new line at the end
prints("ramBits:", 30); // Will write "ramBits:30"

// Print the message prefixed with "Warning:" and using the yellow color.
warn("This is a warning message");

// Print the message prefixed with "Debug:" and using the green color.
debug("This is a debug message");

// Print the message prefixed with "Info:" and using the blue color.
info("This is an info message");

// Print the message prefixed with "Error:" and using the red color. Also raises a WUPException.
error("This is a fatal message followed by an exception");
```

If you want to print a custom class, simply add an output method using **std::ostream**. Like:

```cpp
#include <iostream>
#include <cstdint>

class Result
{
public:
    int i;
    int j;
    int activation;
    
    Result (const int i, const int j, const int activation) : 
        i(i), j(j), activation(activation) 
    { }
};

std::ostream & operator<<(std::ostream & o, const Result & r)
{
    o << "(i:" << r.i, << ", j:" << r.j, ", act:", r.activation, ")";
    return o;
}
```

# String Methods

```cpp
#include <wup/wup.hpp>
using namespace wup;

// Concatenate all
auto tmp1 = cat("Wespa's", "Unified", "Platform");

// Join using the first element
auto tmp2 = join("-", "Wespa's", "Unified", "Platform");

// Join using a single space (" ")
auto tmp3 = joins("Wespa's", "Unified", "Platform");

// Split a string
auto tmp = "Wespa-Intelligent-Systems";
std::vector<std::string> cells;
split(tmp2, "-", cells);

```

# Measuring time

Using the **lap_*** functions in **Clock**.

```cpp
#include <wup/wup.hpp>
using namespace wup;

Clock c; // This automatically starts a measurement.

cap >> frame;
c.lap_nano("Time to read the frame:"); // Prints the message and the time in nanoseconds

flip(frame, frame, 1);
c.lap_micro("Time to flip the image:"); // Prints the message and the time in microseconds

frame.copyTo(canvas);
c.lap_milli("Time to copy the frame to the drawing area :"); // Prints the message and the time in milliseconds

cvtColor(frame, gray, COLOR_BGR2XYZ);
c.lap_seconds("Time to convert to XYZ"); // Prints the message and the time in seconds
```

Using the **start()** and **stop()** functions

```cpp
#include <wup/wup.hpp>
using namespace wup;

Clock c;

// ... //

c.start();

// Do a lot of work here //

c.stop();

// Print in the desired format
print("Operationg took", c.ellapsed_nano(), "ns");
print("Operationg took", c.ellapsed_micro(), "us");
print("Operationg took", c.ellapsed_milli(), "ms");
print("Operationg took", c.ellapsed_seconds(), "s");
```


# Parallel Methods

You need to compile this with **-lpthread**

```cpp
#include <wup/wup.hpp>
#include <thread>
using namespace wup;

int numThreads = std::thread::hardware_concurrency();
int numJobs = 10000;

wup::parallel(numThreads, numJobs, [&](int threadId, int jobId)
{
    // ... //
});
```
