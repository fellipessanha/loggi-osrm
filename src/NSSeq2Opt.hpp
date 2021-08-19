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
  MoveData& mvData;
  const std::vector<Delivery>& allDeliveries;
  const int capacity;

public:
  Move2Opt(const Instance& inst, MoveData mvDt)
    : mvData{ mvDt }
    , instance{ inst }
    , allDeliveries{ inst.getAllDeliveries() }
    , capacity{ inst.getCap() }
  {
  }

  bool canBeApplied(const ESolutionVRP& s) override
  {
    return true;
  }

  uptrMoveVPR apply(ESolutionVRP& se)
  {
    opt02(mvData, se);

    return uptrMoveVPR(new Move2Opt(instance, mvData));
  }

  /*  uptrMoveVPR applyUpdate(ESolutionVRP& se) override
  {
    ESolutionVRP& s = se.first;
    optframe::Evaluation<>& e = se.second;

    std::vector<std::vector<int>>& rep = s.getR();
    int f = 0;
    if (rep(x1, y1).left == rep(x1, y1 - 1).right)
      f++;
    if (rep(x1, y1).up == rep(x1 - 1, y1).down)
      f++;
    if (rep(x1, y1).right == rep(x1, y1 + 1).left)
      f++;
    if (rep(x1, y1).down == rep(x1 + 1, y1).up)
      f++;

    int g = 0;
    if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
      g++;
    if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
      g++;
    if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
      g++;
    if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
      g++;

    uptrMoveVPR rev = apply(se);

    int f2 = 0;
    if (rep(x1, y1).left == rep(x1, y1 - 1).right)
      f2++;
    if (rep(x1, y1).up == rep(x1 - 1, y1).down)
      f2++;
    if (rep(x1, y1).right == rep(x1, y1 + 1).left)
      f2++;
    if (rep(x1, y1).down == rep(x1 + 1, y1).up)
      f2++;

    int g2 = 0;
    if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
      g2++;
    if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
      g2++;
    if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
      g2++;
    if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
      g2++;

    e.setObjFunction(e.getObjFunction() + (f2 - f));
    e.setObjFunction(e.getObjFunction() + (g2 - g));

    return rev;
  } */

  /*  op<optframe::Evaluation<>> cost(const pair<ESolutionVRP, optframe::Evaluation<>>& se, bool allowEstimated) override
  {
    const ESolutionVRP& s = se.first;
    //const optframe::Evaluation<>& e = se.second;

    const std::vector<std::vector<int>>& rep = s.getR();
    // Calculus for the slot (x1, y1)
    double f1 = 0;
    if (rep(x1, y1).left == rep(x1, y1 - 1).right)
      f1++;
    if (rep(x1, y1).up == rep(x1 - 1, y1).down)
      f1++;
    if (rep(x1, y1).right == rep(x1, y1 + 1).left)
      f1++;
    if (rep(x1, y1).down == rep(x1 + 1, y1).up)
      f1++;

    int f2 = 0;
    int neighbor = 0;

    if ((x1 == x2) && (y1 - 1 == y2))
      neighbor++;
    if (rep(x2, y2).left == rep(x1, y1 - 1 + neighbor).right)
      f2++;
    neighbor = 0;

    if ((x1 - 1 == x2) && (y1 == y2))
      neighbor++;
    if (rep(x2, y2).up == rep(x1 - 1 + neighbor, y1).down)
      f2++;
    neighbor = 0;

    if ((x1 == x2) && (y1 + 1 == y2))
      neighbor++;
    if (rep(x2, y2).right == rep(x1, y1 + 1 - neighbor).left)
      f2++;
    neighbor = 0;

    if ((x1 + 1 == x2) && (y1 == y2))
      neighbor++;
    if (rep(x2, y2).down == rep(x1 + 1 - neighbor, y1).up)
      f2++;
    neighbor = 0;

    // Calculus for the slot (x2, y2)
    int g1 = 0;
    if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
      g1++;
    if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
      g1++;
    if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
      g1++;
    if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
      g1++;

    int g2 = 0;
    if ((rep(x1, y1).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
      g2++;
    if ((rep(x1, y1).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
      g2++;
    if ((rep(x1, y1).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
      g2++;
    if ((rep(x1, y1).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
      g2++;

    double f = (f2 - f1) + (g2 - g1);

    //return new MoveCost<> (f, 0);
    return make_optional(optframe::Evaluation<>(f, 0));
  } */

  virtual bool operator==(const optframe::Move<ESolutionVRP>& _m) const override // util busca tabu
  {
    //const Move2Opt& m = (const Move2Opt&)_m;
    //return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
    return false;
  }

  void print() const {}

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
  const int n_routes;
  //
  int curr_max;
  loggibud::MoveData mvDt;
  //
  // This serves as a quota for 2opts.
  // We may make it bigger to avoid small changes to the route.
  int opt02min = 0;

public:
  NSIterator2Opt(const std::vector<std::vector<int>>& se, const loggibud::Instance& inst)
    : instance{ inst }
    , candidate{ se }
    , n_routes{ se.size() }
  {}

  virtual ~NSIterator2Opt()
  {
  }

  virtual void first() override
  {
    // Starts from the last element so
    curr_max = candidate[mvDt.route1].size() - 1;
    mvDt.route1 = 0;
    mvDt.limits1.first = 1;
    mvDt.limits1.second = curr_max;
  }

  virtual void next() override
  {
    mvDt.limits1.second--;
    if (mvDt.limits1.second <= mvDt.limits1.first + opt02min) {
      if (mvDt.limits1.first == curr_max - (opt02min + 1)) {
        mvDt.route1++;
        curr_max = candidate[mvDt.route1].size() - 1;
        mvDt.limits1.first = 1;
        mvDt.limits1.second = curr_max;
      } else {
        mvDt.limits1.first++;
        mvDt.limits1.second = curr_max;
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

  // inutilizado no VND!
  virtual uptrMoveVPR randomMove(const ESolutionVRP& se) override
  {
    const std::vector<std::vector<int>>& rep = se.first;
    const int n = rep.size();

    // random route to apply 2opt
    int route = 1 + rand() % (n - 1);

    auto limits = twoRandNoDepot(n);

    return uptrMoveVPR(new Move2Opt(instance, MoveData(route, limits)));
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

  virtual void print() const
  {
  }

  string id() const {}
};

}