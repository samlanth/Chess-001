#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//  ChessGeneticAlgorithm<T, PARAM_NBIT>
//
//
#ifndef _AL_CHESS_CHESSGA_CHESSCOEVOLVEGA_HPP
#define _AL_CHESS_CHESSGA_CHESSCOEVOLVEGA_HPP

namespace chess
{
    namespace ga
    {
        using namespace galgo;

        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND>
        class ChessCoEvolveGA
        {
            using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            using _BaseGame = BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            using _ChessGeneticAlgorithm = ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT, WEIGHT_BOUND>;

        public:
            ChessCoEvolveGA(_DomainPlayer* playW, _DomainPlayer* playB, BaseGame_Config cfg, size_t num_iter,
                            int popsize, int nbgen, int _tournament_n_player, int _tournament_n_game, char verbose = false)
            {
                _playW = playW;
                _playB = playB;
                _cfg = cfg;
                _gaW = new _ChessGeneticAlgorithm(true,  false, _playW, _playB, _cfg, popsize, nbgen, _tournament_n_player, _tournament_n_game, verbose);
                _gaB = new _ChessGeneticAlgorithm(false, false, _playW, _playB, _cfg, popsize, nbgen, _tournament_n_player, _tournament_n_game, verbose);
                _num_iter = num_iter;
                _verbose = verbose;
            }

            ~ChessCoEvolveGA()
            {
                delete _gaW;
                delete _gaB;
            }

            void run()
            {
                for (size_t i = 0; i < _num_iter; i++)
                {
                    if (_verbose) std::cout << "----------------------------\n" << "iter : " << i + 1 << " ";
                    _gaW->setup_params();
                    _gaW->run((i>0)?true:false);

                    _gaB->setup_params();
                    _gaB->run((i>0) ? true : false);

                    if (_verbose)
                    {
                        // Play some games
                        BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT> game(*_playW, *_playB);
                        game.set_constraints(_cfg);

                        double fit = 0;
                        chess::ExactScore sc;
                        for (size_t i = 0; i < 20; i++)
                        {
                            game.set_board(_playW->get_domain()->get_random_position(true));
                            if (_verbose > 2) game.print_nodes();
                            sc = game.play(_verbose, true);
                            if (sc == chess::ExactScore::WIN)       fit += 1.0;
                            else if (sc == chess::ExactScore::LOSS) fit += 0.0;
                            else if (sc == chess::ExactScore::DRAW) fit += 0.5;        
                        }
                        std::cout << "score= " << fit << " / " << 20 << " Last game => n:" << game.num_pos_eval() << " ply: " << game.ply() << std::endl;
                    }

                }
            }

        private:
            _DomainPlayer* _playW;
            _DomainPlayer* _playB;

            _ChessGeneticAlgorithm* _gaW;
            _ChessGeneticAlgorithm* _gaB;

            BaseGame_Config _cfg;
            size_t _num_iter;
            char _verbose;
        };

    };
};
#endif
