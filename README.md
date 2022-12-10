# MACS Tools

## Build

First, clone and initialize submodules:

```
git@github.com:KatekovAnton/macs_resources_generator.git
git submodule init
git submodule update
```

### Windows

Prerequisites:

1. [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) (Community Editon is enough)
1. [CMake](https://cmake.org/download/) (Do not forget to add it to your PATH)

To configure CMake project on Windows, please follow steps:

1. Start Visual Studio 2022, choose "Continue without code" - it will open an empty Visual Studio window;
1. Go "Tools" -> "Command Line" -> "Developer Command Prompt" - it will open a pre-confugured command line window;
1. Navigate to max repo root folder:
 
   ```shell
   cd path\to\macs_resource_generator
   ```

1. Execute the confuiguration script:

    ```shell
    genOnWin.bat
    ```

    This should create a folder `build` with a `macs_resource_generator.sln` file inside. This is a project solution that you can open with
    Visual Studio 2022.
1. Open `macs_resource_generator.sln` with Visual Studio 2022, find a `Solution Explorer`,
   find project `macs_resource_generator` on the root level, click it with right mouse
   button and select "Set as Startup Project".
1. Press "Local Windows Debugger" button on top panel to build and run the project.

Visual Studio 2017 and 2019 are also fine but you will need to change `genOnWin.bat` and set a correct version
of the generator to use by changing value of `-G` parameter to a different one.
