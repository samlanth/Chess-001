#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// BaseGame<>   : Game class
//
//
#ifndef _AL_CHESS_GAME_GAME_HPP
#define _AL_CHESS_GAME_GAME_HPP

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
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _BaseGame = BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _GameDB_Record = GameDB_Record<PieceID, _BoardSize>;
        using _GameDB = GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB);
        virtual ~BaseGame() {}

        void set_constraints(BaseGame_Config c) { _config = c; }
        void set_board(_Board& initial_position) { _initial_position = initial_position; }

        virtual ExactScore play(bool verbose = false, bool save = false);
        void print_nodes() const;

        _DomainPlayer&          playerW() { return _playerW; }
        _DomainPlayer&          playerB() { return _playerB; }

        bool save(_Board& play_board) const;

   protected:
        _DomainPlayer&          _playerW;
        _DomainPlayer&          _playerB;
        _Board                  _initial_position;
        BaseGame_Config         _config;
        // last play
        PieceColor              _initial_color;
        ExactScore              _score = ExactScore::UNKNOWN;
    };

    // BaseGame()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB) : _playerW(playerW), _playerB(playerB)
    {
    }

    // play()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ExactScore BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::play(bool verbose, bool dosave)
    {
        size_t move_idx;
        std::vector<_Move>  m;

        _Board board = _initial_position;
        _initial_color = board.get_color();

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
                _score = board.final_score(m);
                if (dosave) this->save(board);
                return _score;
            }
            else if (board.get_histo_size() >= _config._max_game_ply)
            {
                _score = ExactScore::DRAW;
                if (dosave) this->save(board);
                return _score;
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

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(_Board& play_board) const
    {
        _GameDB_Record rec;
        rec._signature = "----";
        rec._status = 0;
        rec._board_size = _BoardSize;
        for (uint8_t x = 0; x < _BoardSize; x++)
        {
            for (uint8_t y = 0; y < _BoardSize; y++)
            {
                if (play_board.get_pieceid_at(x, y) != _Piece::empty_id())
                {
                    rec._pieces.push_back(play_board.get_pieceid_at(x, y));
                    rec._xy.push_back(std::pair<uint8_t, uint8_t>({x, y}));
                }
            }
        }
        rec._color = _initial_color;
        rec._history_moves = play_board.get_history_moves();
        rec._score = _score;

        rec._total_num_nodes_explored = 0; // ...

        rec._is_playW_internal = true;
        rec._is_playB_internal = true;
        rec._playerW_persist_id = this->_playerW.persist_key();
        rec._playerB_persist_id = this->_playerB.persist_key();
        rec._playerW_elo = 0;
        rec._playerB_elo = 0;
        rec._game_config = _config;

        _GameDB* db = _playerW.get_domain()->get_game_db();
        db->store_game(rec);
        return true;
    }

};

#endif
