# Compileus (working title)

## Developing on Compileus


### CMake

Compileus uses CMake as a primary build tool.

### Dependecies

Compileus depends on a few external projects. They are all accessed through CMake's `find_package()` function, so as long as
they are visible when you run `cmake`, then you should be good to go.

The required dependencies are:

- spdlog
- doctest
- llvm

You can also use the `scripts/build_dependencies.sh` script to obtain the dependencies at the versions compileus is known to work with. It takes the installation
location for the dependencies as a parameter.

So a typical build from a freshly cloned repo would look like this:

```
./scripts/build_dependencies.sh extern
mkdir _bld
cd _bld
cmake -DCMAKE_PREFIX_PATH=../extern ..
make -j $(nproc)
make test
```

### Build environment

The easiest way to develop on Compileus is to use the provided `buildenv` Dockerfile, which will always contain everything
to build the entire project, including all dependencies already installed.

N.B. 

- The docker container can become out of date after a pull or a merge, forgetting to rebuild it is an easy mistake to make.
- Usual caveats about permissions when running on a linux host apply.

```
docker build -t compileus_env -f build.Dockerfile . 
docker run --rm -it -v $(pwd):/workspace compileus_env
```



### Code styling

Coding style is enforced automatically. Simply run `scripts/format_all.sh` from within the `buildenv` container, and
all code will be automatically formatted.
