#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//  ChessGeneticAlgorithm<T, PARAM_NBIT>
//
//
#ifndef _AL_CHESS_CHESSGENALGO_HPP
#define _AL_CHESS_CHESSGENALGO_HPP

namespace chess
{
    namespace ga
    {
        using namespace galgo;

        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
        class ChessGeneticAlgorithm : public GeneticAlgorithm<TYPE_PARAM, PARAM_NBIT>
        {
            using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        public:
            ChessGeneticAlgorithm(_ConditionValuationNode* node, int popsize, int nbgen, int _tournament_n_player, int _tournament_n_game);

            void setup_params();
            void run() override;

            std::vector<TYPE_PARAM> tournament(bool is_at_creation, const std::vector<TYPE_PARAM>& param) const override
            {
                std::vector<TYPE_PARAM> v;
                TYPE_PARAM fit = 0;
                for (int i = 0; i < tournament_n_player; i++)
                {
                    // select random player in curpop
                    for (int j = 0; j < tournament_n_game; j++)
                    {
                        // score += Game[chromo(param) vs random player]
                        fit += ((std::rand() % 10)-5)/10.0;
                    }
                }
                v.push_back(fit);
                return v;
            }

        private:
            _ConditionValuationNode*                _root;
            std::vector<_ConditionValuationNode*>   _terminal_nodes;
        };


        // ct
        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
        ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::ChessGeneticAlgorithm(
                                            _ConditionValuationNode* node, int popsize, int nbgen,
                                            int _tournament_n_player, int _tournament_n_game)
            : GeneticAlgorithm<TYPE_PARAM, PARAM_NBIT>(popsize, nbgen, _tournament_n_player, _tournament_n_game)
        {
            _root = node;
            _root->get_term_nodes(_terminal_nodes);
            setup_params();
        }

        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
        void ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::setup_params()
        {
            std::vector<double> _lowerBound;
            std::vector<double> _upperBound;
            //std::vector<double> _initialSet;
            for (int i = 0; i < _terminal_nodes.size(); i++)
            {
                _lowerBound.push_back(-10.0);
                _upperBound.push_back(+10.0);
                //_initialSet.push_back(0.00);
            }

            for (int i = 0; i<_lowerBound.size(); i++)
            {
                std::vector<TYPE_PARAM> w;
                w.push_back(_lowerBound[i]); w.push_back(_upperBound[i]); //w.push_back(_initialSet[i]);
                Parameter<double, PARAM_NBIT> p(w);
                param.emplace_back(new decltype(p)(p));

                if (i == 0) idx.push_back(0);
                else idx.push_back(idx[i - 1] + PARAM_NBIT);
            }
            lowerBound = _lowerBound;
            upperBound = _upperBound;
            //initialSet = _initialSet;

            //this->Objective = objective;
            this->nbbit = (int)_lowerBound.size()*PARAM_NBIT;
            this->nbparam = (int)_lowerBound.size();
        }

        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
        void ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::run()
        {
            this->check();

            pop = Population<TYPE_PARAM, PARAM_NBIT>(*this);
            pop.creation(true, false);                          // setup all chromo then evaluate()

            TYPE_PARAM bestResult = pop(0)->getTotal();
            if (output) print();

            for (int nogen = 1; nogen <= nbgen; ++nogen)
            {
                pop.evolution();                                // evaluate() called in recombination, completion newpop[i]->evaluate()

                bestResult = pop(0)->getTotal();
                if (output) print();
            }
        }

    };
};
#endif
