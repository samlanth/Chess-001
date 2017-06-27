#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandlerCore
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_HPP

namespace chess
{
    // TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBH(TB_TYPE t, PieceSet<PieceID, _BoardSize> ps, uint8_t np, TBH_OPTION option) : _type(t), _NPIECE(np), _pieceSet({ ps.wset(), ps.bset() }), _option(option), _work_board(new _Board())
        {
            _piecesID = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);
        }
        virtual ~TBH() { delete _work_board; }
        virtual bool is_symmetry_TBH() const { return false; }

        virtual bool load() = 0;
        virtual bool save() const = 0;
        virtual bool build(char verbose) = 0;
        virtual bool is_build() const = 0;
        virtual bool find_score_children_tb(const _Board& pos, PieceColor color, bool isPromo, ExactScore& ret_sc) const = 0;

        std::string name(PieceColor color_toplay)   const;  // unique name of tB W or B to play
        TBH_OPTION  option()                        const { return _option; }
        uint8_t     get_NPIECE()                    const { return _NPIECE; }
        TB_TYPE     tb_type()                       const { return _type; }
        PieceSet<PieceID, _BoardSize>& pieceSet()   const { return _pieceSet;}

        static ExactScore minmax_dtc(
            PieceColor color_parent, const std::vector<ExactScore>& child_sc, const std::vector<uint8_t>& child_dtc, std::vector<bool>& child_is_promo,
            uint8_t& ret_dtc, size_t& ret_idx);

    protected:
        TB_TYPE                         _type;
        uint8_t                         _NPIECE;
        PieceSet<PieceID, _BoardSize>   _pieceSet;
        TBH_OPTION                      _option;        // none, try_load_on_build, force_rebuild, memory_map_on_build
        std::vector<PieceID>            _piecesID;
        _Board*                         _work_board;
    };

    // STRUCT_TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    struct STRUCT_TBH
    {
        STRUCT_TBH(PieceSet<PieceID, _BoardSize> p) : _ps(p.wset(), p.bset()) , _owner(false) {}
        ~STRUCT_TBH() {}

        TB_TYPE                         _t;
        PieceSet<PieceID, _BoardSize>   _ps;
        TBH<PieceID, _BoardSize>*       _tbh;
        uint16_t                        _nw;
        uint16_t                        _nb;
        bool                            _owner = false;
        size_t                          _child_index;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string TBH<PieceID, _BoardSize>::name(PieceColor color_toplay) const
    {
        if (color_toplay == PieceColor::W) return _pieceSet.name(PieceColor::W);
        return _pieceSet.name(PieceColor::B);
    }

    // static minmax_dtc
    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore TBH<PieceID, _BoardSize>::minmax_dtc(
        PieceColor                      color_parent,
        const std::vector<ExactScore>&  child_sc,
        const std::vector<uint8_t>&     child_dtc,
        std::vector<bool>&              child_is_promo,
        uint8_t& ret_dtc, size_t& ret_idx)
    {
        if (child_sc.size() == 0)
        {
            ret_dtc = 0;
            ret_idx = 0;
            return ExactScore::UNKNOWN;
        }

        if (color_parent == PieceColor::W)
        {
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    if (child_is_promo[i]) // 1 move promo and win
                    {
                        ret_dtc = child_dtc[i];
                        ret_idx = i;
                        return ExactScore::WIN;
                    }
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    // seek lower dtc
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::WIN)
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
                    return ExactScore::WIN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::DRAW;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    // seek longer dtc
                    uint8_t max_dtc = child_dtc[i];
                    size_t  max_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::LOSS)
                            if (child_dtc[j] > max_dtc) { max_dtc = child_dtc[j]; max_idx = j; }
                    }
                    ret_dtc = child_dtc[max_idx];
                    ret_idx = max_idx;
                    return ExactScore::LOSS;
                }
            }
        }
        else
        {
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    if (child_is_promo[i]) // 1 move promo and loss
                    {
                        ret_dtc = child_dtc[i];
                        ret_idx = i;
                        return ExactScore::LOSS;
                    }
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    // seek lower dtc
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::LOSS)
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
                    return ExactScore::LOSS;
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::DRAW;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    // seek longer dtc
                    uint8_t max_dtc = child_dtc[i];
                    size_t  max_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::WIN)
                            if (child_dtc[j] > max_dtc) { max_dtc = child_dtc[j]; max_idx = j; }
                    }
                    ret_dtc = child_dtc[max_idx];
                    ret_idx = max_idx;
                    return ExactScore::WIN;
                }
            }
        }
        ret_idx = 0;
        ret_dtc = 0;
        return ExactScore::UNKNOWN;
    }

};
#endif

