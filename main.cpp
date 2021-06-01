
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
// boost geometry
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

std::mt19937 rng;

// fun to initialise positions
std::vector<float> initialisePositions(int nItems, float maxSize) {
    // real uniform dist
    std::uniform_real_distribution<float> randPos(0.f, maxSize);
    std::vector<float> pos (nItems, 0.f);
    for(int i = 0; i < nItems; i++) {
        pos[i] = randPos (rng);
    }
    return pos;
}

bgi::rtree< value, bgi::quadratic<16> > initRtree (std::vector<float> pos) {
    // initialise rtree
    bgi::rtree< value, bgi::quadratic<16> > tmpRtree;
    for (int i = 0; i < pos.size(); ++i)
    {
        point p = point(pos[i], 0.f);
        tmpRtree.insert(std::make_pair(p, i));
    }
    return tmpRtree;
}

int main(int argc, char *argv[])
{
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    std::vector<std::string> cliArgs(argv, argv+argc);

    int nItems = std::stoi(cliArgs[1]);
    float maxSize = std::stof(cliArgs[2]);
    float distance = std::stof(cliArgs[3]);

    std::vector<float> thesePos = initialisePositions(nItems, maxSize);

    bgi::rtree< value, bgi::quadratic<16> > thisRtree = initRtree(thesePos);

    // query rtree with distance
    std::vector<value> nearItems;
    std::vector<int> itemId;
    for(int i = 0; i < thesePos.size(); i++) {

        std::cout << "\nitem loc = " << bg::wkt<point> (point(thesePos[i], 0.f)) << "\n";

        thisRtree.query(bgi::satisfies([&](value const& v) {
                            return bg::distance(v.first, point(thesePos[i], 0.f)) < distance;}),
                    std::back_inserter(nearItems));


        BOOST_FOREACH(value const& v, nearItems) {
                std::cout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
                itemId.push_back(v.second);
        }

        std::cout << "n near items = " << itemId.size() << "\n";


        itemId.clear();
        nearItems.clear();
    }

    return 0;
}
