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

class MoveShift20 : public optframe::Move<ESolutionVRP>
{
protected:
  const Instance& instance;
  // This bugs if it's a reference. 
  MoveData mvData;
  const std::vector<Delivery>& allDeliveries;

public:
  MoveShift20(const Instance& inst, MoveData mvDt)
    : mvData{ mvDt }
    , instance{ inst }
    , allDeliveries{ inst.getAllDeliveries() }
  {
    // std::cout << "\nInside MoveShift20:\t" << mvData.limitsL.first << ", " << mvData.limitsL.second
              // << "\nInside MoveShift20:\t" << mvDt.limitsL.first << ", " << mvDt.limitsL.second << std::endl;

  }

  bool canBeApplied(const ESolutionVRP& s) override
  {
    auto weight = 0;
    for (auto i: s.first[mvData.route2])
      weight += allDeliveries[i].size;
    
    weight += allDeliveries[s.first[mvData.route1][mvData.limitsL.first]].size
            + allDeliveries[s.first[mvData.route1][mvData.limitsL.first+1]].size;

    return weight <= instance.getCap();
  }

  uptrMoveVPR apply(ESolutionVRP& se)
  {

    shift20(mvData, se);

    return uptrMoveVPR(new MoveShift20(instance, mvData));
  }

  virtual bool operator==(const optframe::Move<ESolutionVRP>& _m) const override // util busca tabu
  {
    //const MoveShift20& m = (const MoveShift20&)_m;
    //return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
    return false;
  }

  void print() const {}
  
}; // class MoveShift20

class NSIteratorShift20 : public optframe::NSIterator<ESolutionVRP>
{
private:
  const loggibud::Instance& instance;
  const std::vector<std::vector<int>>& candidate;
  const size_t n_routes;
  //
  int curr_max;
  loggibud::MoveData mvDt;
  //
  // This serves as a quota for 2opts.
  // We may make it bigger to avoid small changes to the route.
  int opt02min = 0;

public:
  NSIteratorShift20(const std::vector<std::vector<int>>& se, const loggibud::Instance& inst)
    : instance{ inst }
    , candidate{ se }
    , n_routes{ se.size() }
    // , mvDt { mvData }
  {
    // std::cout << "\nInside NSIteratorShift20\n";
  }

  virtual ~NSIteratorShift20()
  {
  }

  virtual void first() override
  {
    // Starts from the last element so
    curr_max = candidate[mvDt.route1].size() - 1;
    mvDt.route1 = 0;
    mvDt.limitsL.first = 1;
    mvDt.limitsL.second = curr_max;
  }

  virtual void next() override
  {
    mvDt.limitsL.second--;
    if (mvDt.limitsL.second <= mvDt.limitsL.first + opt02min) {
      if (mvDt.limitsL.first == curr_max - (opt02min + 1)) {
        mvDt.route1++;
        curr_max = candidate[mvDt.route1].size() - 1;
        mvDt.limitsL.first = 1;
        mvDt.limitsL.second = curr_max;
      } else {
        mvDt.limitsL.first++;
        mvDt.limitsL.second = curr_max;
      }
    }
  }

  virtual bool isDone() override
  {
    return (mvDt.route1 >= n_routes);
  }

  virtual uptrMoveVPR current() override
  {
    return uptrMoveVPR(new MoveShift20(instance, mvDt));
  }
};

class NSSeqShift20 : public optframe::NSSeq<ESolutionVRP>
{
private:
  const Instance& instance;
  //sref<Instance> instance; // shared_ptr ou unique_ptr
  optframe::RandGen& rg;

public:
  NSSeqShift20(const Instance& inst, optframe::RandGen& _rg)
    : instance{ inst }
    , rg{ _rg }
  {
    // std::cout << "2opt iterator" <<std::endl;
  }

  virtual ~NSSeqShift20()
  {
  }

  // inutilizado no VND!
  virtual uptrMoveVPR randomMove(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    const int n = rep.size();

    // random route to apply 2opt
    int routeA = rand() % n;
    
    const auto& inCluster = instance.getRoutesInSameCluster(rep[routeA].back());
    int routeB = inCluster[rand() % inCluster.size()];
    while(routeA == routeB){
      routeB = inCluster[rand() % inCluster.size()];
    }
    // assert(instance.getClusters(routeA) == instance.getClusters(routeB));
    
    int szA  = rep[routeA].size();
    int szB  = rep[routeB].size();

    std::pair<int, int> limits{rand() % (rep[szA].size() - 2), rand() % (rep[szB].size() - 2)};

    auto movemove = MoveData(routeA, routeB, limits);
    // std::cout << "returning MoveShift20" <<std::endl;

    return uptrMoveVPR(new MoveShift20(instance, movemove));
  }

  using uptr_iter = std::unique_ptr<optframe::NSIterator<ESolutionVRP>>;

  virtual uptr_iter getIterator(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    // return an iterator to the neighbors of 'rep'
    return uptr_iter(
      new NSIteratorShift20(
        se.first,
        instance));
  }

  virtual void print() const
  {
  }

  string id() const { return "";}
};

}