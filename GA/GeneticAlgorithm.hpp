#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                      Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
//  class GeneticAlgorithm<T, PARAM_NBIT>
//
//
#ifndef _AL_CHESS_GA_GENETICALGORITHM_HPP
#define _AL_CHESS_GA_GENETICALGORITHM_HPP

namespace galgo 
{
    template <typename T, int PARAM_NBIT>
    class GeneticAlgorithm
    {
       static_assert(std::is_same<float,T>::value || std::is_same<double,T>::value, "T must be float or double");

       template <typename K, int PARAM_NBIT> friend class Population;
       template <typename K, int PARAM_NBIT> friend class Chromosome;
       template <typename K> using Func = std::vector<K> (*)(const std::vector<K>&);

    protected:
       Population<T, PARAM_NBIT> pop;             // population of chromosomes
       std::vector<PAR<T>> param;     // parameter(s) 
       std::vector<T> lowerBound;     // parameter(s) lower bound
       std::vector<T> upperBound;     // parameter(s) upper bound
       std::vector<T> initialSet;     // initial set of parameter(s)
       std::vector<int> idx;          // indexes for chromosome breakdown

    public: 
       Func<T> Objective;                                                                       // objective function pointer
       void (*Selection)    (Population<T, PARAM_NBIT>&) = chess::ga::RWS;                                  // selection method initialized to roulette wheel selection                                 
       void (*CrossOver)    (const Population<T, PARAM_NBIT>&, CHR<T, PARAM_NBIT>&, CHR<T, PARAM_NBIT>&) = chess::ga::P1XO;         // cross-over method initialized to 1-point cross-over 
       void (*Mutation)     (CHR<T, PARAM_NBIT>&) = chess::ga::SPM;                                         // mutation method initialized to single-point mutation                                     
       void (*Adaptation)   (Population<T, PARAM_NBIT>&) = nullptr;                                         // adaptation to constraint(s) method                
       std::vector<T> (*Constraint)(const std::vector<T>&) = nullptr;                           // constraint(s)      

       T covrate = .50;   // cross-over rate
       T mutrate = .05;   // mutation rate   
       T SP = 1.5;        // selective pressure for RSP selection method 
       T tolerance = 0.0; // terminal condition (inactive if equal to zero)
                 
       int elitpop = 1;   // elit population size
       int matsize;       // mating pool size, set to popsize by default
       int tntsize = 10;  // tournament size
       int genstep = 1;  // generation step for outputting results
       int precision = 5; // precision for outputting results

    public:
       // constructor
       GeneticAlgorithm(int popsize, int nbgen,
                       std::vector<T>&  _lowerBound,
                       std::vector<T>&  _upperBound,
                       std::vector<T>&  _initialSet);

       GeneticAlgorithm(int popsize, int nbgen);

       virtual void run(bool reenty = false);       // run genetic algorithm  
       const CHR<T, PARAM_NBIT>& result() const;    // return best chromosome 

       virtual std::vector<T> tournament(bool is_at_creation, const std::vector<T>& param) const
       {
           std::vector<T> v;
           return v;
       }

    protected:
       int nbbit;     // total number of bits per chromosome
       int nbgen;     // number of generations
       int nogen = 0; // numero of generation
       int nbparam;   // number of parameters to be estimated
       int popsize;   // population size
       bool output;   // control if results must be outputted

       //int tournament_n_player;
       //int tournament_n_game;

       void check() const ; // check inputs validity
       void print() const;  // print results for each new generation
    };

    template <typename T, int PARAM_NBIT>
    GeneticAlgorithm<T, PARAM_NBIT>::GeneticAlgorithm(int _popsize, int _nbgen)
    {
        this->nbgen = _nbgen;
        this->popsize = _popsize;
        this->matsize = _popsize;
        this->output = true;
    }

    // constructor
    template <typename T, int PARAM_NBIT>
    GeneticAlgorithm<T, PARAM_NBIT>::GeneticAlgorithm(int _popsize, int _nbgen,
                            std::vector<T>& _lowerBound, 
                            std::vector<T>& _upperBound, 
                            std::vector<T>& _initialSet)
    {
        for (int i = 0; i<_lowerBound.size(); i++)
        {
            std::vector<T> w;
            w.push_back(_lowerBound[i]); w.push_back(_upperBound[i]); w.push_back(_initialSet[i]);
            Parameter<double, PARAM_NBIT> p(w);
            param.emplace_back(new decltype(p)(p));

            if (i == 0) idx.push_back(0);
            else idx.push_back(idx[i - 1] + PARAM_NBIT);
        }
        lowerBound = _lowerBound;
        upperBound = _upperBound;
        initialSet = _initialSet;

        //this->Objective = objective;
        this->nbbit = (int)_lowerBound.size()*PARAM_NBIT;
        this->nbparam = (int)_lowerBound.size();

        this->nbgen = _nbgen;
        this->popsize = _popsize;
        this->matsize = _popsize;
        this->output = true;
    };

