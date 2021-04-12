## What is Kali?
Kali is an Open Source Linux Cheat For Counter Strike : Global Offensive. 
"Master" branch is EyeHook fork. I am currently working on my rework branch. These instructions are for Master branch.
## Compiling

**Note:** _Do NOT download or compile as the root user._

#### Download the dependencies required to build Kali:

If you are having problems compiling make sure you've got the latest version of `g++`.

==================

__Ubuntu-Based / Debian:__
```bash
sudo apt install cmake g++ gdb git libsdl2-dev zlib1g-dev patchelf
```
__Arch:__
```bash
sudo pacman -S base-devel cmake gdb git sdl2 patchelf
```
__Fedora:__
```bash
sudo dnf install cmake gcc-c++ gdb git libstdc++-static mesa-libGL-devel SDL2-devel zlib-devel libX11-devel patchelf
```

===================

#### Download Kali:

```bash
git clone --recursive https://github.com/ArnoldasMk/Kali
```

```bash
cd Kali
```

===================

#### Compile with build script

You can build easily with the included build script.
```bash
./build
```

You can later update with 
```bash
./update
```


## Injecting using the load script

First of all, make sure CS:GO is open, and you are not in any official valve server. However, it is not recommended to inject while CS:GO is loading into a map. 

Navigate to the directory where Kali was built if you have not ready.
```bash
cd Kali
```

Now, you can inject the hack with the `load` script
```bash
./load
```

You might be prompted to enter in your password, this is because the injection script requires root access.

*Note:* if you are getting crashes ( that are unrelated to game updates ) Try disabling shader precaching in your Steam Client -> Steam -> Settings -> Shader Pre-Caching. 

## Using the hack

Now that Kali has been injected into the game, press <kbd>Insert</kbd> on your keyboard to open the hack menu (<kbd>ALT</kbd>+<kbd>I</kbd> if you are using a laptop).

## Unloading the hack

If you wish to unload the hack from the game, you can do so by entering the command:
```bash
./uload
```
## Configs

Configs are stored in a hidden directory in your home folder. Specifically 
```
~/.config/MissedIT/
```

Each `config.json` is stored in a seperately named folder (The name you see in-game, in the config window). 

To add a config, create a folder inside of the `~/.config/MissedIT/` folder with a name of your choice, and paste the `config.json` inside of that folder.

To see hidden folders inside your home folder, press <kbd>CTRL</kbd>+<kbd>H</kbd> when using a file manager.

On your command line, you can also add the -a flag on `ls` e.g.
```bash
ls -la ~/
```
## My glove skin doens't load
So apparently this bug is present with the AMDGPU and Radeon drivers. Henrym11106 discovered that that setting "mesa_no_error" (disable error checking) in adriconf/driconf, .drirc file or by adding `mesa_no_error=true %command%` to CSGO's launch options allows fancy gloves to render correctly. Saddly it doesn't crash only on mesa 20.1.4, for me it crashes on Mesa 20.3.2 and Mesa 20.2.
```bash
mesa_no_error=true %command%
```


## Credits

Special thanks to the Original AimTux project: [https://github.com/AimTuxOfficial/AimTux](https://github.com/AimTuxOfficial/AimTux).

Special thanks to the Fuzion For Panaroma Update And For Good Quality Codes: [https://github.com/LWSS/Fuzion](https://github.com/LWSS/Fuzion).

Special thanks to the MissedIT project: [https://github.com/HackerPolice/MissedIT](https://github.com/HackerPolice/MissedIT).

Special thanks to the EyeHook project: [https://github.com/itsme2417/EyeHook](https://github.com/itsme2417/EyeHook).

Special thanks to Unknowncheats For Various Things :)

Special thanks to the Nimbus project For The UI: [https://github.com/NimbusProjects/Nimbus](https://github.com/NimbusProjects/Nimbus).

Special thanks to [@aixxe](http://www.github.com/aixxe/) ([aixxe.net](http://www.aixxe.net)) for the skin changer and with the initial project, as well as helping this project with source code (Available on [@aixxe's](http://www.github.com/aixxe/) github page.).

This project was also originally based upon Atex's [Linux Basehook](http://unknowncheats.me/forum/counterstrike-global-offensive/181878-linux-basehook.html).

