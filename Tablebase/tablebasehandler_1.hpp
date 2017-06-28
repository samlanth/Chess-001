#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_1   : handle 1 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_1_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_1_HPP

namespace chess
{
    // TBHandler_1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_1 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandler_1(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 1, option)
        {
            Tablebase<PieceID, _BoardSize, 1>* tb_w = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 1>* tb_b = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if (t == TB_TYPE::tb_0vX)
            {
                if (tb_w == nullptr)
                {
                    _tb_W = new Tablebase_0vX<PieceID, _BoardSize>(v, PieceColor::W);       // W to play
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else { _tb_W = tb_w; }

                if (tb_b == nullptr)
                {
                    _tb_B = new Tablebase_0vX<PieceID, _BoardSize>(v, PieceColor::B);       // B to play
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else { _tb_B = tb_b; }
            }
            else if (t == TB_TYPE::tb_Xv0)
            {
                if (tb_w == nullptr)
                {
                    _tb_W = new Tablebase_Xv0<PieceID, _BoardSize>(v, PieceColor::W);
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else { _tb_W = tb_w; }

                if (tb_b == nullptr)
                {
                    _tb_B = new Tablebase_Xv0<PieceID, _BoardSize>(v, PieceColor::B);
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else { _tb_B = tb_b; }
            }
            else
            {
                assert(false);
            }
        }

        virtual ~TBHandler_1() {}

        Tablebase<PieceID, _BoardSize, 1>* tb_W()   const { return _tb_W; }
        Tablebase<PieceID, _BoardSize, 1>* tb_B()   const { return _tb_B; }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;
        void print() const;

    protected:;
        Tablebase<PieceID, _BoardSize, 1>* _tb_W;   // not owner
        Tablebase<PieceID, _BoardSize, 1>* _tb_B;   // not owner

        bool     build_base(Tablebase<PieceID, _BoardSize, 1>* tb, Tablebase<PieceID, _BoardSize, 1>* tb_oppo, char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb);

        bool find_score_children_tb(const _Board& pos, PieceColor color, bool isPromo, ExactScore& ret_sc) const override
        { 
            // No children tb exist
            ret_sc = ExactScore::UNKNOWN;
            return false; 
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::load()
    {
        if (is_build()) return true;
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::build(char verbose)
    {
        // No childs to pre build
        return build_base(_tb_W, _tb_B, verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBHandler_1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb)
    {
        return set_mate_score_v<PieceID, _BoardSize, 1>(color_to_play, tb);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 1>* _tb_W, Tablebase<PieceID, _BoardSize, 1>* _tb_B, char verbose)
    {
        return build_base_v<PieceID, _BoardSize, 1>(this, &_tb_W, &_tb_B, verbose);
    }

};
#endif

