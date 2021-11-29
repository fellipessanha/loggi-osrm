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

class Move2Opt : public optframe::Move<ESolutionVRP>
{
protected:
  const Instance& instance;
  // This bugs if it's a reference.
  MoveData mvData;
  const std::vector<Delivery>& allDeliveries;
  const int capacity;

public:
  Move2Opt(const Instance& inst, MoveData mvDt)
    : mvData{ mvDt }
    , instance{ inst }
    , allDeliveries{ inst.getAllDeliveries() }
    , capacity{ inst.getCap() }
  {
    // std::cout << "\nInside Move2Opt:\t" << mvData.limitsL.first << ", " << mvData.limitsL.second
    //           << "\nInside Move2Opt:\t" << mvDt.limitsL.first << ", " << mvDt.limitsL.second << std::endl;
  }

  bool independentOf(const Move<ESolutionVRP>& _m) override
  {
    auto& m = (Move2Opt&)_m;

    if (mvData.route1 != m.mvData.route1)
      return true;

    auto mlimits = m.mvData.limitsL;
    auto limits = mvData.limitsL;

    if (limits.first > mlimits.second)
      return true;

    if (mlimits.first > limits.second)
      return true;

    return false;
  }

  bool canBeApplied(const ESolutionVRP& s) override
  {

    if (mvData.route1 >= s.first.size())
      return false;

    int n = s.first.at(mvData.route1).size();
    auto optLimits = mvData.limitsL;

    if (optLimits.first > optLimits.second)
      return false;
    if (optLimits.first >= n || optLimits.second >= n)
      return false;

    return true;
  }

  uptrMoveVPR apply(ESolutionVRP& se)
  {

    opt02(mvData, se);

    return uptrMoveVPR(new Move2Opt(instance, mvData));
  }

  // virtual bool operator==(const optframe::Move<ESolutionVRP>& _m) const override // util busca tabu
  // {
  //   return false;
  // }

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "Move2Opt\tmvDt( rt1= " << mvData.route1 << "; limits= (" << mvData.limitsL.first << ", " << mvData.limitsL.second << ")";
    return ss.str();
  }

  virtual bool operator==(const Move<ESolutionVRP>& _m) const
  {
    const Move2Opt& m = (const Move2Opt&)_m; // You can only compare if types are equal

    if (
      (mvData.route1 == m.mvData.route1) &&
      ((mvData.limitsL == m.mvData.limitsL) || mvData.limitsL == std::make_pair(m.mvData.limitsL.second, m.mvData.limitsL.first)))
      return true;
    else
      return false;
  }

  // string id() const
  // {
  //    return "OptFrame:Move:Move2Opt";
  // }
}; // class Move2Opt

class NSIterator2Opt : public optframe::NSIterator<ESolutionVRP>
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
  NSIterator2Opt(const std::vector<std::vector<int>>& se, const loggibud::Instance& inst, loggibud::MoveData mvData = MoveData(0, { 0, 0 }))
    : instance{ inst }
    , candidate{ se }
    , n_routes{ se.size() }
    , mvDt{ mvData }
  {
    // std::cout << "\nInside NSIterator2Opt\n";
  }

  virtual ~NSIterator2Opt()
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
    return uptrMoveVPR(new Move2Opt(instance, mvDt));
  }
};

class NSSeq2Opt : public optframe::NSSeq<ESolutionVRP>
{
private:
  const Instance& instance;
  //sref<Instance> instance; // shared_ptr ou unique_ptr
  optframe::RandGen& rg;

public:
  NSSeq2Opt(const Instance& inst, optframe::RandGen& _rg)
    : instance{ inst }
    , rg{ _rg }
  {
  }

  virtual ~NSSeq2Opt()
  {
  }

  virtual bool isSolutionIndependent() const override
  {
    return true;
  }

  virtual bool supportsMoveIndependence() const override
  {
    return true;
  }

  // inutilizado no VND!
  virtual uptrMoveVPR randomMove(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    const int n = rep.size();

    // random route to apply 2opt
    int route = rg.rand(n);

    // std::cout << "route = " << route << std::endl;
    //           << "limits = " << rep[route].size() << std::endl;

    auto limits = twoRandNoDepotwithrandgen(rep.at(route).size(), rg);

    // std::cout << "xMoveData: (" << route << ", {" << limits.first << ", " << limits.second << "});"
    //           << "\t size = " << rep.at(route).size() << "\t" << std::flush;

    assert(limits.first < rep.at(route).size());
    assert(limits.second < rep.at(route).size());

    auto movemove = MoveData(route, limits);
    // std::cout << "returning Move2Opt" <<std::endl;

    return uptrMoveVPR(new Move2Opt(instance, movemove));
  }

  using uptr_iter = std::unique_ptr<optframe::NSIterator<ESolutionVRP>>;

  virtual uptr_iter getIterator(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    // return an iterator to the neighbors of 'rep'
    return uptr_iter(
      new NSIterator2Opt(
        se.first,
        instance));
  }

  string id() const { return ""; }
};

}