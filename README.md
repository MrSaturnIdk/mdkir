# mdkir
The **Unix** (MacOS/Linux) program that insults you for misspelling a command

Name inspired by that fact I kept spelling `mkdir` wrong
## Install
```sh
git clone https://github.com/MrSaturnIdk/mdkir.git
cd mdkir
mkdir build
cd build
cmake ..
cmake --build . --target mdkir
cmake --install .
```
## Usage
Just run
```sh
mdkir --help
```
## How it works
Just makes a symlink to `/usr/local/bin/mkdir`
> [!WARNING]
> If you wish to rename `mdkir`, you will have to pass `-DCALL_NAME="(insert name)"` to CMake.
>
> It depends on how its called to calculate whether or not to insult.
## License & Copyright
Copyright © 2026 MrSaturnIdk.

Licensed under the [AGPL-3.0-or-later](LICENSE).
