#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Board<PieceID, _BoardSize>
//
// Board represent a chess board of size [_BoardSize*_BoardSize]
// Board internal representation is std::vector<PieceID>
// _BoardSize maximum is 255
//
// PieceID type is the piece identifier type  
// PieceID type must be convertible to integral type
//
//
#ifndef _AL_CHESS_BOARD_H
#define _AL_CHESS_BOARD_H

#include "core/piece.hpp"
#include "core/move.hpp"
#include <list>
#include <future>

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID, typename uint8_t _BoardSize> class BoardFunc;

    template <typename PieceID, typename uint8_t _BoardSize>
    class Board
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        friend class BoardFunc<PieceID, _BoardSize>;

    public:
        Board(  bool set_classic = false,
                bool allow_self_check = true,
                bool check_repeating_move_draw = false,
                bool check_50_moves_draw = false);

        Board(const Board&)                 = default;
        Board & operator=(const Board &)    = default;
        Board(Board&&)                      = default;
        Board & operator=(Board&&)          = default;
        ~Board()                            = default;

        const uint8_t index_at(uint8_t x, uint8_t y) const { return _BoardSize * y + x; }
        const _Piece* piece_at(uint8_t x, uint8_t y) const { return _Piece::get(_cells.at(index_at(x, y))); }

        const PieceID get_pieceid_at(uint8_t x, uint8_t y) const { return _cells.at(index_at(x, y)); }
        void          set_pieceid_at(PieceID id, uint8_t x, uint8_t y) { _cells.at(index_at(x, y)) = id; }

        const PieceColor get_color() const;
        const PieceColor get_opposite_color() const;
        void set_opposite_color();

        void apply_move(const _Move& m);
        void undo_move();

        bool can_capture_opposite_king(const std::vector<_Move>& m, size_t& ret_index) const;
        void set_classic_pos();
        bool has_piece(PieceName n, PieceColor c) const;
        size_t cnt_piece(PieceName n, PieceColor c) const;
        size_t cnt_all_piece() const;
        bool is_final(const std::vector<_Move>& m) const;

        const std::vector<_Move>    generate_moves(bool is_recursive_call = false);
        const std::list<_Move>      get_history_moves() const { return _history_moves; }

        bool is_allow_self_check()          const { return _allow_self_check; }
        bool is_check_repeating_move_draw() const { return _check_repeating_move_draw; }
        bool is_check_50_moves_draw()       const { return _check_50_moves_draw; }

    private:
        PieceColor              _color_toplay;
        std::vector<PieceID>    _cells;

        // Features
        bool _allow_self_check;
        bool _check_repeating_move_draw;
        bool _check_50_moves_draw;

        // History
        std::list<_Move> _history_moves; 
    };

    // generate_moves_no_self_check()
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<Move<PieceID>> generate_moves_no_self_check(Board<PieceID, _BoardSize>&, std::vector<Move<PieceID>>& m, size_t from, size_t to);

    // Board()
    template <typename PieceID, typename uint8_t _BoardSize>
    Board<PieceID, _BoardSize>::Board(
                bool set_classic,
                bool allow_self_check,
                bool check_repeating_move_draw,
                bool check_50_moves_draw)
        : 
                _cells(_BoardSize*_BoardSize),
                _allow_self_check(allow_self_check),
                _check_repeating_move_draw(check_repeating_move_draw),
                _check_50_moves_draw(check_50_moves_draw)
    {
        _color_toplay = PieceColor::none;
        PieceID emptyid = _Piece::empty_id();
        for (auto &v : _cells) v = emptyid;
        if (set_classic) set_classic_pos();
    }

    // get_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceColor Board<PieceID, _BoardSize>::get_color() const
    { 
        return _color_toplay;
    }

    // get_opposite_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceColor Board<PieceID, _BoardSize>::get_opposite_color() const
    {
        if (_color_toplay == PieceColor::B) return PieceColor::W;
        else if (_color_toplay == PieceColor::W) return PieceColor::B;
        return PieceColor::none;
    }

    // set_opposite_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::set_opposite_color() {
        if (_color_toplay == PieceColor::B) _color_toplay = PieceColor::W;
        else if (_color_toplay == PieceColor::W) _color_toplay = PieceColor::B;
    }

    // set_classic_pos()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::set_classic_pos()
    {
        if (_BoardSize < 8) return;	// throw...

        _color_toplay = PieceColor::W;
        _history_moves.clear();

        PieceID emptyid = _Piece::empty_id();
        for (auto &v : _cells) v = emptyid;

        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 0, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 1, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 2, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::W), 3, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::W), 4, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 5, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 6, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 7, 0);
        for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::W), i, 1);

        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 0, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 1, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 2, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::B), 3, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::B), 4, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 5, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 6, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 7, 7);
        for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::B), i, 6);
    }

    // has_piece()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::has_piece(PieceName n, PieceColor c) const
    {
        PieceID id = _Piece::get_id(n, c);
        for (const auto &m : _cells)
            if (m == id) return true;
        return false;
    }

    // cnt_piece
    template <typename PieceID, typename uint8_t _BoardSize>
    inline size_t Board<PieceID, _BoardSize>::cnt_piece(PieceName n, PieceColor c) const
    {
        size_t cnt = 0;
        PieceID id = _Piece::get_id(n, c);
        for (const auto &v : _cells)
        {
            if (v == id) cnt++;
        }
        return cnt;
    }

    // cnt_all_piece
    template <typename PieceID, typename uint8_t _BoardSize>
    inline size_t Board<PieceID, _BoardSize>::cnt_all_piece() const
    {
        size_t cnt = 0;
        for (const auto &v : _cells)
        {
            cnt++;
        }
        return cnt;
    }

    // can_capture_opposite_king()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::can_capture_opposite_king(const std::vector<_Move>& m, size_t& ret_index) const
    {
        size_t index = 0;
        PieceID K_id = _Piece::get_id(PieceName::K, get_opposite_color());
        for (const auto &mv : m)
        {
            if (mv.prev_dst_id == K_id)
            {
                ret_index = index;
                return true;
            }
            index++;
        }
        return false;
    }

    // is_final()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::is_final(const std::vector<_Move>& m) const
    {
        if (!has_piece(PieceName::K, PieceColor::W)) return true;
        if (!has_piece(PieceName::K, PieceColor::B)) return true;
        if (m.size() == 0) return true;

        if (!_allow_self_check)
        {
            // No extra processing required:
            // 1 - K checkmated then (m.size() == 0) - already checked
            // 2 - Opponent_K checkmated then on next move would be case 1 - ok
            // 3 - Draw then (m.size() == 0) - already checked
        }

        if (_check_repeating_move_draw)
        {
            // ...
        }

        if (_check_50_moves_draw)
        {
            // ...
        }
        return false;
    }

    // apply_move
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::apply_move(const _Move& m)
    {
        _cells.at(index_at(m.dst_x, m.dst_y)) = _cells.at(index_at(m.src_x, m.src_y));
        _cells.at(index_at(m.src_x, m.src_y)) = _Piece::empty_id();
        if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
        {
            _cells.at(index_at(m.src_x + m.mu.x * 1, m.src_y - m.mu.y * 1)) = _Piece::empty_id();
        }
        else if (m.mu.context_extra == "Q") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::Q, _color_toplay);
        else if (m.mu.context_extra == "R") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        else if (m.mu.context_extra == "B") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::B, _color_toplay);
        else if (m.mu.context_extra == "N") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::N, _color_toplay);
        else if (m.mu.context_extra == "castlingK_R") _cells.at(index_at(7, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        else if (m.mu.context_extra == "castlingQ_R") _cells.at(index_at(3, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        _history_moves.push_back(m);
        set_opposite_color();
    }

    // undo_move()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::undo_move()
    {
        if (_history_moves.size() == 0) return;

        _Move m = _history_moves.back();
        _history_moves.pop_back();

        _cells.at(index_at(m.src_x, m.src_y)) = m.prev_src_id;
        _cells.at(index_at(m.dst_x, m.dst_y)) = m.prev_dst_id;
        if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
        {
            if (_history_moves.size() > 0)
            {
                _Move mh = _history_moves.back();
                _cells.at(index_at(mh.dst_x, mh.dst_y)) = mh.prev_src_id;
            }
        }
        //castling...
        set_opposite_color();
    }

    // generate_moves()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::vector<Move<PieceID>> Board<PieceID, _BoardSize>::generate_moves(bool is_recursive_call)
    {
        using _Board = Board<PieceID, _BoardSize>;

        const _Piece*  p_src;
        const _Piece*  p_dst;
        std::vector<_Move> m;

        for (uint8_t i = 0; i < _BoardSize; i++)
        {
            for (uint8_t j = 0; j < _BoardSize; j++)
            {
                p_src = this->piece_at(i, j);

                if (p_src->color != this->_color_toplay) continue;
                if (p_src->name != PieceName::none) // valid src piece
                {
                    for (auto &mu : p_src->moves)
                    {
                        bool prev_path_empty = true;
                        for (uint8_t n = 1; n <= mu.len; n++) // follow mu direction
                        {
                            if ((prev_path_empty) &&
                                (i + mu.x*n >= 0) && (i + mu.x*n < _BoardSize) &&
                                (j + mu.y*n >= 0) && (j + mu.y*n < _BoardSize)
                                )
                            {
                                p_dst = this->piece_at((uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n));

                                // Move
                                _Move mv( i, j, (uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n),  p_src->get_id(), p_dst->get_id(), mu );
                                if (_history_moves.size() > 0)
                                {
                                    mv.copy_state(_history_moves.back());
                                }

                                // castling
                                if (    (mu.flag == MoveUnit::FLAG::conditional) &&
                                        (p_src->get_name() == PieceName::K) &&
                                        ((mu.flag_spec == "castlingK") || (mu.flag_spec == "castlingQ"))
                                    )
                                {
                                    uint8_t rank_y = (p_src->get_color() == PieceColor::W) ? 0 : 7;
                                    if ( (mu.flag_spec == "castlingK") && (mv.get_state_castling(p_src->get_color(), PieceName::K)) )
                                    {
                                        if (    (get_pieceid_at(4, rank_y) == _Piece::get_id(PieceName::K, p_src->get_color()) ) &&
                                                (get_pieceid_at(5, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(6, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(7, rank_y) == _Piece::get_id(PieceName::R, p_src->get_color()) )
                                            )
                                        {
                                            mv.mu.context_extra = "castlingK_R";
                                            mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                            m.push_back(mv);
                                        }                 
                                    }
                                    else if ( (mu.flag_spec == "castlingQ") && (mv.get_state_castling(p_src->get_color(), PieceName::Q)) )
                                    {
                                        if (    (get_pieceid_at(0, rank_y) == _Piece::get_id(PieceName::R, p_src->get_color()) ) &&
                                                (get_pieceid_at(1, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(2, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(3, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(4, rank_y) == _Piece::get_id(PieceName::K, p_src->get_color()) )
                                            )
                                        {
                                            mv.mu.context_extra = "castlingQ_R";
                                            mv.set_state_castling(p_src->get_color(), PieceName::Q, false);
                                            m.push_back(mv);
                                        }
                                    }
                                }
                                else if (p_dst->name == PieceName::none)
                                {
                                    if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
                                    {
                                        if (p_src->get_name() == PieceName::K)
                                            mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                        m.push_back(mv);
                                    }
                                    else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
                                    {
                                        if (mu.flag == MoveUnit::FLAG::conditional)
                                        {
                                            if ( ((mu.flag_spec == "y1") && (j == 1)) ||
                                                 ((mu.flag_spec == "y6") && (j == 6)) )
                                            {
                                                uint8_t prev_x = (mu.x > 1) ? 1 : mu.x;
                                                uint8_t prev_y = (mu.y > 1) ? 1 : mu.y;
                                                uint8_t prev_mv_dst_x = (uint8_t)(i + prev_x * 1);
                                                uint8_t prev_mv_dst_y = (uint8_t)(j + prev_y * 1);
                                                if (get_pieceid_at(prev_mv_dst_x, prev_mv_dst_y) == _Piece::empty_id()) // empty path
                                                {
                                                    mv.set_state_ep(get_color(), i, false);
                                                    m.push_back(mv);
                                                }
                                            }
                                            // en passant
                                            else if ( ((mu.flag_spec == "y5_ep") && (j == 5) && (get_color() == PieceColor::W) && (_history_moves.size() > 0)) ||
                                                      ((mu.flag_spec == "y2_ep") && (j == 2) && (get_color() == PieceColor::B) && (_history_moves.size() > 0)) )
                                            {
                                                if (mv.get_state_ep(get_color(), i))
                                                {
                                                    _Move mh = _history_moves.back();
                                                    if (mh.prev_src_id == _Piece::get_id(PieceName::P, get_opposite_color()))
                                                    {
                                                        if ((mh.src_x == i + mu.x) && (mh.src_y == j + mu.y) && (abs(mh.dst_y - mh.src_y) == 2))
                                                        {
                                                            mv.set_state_ep(get_color(), i, false);
                                                            m.push_back(mv);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (std::abs(mu.x) != std::abs(mu.y)) // not diago
                                            {
                                                // promo
                                                if (((p_src->color == PieceColor::W) && (j == 6)) || ((p_src->color == PieceColor::B) && (j == 1)))
                                                {
                                                    _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                    _Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
                                                    _Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
                                                    _Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
                                                }
                                                else
                                                {
                                                    m.push_back(mv);
                                                }
                                            }
                                            else
                                            {
                                            }
                                        }
                                    }
                                }
                                // capture dst
                                else if (p_dst->color != p_src->color)
                                {
                                    if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
                                    {
                                        if (p_src->get_name() == PieceName::K)
                                            mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                        m.push_back(mv);
                                    }
                                    else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
                                    {
                                        if (mu.flag == MoveUnit::FLAG::conditional)
                                        {
                                        }
                                        else
                                        {
                                            if (std::abs(mu.x) != std::abs(mu.y)) // not diago
                                            {
                                            }
                                            else
                                            {
                                                // promo
                                                if (((p_src->color == PieceColor::W) && (j == 6)) || ((p_src->color == PieceColor::B) && (j == 1)))
                                                {
                                                    _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                    _Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
                                                    _Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
                                                    _Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
                                                }
                                                else
                                                {
                                                    m.push_back(mv);
                                                }
                                            }
                                        }
                                    }
                                    prev_path_empty = false;
                                }
                                else if (p_dst->color == p_src->color) // same color dst
                                {
                                    prev_path_empty = false;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (is_recursive_call)
        {
            // ok no more recursive
        }
        else if (!_allow_self_check)  // Splitting computation into multi thread
        {
            unsigned concurrenty = std::thread::hardware_concurrency();
            if (concurrenty < 2) concurrenty = 2;

            _Board*                          bb  = new _Board[concurrenty];
            std::vector<_Move>*              bm  = new std::vector<_Move>[concurrenty];
            std::future<std::vector<_Move>>* fut = new std::future<std::vector<_Move>>[concurrenty];

            size_t n = (size_t)m.size() / concurrenty;
            if (n < 1) n = 1;
            for (size_t i = 0; i < concurrenty; i++)
            {
                bb[i] = *this; // Get a copy of board per thread
                if (i<concurrenty-1)
                    fut[i] = std::async(generate_moves_no_self_check<PieceID, _BoardSize>, bb[i], m, i*n, (i+1)*n-1);
                else
                    fut[i] = std::async(generate_moves_no_self_check<PieceID, _BoardSize>, bb[i], m, i*n, m.size()-1);
            }

            for (size_t i = 0; i < concurrenty; i++)
                bm[i]=fut[i].get();

            // reuse bm[0]
            bm[0].reserve(m.size()+1);
            for (size_t i = 1; i < concurrenty; i++)
                bm[0].insert(bm[0].end(), bm[i].begin(), bm[i].end());

            std::vector<_Move> mm = bm[0];
            delete[]bb;
            delete[]fut;
            delete[]bm;
            return mm;
        }

        return m;
    }

    // generate_moves_no_self_check()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<Move<PieceID>> generate_moves_no_self_check(Board<PieceID, _BoardSize>& bb, std::vector<Move<PieceID>>& m, size_t from, size_t to)
    {
        std::vector<Move<PieceID>> mm;
        if (to < from) return mm;

        size_t ret_idx;
        for (size_t i=from;i<=to;i++)
        {
            bb.apply_move(m[i]);
            {
                std::vector<Move<PieceID>> m_oppon = bb.generate_moves(true);
                if (!bb.can_capture_opposite_king(m_oppon, ret_idx))
                {
                    mm.push_back(m[i]);
                }
            }
            bb.undo_move();
        }
        return mm;
    }

};
#endif
