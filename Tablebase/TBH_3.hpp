#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_3   : handle 3 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_3_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_3_HPP

namespace chess
{
    // TBHandler_3
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_3 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandler_3(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 3, option)
        {
            Tablebase<PieceID, _BoardSize, 3>* _tb_W;
            Tablebase<PieceID, _BoardSize, 3>* _tb_B;
            Tablebase<PieceID, _BoardSize, 3>* tb_w = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 3>* tb_b = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            // TB bases 2v1 (only 2v1 for 3 pieces, 1v2 would be a symmetry TB) 
            if (tb_w == nullptr)
            {
                _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
            } 
            else 
            { 
                _tb_W = tb_w; 
            }

            if (tb_b == nullptr)
            {
                _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
            } 
            else 
            { 
                _tb_B = tb_b; 
            }

            // TB
            set_TB_W(_tb_W);
            set_TB_B(_tb_B);


            // Children handlers
            _tb_children_info = TablebaseManager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps, option);
            for (size_t i = 0; i < _tb_children_info.size(); i++)
            {
                if (_tb_children_info[i]._tbh != nullptr)
                {
                    // Building handlers
                    // TB was not found then a TBH was immediatley created by manager, so we can build it
                    _tbh_children.push_back(_tb_children_info[i]._tbh);
                    _tb_children_info[i]._owner = true;
                    _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                }
                else
                {
                    // Visitor handlers
                    // TB was found and a TBH was not created by manager, we create a handler so we can traverse hiearchy as needed
                    if (_tb_children_info[i]._t == TB_TYPE::tb_2v1) // promo
                    {
                        _tbh_children.push_back(new TBHandler_2v1<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    if (_tb_children_info[i]._t == TB_TYPE::tb_1v1)
                    {
                        _tbh_children.push_back(new TBHandler_1v1<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    else if (_tb_children_info[i]._t == TB_TYPE::tb_Xv0)
                    {
                        _tbh_children.push_back(new TBHandler_Xv0<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    else if (_tb_children_info[i]._t == TB_TYPE::tb_0vX)
                    {
                        _tbh_children.push_back(new TBHandler_0vX<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                }
            }
        }     

        virtual ~TBHandler_3() {}
        bool build(char verbose = 0) override;

    protected:
        bool     build_base(char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play);
        uint64_t set_marker(PieceColor color_to_play);
        uint64_t process_marker(PieceColor color_to_play);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build(char verbose)
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }
        return build_base(verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            return set_mate_score_vv(color_to_play, _TB_W);
        else
            return set_mate_score_vv(color_to_play, _TB_B);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            setup_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_W, _TB_B);
        else
            return setup_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::process_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            process_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_W, _TB_B);
        else
            return process_marker_vv<PieceID, _BoardSize, 3>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build_base(char verbose)
    {
        return build_base_vv<PieceID, _BoardSize, 3>(this, _TB_W, _TB_B, verbose);
    }
};
#endif

