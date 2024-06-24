 # Boids

 Implemenation of boids in c/c++, compiled for the web.
 
 [Here you can find the deployment of the latest release.](https://eliasnijs.xyz/projects/boids/boids.html)

## Build

To build the project, you need to have emscripten installed. You can find the installation instructions [here](https://emscripten.org/docs/getting_started/downloads.html).

After you have installed emscripten, clone the repository:

```bash
git clone --recurse-submodules git@github.com:eliasnijs/c-web-boids.git
```

Install the dependencies with the following commands:

```bash
cd libs 
./buildlibs.sh
cd ..
```

Then you can build the project with the following commands:

```bash
./build.sh
```
