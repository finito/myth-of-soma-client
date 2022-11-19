# Myth of Soma Client
## Intro
A client for Myth of Soma that written in C++. Mostly an experiment to see if it was possible at the time to create a performant client that made use of OpenGL - *Spoiler: It was not performant at the time and ran really bad on Intel GPUs found within laptops and also uses more resources than the original client*.
The client does not use the original assets of the game (They have been converted using scripts written for https://www.gimp.org/ - can provide these separately when I've found and sorted them out) and instead uses assets that at the time were optimized for use with OpenGL. - Though it is probably worth switching to use the original assets instead or setting up a better pipeline for converting them. A link to the assets for the client - https://mega.nz/file/c0c2jAja#ZpMaMk6UFTlDBL55daPv0ix4fZJfIGd8ZTQ9OrFRwyo

The client source code is provided as is and was written mostly between 2011-2013, it is incomplete and doesn't have all the features - mainly missing proper Magic and Aura effects.

Should compile using **VS2010** or **VS2015** it has not been compiled in any other version. Would be worth setting up **CMake** - https://cmake.org/ for this project.

## Misc
I have some notes laying around that contain development notes and other stuff to do with this client, will find them in the Notes folder.

## Uses:
* Boost libraries - Either get and use or replace with alternatives
	* smart pointers - These can be replaced by their equivalent C++ standard smart pointers
	* threading - The locks can be replaced by their equivalent C++ standard ones
	* boost format - Though I would recommend replacing and using https://github.com/fmtlib/fmt instead.
	* xml parsing - Could be replaced with something else instead
	* random - Again could be replaced
	* Other stuff that I've forgot to mention - Should be easily replacble
* https://www.sfml-dev.org/ 
* http://cegui.org.uk/ - version 0.7.5 is what client used at time of development
* Artemis C++ port of entity system https://bitbucket.org/stalei/artemiscpp/src/master/
* Other stuff that I've forgotten to mention

## License Information:
The client code itself has been licensed under MIT though there are components of it that will have their own respective licenses. **Disclaimer**: I don't know how all these would work together so may need sorting out :)