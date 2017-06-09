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

        void set_constraints(size_t _w_max_num_position_per_move, uint16_t _w_max_depth_per_move, size_t _b_max_num_position_per_move, uint16_t _b_max_depth_per_move, uint16_t max_game_ply);
        void set_board(_Board& initial_position) { _initial_position = initial_position; }

        virtual ExactScore play(bool verbose = false);

    protected:
        _DomainPlayer&          _playerW;
        _DomainPlayer&          _playerB;
        _Board                  _initial_position;
        size_t                  _w_max_num_position_per_move;
        uint16_t                _w_max_depth_per_move;
        size_t                  _b_max_num_position_per_move;
        uint16_t                _b_max_depth_per_move;
        uint16_t                _max_game_ply;
    };

    // BaseGame()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
        ::BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB) : _playerW(playerW), _playerB(playerB)
    {
    }

    // set_constraints
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline void BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
        ::set_constraints(size_t w_max_num_position_per_move, uint16_t w_max_depth_per_move, size_t b_max_num_position_per_move, uint16_t b_max_depth_per_move, uint16_t max_game_ply)
    {
        _w_max_num_position_per_move = w_max_num_position_per_move;
        _b_max_num_position_per_move = b_max_num_position_per_move;
        _w_max_depth_per_move = w_max_depth_per_move;
        _b_max_depth_per_move = b_max_depth_per_move;
        _max_game_ply = max_game_ply;
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
            std::cout << board.to_str() << std::endl;
        }

        while (true)
        {
            m = board.generate_moves();
            if ( board.is_final(m) )
            {
                return board.final_score(m);
            }
            else if (board.get_histo_size() >= _max_game_ply)
            {
                return ExactScore::DRAW;
            }

            if (board.get_color() == PieceColor::W)
                move_idx = _playerW.select_move_algo(board, m, _w_max_num_position_per_move, _w_max_depth_per_move, _max_game_ply);
            else
                move_idx = _playerB.select_move_algo(board, m, _b_max_num_position_per_move, _b_max_depth_per_move, _max_game_ply);

            assert(move_idx < m.size());
            board.apply_move(m[move_idx]);

            if (verbose)
            {
                std::cout << "ply:" << board.get_histo_size() << std::endl;
                std::cout << board.to_str() << std::endl;
            }
        }
    }

};

#endif
