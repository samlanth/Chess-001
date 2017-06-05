#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//              Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
//  class Chromosome<T>
//
//
#ifndef _AL_CHESS_GA_CHROMOSOME_HPP
#define _AL_CHESS_GA_CHROMOSOME_HPP

namespace galgo 
{
    template <typename T>
    class Chromosome
    {
       static_assert(std::is_same<float,T>::value || std::is_same<double,T>::value, "T must be float or double");

    public:
       Chromosome(const GeneticAlgorithm<T>& ga);
       Chromosome(const Chromosome<T>& rhs);
       void create();
       void initialize();
       void evaluate(); 
       void reset();
       void setGene(int k);
       void initGene(int k, T value);
       void addBit(char bit);
       void setBit(char bit, int pos);
       void flipBit(int pos);
       char getBit(int pos) const;
       void setPortion(const Chromosome<T>& x, int start, int end);
       void setPortion(const Chromosome<T>& x, int start);
       const std::vector<T>& getParam() const;
       const std::vector<T>& getResult() const;
       T getTotal() const;
       const std::vector<T> getConstraint() const;
       int size() const;
       T mutrate() const;
       int nbgene() const;
       int nogen() const;
       const std::vector<T>& lowerBound() const;
       const std::vector<T>& upperBound() const;

    private:
       std::vector<T> param;                     // estimated parameter(s)
       std::vector<T> result;                    // chromosome objective function(s) result
       std::string chr;                          // string of bits representing chromosome
       const GeneticAlgorithm<T>* ptr = nullptr; // pointer to genetic algorithm
    public:
       T fitness;                                // chromosome fitness, objective function(s) result that can be modified (adapted to constraint(s), set to positive values, etc...)
    private:
       T total;                                  // total sum of objective function(s) result
       int chrsize;                              // chromosome size (in number of bits)
       int numgen;                               // numero of generation
    };

    // constructor
    template <typename T>
    Chromosome<T>::Chromosome(const GeneticAlgorithm<T>& ga) 
    {
       param.resize(ga.nbparam);
       ptr = &ga;
       chrsize = ga.nbbit;
       numgen = ga.nogen;
    }

    // copy constructor
    template <typename T>
    Chromosome<T>::Chromosome(const Chromosome<T>& rhs) 
    {
       param = rhs.param;
       result = rhs.result;
       chr = rhs.chr;
       ptr = rhs.ptr;
       // re-initializing fitness to its original value
       fitness = rhs.total;
       total = rhs.total;
       chrsize = rhs.chrsize;
       numgen = rhs.numgen;
    }

    // create new chromosome
    template <typename T>
    inline void Chromosome<T>::create()
    {
       chr.clear();

       for (const auto& x : ptr->param) {
          // encoding parameter random value
          std::string str = x->encode();
          chr.append(str);
       }  
    }

    // initialize chromosome (from known parameter values)
    template <typename T>
    inline void Chromosome<T>::initialize()
    {
       chr.clear();

       int i(0);
       for (const auto& x : ptr->param) {
          // encoding parameter initial value
          std::string str = x->encode(ptr->initialSet[i++]);
          chr.append(str);
       }      
    }

    // evaluate chromosome fitness
    template <typename T>
    inline void Chromosome<T>::evaluate() 
    {
       int i(0);
       for (const auto& x : ptr->param) {
          // decoding chromosome: converting chromosome string into a real value
          param[i] = x->decode(chr.substr(ptr->idx[i], x->size()));
	      i ++ ;
       } 
       // computing objective result(s) 
       result = ptr->Objective(param);
       // computing sum of all results (in case there is not only one objective functions)
       total = std::accumulate(result.begin(), result.end(), 0.0);
       // initializing fitness to this total
       fitness = total;
    } 

    // reset chromosome
    template <typename T>
    inline void Chromosome<T>::reset()
    {
       chr.clear();
       result = 0.0;
       total = 0.0;
       fitness = 0.0;
    }

    // set or replace kth gene by a new one
    template <typename T>
    inline void Chromosome<T>::setGene(int k)
    {
       #ifndef NDEBUG
       if (k < 0 || k >= ptr->nbparam) {
          throw std::invalid_argument("Error: in galgo::Chromosome<T>::setGene(int), argument cannot be outside interval [0,nbparam-1], please amend.");
       }
       #endif

       // generating a new gene
       std::string s = ptr->param[k]->encode();
       // adding or replacing gene in chromosome
       chr.replace(ptr->idx[k], s.size(), s, 0, s.size());
    }

