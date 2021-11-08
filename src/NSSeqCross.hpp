#pragma once

// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// Framework includes
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/NSIterator.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>
//
//#include "loggi-include.hpp"

#include "MoveData.hpp"

#include "loggi-context-loader.hpp"

#include "heuristic_moves.hpp"

//using namespace optframe;

// forward declaration
//const loggibud::MoveData&
//opt02(const loggibud::MoveData& moveData, ESolutionVRP& candidate);

namespace loggibud {

class MoveCross : public optframe::Move<ESolutionVRP>
{
protected:
  Instance& instance;
  //
  // This bugs if it's a reference.
  MoveData mvData;
  const std::vector<Delivery>& allDeliveries;
  const int capacity;
  int newWeightA;
  int newWeightB;

public:
  MoveCross(Instance& inst, MoveData mvDt)
    : mvData{ mvDt }
    , instance{ inst }
    , allDeliveries{ inst.getAllDeliveries() }
    , capacity{ inst.getCap() }
    , newWeightA{ 0 }
    , newWeightB{ 0 }
  {
    // std::cout << "\nInside MoveCross:\t" << mvData.limitsL.first << ", " << mvData.limitsL.second
    //           << "\nInside MoveCross:\t" << mvDt.limitsL.first << ", " << mvDt.limitsL.second << std::endl;
  }

  bool canBeApplied(const ESolutionVRP& s) override
  {
    std::cout << "checking possibility\t" << std::flush;

    // int indexA = mvData.route1;
    // auto& routeA = s.first[indexA];
    // auto weightA = instance.getWeight(indexA);
    // //
    // int indexB = mvData.route2;
    // auto& routeB = s.first[indexB];
    // auto weightB = instance.getWeight(indexB);

    // auto outofA = 0;
    // for (size_t i = 0; i < indexA; i++)
    //   outofA += instance.getWeight(routeA[i]);

    // auto outofB = 0;
    // for (size_t i = indexB; i < routeB.size(); i++)
    //   outofA += instance.getWeight(routeB[i]);
    // //
    // newWeightA = weightA - outofA + outofB;
    // newWeightB = weightB - outofB + outofA;
    //

    auto& routeA = s.first[mvData.route1];
    auto& routeB = s.first[mvData.route2];
    auto del = instance.getAllDeliveries();

    for (size_t i = 0; i < routeA.size(); i++) {
      if (i < mvData.limitsL.first)
        newWeightA += del[routeA[i]].size;
      else
        newWeightB += del[routeA[i]].size;
    }

    for (size_t i = 0; i < routeB.size(); i++) {
      if (i < mvData.limitsL.second)
        newWeightB += del[routeB[i]].size;
      else
        newWeightA += del[routeB[i]].size;
    }
    return (newWeightA <= capacity && newWeightB <= capacity);
  }

  uptrMoveVPR apply(ESolutionVRP& se)
  {
    std::cout << "applying Cross\t";
    // auto aux = instance.getWeight(mvData.route1);
    // instance.setWeight(mvData.route1, newWeightA);
    // newWeightA = aux;
    //
    // aux = instance.getWeight(mvData.route2);
    // instance.setWeight(mvData.route2, newWeightB);
    // newWeightB = aux;

    auto reverse = cross(mvData, se);

    return uptrMoveVPR(new MoveCross(instance, reverse));
  }

  virtual bool operator==(const optframe::Move<ESolutionVRP>& _m) const override // util busca tabu
  {
    //const MoveCross& m = (const MoveCross&)_m;
    //return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
    return false;
  }

  void print() const {}

}; // class MoveCross

class NSIteratorCross : public optframe::NSIterator<ESolutionVRP>
{
private:
  loggibud::Instance& instance;
  const std::vector<std::vector<int>>& candidate;
  const std::vector<int>& routesInCluster;
  int currRoute;
  const size_t n_routes;
  //
  int initialPoint = 0;
  loggibud::MoveData mvDt;

public:
  // The MoveData passed in this must have a route1
  // route2 and limits will vary based on route1's cluster
  NSIteratorCross(const std::vector<std::vector<int>>& se, loggibud::Instance& inst, const int rte)
    : instance{ inst }
    , candidate{ se }
    , routesInCluster{ instance.getRoutesInSameCluster(rte) }
    , n_routes{ routesInCluster.size() }

  {
    mvDt.route1 = rte;
    currRoute = mvDt.route1 != routesInCluster[0] ? 0 : 1;
    // std::cout << "\nInside NSIteratorCross\n";
  }

  virtual ~NSIteratorCross()
  {
  }

  virtual void first() override
  {
    // Starts from the last element so
    initialPoint = 0;
    mvDt.route2 = routesInCluster[currRoute];
    mvDt.limitsL.first = 1;
    mvDt.limitsL.second = 1;
  }

  virtual void next() override
  {
    mvDt.limitsL.second++;
    if (mvDt.limitsL.second >= candidate[mvDt.route2].size()) {
      mvDt.limitsL.first++;
      mvDt.limitsL.second = initialPoint;

      if (mvDt.limitsL.first >= candidate[mvDt.route1].size()) {
        currRoute += mvDt.route1 != routesInCluster[currRoute + 1] ? 1 : 2;
        mvDt.route2 = routesInCluster[currRoute];
        mvDt.limitsL.first = initialPoint;
        mvDt.limitsL.second = initialPoint;
      }
    }
  }

  virtual bool isDone() override
  {
    return (mvDt.route1 > n_routes);
  }

  virtual uptrMoveVPR current() override
  {
    return uptrMoveVPR(new MoveCross(instance, mvDt));
  }
};

class NSSeqCross : public optframe::NSSeq<ESolutionVRP>
{
private:
  Instance& instance;
  //sref<Instance> instance; // shared_ptr ou unique_ptr
  optframe::RandGen& rg;

public:
  NSSeqCross(Instance& inst, optframe::RandGen& _rg)
    : instance{ inst }
    , rg{ _rg }
  {
  }

  virtual ~NSSeqCross()
  {
  }

  // inutilizado no VND!
  virtual uptrMoveVPR randomMove(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    const int n = rep.size();

    // random route to apply cross
    std::cout << "randomizing stop...\t";

    // getting non-trivial route1 for safety
    int route1 = rand() % rep.size();
    while (instance.getRoutesInSameCluster(route1).size() < 3) {
      route1 = rand() % rep.size();
    }
    auto& inCluster = instance.getRoutesInSameCluster(route1);
    std::cout << inCluster.size();

    // Getting random route2 in asme cluster as route1. Careful not to repeat the same route.
    int route2 = inCluster[rand() % inCluster.size()];
    while (route1 == route2) {
      route2 = inCluster[rand() % inCluster.size()];
    }

    assert(instance.getClusters(route1) == instance.getClusters(route2));
    //
    std::cout << "randomizing limits...\t";
    auto movemove = MoveData(route1, route2, std::make_pair(rand() % rep[route1].size(), rand() % rep[route2].size()));

    std::cout << "calling MoveCross.\n";
    return uptrMoveVPR(new MoveCross(instance, movemove));
  }

  using uptr_iter = std::unique_ptr<optframe::NSIterator<ESolutionVRP>>;

  virtual uptr_iter getIterator(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    int route = rand() % rep.size();

    while (instance.getRoutesInSameCluster(route).size() < 3) {
      route++;
    }

    // return an iterator to the neighbors of 'rep'
    return uptr_iter(
      new NSIteratorCross(
        se.first,
        instance,
        route));
  }

  virtual void print() const
  {
  }

  string id() const { return ""; }
};

}