#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// BaseGame<>   : Game class
//
//
#ifndef _AL_CHESS_GAME_H
#define _AL_CHESS_GAME_H

namespace chess
{
    struct BaseGame_Config
    {
        size_t      _w_max_num_position_per_move;
        uint16_t    _w_max_depth_per_move;
        size_t      _b_max_num_position_per_move;
        uint16_t    _b_max_depth_per_move;
        uint16_t    _max_game_ply;
    };

    // BaseGame interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BaseGame
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _BaseGame = BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB);
        virtual ~BaseGame() {}

        void set_constraints(BaseGame_Config c) { _config = c; }
        void set_board(_Board& initial_position) { _initial_position = initial_position; }

        virtual ExactScore play(bool verbose = false);
        void print_nodes() const;

        _DomainPlayer&          playerW() { return _playerW; }
        _DomainPlayer&          playerB() { return _playerB; }

   protected:
        _DomainPlayer&          _playerW;
        _DomainPlayer&          _playerB;
        _Board                  _initial_position;
        BaseGame_Config         _config;
    };

    // BaseGame()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
        ::BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB) : _playerW(playerW), _playerB(playerB)
    {
    }

    // play()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ExactScore BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::play(bool verbose)
    {
        size_t move_idx;
        std::vector<_Move>  m;

        _Board board = _initial_position;

        if (verbose)
        {
            std::cout << "ply:" << board.get_histo_size() << std::endl;
            if (board.is_in_check())  std::cout << "in_check " << std::endl;
            std::cout << board.to_str() << std::endl;
        }

        while (true)
        {
            m = board.generate_moves();
            if ( board.is_final(m) )
            {
                return board.final_score(m);
            }
            else if (board.get_histo_size() >= _config._max_game_ply)
            {
                return ExactScore::DRAW;
            }

            if (board.get_color() == PieceColor::W)
                move_idx = _playerW.select_move_algo(board, m, _config._w_max_num_position_per_move, _config._w_max_depth_per_move, _config._max_game_ply, verbose);
            else
                move_idx = _playerB.select_move_algo(board, m, _config._b_max_num_position_per_move, _config._b_max_depth_per_move, _config._max_game_ply, verbose);

            assert(move_idx < m.size());
            board.apply_move(m[move_idx]);

            if (verbose)
            {
                std::cout << "ply:" << board.get_histo_size() << std::endl;
                if (board.is_in_check())  std::cout << "in_check " << std::endl;
                std::cout << board.to_str() << std::endl;
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline void BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::print_nodes() const
    {
        for (int i = 0; i < _playerW.get_root()->positive_child()->weights().size(); i++)
            std::cout << _playerW.get_root()->positive_child()->weights().at(i) << " ";
        std::cout << " : ";
        for (int i = 0; i < _playerW.get_root()->negative_child()->weights().size(); i++)
            std::cout << _playerW.get_root()->negative_child()->weights().at(i) << " ";
        std::cout << std::endl;
        for (int i = 0; i < _playerB.get_root()->positive_child()->weights().size(); i++)
            std::cout << _playerB.get_root()->positive_child()->weights().at(i) << " ";
        std::cout << " : ";
        for (int i = 0; i < _playerB.get_root()->negative_child()->weights().size(); i++)
            std::cout << _playerB.get_root()->negative_child()->weights().at(i) << " ";
        std::cout << std::endl;
    }
};

#endif
