#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//              Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
//
#include "Galgo.hpp"

namespace galgo_example
{
    template <typename T>
    class MyObjective2
    {
    public:
        // objective function example : Rosenbrock function
        // minimizing f(x,y) = (1 - x)^2 + 100 * (y - x^2)^2
        static std::vector<T> Objective(const std::vector<T>& x)
        {
            T obj = 0.47;
            return{ obj };
        }
        // NB: GALGO maximize by default so we will maximize -f(x,y)
    };

    template <typename T>
    class MyObjective
    {
    public:
        // objective function example : Rosenbrock function
        // minimizing f(x,y) = (1 - x)^2 + 100 * (y - x^2)^2
        static std::vector<T> Objective(const std::vector<T>& x)
        {
            T obj = -(pow(1 - x[0], 2) + 100 * pow(x[1] - x[0] * x[0], 2));
            return{ obj };
        }
        // NB: GALGO maximize by default so we will maximize -f(x,y)
    };

    // constraints example:
    // 1) x * y + x - y + 1.5 <= 0
    // 2) 10 - x * y <= 0
    template <typename T>
    std::vector<T> MyConstraint(const std::vector<T>& x)
    {
        return{ x[0] * x[1] + x[0] - x[1] + 1.5,10 - x[0] * x[1] };
    }
    // NB: a penalty will be applied if one of the constraints is > 0 
    // using the default adaptation to constraint(s) method

    int galgo_example_001()
    {
        // initializing parameters lower and upper bounds
        // an initial value can be added inside the initializer list after the upper bound
        galgo::Parameter<double> par1({ 0.0,1.0 });
        galgo::Parameter<double> par2({ 0.0,13.0 });
        // here both parameter will be encoded using 16 bits the default value inside the template declaration
        // this value can be modified but has to remain between 1 and 64

        //MyObjective<double>::Objective
        //galgo::GeneticAlgorithm<double, 16> ga(100, 50, true, par1, par2); // init genetic algorithm
        //ga.Constraint = MyConstraint;       // setting constraints
        //ga.run();                           // running genetic algorithm

        return 0;
    }
}
