# ofPackageManager

## Description

ofPackageManager is a package manager for openFrameworks.


## Installation

### OSX

#### brew

On OSX you can install the package manager via brew. This will download the most recent version of the package manager and add it to your path.

```
brew tap thomasgeissl/tools
brew install ofpackagemanager
# brew upgrade ofpackagemanager
```

### Windows

On Windows you have to install the `exe` manually.

1. Download the latest release for Windows [here](https://github.com/thomasgeissl/ofPackageManager/releases).
1. Unzip the package anywhere on your hard drive. Make sure to keep the entire contents of the folder, including the DLLs.
1. Add the package folder to the `PATH` variable. Steps on how to do this can be found [here](https://www.addictivetips.com/windows-tips/set-path-environment-variables-in-windows-10/).

![](win-path.png)

## Building

- Clone the app into openFrameworks/apps/myApps: `git clone https://github.com/thomasgeissl/ofPackageManager.git`. 
- Its dependencies can be cloned via the provided `install_dependencies.sh` script or the online version of the package manager.
- On linux and OSX it should build with `make`
- Or you can use the project generator to generate an IDE project

## Usage

It comes with 3 different interfaces.

1. gui - similar to the project generator
2. cli - similar to npm
3. json - accepts commands in json format, can be used by third party tools (currently not maintained, most likely broken)

### gui
- just double click the app or open it from the terminal without providing command line args

| ![Open Project](./docs/open_project.png) | ![Configure Project](./docs/configure_project.png) | ![Install Addon](./docs/install_addon.png) |
|:----------------------------------------:|:--------------------------------------------------:|:------------------------------------------:|
| Open Project                             | Configure Project                                 | Install Addon                              |



### cli

- Install package by id: `ofPackageManager install ofxMidi`
- Install package by github name (user/repo): `ofPackageManager install arturoc/ofxAruco`
- Install package by url: `ofPackageManager install https://github.com/kylemcdonald/ofxCv.git`
- Installing an addon will also install its dependencies: `ofPackageManager install https://github.com/thomasgeissl/ofxPackageManageableExampleAddon.git`
- Install addons globally: `ofPackageManager install -g kylemcdonald/ofxCv`
- Install specific commit or tag by appending an @ followed by the hash or tag name: e.g. `ofPackageManager install ofxMidi@2f0e6343c817a4f5a33b60339c82b5d10be8af01`
- Add already cloned repositories: `ofPackageManager add -A local_addons`
- Install dependencies of a package: `ofPackageManager install`
- Search package in database: `ofPackageManager search ofxHTTP`
- Search package on github by name: `ofPackageManager search github ofxSyphon`
- Search package on github by user: `ofPackageManager search github --user arturoc`


- Info: `ofPackageManager info`
- Help/man: `ofPackageManager help`
- Print version: `ofPackageManager version`
- Check for updates: `ofPackageManager doctor`
- Update packages database: `ofPackageManager update`

### json interface (currently not maintained)
[More details on JSON Interface](./docs/jsonInterface.md)

## Best practices

- Do not forget to add local_addons to your .gitignore: `echo "./local_addons" >> .gitignore`
- You can also use the bash-based online version if you do not have the package manager installed on your machine. e.g. `cd into/your/project/app` and execute the following to install its dependencies: `bash -c "$(curl -sSL https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/scripts/ofPackageManager.sh)" install`

## License

This software is distributed under the [MIT License](https://en.wikipedia.org/wiki/MIT_License).

Copyright (c) 2019 Thomas Geissl

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Changelog
