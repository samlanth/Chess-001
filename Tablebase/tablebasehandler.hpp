#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandlerCore
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP

namespace chess
{
    // TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH
    {
    public:
        TBH(TB_TYPE t, PieceSet<PieceID, _BoardSize> ps, uint8_t np) : _type(t), _NPIECE(np), _pieceSet({ ps.wset(), ps.bset() }) { }
        virtual ~TBH() {}

        virtual bool is_symmetry_TBH() const = 0;
        virtual bool load() = 0;
        virtual bool save() const = 0;
        virtual bool build(char verbose) = 0;
        virtual bool is_build() const = 0;

        std::string name(PieceColor color_toplay)   const;

        uint8_t get_NPIECE()                        const { return _NPIECE; }
        TB_TYPE tb_type()                           const { return _type; }
        PieceSet<PieceID, _BoardSize>& pieceSet()   const { return _pieceSet;}

    protected:
        TB_TYPE                         _type;
        uint8_t                         _NPIECE;
        PieceSet<PieceID, _BoardSize>   _pieceSet;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string TBH<PieceID, _BoardSize>::name(PieceColor color_toplay) const
    {
        if (color_toplay == PieceColor::W) return _pieceSet.name(PieceColor::W);
        return _pieceSet.name(PieceColor::B);
    }

    // TBHandlerCore
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandlerCore : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandlerCore(const PieceSet<PieceID, _BoardSize>& ps, uint8_t aNPIECE, TB_TYPE t) : TBH(t, ps, aNPIECE), _work_board(new _Board())
        {
            _piecesID = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);
        }

        virtual ~TBHandlerCore()
        {
            delete _work_board;
        }

        bool is_symmetry_TBH()  const override { return false; }
        virtual bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const  = 0;

        static ExactScore TBHandlerCore<PieceID, _BoardSize>::minmax_dtc(
            PieceColor color_parent, const std::vector<ExactScore>& child_sc, const std::vector<uint8_t>& child_dtc, 
            uint8_t& ret_dtc, size_t& ret_idx);

    protected:
        std::vector<PieceID>            _piecesID;
        _Board*                         _work_board;
    };
    

    // minmax_dtc
    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore TBHandlerCore<PieceID, _BoardSize>::minmax_dtc(
        PieceColor color_parent,
        const std::vector<ExactScore>& child_sc,
        const std::vector<uint8_t>& child_dtc,
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
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
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
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
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
        ret_dtc = 0;
        return ExactScore::UNKNOWN;
    }

};
#endif

