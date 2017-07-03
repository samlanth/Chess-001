#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH_1   : handle 1 piece TB (White to play + Black to play)
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_1_HPP
#define _AL_CHESS_TABLEBASE_TBH_1_HPP

namespace chess
{
    // TBH_1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_1 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBH_1(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 1, option)
        {
            Tablebase<PieceID, _BoardSize, 1>* tb_w = TB_Manager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 1>* tb_b = TB_Manager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if ((t == TB_TYPE::tb_0vX) || (t == TB_TYPE::tb_Xv0))
            {
                Tablebase<PieceID, _BoardSize, 1>* _tb_W;
                Tablebase<PieceID, _BoardSize, 1>* _tb_B;

                if (tb_w == nullptr)
                {
                    _tb_W = new TB_1<PieceID, _BoardSize>(v, PieceColor::W);       // W to play
                    TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else 
                { 
                    _tb_W = tb_w; 
                }

                if (tb_b == nullptr)
                {
                    _tb_B = new TB_1<PieceID, _BoardSize>(v, PieceColor::B);       // B to play
                    TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else 
                { 
                    _tb_B = tb_b;
                }

                // TB
                set_TB_W(_tb_W);
                set_TB_B(_tb_B);
            }
            else
            {
                assert(false);
            }

            // No childs TB
        }

        virtual ~TBH_1() {}
        bool build(char verbose = 0) override;

    protected:;
        bool     build_base(char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play);

        bool find_score_children_tb(const _Board& pos, PieceColor color_pos, bool isPromo, bool isCapture, ExactScore& ret_sc, uint8_t& ret_dtc) const override
        { 
            // No children tb exist
            ret_sc = ExactScore::UNKNOWN;
            ret_dtc = 0;
            return false; 
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_1<PieceID, _BoardSize>::build_base(char verbose)
    {
        return build_base_vv<PieceID, _BoardSize, 1>(this, _TB_W, _TB_B, verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_1<PieceID, _BoardSize>::build(char verbose)
    {
        // No childs to pre build
        return build_base(verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            return set_mate_score_vv(color_to_play, _TB_W);
        else
            return set_mate_score_vv(color_to_play, _TB_B);
    }

};
#endif

