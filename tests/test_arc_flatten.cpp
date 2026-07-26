#include <gtest/gtest.h>

#include "CNC_Daten/arc_flatten.h"

using namespace arc_flatten;

// ---------- radiusFromIJ ----------

TEST(ArcFlatten, RadiusFromIJ)
{
    EXPECT_DOUBLE_EQ(radiusFromIJ(3.0, 4.0), 5.0);
    EXPECT_DOUBLE_EQ(radiusFromIJ(0.0, 0.0), 0.0);
}

// ---------- shouldFlatten ----------

TEST(ArcFlatten, ThresholdZeroDisablesFlattening)
{
    EXPECT_FALSE(shouldFlatten(0.1, 0.0));
    EXPECT_FALSE(shouldFlatten(0.1, -1.0));
}

TEST(ArcFlatten, FlattensBelowThresholdOnly)
{
    EXPECT_TRUE(shouldFlatten(0.4, 0.5));
    EXPECT_FALSE(shouldFlatten(0.5, 0.5)); // exakt am Schwellwert -> nicht glätten
    EXPECT_FALSE(shouldFlatten(0.6, 0.5));
    EXPECT_FALSE(shouldFlatten(0.0, 0.5)); // entarteter Radius
}

// ---------- flattenArcLine ----------

TEST(ArcFlatten, SmallArcBecomesLine)
{
    // r = hypot(0.3, 0.4) = 0.5 < 1.0 -> glätten
    const std::string in = "G02 X10.0 Y20.0 I0.3 J0.4";
    EXPECT_EQ(flattenArcLine(in, 1.0), "G01 X10.0 Y20.0");
}

TEST(ArcFlatten, LargeArcUnchanged)
{
    // r = 5.0 >= 1.0 -> unverändert
    const std::string in = "G02 X10.0 Y20.0 I3.0 J4.0";
    EXPECT_EQ(flattenArcLine(in, 1.0), in);
}

TEST(ArcFlatten, ThresholdZeroLeavesLineUntouched)
{
    const std::string in = "G03 X1 Y2 I0.01 J0.0";
    EXPECT_EQ(flattenArcLine(in, 0.0), in);
}

TEST(ArcFlatten, NonArcLinesUntouched)
{
    EXPECT_EQ(flattenArcLine("G01 X10 Y20", 1.0), "G01 X10 Y20");
    EXPECT_EQ(flattenArcLine("G00 X0 Y0", 1.0), "G00 X0 Y0");
    EXPECT_EQ(flattenArcLine("; Kommentar", 1.0), "; Kommentar");
}

TEST(ArcFlatten, G03AlsoFlattened)
{
    const std::string in = "G03 X5 Y5 I0.1 J0.0"; // r = 0.1 < 1.0
    EXPECT_EQ(flattenArcLine(in, 1.0), "G01 X5 Y5");
}

TEST(ArcFlatten, PreservesDecimalCommaInXY)
{
    // X/Y werden wörtlich übernommen (Dezimalkomma bleibt erhalten),
    // Radius aus I/J = hypot(0,3; 0,4) = 0,5 < 1.0.
    const std::string in = "G02 X10,5 Y20,7 I0,3 J0,4";
    EXPECT_EQ(flattenArcLine(in, 1.0), "G01 X10,5 Y20,7");
}

TEST(ArcFlatten, SeparatedLetterValueForm)
{
    // Getrennte Schreibweise "I 0.3" / "J 0.4": Wert-Token wird mit entfernt.
    const std::string in = "G02 X10 Y20 I 0.3 J 0.4";
    EXPECT_EQ(flattenArcLine(in, 1.0), "G01 X10 Y20");
}

TEST(ArcFlatten, LeadingZeroGCodeNormalised)
{
    // "G0002" normalisiert zu "2" -> als Bogen erkannt.
    const std::string in = "G0002 X1 Y1 I0.1 J0.0";
    EXPECT_EQ(flattenArcLine(in, 1.0), "G01 X1 Y1");
}
