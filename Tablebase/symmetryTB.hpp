#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// SymmetryTablebase
//  number white pieces n_w < number black pieces n_b - no new build/file, just linking to master TB (n_w >= n_b)
//  Only TB with same number of pieces (ex: 2v2) have a symmetryTB child (ex: 1v2)
//
#ifndef _AL_CHESS_TABLEBASE_SymmetryTablebase_HPP
#define _AL_CHESS_TABLEBASE_SymmetryTablebase_HPP

namespace chess
{
    // SymmetryTablebase
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class SymmetryTablebase : public TablebaseBase<PieceID, _BoardSize>
    {
    public:
        SymmetryTablebase(const Tablebase<PieceID, _BoardSize, NPIECE>* refTB) : TablebaseBase(), _refTB(refTB) {}
        ~SymmetryTablebase() {}

        bool is_symmetry_TB() const override { return true; }
        bool is_build() const       { return _refTB->is_build(); }
        bool load() override        { return _refTB->load(); }
        bool save() const override  { return _refTB->save(); }

        ExactScore  score_v(const std::vector<uint16_t>& sq) const override { return reverse_score(_refTB->score_v(reverse_sq_v(sq, _BoardSize))); }
        uint8_t     dtc_v(const std::vector<uint16_t>& sq) const override   { return _refTB->dtc_v(reverse_sq_v(sq, _BoardSize)); }
        
        const Tablebase<PieceID, _BoardSize, NPIECE>* refTB() const { return _refTB; }

    protected:
        const Tablebase<PieceID, _BoardSize, NPIECE>* _refTB;
    };

    // TBH_Symmetry
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class TBH_Symmetry : public TBH<PieceID, _BoardSize>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBH_Symmetry(const TBH<PieceID, _BoardSize>* refTBH, TB_TYPE t , PieceSet<PieceID, _BoardSize> ps)
            : TBH<PieceID, _BoardSize>(t, ps, NPIECE), _symTBH(refTBH), _type(t) 
        {
            assert( t == TB_TYPE::sym_tb_type(refTBH->tb_type()) );
        }
        ~TBH_Symmetry() {}

        bool is_symmetry_TBH() const override { return true; }

        bool load() override                { return _symTBH->load(); }
        bool save() const override          { return _symTBH->save(); }
        bool build(char verbose) override   { return _symTBH->build(verbose); }
        bool is_build() const override      { return _symTBH->is_build(); }

        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override 
        { 
            // Not needed since not building a TB just linking to one
            return false; 
        }

        TBH<PieceID, _BoardSize>* symTBH() const { return _symTBH; }

    protected:
        TB_TYPE                         _type;
        const TBH<PieceID, _BoardSize>* _symTBH;
    };

};
#endif