    // check inputs validity
    template <typename T, int PARAM_NBIT>
    void GeneticAlgorithm<T, PARAM_NBIT>::check() const
    {
       if (!initialSet.empty()) {
          for (int i = 0; i < nbparam; ++i) {
             if (initialSet[i] < lowerBound[i] || initialSet[i] > upperBound[i]) {
                throw std::invalid_argument("Error: in class galgo::Parameter<T,N>, initial parameter value cannot be outside the parameter boundaries, please choose a value between its lower and upper bounds.");
             }
          }
          if (initialSet.size() != (unsigned)nbparam) {
             throw std::invalid_argument("Error: in class galgo::GeneticAlgorithm<T, PARAM_NBIT>, initial set of parameters does not have the same dimension than the number of parameters, please adjust.");
          }
       }
       if (SP < 1.0 || SP > 2.0) {
          throw std::invalid_argument("Error: in class galgo::GeneticAlgorithm<T, PARAM_NBIT>, selective pressure (SP) cannot be outside [1.0,2.0], please choose a real value within this interval.");
       }
       if (elitpop > popsize || elitpop < 0) {
          throw std::invalid_argument("Error: in class galgo::GeneticAlgorithm<T, PARAM_NBIT>, elit population (elitpop) cannot outside [0,popsize], please choose an integral value within this interval.");
       }
       if (covrate < 0.0 || covrate > 1.0) {
          throw std::invalid_argument("Error: in class galgo::GeneticAlgorithm<T, PARAM_NBIT>, cross-over rate (covrate) cannot outside [0.0,1.0], please choose a real value within this interval.");
       }
       if (genstep <= 0) {
          throw std::invalid_argument("Error: in class galgo::GeneticAlgorithm<T, PARAM_NBIT>, generation step (genstep) cannot be <= 0, please choose an integral value > 0.");
       }
    }

    // run genetic algorithm
    template <typename T, int PARAM_NBIT>
    void GeneticAlgorithm<T, PARAM_NBIT>::run(bool)
    {
       // checking inputs validity
       this->check();

       // setting adaptation method to default if needed
       if (Constraint != nullptr && Adaptation == nullptr) {
          Adaptation = chess::ga::DAC;
       }

       // initializing population
       pop = Population<T, PARAM_NBIT>(*this);

       if (output) {
          std::cout << "\n Running Genetic Algorithm...\n";
          std::cout << " ----------------------------\n";
       }

       // creating population
       pop.creation();

       // initializing best result and previous best result
       T bestResult = pop(0)->getTotal();
       T prevBestResult = bestResult;

       // outputting results 
       if (output) print();
    
       // starting population evolution
       for (nogen = 1; nogen <= nbgen; ++nogen) 
       {
          // evolving population
          pop.evolution();
          // getting best current result
          bestResult = pop(0)->getTotal();
          // outputting results
          if (output) print();
          // checking convergence
          if (tolerance != 0.0) {
             if (fabs(bestResult - prevBestResult) < fabs(tolerance)) {
                break;
             }
             prevBestResult = bestResult;
          }
       } 

       // outputting contraint value
       if (Constraint != nullptr) 
       {
          // getting best parameter(s) constraint value(s)
          std::vector<T> cst = pop(0)->getConstraint(); 
          if (output) {
             std::cout << "\n Constraint(s)\n";
             std::cout << " -------------\n";
             for (unsigned i = 0; i < cst.size(); ++i) 
             {
                std::cout << " C"; 
                if (nbparam > 1) {
                   std::cout << std::to_string(i + 1);
                }
                std::cout << "(x) = " << std::setw(6) << std::fixed << std::setprecision(precision) << cst[i] << " "; 
             }
             std::cout << " "; 
          }
       }   
    }

    // return best chromosome
    template <typename T, int PARAM_NBIT>
    inline const CHR<T, PARAM_NBIT>& GeneticAlgorithm<T, PARAM_NBIT>::result() const
    {
       return pop(0);
    }

    // print results for each new generation
    template <typename T, int PARAM_NBIT>
    void GeneticAlgorithm<T, PARAM_NBIT>::print() const
    {
       // getting best parameter(s) from best chromosome
       std::vector<T> bestParam = pop(0)->getParam();
       std::vector<T> bestResult = pop(0)->getResult();

       if (nogen % genstep == 0) {
          std::cout << " Generation = " << std::setw(std::to_string(nbgen).size()) << nogen << " |";
          for (int i = 0; i < nbparam; ++i) {
	          std::cout << " X";
             if (nbparam > 1) {
                std::cout << std::to_string(i + 1);
             }
             std::cout << " = "  << std::setw(9) << std::fixed << std::setprecision(precision) << bestParam[i] << " |";
	       }
          for (unsigned i = 0; i < bestResult.size(); ++i) {
	          std::cout << " F";
             if (bestResult.size() > 1) {
                std::cout << std::to_string(i + 1);
             }
             std::cout << "(x) = " << std::setw(12) << std::fixed << std::setprecision(precision) << bestResult[i];
             if (i < bestResult.size() - 1) {
                std::cout << " |";
             } else {
                std::cout << " ";
             }
	       }
 
       }
    }
}

#endif
