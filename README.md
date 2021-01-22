 Warning
 this fork will probably contain a lot of pasted, shitty code.
===================
<p align="center">
<img src="./Pictures/eyehook.jpg">
</p>
===================
# EyeHook

Want to chat or play with other MissedIT / eyehook users? Join us on Discord :- <a href = https://discord.gg/PDtg8hd><b>Hacker.Police Discord</b></a>


## What is EyeHook?

A fully featured internal hack for *CounterStrike : Global Offensive* which is a fork of <a href = https://github.com/HackerPolice/MissedIT><b>MissedIT</b></a>
.

## Compiling

**Note:** _Do NOT download or compile as the root user._

#### Download the dependencies required to build EyeHook:

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

#### Download EyeHook:

```bash
git clone --recursive https://github.com/itsme2417/EyeHook
```

```bash
cd EyeHook
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

Navigate to the directory where MissedIT was built if you have not ready.
```bash
cd MissedIT
```

Now, you can inject the hack with the `load` script
```bash
./load
```

You might be prompted to enter in your password, this is because the injection script requires root access.

The text printed out during injection is not important. 

If the injection was successful you will see a message at the bottom saying `Successfully injected!`, however, if the message says `Injection failed`, then you've most likely done something wrong.

Now, go back into CS:GO, if you are in the main menu of the game you should see a banner in the top left like so:

![this](Pictures/mainmenu-ss.png)

*Note:* if you are getting crashes ( that are unrelated to game updates ) Try disabling shader precaching in your Steam Client -> Steam -> Settings -> Shader Pre-Caching. 
You will need to install this font https://mega.nz/file/omZBlaBQ#AOF2CpAuWTg2Z45zMjmD6da8_hsxZMdh8IwOEZDQ0Ns to both /usr/share/fonts/TTF and (CSGODIR)/csgo/panorama/fonts/ for the weapon icon esp to work

## Using the hack

Now that eyehook has been injected into the game, press <kbd>Insert</kbd> on your keyboard to open the hack menu (<kbd>ALT</kbd>+<kbd>I</kbd> if you are using a laptop).

If you want to change skins, create and load configs or open the player list, you can find those buttons at the top of the screen.


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


## Grenade Configs

```
~/.config/MissedIT/
```

Each `config.json` is stored in the folder named after them map name.

To add a config, copy the folder containing it to `~/.config/MissedIT/`


## Screenshots

![Visuals](Pictures/visuals.png)
![LegitBot](Pictures/legitbot.png)
![RageBot](Pictures/ragebot.png)
![AntiAim](Pictures/antiaim.png)
![esp](Pictures/visualstab.png)
![skinchanger](Pictures/skinchanger.png)
![Misc](Pictures/misc.png)

## Media
https://imgur.com/a/JxWryuy made by Dehumanization » Mass Genocide#3668

https://imgur.com/a/q4KV8PE made by Dehumanization » Mass Genocide#3668

https://www.youtube.com/watch?v=UXm4ISktAmQ made by Dehumanization » Mass Genocide#3668

https://cdn.discordapp.com/attachments/774860310703243274/774860348393390123/untitled.mp4 made by Dehumanization » Mass Genocide#3668

https://www.youtube.com/watch?v=_HiIKhbuhog made by Aαɾσɳ#1883

## Credits

Special thanks to the Original AimTux project: [https://github.com/AimTuxOfficial/AimTux](https://github.com/AimTuxOfficial/AimTux).

Special thanks to the Fuzion For Panaroma Update And For Good Quality Codes: [https://github.com/LWSS/Fuzion](https://github.com/LWSS/Fuzion).

Special thanks to Unknowncheats For Various Things :)

Special thanks to the Nimbus project For The UI: [https://github.com/NimbusProjects/Nimbus](https://github.com/NimbusProjects/Nimbus)

Special thanks to [@aixxe](http://www.github.com/aixxe/) ([aixxe.net](http://www.aixxe.net)) for the skin changer and with the initial project, as well as helping this project with source code (Available on [@aixxe's](http://www.github.com/aixxe/) github page.).

This project was also originally based upon Atex's [Linux Basehook](http://unknowncheats.me/forum/counterstrike-global-offensive/181878-linux-basehook.html).

