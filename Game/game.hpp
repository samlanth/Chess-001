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
        size_t      _max_num_position;
        uint16_t    _max_game_ply;
    };
    std::ofstream& operator<<(std::ofstream& os, const BaseGame_Config& m)
    {
        os << m._w_max_num_position_per_move << " " << m._w_max_depth_per_move << " ";
        os << m._b_max_num_position_per_move << " " << m._b_max_depth_per_move << " ";
        os << m._max_num_position << " " << m._max_game_ply << " ";
        return os;
    }

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

        void set_constraints(BaseGame_Config c)     { _config = c; }
        void set_board(_Board& initial_position)    { _initial_position = initial_position; }

        virtual ExactScore play(char verbose = false, bool save = false);

        _DomainPlayer&  playerW() { return _playerW; }
        _DomainPlayer&  playerB() { return _playerB; }

        bool save(_Board& play_board) const;
        size_t num_pos_eval() const { return _num_pos_eval; }
        size_t ply() const { return _ply; }
        size_t saved_index() const { return _saved_index; }
        void set_saved_index(size_t idx) const{ _saved_index = idx; }
        bool save_ss() const;
        const std::string ss() const { return _ss.str(); }

        void reset_game();

   protected:
        _DomainPlayer&          _playerW;
        _DomainPlayer&          _playerB;
        _Board                  _initial_position;
        BaseGame_Config         _config;

        // last play() data
        PieceColor              _initial_color;
        ExactScore              _score = ExactScore::UNKNOWN;
        size_t                  _num_pos_eval = 0;
        size_t                  _ply = 0;
        mutable size_t          _saved_index;
        std::stringstream       _ss;
    };

    // BaseGame()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::BaseGame(_DomainPlayer& playerW, _DomainPlayer& playerB) : _playerW(playerW), _playerB(playerB)
    {
        _saved_index = 0;
    }

    // reset_game
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline void BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::reset_game()
    {
        _score = ExactScore::UNKNOWN;
        _num_pos_eval = 0;
        _ply = 0;
        _saved_index = 0;
        _ss.clear();
        _ss = std::stringstream();
    }

    // play()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ExactScore BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::play(char verbose, bool dosave)
    {
        size_t move_idx;
        std::vector<_Move>  m;

        _Board board = _initial_position;
        _initial_color = board.get_color();
        reset_game();

        if (verbose > 1)
        {
            _ss << "ply:" << board.get_histo_size() << std::endl;
            if (board.is_in_check())  _ss << "in_check " << std::endl;
            _ss << board.to_str() << std::endl;
        }

        while (true)
        {
            _ply = board.get_histo_size();
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

            // select_move_algo()
            if (board.get_color() == PieceColor::W) move_idx = _playerW.select_move_algo(board, m, _config._w_max_num_position_per_move, _config._max_num_position, _config._w_max_depth_per_move, _config._max_game_ply, _num_pos_eval, verbose, _ss);
            else                                    move_idx = _playerB.select_move_algo(board, m, _config._b_max_num_position_per_move, _config._max_num_position, _config._b_max_depth_per_move, _config._max_game_ply, _num_pos_eval, verbose, _ss);

            assert(move_idx < m.size());
            board.apply_move(m[move_idx]);

            if (verbose > 1)
            {
                _ss << "ply:" << board.get_histo_size() << std::endl;
                if (board.is_in_check())  _ss << "in_check " << std::endl;
                _ss << board.to_str() << std::endl;
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(_Board& play_board) const
    {
        _GameDB_Record rec;
        rec._signature = "----";
        rec._status = 0;
        rec._board = play_board;
        rec._initial_color = _initial_color;
        rec._score = _score;
        rec._total_num_nodes_explored = _num_pos_eval;
        rec._is_playW_internal = true;
        rec._is_playB_internal = true;
        rec._playerW_persist_id = this->_playerW.persist_key();
        rec._playerB_persist_id = this->_playerB.persist_key();
        rec._playerW_elo = 0;
        rec._playerB_elo = 0;
        rec._game_config = _config;

        _GameDB* db = _playerW.domain()->get_game_db();
        size_t saved_index;
        bool r = db->store_game(rec, saved_index);
        this->set_saved_index(saved_index);
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save_ss() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("gamedisplay", std::to_string(_saved_index));
        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (os.good())
        {
            os << _ss.str() << std::endl;
            os.close();
            return true;
        }
        os.close();
        return false;
    }
};

#endif
