Artemis C++
===

A C++ port of [Artemis Entity System Framework](http://gamadu.com/artemis/tutorial.html).

The port was orignially written by [Sidar Talei](https://bitbucket.org/stalei/artemiscpp/src), in which he used several C++11 features such as deleted function, variadic templates, nullptr, etc…
We wanted the framework to be portable, so we removed all C++11 feature usages.

### PORTED CLASSES

- Component
- ComponentMapper
- ComponentType
- ComponentTypeManager
- Entity
- EntityProcessingSystem
- EntitySystem
- SystemBitManager
- ImmutableBag
- EntityManager
- DelayedEntityProcessingSystem
- DelayedEntitySystem
- GroupManager
- IntervalEntityProcessingSystem
- IntervalEntitySystem
- Manager
- SystemManager
- TagManager
- utils
- World
- Bag

### EXAMPLE

`VelocityComponent` and `PositionComponent`.

``` cpp
class VelocityComponent : public artemis::Component {
public:
    float velocityX;
    float velocityY;

    MovementComponent(float velocityX, float velocityY)
    {
        this->velocityX = velocityX;
        this->velocityY = velocityY;
    };
};

class PositionComponent : public artemis::Component
{

public:
    float posX;
    float posY;
    PositionComponent(float posX, float posY)
    {
        this->posX = posX;
        this->posY = posY;
    };
};
```
	
MovementSystem updates positions base on velocities

``` cpp
class MovementSystem : public artemis::EntityProcessingSystem {
private:
    artemis::ComponentMapper<MovementComponent> velocityMapper;
    artemis::ComponentMapper<PositionComponent> positionMapper;

public:
    MovementSystem() {
        addComponentType<MovementComponent>();
        addComponentType<PositionComponent>();
    };

    virtual void initialize() {
        velocityMapper.init(*world);
        positionMapper.init(*world);
    };

    virtual void processEntity(artemis::Entity &e) {
        positionMapper.get(e)->posX += velocityMapper.get(e)->velocityX * world->getDelta();
        positionMapper.get(e)->posY += velocityMapper.get(e)->velocityY * world->getDelta();
    };

};
```
	
Usage:

``` cpp
int main(int argc, char **argv) {

    artemis::World world;
    artemis::SystemManager * sm = world.getSystemManager();
    MovementSystem * movementsys = (MovementSystem*)sm->setSystem(new MovementSystem());
    artemis::EntityManager * em = world.getEntityManager();

    sm->initializeAll();

    artemis::Entity & player = em->create();

    player.addComponent(new MovementComponent(2,4));
    player.addComponent(new PositionComponent(0,0));
    player.refresh();

    PositionComponent * comp = (PositionComponent*)player.getComponent<PositionComponent>();

    while(true){

        world.loopStart();
        world.setDelta(0.0016f);
        movementsys->process();

        std::cout << "X:"<< comp->posX << std::endl;
        std::cout << "Y:"<< comp->posY << std::endl;
        Sleep(160);
    }

    return 0;
}
```
### LOGS

- Dec 10, 2012:
	- [Fixed  bug](https://github.com/vinova/Artemis-Cpp/commit/449ee9d3167d6bdf8056a8da7554ebec016e5b65): calling Bag.get(index) not returning NULL when index > bag's size,
	which leads to memory violation when the number of entities becomes greater than initialized entities bag size.
- Nov 15, 2012:
	- [Fixed  bug](https://github.com/vinova/Artemis-Cpp/commit/fe291598b699cd283fc029ee727669b8e7a76e24): memory leak when add a component to an entity
	who already had this component.
- Sept 11, 2012:
	- [Fixed critical bug](https://github.com/vinova/Artemis-Cpp/commit/731d2c3e6f4afbd32e4d33f08f23373d62b91dd9): deleting World doesn't delete/reset all neccessary data,
which will lead to memory violation when a World is deleted and a new one is created.
- Sept 12, 2012:
	- Removed all C++11 feature usages.

### LISENCE

Copyright 2011 GAMADU.COM. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY GAMADU.COM ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GAMADU.COM OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of GAMADU.COM.
# Myth of Soma Client
## Intro
A client for Myth of Soma that written in C++. Mostly an experiment to see if it was possible at the time to create a performant client that made use of OpenGL - *Spoiler: It was not performant at the time and ran really bad on Intel GPUs found within laptops and also uses more resources than the original client*.
The client does not use the original assets of the game (They have been converted using scripts written for https://www.gimp.org/ - can provide these separately when I've found and sorted them out) and instead uses assets that at the time were optimized for use with OpenGL. - Though it is probably worth switching to use the original assets instead or setting up a better pipeline for converting them. A link to the assets for the client will be provided separately - **Not yet uploaded**.

The client source code is provided as is and was written mostly between 2011-2013, it is incomplete and doesn't have all the features - mainly missing proper Magic and Aura effects.

Should compile using **VS2010** or **VS2015** it has not been compiled in any other version. Would be worth setting up **CMake** - https://cmake.org/ for this project.

## License Information:
The client code itself has been licensed under MIT though there are components of it that will have their own respective licenses. **Disclaimer**: I don't know how all these would work together so may need sorting out :)

## Uses:
* Boost libraries - Either get and use or replace with alternatives
	* smart pointers - These can be replaced by their equivalent C++ standard smart pointers
	* threading - The locks can be replaced by their equivalent C++ standard ones
	* boost format - Though I would recommend replacing and using https://github.com/fmtlib/fmt instead.
	* xml parsing - Could be replaced with something else instead
	* random - Again could be replaced
	* Other stuff that I've forgot to mention - Should be easily replacble
* https://www.sfml-dev.org/
* http://cegui.org.uk/
* Artemis C++ port of entity system https://bitbucket.org/stalei/artemiscpp/src/master/
* Other stuff that I've forgotten to mention