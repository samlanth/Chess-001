#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH_2   :  handle 2 pieces TB (1v1)
//                  0vX or Xv0 is considered as 1 piece (X: any number of pieces)
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_2_HPP
#define _AL_CHESS_TABLEBASE_TBH_2_HPP

namespace chess
{
    // TBH_2
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_2 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBH_2(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 2, option)
        {
            Tablebase<PieceID, _BoardSize, 2>* tb_w = TB_Manager<PieceID, _BoardSize>::instance()->find_2(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 2>* tb_b = TB_Manager<PieceID, _BoardSize>::instance()->find_2(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if (t == TB_TYPE::tb_1v1)
            {
                Tablebase<PieceID, _BoardSize, 2>* _tb_W;
                Tablebase<PieceID, _BoardSize, 2>* _tb_B;

                if (tb_w == nullptr)
                {
                    _tb_W = new TB_2<PieceID, _BoardSize>(v, PieceColor::W);
                    TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else 
                { 
                    _tb_W = tb_w; 
                }
   

                if (tb_b == nullptr)
                {
                    _tb_B = new TB_2<PieceID, _BoardSize>(v, PieceColor::B);
                    TB_Manager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else 
                { 
                    _tb_B = tb_b; 
                }

                // TB
                set_TB_W(_tb_W);
                set_TB_B(_tb_B);

                // Children TBH
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
        }

        virtual ~TBH_2() {}
        bool build(char verbose = 0) override;

    protected:       
        bool     build_base(char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play);
        uint64_t set_marker(PieceColor color_to_play);
        uint64_t process_marker(PieceColor color_to_play);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_2<PieceID, _BoardSize>::build(char verbose)
    {
        // Make sure children are build first
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }
        return build_base(verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_2<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W) 
            return set_mate_score_vv(color_to_play, _TB_W);
        else 
            return set_mate_score_vv(color_to_play, _TB_B);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_2<PieceID, _BoardSize>::set_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W) 
            setup_marker_vv<PieceID, _BoardSize, 2>(this, color_to_play, _TB_W, _TB_B);
        else 
            return setup_marker_vv<PieceID, _BoardSize, 2>(this, color_to_play, _TB_B, _TB_W);
    }
    
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBH_2<PieceID, _BoardSize>::process_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W) 
            process_marker_vv<PieceID, _BoardSize, 2>(this, color_to_play, _TB_W, _TB_B);
        else 
            return process_marker_vv<PieceID, _BoardSize, 2>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH_2<PieceID, _BoardSize>::build_base(char verbose)
    {
        return build_base_vv<PieceID, _BoardSize, 2>(this, _TB_W, _TB_B, verbose);
    }

};
#endif

