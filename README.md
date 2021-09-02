[![Code Climate](https://codeclimate.com/github/hugopl/RubyCreator/badges/gpa.svg)](https://codeclimate.com/github/hugopl/RubyCreator)

# ⚠️ Notice

This repository is archived, I'm not working on this anymore, for an updated fork see https://github.com/NickLion/RubyCreator

So Long, and Thanks for All the Fish! 🐬

# RubyCreator

Plugin to add Ruby language support to QtCreator IDE.

More info can be found at: http://hugopl.github.io/RubyCreator/

## How to install

Packages are available only for Arch Linux in AUR, for anything different you will need to clone the repository and compile it yourself.

# Note About branches

`master` branch should work with the `master` branch of QtCreator, it may not compile since QtCreator changes their API very often.

There is usually one, sometimes two, branches named with a version number like 4.9.x, etc. These version numbers should match the QtCreator version they are supposed to work.

~As I use this plugin on my every day work~ (now I'm using/developing [Tijolo](https://github.com/hugopl/tijolo)), I do the development on a version branch that matches the current QtCreator version packaged by ArchLinux.

When Archlinux upgrades the QtCreator package minor version, I just create a version tag and another version branch to match the new version.

## How to compile

**You need Qt5!!**

If you want to try QtCreator but don't want to have a custom QtCreator compiled just to do that, follow these instructions:

* ./configure.rb
* cd build && make

At the end of the build you will see an error about lack of permissions to move the plugin library to /usr/..., move it by yourself and it's done.

If you intent to contribute with RubyCreator or already write plugins for QtCreator you probably already have a custom build of QtCreator installed in
a sandbox somewhere in your system, so just call qmake passing QTC_SOURCE and QTC_BUILD variables.
