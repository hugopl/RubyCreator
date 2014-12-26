# RubyCreator

Plugin to add Ruby language support to QtCreator IDE.

More info can be found at: http://hugopl.github.io/RubyCreator/

## How to compile

If you want to try QtCreator but don't want to have a custom QtCreator compiled just to do that, follow these instructions:

* ./configure.rb
* cd build && make

At the end of the build you will see an error about lack of permissions to move the plugin library to /usr/..., move it by yourself and it's done.

If you pretend to contribute with RubyCreator or already write plugins for QtCreator you probably already have a custom build of QtCreator installed in
a sandbox somewhere in your system, so just call cmake passing QTC_SOURCE and QTC_BUILD variables.
