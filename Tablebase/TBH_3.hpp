#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH_3   : handle 3 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_3_HPP
#define _AL_CHESS_TABLEBASE_TBH_3_HPP

namespace chess
{
    // TBH_3
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_3 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBH_3(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 3, option)
        {
            Tablebase<PieceID, _BoardSize, 3>* _tb_W;
            Tablebase<PieceID, _BoardSize, 3>* _tb_B;
            Tablebase<PieceID, _BoardSize, 3>* tb_w = TB_Manager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 3>* tb_b = TB_Manager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            // TB bases 2v1 (only 2v1 for 3 pieces, 1v2 would be a symmetry TB) 
            if (tb_w == nullptr)
            {
                // Not in memory
                _tb_W = new TB_3<PieceID, _BoardSize>(v, PieceColor::W);
                TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
            } 
            else 
            { 
                _tb_W = tb_w; 
            }

            if (tb_b == nullptr)
            {
                // Not in memory
                _tb_B = new TB_3<PieceID, _BoardSize>(v, PieceColor::B);
                TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
            } 
            else 
            { 
                _tb_B = tb_b; 
            }

            // TB
            set_TB_W(_tb_W);
            set_TB_B(_tb_B);

            // Children handlers
            _tb_children_info = TB_Manager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps, option);
            for (size_t i = 0; i < _tb_children_info.size(); i++)
            {
                if (_tb_children_info[i]._tbh != nullptr)
                {
                    _tbh_children.push_back(_tb_children_info[i]._tbh);
                    _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                }
                else
                    assert(false);
            }
        }     

        virtual ~TBH_3() {}
        bool build(char verbose = 0) override;

    protected:
        bool     build_base(char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play);
        uint64_t set_marker(PieceColor color_to_play);
        uint64_t process_marker(PieceColor color_to_play);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_3<PieceID, _BoardSize>::build(char verbose)
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }
        return build_base(verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_3<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            return set_mate_score_vv(color_to_play, _TB_W);
        else
            return set_mate_score_vv(color_to_play, _TB_B);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_3<PieceID, _BoardSize>::set_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            setup_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_W, _TB_B);
        else
            return setup_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_3<PieceID, _BoardSize>::process_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            process_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_W, _TB_B);
        else
            return process_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_3<PieceID, _BoardSize>::build_base(char verbose)
    {
        return build_base_vv<PieceID, _BoardSize, 3>(this, _TB_W, _TB_B, verbose);
    }
};
#endif

