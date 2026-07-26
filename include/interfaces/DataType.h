#pragma once
#include <string>


enum class DataType
{
    Int16,
    Int32,
    Float,
    Double,
    Bool,
    String,
    Int16Array,
    Int32Array,
    FloatArray,
    DoubleArray,
    BoolArray,
    StringArray,
    WStringArray, // Wide String Array
    Unknown // Für unbekannte oder nicht unterstützte Datentypen
};