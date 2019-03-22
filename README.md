# cpython-clion-demo
Demo of debugging CPython from CLion

## Usage

1. Clone this repository and open it up as a project in CLion
2. Clone CPython 3.8 and build it with `--with-pydebug` (see CPython devguide for instructions)
3. Edit CMakeLists.txt and set the `CPYTHON_SOURCE` to the path to the root of your CPython source (and built) copy
4. Change the debug configurations to set the targeted executable and have an argument with any Python statement in quotes.
 ![](screenshot2.png)
5. Run the debugger with breakpoints around https://github.com/tonybaloney/cpython-clion-demo/blob/master/main.c#L80-L93
6. Add watches to `co`, `tstate`, etc.

## Screenshots

![](screenshot.png)

Using the External Libraries navigation in Explorer you can see the CPython source and add breakpoints, simply rebuild and run the debugger to breakpoint into any part of CPython!

![](screenshot3.png)
