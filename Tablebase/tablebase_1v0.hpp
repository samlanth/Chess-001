#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_1v0
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_1v0_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_1v0_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v0 : public Tablebase<PieceID, _BoardSize, 1>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Tablebase_1v0(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c)
        {
        }

        bool load() override
        {
            _is_build = read_tb();
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build;
        }

        bool save() const override { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_1v0<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 1) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)))
        {
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_1v0 : public TablebaseBaseHandler_1<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;

    public:
        TablebaseHandler_1v0(std::vector<PieceID>& v) : TablebaseBaseHandler_1<PieceID, _BoardSize>(v)
        {
            _tb_W = new Tablebase_1v0<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_1v0<PieceID, _BoardSize>(v, PieceColor::B);
        }
        ~TablebaseHandler_1v0()
        {
            delete _tb_W;
            delete _tb_B;
        }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_1v0<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_1v0<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:
        void print() const;

        Tablebase_1v0<PieceID, _BoardSize>* _tb_W;
        Tablebase_1v0<PieceID, _BoardSize>* _tb_B;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v0<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;  
        return true;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v0<PieceID, _BoardSize>::load()
    {
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v0<PieceID, _BoardSize>::build(char verbose)
    {
        return build_base(_tb_W, _tb_B, verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_1v0<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

