RubyCreator
===========

Plugins to add Ruby language support to QtCreator IDE.

How to compile
==============

* Install [meique](http://www.meique.org)
* Go to RubyCreator source directory
* mkdir build; cd build
* meique ..

qtcreator binary must be on your path, the build system will identify the QtCreator version, download the sources for it then compile RubyCreator.

If your QtCreator plugins parent directory isn't "/usr/lib/qtcreator/", instead of "meique .." you should use: "meique --QtCreatorLibDir=YOUR_CUSTOM_DIR .."

To install the plugin in the right place, type:

$ DESTDIR=/usr/lib/qtcreator sudo meique -i
