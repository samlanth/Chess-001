#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PieceSet
//
//
#ifndef _AL_CHESS_TABLEBASE_PIECESET_HPP
#define _AL_CHESS_TABLEBASE_PIECESET_HPP


namespace chess
{
    // prototype..

    template <typename PieceID, typename uint8_t _BoardSize>
    class PieceSet
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    public:    
        PieceSet(const std::vector<std::pair<PieceID, uint8_t>>& w_set, const std::vector<std::pair<PieceID, uint8_t>>& b_set)
            : _is_valid(true), _wset(w_set), _bset(b_set)
        {
            if (validate())
                make_children();
        }
        bool is_valid() { return _is_valid; }

        size_t children_size(PieceColor c) 
        { 
            if (c == PieceColor::W) return _wchildren.size();
            return _bchildren.size();
        }

        std::string name(PieceColor c, size_t children_index)
        {
            std::string str_pieces;
            std::vector<std::pair<PieceID, uint8_t>> v = children(c, children_index);

            if (c == PieceColor::W) str_pieces = "W_"; else str_pieces = "B_";
            for (size_t i = 0; i < v.size(); i++)
            {
                if (v[i].second > 0)
                {
                    for (size_t j = 0; j < v[i].second; j++)
                        str_pieces += _Piece::to_str(v[i].first, false);
                }
            }
            return str_pieces;
        }

        uint16_t find_rank_index(PieceColor c, size_t rank, uint16_t& ret_count)
        {
            uint16_t n = 0; ret_count = 0;
            if (c == PieceColor::W)
            {
                for (uint16_t i = 0; i < _wset.size(); i++)
                {
                    for (uint16_t j = 0; j < _wset[i].second; j++)
                    {
                        if (n == rank) 
                        {
                            ret_count = _wset[i].second; 
                            return i;
                        }
                        n++;
                    }
                }
            }
            if (c == PieceColor::B)
            {
                for (uint16_t i = 0; i < _bset.size(); i++)
                {
                    for (uint16_t j = 0; j < _bset[i].second; j++)
                    {
                        if (n == rank)
                        {
                            ret_count = _bset[i].second;
                            return i;
                        }
                        n++;
                    }
                }
            }
            return -1; // count is 0
        }

        uint16_t count_one_piece(PieceColor c, PieceID id) 
        {
            uint16_t n = 0;
            if (c == PieceColor::W)
            {
                for (size_t i = 0; i < _wset.size(); i++)
                    if (_wset[i].first == id)
                        n += _wset[i].second;
            }
            if (c == PieceColor::B)
            {
                for (size_t i = 0; i < _bset.size(); i++)
                    if (_bset[i].first == id)
                        n += _bset[i].second;
            }
            return n;
        }

        uint16_t count_all_piece(PieceColor c) 
        { 
            uint16_t n = 0;
            if (c == PieceColor::W)
            {
                for (size_t i = 0; i < _wset.size(); i++)
                    n += _wset[i].second;
            }
            if (c == PieceColor::B)
            {
                for (size_t i = 0; i < _bset.size(); i++)
                    n += _bset[i].second;
            }
            return n;
        }

        std::vector<std::pair<PieceID, uint8_t>> children(PieceColor c, size_t children_index) 
        { 
            if (c == PieceColor::W)
            {
                if (children_index < _wchildren.size())
                    return _wchildren[children_index];
            }
            if (c == PieceColor::B)
            {
                if (children_index < _bchildren.size())
                    return _bchildren[children_index];
            }
            std::vector<std::pair<PieceID, uint8_t>> v;  
            return v; 
        }

        std::vector<std::pair<PieceID, uint8_t>>& wset() const { return _wset;}
        std::vector<std::pair<PieceID, uint8_t>>& bset() const { return _bset; }

    protected:
        bool _is_valid;
        std::vector<std::pair<PieceID, uint8_t>> _wset; // white pieces and count
        std::vector<std::pair<PieceID, uint8_t>> _bset;
        std::vector<std::vector<std::pair<PieceID, uint8_t>>> _wchildren;    // all combination with 1 piece less
        std::vector<std::vector<std::pair<PieceID, uint8_t>>> _bchildren;

        bool validate() { return _is_valid; } // do sorting...
        void make_children() 
        {
            std::vector<std::pair<PieceID, uint8_t>> workset;
            std::pair<PieceID, uint8_t>              piece_count;

            _wchildren.clear();
            for (size_t i = 0; i < _wset.size(); i++)
            {
                // remove 1 ith piece
                workset = _wset;
                piece_count = workset[i];
                if (piece_count.second > 0)
                {
                    piece_count.second--;
                    workset[i] = piece_count;
                    _wchildren.push_back(workset);
                }
            }

            _bchildren.clear();
            for (size_t i = 0; i < _bset.size(); i++)
            {
                // remove 1 ith piece
                workset = _bset;
                piece_count = workset[i];
                if (piece_count.second > 0)
                {
                    piece_count.second--;
                    workset[i] = piece_count;
                    _bchildren.push_back(workset);
                }
            }
        }
    };

};
#endif

