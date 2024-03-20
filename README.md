# Simple C++ Raytracing Application using Walnut and ImGui

## Features
* Spheres and Cubes
* Reflections, Emmission, Albedo
* Simulated Roughness/Metalness (metallic effect)
* Transparency with internal reflections and total reflection using Snell's Law

## Restrictions
Currently, only Windows is supported as a limitation by Walnut.
The development and testing was done on Windows 10

## Getting Started
First it is REQUIRED to have Vulkan installed: https://vulkan.lunarg.com/sdk/home#windows  
Be sure everything installed correctly, by executing `vkcube.exe` in the `... \Vulkan\1.3.268\Bin` directory.  
It should render a window with a rotating cube. Only if your able to see this cube continue, otherwise repeat the installation process.

To clone the repository into your projects folder, use:
```bash
git clone --recursive https://github.com/marvin297/RaytraceComputergrafik.git
```
The `--recusive` directive is needed to also clone the Walnut submodule into your project.

Then run the batch file `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files.

After that you just need to double click on the Visual Studio Solution file (.sln)

## Credits
I'm using a simple app template for [Walnut](https://github.com/TheCherno/Walnut), this keeps Walnut as an external submodule and is much more sensible for actually building applications.
See the [Walnut](https://github.com/TheCherno/Walnut) repository for more details.
