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
    // PieceSet
    template <typename PieceID, typename uint8_t _BoardSize>
    class PieceSet
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

    public:
        // EX: KQPPvKRPP - w_set is KQP(2), b_set is KRP(2)
        PieceSet(const std::vector<std::pair<PieceID, uint8_t>>& w_set, const std::vector<std::pair<PieceID, uint8_t>>& b_set);
        PieceSet(std::vector<const _Piece*>& v);
        ~PieceSet() {}

        bool is_valid()  const { return _is_valid; }
        std::vector<std::pair<PieceID, uint8_t>> wset() const { return _wset; }
        std::vector<std::pair<PieceID, uint8_t>> bset() const { return _bset; }
        std::vector<std::pair<PieceID, uint8_t>>& wset_ref()  { return _wset; }
        std::vector<std::pair<PieceID, uint8_t>>& bset_ref()  { return _bset; }

        std::string name(PieceColor color_toplay) const;
        uint16_t find_rank_index(size_t rank, PieceID& ret_id, uint16_t& ret_count, uint16_t& ret_instance)  const;
        uint16_t count_one_piece(PieceColor c, PieceID id)  const;
        uint16_t count_all_piece(PieceColor c) const;

        size_t children_size(PieceColor c) const;
        std::string name_child(PieceColor color_toplay, PieceColor color_child, size_t children_index) const;
        std::vector<std::pair<PieceID, uint8_t>> children(PieceColor c, size_t children_index) const;

        // After a capture/promo, return the child index matching the new position
        bool find_child_index(PieceColor c, const _Board& pos, bool isPromo, size_t& ret_child_index) const;

        // All combination of 1 piece less either white or black + promo
        std::vector<PieceSet> get_all_child() const;

        std::vector<std::pair<PieceID, uint8_t>> merge() const;
        static std::vector<PieceID> to_pieces(const std::vector<std::pair<PieceID, uint8_t>>& v);
        static std::vector<PieceID> ps_to_pieces(const PieceSet<PieceID, _BoardSize>& v);
        static std::vector<std::pair<PieceID, uint8_t>> to_set(const std::vector<PieceID>& v);
        static std::vector<std::pair<PieceID, uint8_t>> to_set_p(std::vector<const _Piece*>& v, PieceColor c);

    protected:
        bool _is_valid;
        std::vector<std::pair<PieceID, uint8_t>>                _wset;          // white pieces and count
        std::vector<std::pair<PieceID, uint8_t>>                _bset;          // black pieces and count
        std::vector<std::vector<std::pair<PieceID, uint8_t>>>   _wchildren;     // all white combination with 1 piece less
        std::vector<std::vector<std::pair<PieceID, uint8_t>>>   _bchildren;     // all black combination with 1 piece less

        bool validate();
        void make_children();
        static void add_to_v(PieceID id, std::vector<std::pair<PieceID, uint8_t>>& v);
    };

    // ct
    template <typename PieceID, typename uint8_t _BoardSize>
    PieceSet<PieceID, _BoardSize>::PieceSet(const  std::vector<std::pair<PieceID, uint8_t>>& w_set, const  std::vector<std::pair<PieceID, uint8_t>>& b_set)
            : _is_valid(true), _wset(w_set), _bset(b_set)
    {
        if (validate())
            make_children();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    PieceSet<PieceID, _BoardSize>::PieceSet(std::vector<const _Piece*>& v)
        : _is_valid(true), 
        _wset(PieceSet<PieceID, _BoardSize>::to_set_p(v, PieceColor::W)),
        _bset(PieceSet<PieceID, _BoardSize>::to_set_p(v, PieceColor::B))
    {
        if (validate())
            make_children();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline size_t PieceSet<PieceID, _BoardSize>::children_size(PieceColor c)  const
    {
        if (!_is_valid) return 0;

        if (c == PieceColor::W) return _wchildren.size();
        return _bchildren.size();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string  PieceSet<PieceID, _BoardSize>::name_child(PieceColor color_toplay, PieceColor color_child, size_t children_index)  const
    {
        assert(children_index < children_size(color_child));
        PieceSet child_set( (color_child == PieceColor::W) ? children(PieceColor::W, children_index) : _wset,
                            (color_child == PieceColor::W) ? _bset : children(PieceColor::B, children_index));
        return child_set.name(color_toplay);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string  PieceSet<PieceID, _BoardSize>::name(PieceColor color_toplay)  const
    {
        std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(*this);
        return TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(v, color_toplay);
    }

    // find_rank_index
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t PieceSet<PieceID, _BoardSize>::find_rank_index(size_t rank, PieceID& ret_pieceid, uint16_t& ret_count, uint16_t& ret_instance)  const
    {
        uint16_t n = 0; ret_count = 0;

        for (uint16_t i = 0; i < _wset.size(); i++)
        {
            for (uint16_t j = 0; j < _wset[i].second; j++)
            {
                if (n == rank)
                {
                    ret_count   = _wset[i].second;
                    ret_pieceid = _wset[i].first;
                    ret_instance = j;
                    return i;
                }
                n++;
            }
        }

        for (uint16_t i = 0; i < _bset.size(); i++)
        {
            for (uint16_t j = 0; j < _bset[i].second; j++)
            {
                if (n == rank)
                {
                    ret_count   = _bset[i].second;
                    ret_pieceid = _bset[i].first;
                    ret_instance = j;
                    return i;
                }
                n++;
            }
        }

        return -1; // ret_count is 0
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t PieceSet<PieceID, _BoardSize>::count_one_piece(PieceColor c, PieceID id)  const
    {
        uint16_t n = 0;
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
                if (_wset[i].first == id)
                    n += _wset[i].second;
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
                if (_bset[i].first == id)
                    n += _bset[i].second;
        }
        return n;
    }

    template <typename PieceID, typename uint8_t _BoardSize> 
    inline uint16_t PieceSet<PieceID, _BoardSize>::count_all_piece(PieceColor c)  const
    {
        uint16_t n = 0;
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
                n += _wset[i].second;
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
                n += _bset[i].second;
        }
        return n;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::children(PieceColor c, size_t children_index)  const
    {
        if (c == PieceColor::W)
        {
            if (children_index < _wchildren.size())
                return _wchildren[children_index];
        }
        else if (c == PieceColor::B)
        {
            if (children_index < _bchildren.size())
                return _bchildren[children_index];
        }
        std::vector<std::pair<PieceID, uint8_t>> v;
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool PieceSet<PieceID, _BoardSize>::validate()
    {
        // todo check sorting...
        return _is_valid;
    }

    // make_children
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void PieceSet<PieceID, _BoardSize>::make_children()
    {
        bool DO_PROMO = true;
        std::vector<std::pair<PieceID, uint8_t>> workset;
        std::pair<PieceID, uint8_t>              pair_piece_count;
        std::pair<PieceID, uint8_t>              pr;

        bool w_promo = false;
        bool b_promo = false;
        size_t w_promo_idx;
        size_t b_promo_idx;

        _wchildren.clear();
        for (size_t i = 0; i < _wset.size(); i++)
        {
            workset = _wset;
            pair_piece_count = workset[i];
            if (pair_piece_count.second > 0)
            {
                // Capture remove 1 ith piece
                pair_piece_count.second--;
                workset[i] = pair_piece_count;
                _wchildren.push_back(workset);

                if (DO_PROMO)
                {
                    // Promo 
                    pr = workset.at(i);
                    if ((!w_promo) && (pr.first == _Piece::get_id(PieceName::P, PieceColor::W)))
                    {
                        // only Q for now...
                        pair_piece_count.first = _Piece::get_id(PieceName::Q, PieceColor::W);
                        pair_piece_count.second = 1;

                        workset = _wset;
                        workset[i].second--;
                        workset.push_back(pair_piece_count);
                        _wchildren.push_back(workset);
                        w_promo = true;
                        w_promo_idx = _wchildren.size() - 1;
                    }

                    // merge the Q entries in the vector
                    if (w_promo)
                    {
                        uint16_t n = 0;
                        for (size_t i = 0; i < _wchildren[w_promo_idx].size(); i++)
                        {
                            if (_wchildren[w_promo_idx][i].first == _Piece::get_id(PieceName::Q, PieceColor::W))
                                n++;
                        }

                        if (n > 1)
                        {
                            size_t i1 = -1; size_t i2 = -1;
                            for (size_t i = 0; i < _wchildren[w_promo_idx].size(); i++)
                            {
                                if (_wchildren[w_promo_idx][i].first == _Piece::get_id(PieceName::Q, PieceColor::W))
                                {
                                    if (i1 == -1) i1 = i;
                                    else i2 = i;
                                }
                            }
                            // merge
                            if (i2 != -1)
                            {
                                while (_wchildren[w_promo_idx][i2].second > 0)
                                {
                                    _wchildren[w_promo_idx][i1].second++;
                                    _wchildren[w_promo_idx][i2].second--;
                                }
                            }
                        }

                        // remove empty Q piece and P possibly
                        for (size_t i = 0; i < _wchildren[w_promo_idx].size(); i++)
                        {
                            if (_wchildren[w_promo_idx][i].second == 0)
                            {
                                auto& v = _wchildren[w_promo_idx].begin() + i;
                                _wchildren[w_promo_idx].erase(v);
                                i = 0;
                            }
                        }
                    }
                }
            }
        }

        _bchildren.clear();
        for (size_t i = 0; i < _bset.size(); i++)
        {
            workset = _bset;
            pair_piece_count = workset[i];
            if (pair_piece_count.second > 0)
            {
                // Capture remove 1 ith piece
                pair_piece_count.second--;
                workset[i] = pair_piece_count;
                _bchildren.push_back(workset);
            }

            if (DO_PROMO)
            {
                // Promo 
                pr = workset.at(i);
                if ((!b_promo) && (pr.first == _Piece::get_id(PieceName::P, PieceColor::B)))
                {
                    // only Q for now...
                    pair_piece_count.first = _Piece::get_id(PieceName::Q, PieceColor::B);
                    pair_piece_count.second = 1;

                    workset = _wset;
                    workset[i].second--;
                    workset.push_back(pair_piece_count);
                    _bchildren.push_back(workset);
                    b_promo = true;
                    b_promo_idx = _bchildren.size() - 1;
                }

                if (b_promo)
                {
                    uint16_t n = 0;
                    for (size_t i = 0; i < _bchildren[b_promo_idx].size(); i++)
                    {
                        if (_bchildren[b_promo_idx][i].first == _Piece::get_id(PieceName::Q, PieceColor::B))
                            n++;
                    }
                    if (n > 1)
                    {
                        size_t i1 = -1; size_t i2 = -1;
                        for (size_t i = 0; i < _bchildren[b_promo_idx].size(); i++)
                        {
                            if (_bchildren[b_promo_idx][i].first == _Piece::get_id(PieceName::Q, PieceColor::B))
                            {
                                if (i1 == -1) i1 = i;
                                else i2 = i;
                            }
                        }
                        // merge
                        if (i2 != -1)
                        {
                            while (_bchildren[b_promo_idx][i2].second > 0)
                            {
                                _bchildren[b_promo_idx][i1].second++;
                                _bchildren[b_promo_idx][i2].second--;
                            }
                        }
                    }
                    // remove empty Q piece and P possibly
                    for (size_t i = 0; i < _bchildren[b_promo_idx].size(); i++)
                    {
                        if (_bchildren[b_promo_idx][i].second == 0)
                        {
                            auto& v = _bchildren[b_promo_idx].begin() + i;
                            _bchildren[b_promo_idx].erase(v);
                            i = 0;
                        }
                    }
                }
            }
        }
    }

    // find_child_index
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool PieceSet<PieceID, _BoardSize>::find_child_index(PieceColor c, const _Board& pos, bool isPromo, size_t& ret_child_index) const
    {
        uint16_t cnt_piece_c;
        uint16_t cnt_piece_c_oppo;
        uint16_t cnt_piece_board_c;
        uint16_t cnt_piece_board_c_oppo;
        PieceColor c_oppo = (c == PieceColor::W) ? PieceColor::B : PieceColor::W;

        cnt_piece_board_c = pos.cnt_all_piece(c);
        cnt_piece_board_c_oppo = pos.cnt_all_piece(c_oppo);

        bool child_ok;
        PieceColor color_change = c;
        // capture - removing piece from c
        // promo   - changing piece from c_oppo 
        if (isPromo) color_change = c_oppo;

        for (size_t j = 0; j < children_size(color_change); j++)
        {
            child_ok = true;
            PieceSet child_set( (color_change == PieceColor::W) ? children(PieceColor::W, j) : _wset,
                                (color_change == PieceColor::W) ? _bset : children(PieceColor::B, j));

            cnt_piece_c = child_set.count_all_piece(c);
            cnt_piece_c_oppo = child_set.count_all_piece(c_oppo);
            if (cnt_piece_c != cnt_piece_board_c)           { child_ok = false; continue; }
            if (cnt_piece_c_oppo != cnt_piece_board_c_oppo) { child_ok = false; continue; }

            for (size_t i = 0; i < child_set.wset_ref().size(); i++)
            {
                if (pos.cnt_piece(child_set.wset_ref()[i].first) != child_set.wset_ref()[i].second) { child_ok = false; break; }
            }
            if (child_ok == false) continue;

            for (size_t i = 0; i < child_set.bset_ref().size(); i++)
            {
                if (pos.cnt_piece(child_set.bset_ref()[i].first) != child_set.bset_ref()[i].second) { child_ok = false; break; }
            }
            if (child_ok == false) continue;

            if (child_ok)
            {
                ret_child_index = j;
                return true;
            }
        }
        return false;
    }

    // All combination of 1 piece capture/promo, either white or black
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<PieceSet<PieceID, _BoardSize>>  PieceSet<PieceID, _BoardSize>::get_all_child() const
    {
        std::vector<PieceSet<PieceID, _BoardSize>> v;
        for (size_t i = 0; i < _wchildren.size(); i++)
        {
            v.push_back(PieceSet<PieceID, _BoardSize>({ children(PieceColor::W, i), _bset }));
        }
        for (size_t i = 0; i < _bchildren.size(); i++)
        {
            v.push_back(PieceSet<PieceID, _BoardSize>({ _wset, children(PieceColor::B, i) }));
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::merge() const
    {
        std::vector<std::pair<PieceID, uint8_t>> v = _wset;
        for (size_t i = 0; i < _bset.size(); i++)
        {
            v.push_back(_bset[i]);
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<PieceID> PieceSet<PieceID, _BoardSize>::to_pieces(const std::vector<std::pair<PieceID, uint8_t>>& v)
    {
        std::vector<PieceID> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            for (size_t j = 0; j < v[i].second; j++)
                r.push_back(v[i].first);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<PieceID> PieceSet<PieceID, _BoardSize>::ps_to_pieces(const PieceSet<PieceID, _BoardSize>& v)
    {
        std::vector<PieceID> r;
        for (size_t i = 0; i < v._wset.size(); i++)
        {
            for (size_t j = 0; j < v._wset[i].second; j++)
                r.push_back(v._wset[i].first);
        }
        for (size_t i = 0; i < v._bset.size(); i++)
        {
            for (size_t j = 0; j < v._bset[i].second; j++)
                r.push_back(v._bset[i].first);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void PieceSet<PieceID, _BoardSize>::add_to_v(PieceID id, std::vector<std::pair<PieceID, uint8_t>>& v)
    {
        uint8_t n = 0;
        bool found = false;
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i].first == id)
            { 
                n++;
                found = true;
            }
        }
        if (found == false) v.push_back(std::pair<PieceID, uint8_t>({ id, 1 }));
        else
        {
            for (size_t i = 0; i < v.size(); i++)
            {
                if (v[i].first == id)
                {
                    v[i].second = n+1;
                    break;
                }
            }
        }
    }
    
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::to_set(const std::vector<PieceID>& v)
    {
        std::vector<std::pair<PieceID, uint8_t>> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            add_to_v(v[i], r);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::to_set_p(std::vector<const _Piece*>& v, PieceColor c)
    {
        std::vector<std::pair<PieceID, uint8_t>> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i]->get_color() == c)
                add_to_v(v[i]->get_id(), r);
        }
        return r;
    }

};
#endif

