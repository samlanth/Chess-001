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
        TBH(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps, uint8_t np, TBH_OPTION option) : _type(t), _NPIECE(np), _pieceSet({ ps.wset(), ps.bset() }), _option(option)
        {
            _piecesID = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);
        }
        virtual ~TBH() {}
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
        const PieceSet<PieceID, _BoardSize>& pieceSet()   const { return _pieceSet;}

    protected:
        TB_TYPE                         _type;
        uint8_t                         _NPIECE;
        PieceSet<PieceID, _BoardSize>   _pieceSet;
        TBH_OPTION                      _option;        // none, try_load_on_build, force_rebuild, memory_map_on_build
        std::vector<PieceID>            _piecesID;
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

};
#endif

