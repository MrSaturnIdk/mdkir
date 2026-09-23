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
cmake --build .
sudo cmake --install .
```
## Usage
Just run
```sh
mdkir --help
```
## Configure time customizations
- `-DMDKIR_CALL_NAME=mdkir`: Since mdkir relies on how its called to know if it should insult or configure,
you must pass a custom name if you wish to rename it.
- `-DMDKIR_NEEDS_SUDO=1`: If you need sudo to edit the commands, `0` for false, `1` for true.
## How it works
Just makes a symlink to `/usr/local/bin/mkdir`
## Why this is not getting a Windows port anytime soon
2 things.

**Symlinks** and reading.

On **Unix**, when a command is called via a symlink, it recieves what it was called by (the symlink).

On **Windows**, if you were to make a shorcut (`.lnk`),
the program would actually receive the name of the original file, resulting in it **not working**.

And **symlinks**, the entire reason this program works, are a **pain** on Windows.

On **Unix**, symlinks are easy to create and require no permissions.

On **Windows**, symlinks are **very hard** to create and configure and require **admin** permissions
(or **Developer mode**).

I am **not** going to make myself deal with the Windows filesystem, so port it yourself if you really want it.

Seriously. The source code is right there.
## License & Copyright
Copyright (c) 2026 MrSaturnIdk.

Licensed under the [AGPL-3.0-or-later](LICENSE).
