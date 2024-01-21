# anywho

[![ci](https://github.com/johannes-graeter/anywho/actions/workflows/ci.yml/badge.svg)](https://github.com/johannes-graeter/anywho/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/johannes-graeter/anywho/branch/main/graph/badge.svg)](https://codecov.io/gh/johannes-graeter/anywho)
[![CodeQL](https://github.com/johannes-graeter/anywho/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/johannes-graeter/anywho/actions/workflows/codeql-analysis.yml)

## About anywho
Error management for c++23 just like rust anyhow

## Why jet another error handling library
Having dug into rust, I found error management with the anyhow library (https://docs.rs/anyhow/latest/anyhow/) very pleasing and the best I experienced so far.
With c++23 std::expected we are headed into that direction but are not quite there yet.
This repo adds an error type and some convenience functions to come closer to Rust anyhow.

# Design goals
* zero overhead compared to using std::expected
* error should be redirected in 90 percent of the cases. This mnust be very lean (max. 1 line of code to add information to the error)
* cutting edge c++ features
More details in [here](design.md)

# Current support
* Ubuntu 22.04, Clang-17

## More Details

 * [Dependency Setup](README_dependencies.md)
 * [Building Details](README_building.md)
 * [Troubleshooting](README_troubleshooting.md)
 * [Docker](README_docker.md)
