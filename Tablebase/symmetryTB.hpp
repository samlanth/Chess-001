#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// SymmetryTablebase
//
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

        const Tablebase<PieceID, _BoardSize, NPIECE>* refTB() const { return _refTB; }
        bool is_build() { return _refTB->is_build(); }

        ExactScore score(const uint16_t& sq0)  const 
        {
            return reverse_score(_refTB->score(sq0));
        }
        ExactScore score(const uint16_t& sq0, const uint16_t& sq1)  const
        {
            return reverse_score(_refTB->score(sq0, sq1));
        }
        ExactScore score(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2)  const
        {
            return reverse_score(_refTB->score(sq0, sq1, sq2));
        }

        uint8_t dtc(const uint16_t& sq0)  const
        {
            return _refTB->dtc(sq0);
        }
        uint8_t dtc(const uint16_t& sq0, const uint16_t& sq1)  const
        {
            return _refTB->dtc(sq0, sq1);
        }
        uint8_t dtc(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2)  const
        {
            return _refTB->dtc(sq0, sq1, sq2);
        }

    protected:
        const Tablebase<PieceID, _BoardSize, NPIECE>* _refTB;

        ExactScore reverse_score(ExactScore sc) const
        {
            if (sc == ExactScore::WIN) return ExactScore::LOSS;
            else if (sc == ExactScore::LOSS) return ExactScore::WIN;
            else if (sc == ExactScore::DRAW) return ExactScore::DRAW;
            return sc; // UNKNOWN
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class TBH_Symmetry : public TBH<PieceID, _BoardSize>
    {
    public:
        TBH_Symmetry(const TBH<PieceID, _BoardSize>* refTBH, TB_TYPE t , PieceSet<PieceID, _BoardSize> ps)
            : TBH<PieceID, _BoardSize>(t, ps, NPIECE), _symTBH(refTBH)
        {
        }
        ~TBH_Symmetry() 
        {

        }
        bool is_symmetry_TBH() const override { return true; }
        bool load() override { return true; }
        bool save() const override { return true; }
        bool build(char verbose) override { return false; }
        bool is_build() const override { return true; }

        TBH<PieceID, _BoardSize>* symTBH() const { return _symTBH; }

    protected:
        const TBH<PieceID, _BoardSize>* _symTBH;
    };

};
#endif