    // initialize or replace kth gene by a know value
    template <typename T>
    inline void Chromosome<T>::initGene(int k, T x)
    {
       #ifndef NDEBUG
       if (k < 0 || k >= ptr->nbparam) {
          throw std::invalid_argument("Error: in galgo::Chromosome<T>::initGene(int), first argument cannot be outside interval [0,nbparam-1], please amend.");
       }
       #endif

       // encoding gene
       std::string s = ptr->param[k]->encode(x);
       // adding or replacing gene in chromosome
       chr.replace(ptr->idx[k], s.size(), s, 0, s.size());
    }

    // add chromosome bit to chromosome (when constructing a new one)
    template <typename T>
    inline void Chromosome<T>::addBit(char bit)
    {
       chr.push_back(bit);

       #ifndef NDEBUG
       if (chr.size() > chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::setBit(char), exceeding chromosome size.");
       }
       #endif
    }

    // initialize or replace an existing chromosome bit   
    template <typename T>
    inline void Chromosome<T>::setBit(char bit, int pos)
    {  
       #ifndef NDEBUG
       if (pos >= chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::replaceBit(char, int), second argument cannot be equal or greater than chromosome size.");
       }
       #endif

       std::stringstream ss;
       std::string str;
       ss << bit;
       ss >> str;
       chr.replace(pos, 1, str);
       std::cout << chr << "\n";
    }

    // flip an existing chromosome bit
    template <typename T>
    inline void Chromosome<T>::flipBit(int pos)
    {
       #ifndef NDEBUG
       if (pos >= chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::flipBit(int), argument cannot be equal or greater than chromosome size.");
       }
       #endif

       if (chr[pos] == '0') {
          chr.replace(pos, 1, "1");
       } else {
          chr.replace(pos, 1, "0");
       }
    }

    // get a chromosome bit
    template <typename T>
    inline char Chromosome<T>::getBit(int pos) const
    {
       #ifndef NDEBUG
       if (pos >= chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::getBit(int), argument cannot be equal or greater than chromosome size.");
       }
       #endif

       return chr[pos];
    }

    // initialize or replace a portion of bits with a portion of another chromosome (from position start to position end included)
    template <typename T>
    inline void Chromosome<T>::setPortion(const Chromosome<T>& x, int start, int end)
    {
       #ifndef NDEBUG
       if (start > chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::setPortion(const Chromosome<T>&, int, int), second argument cannot be greater than chromosome size.");
       }
       #endif

       chr.replace(start, end - start + 1, x.chr, start, end - start + 1);
    }

    // initialize or replace a portion of bits with a portion of another chromosome (from position start to the end of he chromosome)
    template <typename T>
    inline void Chromosome<T>::setPortion(const Chromosome<T>& x, int start)
    {
       #ifndef NDEBUG
       if (start > chrsize) {
          throw std::out_of_range("Error: in galgo::Chromosome<T>::setPortion(const Chromosome<T>&, int), second argument cannot be greater than chromosome size.");
       }
       #endif

       chr.replace(start, chrsize, x.chr, start, x.chrsize);
    }

    // get parameter value(s) from chromosome
    template <typename T>
    inline const std::vector<T>& Chromosome<T>::getParam() const
    {
       return param;
    }

    // get objective function result
    template <typename T>
    inline const std::vector<T>& Chromosome<T>::getResult() const
    {
       return result;
    }

    // get the total sum of all objective function(s) result
    template <typename T>
    inline T Chromosome<T>::getTotal() const
    {
       return total;
    }

    // get constraint value(s) for this chromosome
    template <typename T>
    inline const std::vector<T> Chromosome<T>::getConstraint() const
    {
       return ptr->Constraint(param);
    }

    // return chromosome size in number of bits
    template <typename T>
    inline int Chromosome<T>::size() const
    {
       return chrsize;
    }

    // return mutation rate 
    template <typename T>
    inline T Chromosome<T>::mutrate() const
    {
       return ptr->mutrate;
    }

    // return number of genes in chromosome
    template <typename T>
    inline int Chromosome<T>::nbgene() const
    {
       return ptr->nbparam;
    }

    // return numero of generation this chromosome belongs to
    template <typename T>
    inline int Chromosome<T>::nogen() const
    {
       return numgen;
    }

    // return lower bound(s)
    template <typename T>
    inline const std::vector<T>& Chromosome<T>::lowerBound() const
    {
       return ptr->lowerBound;
    }

    // return upper bound(s)
    template <typename T>
    inline const std::vector<T>& Chromosome<T>::upperBound() const
    {
       return ptr->upperBound;
    }

}

#endif
