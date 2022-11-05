#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <exception>
#include <cstdio>
#include <cassert>
#include <ctime>
#include <iterator>
#include <cmath>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <CEGUI/CEGUI.h>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include "Artemis/EntitySystem.h"
#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/World.h"
#include "Artemis/EntityManager.h"

#define CONNECT_TO_TEST_SERVER