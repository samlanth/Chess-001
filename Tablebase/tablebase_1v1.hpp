#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_1v1
// TablebaseHandler_1v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_1v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_1v1_HPP


namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v1 : public Tablebase<PieceID, _BoardSize, 2>
    {        
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;

    public:
        Tablebase_1v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c)
        {
        }

        bool load() override
        {
            _is_build = read_tb();
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build;
        }

        bool save() const override  { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_1v1<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 2) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_1v1 : public TablebaseBaseHandler_2<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;       

    public:
        TablebaseHandler_1v1(std::vector<PieceID>& v) : TablebaseBaseHandler_2<PieceID, _BoardSize>(v)
        {
            _tb_W = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::B);

            _p_1v0.push_back(v[0]);
            _p_0v1.push_back(v[1]);

            _tb_1v0_w = new Tablebase_1v0<PieceID, _BoardSize>(_p_1v0, PieceColor::W);
            _tb_1v0_b = new Tablebase_1v0<PieceID, _BoardSize>(_p_1v0, PieceColor::B);
            _tb_0v1_w = new Tablebase_0v1<PieceID, _BoardSize>(_p_0v1, PieceColor::W);
            _tb_0v1_b = new Tablebase_0v1<PieceID, _BoardSize>(_p_0v1, PieceColor::B);
        }
        ~TablebaseHandler_1v1()
        {
            delete _tb_W;
            delete _tb_B;

            delete _tb_1v0_w;
            delete _tb_1v0_b;
            delete _tb_0v1_w;
            delete _tb_0v1_b;
        }

        bool build(char verbose = 0) override;
        bool is_build() const override  { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_1v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_1v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:
        Tablebase_1v0<PieceID, _BoardSize>* locate_children_1v0(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0) const;
        Tablebase_0v1<PieceID, _BoardSize>* locate_children_0v1(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0) const;
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const;
        void print() const;

        Tablebase_1v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_1v1<PieceID, _BoardSize>* _tb_B;

        std::vector<PieceID> _p_1v0;
        std::vector<PieceID> _p_0v1;

        // children TB
        Tablebase_1v0<PieceID, _BoardSize>* _tb_1v0_w;
        Tablebase_1v0<PieceID, _BoardSize>* _tb_1v0_b;
        Tablebase_0v1<PieceID, _BoardSize>* _tb_0v1_w;
        Tablebase_0v1<PieceID, _BoardSize>* _tb_0v1_b;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        if (!_tb_1v0_w->save()) return false;
        if (!_tb_1v0_b->save()) return false;
        if (!_tb_0v1_w->save()) return false;
        if (!_tb_0v1_b->save()) return false;
        return true;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::load()
    {
        if (!_tb_1v0_w->load()) return false;
        if (!_tb_1v0_b->load()) return false;
        if (!_tb_0v1_w->load()) return false;
        if (!_tb_0v1_b->load()) return false;
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::build(char verbose)
    {
        _tb_1v0_w->build(verbose);
        _tb_1v0_b->build(verbose);
        _tb_0v1_w->build(verbose);
        _tb_0v1_b->build(verbose);

        assert(_tb_1v0_w->is_build() == true);
        assert(_tb_1v0_b->is_build() == true);
        assert(_tb_0v1_w->is_build() == true);
        assert(_tb_0v1_b->is_build() == true);

        return build_base(_tb_W, _tb_B, verbose);
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        uint16_t ret_child_sq0;
        Tablebase_1v0<PieceID, _BoardSize>* tb_1v0 = locate_children_1v0(pos, color, ret_child_sq0);
        if (tb_1v0 != nullptr)
        {
            ret_sc = tb_1v0->score(ret_child_sq0);
            return true;
        }
        Tablebase_0v1<PieceID, _BoardSize>* tb_0v1 = locate_children_0v1(pos, color, ret_child_sq0);
        if (tb_0v1 != nullptr)
        {
            ret_sc = tb_0v1->score(ret_child_sq0);
            return true;
        }
        return false;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v0<PieceID, _BoardSize>* TablebaseHandler_1v1<PieceID, _BoardSize>::locate_children_1v0(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0) const
    {
        if ((color == PieceColor::W) && (_tb_1v0_w->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return _tb_1v0_w;
        }
        if ((color == PieceColor::B) && (_tb_1v0_b->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return _tb_1v0_b;
        }
        return nullptr;
    }
    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_0v1<PieceID, _BoardSize>* TablebaseHandler_1v1<PieceID, _BoardSize>::locate_children_0v1(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0) const
    {
        if ((color == PieceColor::W) && (_tb_0v1_w->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_0v1[0])->get_name(), PieceColor::B);
            return _tb_0v1_w;
        }
        if ((color == PieceColor::B) && (_tb_0v1_b->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_0v1[0])->get_name(), PieceColor::B);
            return _tb_0v1_b;
        }
        return nullptr;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_1v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

